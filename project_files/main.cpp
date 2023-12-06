/*
Pure Sine Wave Inverter Using STM32F103C8T6
Version: 01 revision 1

*/

//
//
//**************************************************************

#include "main.h"




int main(void)
{

  SystemCoreClockUpdate();															//
  SysTick_Config(SystemCoreClock/1000);										//
  printf("System Clock =%u Hz \r\n",SystemCoreClock);					//Debug print
	
	Main();																					//main function
	
	while(1)
	{
		
	}
}



