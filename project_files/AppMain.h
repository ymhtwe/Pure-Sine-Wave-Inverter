
#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <math.h>
#include <string.h>

#define PWM_MODE1   		(6)
#define PWM_MODE2   		(7)
#define DUTY        				(80)

#ifdef DEBUG
 #define debug_print(x,...) fprintf( stderr,x,__VA_ARGS__);
#else
 #define debug_print(x,...)
#endif


void Main(void);  											 //C++ function
void TIM2_Init(void);										 //Timer 2 init
void TIM3_Init(void);
void TIM4_Init(void);
void GPIO_Init(void);
void init_timer(TIM_TypeDef *timer);
void DMA1_Init(void);
void DMA1_Channel5_Init(void);
void DMA1_Channel7_Init(void);

