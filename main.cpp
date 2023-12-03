/*
Pure Sine Wave Inverter Using STM32F103C8T6
Version : 01 revision 1

*/

//
//


#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdbool.h>



void Main(void);   // C++ function

int main(void)
{

  SystemCoreClockUpdate();																//
	SysTick_Config(SystemCoreClock/1000);										//
  printf("System Clock =%u Hz \r\n",SystemCoreClock);			//Debug print
	
	Main();																									//main function from AppMain.cpp
	
	while(1)
	{
		
	}
}



