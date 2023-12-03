/*  
      PWM Generator
			Using TIMER2 with DMA
			Table Size 64
			Sinewave frequency 50 Hz
			PWM frequency = 64*50= 3200 Hz
*/

#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <math.h>
#include <string.h>

#define PWM_MODE1   (6)
#define PWM_MODE2   (7)
#define DUTY        (80)
#ifdef DEBUG
 #define debug_print(x,...) fprintf( stderr,x,__VA_ARGS__);
#else
 #define debug_print(x,...)
#endif
void TIM2_Init(void);
void TIM3_Init(void);
void TIM4_Init(void);
void GPIO_Init(void);
void init_timer(TIM_TypeDef *timer);
void DMA1_Init(void);
void DMA1_Channel5_Init(void);
void DMA1_Channel7_Init(void);

#define SIZE 200
uint16_t SineTable[SIZE];
uint16_t OutputTable[SIZE*3];
uint16_t output;

int Main()
{
	for(int i=0;i<SIZE;i++)
	{
		 SineTable[i]= 3600*sin(3.1415926*i/SIZE);
	}
	 memset(OutputTable,0,SIZE*6);
	 memcpy(OutputTable,SineTable,SIZE*2);
   memcpy(&OutputTable[400],SineTable,SIZE*2);
	
	 GPIO_Init();
	// DMA1_Init();
	  DMA1_Channel5_Init();
    DMA1_Channel7_Init();
    init_timer(TIM2);
   
	while(1)
	{
	}
}

extern "C" void stdout_putchar(char c)
{
	USART1->DR=c;
}
/*
    Fpsc=72 MHz
    Fcnt = 72 MHz/3200=22500 Hz
    PSC=0
    ARR=22500
*/

void init_timer(TIM_TypeDef *timer)
{
    // Enable the clock for the timer
    if (timer == TIM2) 
        RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    
    // Set the prescaler value
    timer->PSC =0;

    // Set the auto-reload value
    timer->ARR = 3600-1;
			
		// Duty Cycle
    timer->CCR1 = timer->ARR * DUTY/100;			
			
		// PWM Mode	
		timer->CCMR1 =  PWM_MODE1 <<12 | /* CH2 PWM OUTPUT Mode1*/ 
		                1<<11 |   /* CH2 Preload Enabled */ 
                 		1<< 3 |   // Preload enabled
                		PWM_MODE1 << 4;	
			
		// Enable
    timer->CCER |= TIM_CCER_CC1E|
		               TIM_CCER_CC2E;
		// DMA Enable
		timer->DIER |=TIM_DIER_CC2DE |
		              TIM_DIER_CC1DE;
		
		// Enable the timer
    timer->CR1 |=  //1 << 5      | // Up Down Counting
		              // TIM_CR1_DIR | // Down Counting 
						      TIM_CR1_CEN;
}

void DMA1_Channel5_Init(void)
{
	uint32_t temp;
	 RCC->AHBENR|= RCC_AHBENR_DMA1EN;
	 DMA1_Channel5->CMAR = (uint32_t) OutputTable;
	 DMA1_Channel5->CPAR = (uint32_t) & TIM2->CCR1;
	 DMA1_Channel5->CNDTR= SIZE*2;
	 DMA1_Channel5->CCR =  2 << 12 | /* priority is high */
	                       1 << 10 | /* memmory size 16 bit  */ 
												 1 << 8  | /* peripheral size 16 bit */
                         1 << 7  | /* memory increment enabled */ 
                         0 << 6  | /* peripheral increment disabled */ 	
	                       1 << 5  | /* circular mode disabled */
												 1 << 4  | /* Read from memory */ 
	                       1<< 2   | /* Half Transfer interrupt enabled */
												 1 ;       /* Enabled */
	 //DMA1_Channel5->CCR=temp; 
	NVIC_EnableIRQ(DMA1_Channel5_IRQn);
}

void DMA1_Channel7_Init(void)
{
	 RCC->AHBENR|= RCC_AHBENR_DMA1EN;
	 DMA1_Channel7->CMAR = (uint32_t)&OutputTable[200];
	 DMA1_Channel7->CPAR = (uint32_t) & TIM2->CCR2;
	 DMA1_Channel7->CNDTR= SIZE*2;
	 DMA1_Channel7->CCR =  2 << 12 | /* priority is high */
	                       1 << 10 | /* memmory size 16 bit  */ 
												 1 << 8  | /* peripheral size 16 bit */
                         1 << 7  | /* memory increment enabled */ 
                         0 << 6  | /* peripheral increment disabled */ 	
	                       1 << 5  | /* circular mode enabled */
												 1 << 4  | /* Read from memory */ 
	                       1 <<1   |    //DMA_CCR7_TCIE;|
												 1 ;       /* Enabled */
	NVIC_EnableIRQ(DMA1_Channel7_IRQn);
}

void GPIO_Init(void)
{
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	GPIOA->CRL &=0x00000000;
	GPIOA->CRL |= 0x3333BBBB;
}




extern "C"
void TIM2_IRQHandler(void)
{
	static uint16_t T,PW;
	
	    
}
extern "C"
void DMA1_Channel5_IRQHandler (void)
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
}

extern "C"
void  DMA1_Channel7_IRQHandler(void)
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
}


uint32_t systick;
extern "C"
void SysTick_Handler()
{
//	static int i=0;
	systick++;
//	output= SineTable[i];
//	i++;
//	if(i==SIZE)
//		i=0;
	
}

