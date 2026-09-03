#include "stm32f4xx.h"
#include "nvic.h"

void NVIC_init(void){

	/*Set USART1 priority*/
NVIC_SetPriority(USART2_IRQn, 0);
	
	/*Enable USART1 interrupt*/
NVIC_EnableIRQ(USART2_IRQn);
	
}