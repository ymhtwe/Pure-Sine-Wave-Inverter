/*  
      PWM Generator for Pure Sine Wave Inverter
			Using TIMER with DMA
			Table Size 200
			Sinewave frequency 50 Hz
			PWM frequency = 64*50= 3200 Hz
*/
//20231207
//----------
//change timer 2 to timer 1 (advanced timer)
//	- to use break and dead time control feature of timer 1 (for over current detection purpose)
//created const sine table instead of generating 
//created AppMain.h and moved include files to there 
//
//

#include "AppMain.h"

const uint16_t SineTable[SIZE]={ 
                                 0,  56, 113, 169, 226, 282, 338, 395, 451, 507, 563, 618, 674, 730, 785, 840,
                               895, 949,1004,1058,1112,1166,1219,1272,1325,1377,1429,1481,1532,1583,1634,1684,
                              1734,1783,1832,1880,1928,1976,2023,2070,2116,2161,2206,2250,2294,2338,2380,2422,
                              2464,2505,2545,2585,2624,2662,2700,2737,2773,2809,2844,2878,2912,2945,2977,3008,
                              3039,3069,3098,3127,3154,3181,3207,3232,3257,3281,3303,3325,3347,3367,3387,3405,
                              3423,3440,3457,3472,3486,3500,3513,3525,3536,3546,3555,3564,3571,3578,3584,3588,
                              3592,3596,3598,3599,3599,3599,3598,3596,3592,3588,3584,3578,3571,3564,3555,3546,
                              3536,3525,3513,3500,3486,3472,3457,3440,3423,3405,3387,3367,3347,3325,3303,3281,
                              3257,3232,3207,3181,3154,3127,3098,3069,3039,3008,2977,2945,2912,2878,2844,2809,
                              2773,2737,2700,2662,2624,2585,2545,2505,2464,2422,2380,2338,2294,2250,2206,2161,
                              2116,2070,2023,1976,1928,1880,1832,1783,1734,1684,1634,1583,1532,1481,1429,1377,
                              1325,1272,1219,1166,1112,1058,1004, 949, 895, 840, 785, 730, 674, 618, 563, 507,
                               451, 395, 338, 282, 226, 169, 113,  56};


uint16_t OutputTable[SIZE*3];
uint16_t output;

//****************
void Main()
//****************
{
	PrintTable();																			//print sine table
	
	memset(OutputTable,0,SIZE*6);													//create output table with size equal to SIZE*6 (1200) and fill all with zero
	memcpy(OutputTable,SineTable,SIZE*2);										//fill output table with values from sine table: first 400 (SIZE*2)   
    memcpy(&OutputTable[400],SineTable,SIZE*2);								//
	
	 GPIO_Init();
	// DMA1_Init();
	//DMA1_Channel5_Init();															//DMA1 channel 5 init
    //DMA1_Channel7_Init();															//DMA1 channel 7 init
	DMA1_Channel2_Init();																//DMA1 channel 2 init
    DMA1_Channel3_Init();																//DMA1 channel 3 init
    
	//init_timer(TIM2);																	//initialise timer 2		
   init_timer(TIM1);																		//initialise timer 1
	
	while(1)
	{
		
	}
}//end of Main() function


//function to print sine table
///****************************
void PrintTable(void)
//*****************************
{
	int m;
	printf("const uint16_t SineTable[SIZE]={ \n");
	printf("                              ");
	
	for( int i=0,j=1; i<SIZE;i++,j++)
	{ 
		printf("%4d,",(int)(3600*sin(3.1415926*i/SIZE)));
		if( j%16==0)
		{
			printf("\n                              ");
		}
	}
	printf("};\n");
	
}//end of PrintTable() function


//
//********************************************
extern "C" void stdout_putchar(char c)
//********************************************
{
	USART1->DR=c;
}//end of stdout_putchar() function


//function to initialise timers
//Fpsc=72 MHz
//Fcnt = 72 MHz/3200=22500 Hz          //need to fix this
//PSC=0
//ARR=22500										//need to fix this
//***************************************
void init_timer(TIM_TypeDef *timer)
//***************************************
{
    // Enable the clock for the timers
    if (timer == TIM2) 
        RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;						//
	
	if (timer == TIM1) 
        RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;						//
    
    // Set the prescaler value
    timer->PSC =0;

    // Set the auto-reload value
    timer->ARR = 3600-1;														//
			
	// Duty Cycle
    timer->CCR1 = timer->ARR * DUTY/100;								//
			
	// PWM Mode	
	timer->CCMR1 =  	PWM_MODE1 <<12	|	 					/* CH2 PWM OUTPUT Mode1*/ 
								1<<11	|   										/* CH2 Preload Enabled */ 
								1<< 3 	|   										// Preload enabled
								PWM_MODE1 << 4;							//
			
	// Enable
    timer->CCER |= 	TIM_CCER_CC1E	|									//
							TIM_CCER_CC2E;										//
	// DMA Enable
	timer->DIER |=	TIM_DIER_CC2DE	|									//									
							TIM_DIER_CC1DE;
		
	// Break and DeadTime Register
	timer->BDTR |=	TIM_BDTR_MOE;   									// main output enabled
		
	// Enable the timer
    timer->CR1 |=	//<< 5	|												 // up down counting
							//M_CR1_DIR |										 // down counting 
						    TIM_CR1_CEN;
							
}//end of init_timer() function


//function to initialise DMA1 channel 2
//***************************************
void DMA1_Channel2_Init(void)
//***************************************
{
	RCC->AHBENR	|=	RCC_AHBENR_DMA1EN;
	DMA1_Channel2->CMAR = (uint32_t)&OutputTable;
	DMA1_Channel2->CPAR = (uint32_t) & TIM1->CCR1;
	DMA1_Channel2->CNDTR= SIZE*2;
	
	 DMA1_Channel2->CCR = 2 << 12	| 													//priority is high
										 1 << 10 	|		 											//memmory size 16 bit
										 1 << 8  	| 													//peripheral size 16 bit
										 1 << 7  	| 													//memory increment enabled 
										 0 << 6  	| 													//peripheral increment disabled	
										 1 << 5  	| 													//circular mode enabled
										 1 << 4  	| 													//read from memory
										 1 <<1   	|    												//transfer complete interrupt enabled
										 1 ;       															//Enabled 
	NVIC_EnableIRQ(DMA1_Channel7_IRQn);
	
}//end of DMA1_Channel2_Init() function


//function to initialise DMA1 channel 3
//***************************************
void DMA1_Channel3_Init(void)
//***************************************
{
	 RCC->AHBENR	|=	RCC_AHBENR_DMA1EN;
	 DMA1_Channel3->CMAR = (uint32_t)&OutputTable[200];
	 DMA1_Channel3->CPAR = (uint32_t) & TIM1->CCR2;
	 DMA1_Channel3->CNDTR= SIZE*2;
	
	 DMA1_Channel3->CCR = 2 << 12	| 													//priority is high
										 1 << 10 	|		 											//memmory size 16 bit
										 1 << 8  	| 													//peripheral size 16 bit
										 1 << 7  	| 													//memory increment enabled 
										 0 << 6  	| 													//peripheral increment disabled	
										 1 << 5  	| 													//circular mode enabled
										 1 << 4  	| 													//read from memory
										 1 <<1   	|    												//transfer complete interrupt enabled
										 1 ;       															//Enabled 
	NVIC_EnableIRQ(DMA1_Channel7_IRQn);
	
}//end of DMA1_Channel3_Init() function


//function to initialise DMA1 channel 5
//***************************************
void DMA1_Channel5_Init(void)
//***************************************
{
	uint32_t temp;
	RCC->AHBENR	|=	RCC_AHBENR_DMA1EN;											//
	DMA1_Channel5->CMAR	= (uint32_t) OutputTable;									//
	DMA1_Channel5->CPAR  = (uint32_t) & TIM2->CCR1;								//
	DMA1_Channel5->CNDTR = SIZE*2;														//
	
	DMA1_Channel5->CCR =	2 << 12	| 														//priority is high 
										1 << 10 | 														//memmory size 16 bit
										1 << 8	| 														//peripheral size 16 bit
										1 << 7	| 														//memory increment enabled 
										0 << 6 	| 														//peripheral increment disabled 	
										1 << 5 	| 														//circular mode disabled
										1 << 4 	| 														//read from memory 
										1 << 2	| 														//half Transfer interrupt enabled
										1 ;       															//enabled
	 //DMA1_Channel5->CCR=temp; 
	NVIC_EnableIRQ(DMA1_Channel5_IRQn);												//
	
}//end of DMA1_Channel5_Init() function 


//function to initialise DMA1 channel 7
//***************************************
void DMA1_Channel7_Init(void)
//***************************************
{
	 RCC->AHBENR	|=	RCC_AHBENR_DMA1EN;
	 DMA1_Channel7->CMAR = (uint32_t)&OutputTable[200];
	 DMA1_Channel7->CPAR = (uint32_t) & TIM2->CCR2;
	 DMA1_Channel7->CNDTR= SIZE*2;
	
	 DMA1_Channel7->CCR = 2 << 12	| 													//priority is high
										 1 << 10 	|		 											//memmory size 16 bit
										 1 << 8  	| 													//peripheral size 16 bit
										 1 << 7  	| 													//memory increment enabled 
										 0 << 6  	| 													//peripheral increment disabled	
										 1 << 5  	| 													//circular mode enabled
										 1 << 4  	| 													//read from memory
										 1 <<1   	|    												//transfer complete interrupt enabled
										 1 ;       															//Enabled 
	NVIC_EnableIRQ(DMA1_Channel7_IRQn);
}//end of DMA1_Channel7_Init() function


//function to initialise GPIO
//**************************
void GPIO_Init(void)
//**************************
{
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	GPIOA->CRL &=0x00000000;
	GPIOA->CRL |= 0x3333BBBB;
	
}//end of GPIO_Init() function

//
//*******************************************
extern "C"
void DMA1_Channel2_IRQHandler (void)
//*******************************************
{
	if(DMA1->ISR & DMA_ISR_HTIF2 )
	{
		DMA1->IFCR |= DMA_IFCR_CHTIF2;
		for(int i=0;i<200;i++)
		{
			OutputTable[i]=SineTable[i];
		} 
		 //DMA1_Channel7->CNDTR= SIZE;
		 // DMA1_Channel7->CCR|=DMA_CCR7_EN;
	 }
	
}//end of DMA1_Channel2_IRQHandler (void) function


//
//*******************************************
extern "C"
void  DMA1_Channel3_IRQHandler(void)
//*******************************************
{
	if(DMA1->ISR & DMA_ISR_TCIF3)
	{
		DMA1->IFCR |= DMA_IFCR_CGIF3;  // Clear Transfer Complete Interrupt
		for(int i=0;i<200;i++)
		{
			OutputTable[i+400]=SineTable[i];
		}
 
		 // DMA1_Channel5->CNDTR= SIZE;
		  //DMA1_Channel5->CCR|=DMA_CCR5_EN;
	}
	
}//end of DMA1_Channel3_IRQHandler(void) function

//
//*************************************
extern "C"
void TIM2_IRQHandler(void)
//*************************************
{
	static uint16_t T,PW;
	
}//end of TIM2_IRQHandler(void) function

//
//**********************************************
extern "C"
void DMA1_Channel5_IRQHandler (void)
//**********************************************
{
	if(DMA1->ISR & DMA_ISR_HTIF5 )
	{
		DMA1->IFCR |= DMA_IFCR_CHTIF5;
		for(int i=0;i<200;i++)
		{
			OutputTable[i]=SineTable[i];
		}
	  
		  //DMA1_Channel7->CNDTR= SIZE;
		 // DMA1_Channel7->CCR|=DMA_CCR7_EN;
	 }
}//end of DMA1_Channel5_IRQHandler (void) function


//
//********************************************
extern "C"
void  DMA1_Channel7_IRQHandler(void)
//********************************************
{
	if(DMA1->ISR & DMA_ISR_TCIF7)
	{
		DMA1->IFCR |= DMA_IFCR_CGIF7;  // Clear Transfer Complete Interrupt
		for(int i=0;i<200;i++)
		{
			OutputTable[i+400]=SineTable[i];
		}
		 
		// DMA1_Channel5->CNDTR= SIZE;
		//DMA1_Channel5->CCR|=DMA_CCR5_EN;
	 }
	
}//end of DMA1_Channel7_IRQHandler(void) function


uint32_t systick;

//
//************************
extern "C"
void SysTick_Handler()
//***********************
{
//	static int i=0;
	systick++;
//	output= SineTable[i];
//	i++;
//	if(i==SIZE)
//		i=0;
	
}

