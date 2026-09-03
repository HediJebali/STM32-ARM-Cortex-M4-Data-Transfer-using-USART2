#ifndef USART1_H
#define USART1_H
#include "stdint.h"
#include "stddef.h"


typedef enum {
	USART2_IDLE,
	USART2_WAIT_FOR_RESPONCE,
	USART2_ASK_FOR_NAME,
	USART2_WAIT_FOR_NAME,
	USART2_WAIT_FOR_COMMAND,
}	USART2_STATUS_TYPE;



typedef enum {
	USART2_NO_IRQ,
	USART2_CHAR_RECIEVED,
	USART2_PARITY_ERROR,
}	USART2_IRQStatusType;


typedef enum {
	STR_NOT_EQUAL,
	STR_EQUAL,
}	strCmpReturnType;


static strCmpReturnType strCmp (const char * str1 , const char * str2, const uint8_t size);
static void strTransmit (const char * str , uint8_t size );
static void strRecieve( void);
void USART2_GPIO_Config ( void);
void USART2_Init(void);
void USART2_Enable (void);
void USART2_IRQ_Callback(void);
void USART2_Process(void) ;



#endif