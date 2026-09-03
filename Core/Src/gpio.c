#include "gpio.h"
#include "stm32f4xx.h"
void gpio_init(void){
/*set clock of GPIOA*/
RCC -> AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
/*set PA5 and PA6 as OUTPUTS */
GPIOA -> MODER &= ~(GPIO_MODER_MODER5 | GPIO_MODER_MODER6);
GPIOA -> MODER |= ((1<<10) | (1<<12));
/*set PA5 and PA6 as push pull outputs */
GPIOA ->  OTYPER &= ~ (GPIO_OTYPER_OT5 | GPIO_OTYPER_OT6);
/*set medium speed */
GPIOA -> OSPEEDR &= ~((3 << 10) | (3<< 12));
GPIOA -> OSPEEDR |= ((1<<10) | (1<<12));
	
}