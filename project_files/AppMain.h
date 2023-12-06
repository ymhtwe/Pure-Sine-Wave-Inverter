
#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdbool.h>
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

//generating sine table
#define SIZE 200



//function prototypes
void Main(void);  											 //C++ function

void GPIO_Init(void);										//GPIO initi

void init_timer(TIM_TypeDef *timer);

void TIM2_Init(void);										//Timer 2 init
void TIM3_Init(void);										//Timer 3 init
void TIM4_Init(void);										//Timer 4 init

void DMA1_Init(void);
void DMA1_Channel2_Init(void);						//DMA1 ch 2
void DMA1_Channel3_Init(void);						//DMA1 ch 3
void DMA1_Channel5_Init(void);						//DMA1 ch 5
void DMA1_Channel7_Init(void);						//DMA1 ch 7

void PrintTable(void);										//printing sine table



























