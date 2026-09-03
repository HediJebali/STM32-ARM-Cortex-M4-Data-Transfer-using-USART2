#include "stm32f4xx.h"
#include "usart1.h"




static USART2_STATUS_TYPE currentState = USART2_IDLE;

static USART2_IRQStatusType currentIRQstatus = USART2_NO_IRQ ;



/* USART1 messages to be transmitted */
static const char hello_world[] = "Hello World! " ;
static const char ask_for_name[] = "wWhat is your name ? " ;
static const char hi[] = "Hi! " ;
static const char ask_for_command [] = "Please send command";
static const char ask_for_command_ex [] = "Action [turn on/ turn off] Led [ green led/ red led ] ";
static const char turn_on_green_led [] = "turn on green led";
static const char turn_off_green_led [] = "turn off green led";
static const char turn_on_red_led [] = "turn on red led";
static const char turn_off_red_led [] = "turn off red led";
static const char Done [] = "Done";
static const char Wrong_command [] = "Wrong command";
static const char Parity_error [] = "Parity error";

static char RxChar = 0 ;

/*maximum USART reception Buffer length*/
#define MAX_BUFFER_LENGTH      ((uint32_t) 200u)

static char RxBuffer[MAX_BUFFER_LENGTH];
static uint8_t RxMessageLength ;


static strCmpReturnType strCmp (const char * str1 , const char * str2, const uint8_t size){
/*compare status*/
	strCmpReturnType cmpStatus = STR_EQUAL;
	
/*check NULL pointers*/
	if((NULL != str1) && (NULL != str2)){
		/*start comparing*/
		for (int idx = 0 ; idx < size ; idx ++){
		
			if (str1[idx] != str2[idx]) {
			
			cmpStatus = STR_NOT_EQUAL ;
			}
			else {
			/*do nothing*/
			}
		}
	}
	else {
	/* do nothing , null pointers */
	}
	return cmpStatus;
}
 

static void strTransmit (const char * str , uint8_t size ){
/*check NULL pointers */ 
	if (NULL != str){
		/*send all string characters */
		for (int idx =0 ; idx <size ; idx++){
		
		/*check USART status register*/
			while (!(USART2 ->SR & USART_SR_TXE)){
				/*wait for transmission buffer empty flag*/
		}
		
		
		/* write data into transmit data register*/
		USART2 ->DR = str[idx];
	}
}
else {
/* null pointers , do nothing */
}
} 


static void strRecieve( void) {
	/*local string buffer*/
  static char RxLocalBuffer[MAX_BUFFER_LENGTH];
    static int RxIndex = 0;
    
    if (RxChar == '\0') {
        if (RxIndex != 0) {
            /* Copy string data to global RxBuffer */
            for (int idx = 0; idx < RxIndex; idx++) {
                RxBuffer[idx] = RxLocalBuffer[idx];
            }
            /* Add null terminator */
            RxBuffer[RxIndex] = '\0';
            
            /* Set message length & reset index */
            RxMessageLength = RxIndex + 1;
            RxIndex = 0;
        }
    } 
    else {
        /* Store incoming character and increment buffer index */
        if (RxIndex < (MAX_BUFFER_LENGTH - 1)) {
            RxLocalBuffer[RxIndex] = RxChar;
            RxIndex++;
        }
    }
}







/** 
* @brief configure 	GPIO with AF7 , USART2 CONNECTED to APB1 with clock (fck)= 45 MHz 
* @note  PA2 --> USART2_TX , PA3 --> USART2_RX
*/
	
void USART2_GPIO_Config ( void){
/* Enable Port A  clock */
	RCC -> AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	
/* Select alternate function mode */
	GPIOA ->MODER &= ~ (GPIO_MODER_MODER2 | GPIO_MODER_MODER3 );
	GPIOA ->MODER |= (2 << 4 | 2 << 6 );

	
/*select output type push-pull for TX ( PA2)*/
 GPIOA -> OTYPER &= ~ (GPIO_OTYPER_OT2);
	
/* select ouptput speed medium for TX ( PA2) */
GPIOA -> OSPEEDR &= ~(3 << 4);
GPIOA -> OSPEEDR |= (1 << 4);

	
/* select pull-up */
	GPIOA ->PUPDR &= ~ (GPIO_PUPDR_PUPDR2 | GPIO_PUPDR_PUPDR3 );
	GPIOA ->PUPDR |= (1 << 4) | (1 << 6);


/* select AF7 */
  GPIOA->AFR[0] &= ~(GPIO_AFRL_AFSEL2 | GPIO_AFRL_AFSEL3);
  GPIOA ->AFR[0] |= (7<<8) | (7<< 12);
}



void USART2_Init(void){
/* Enable USART2 clock */
RCC -> APB1ENR |= RCC_APB1ENR_USART2EN;
	
/* select Oversampling by 16 mode */
USART2 ->CR1 &= ~(USART_CR1_OVER8);

/* select One sample bit method */
USART2 ->CR3 |= ( USART_CR3_ONEBIT);

/* select 1 start bit , 9 data bits , n stop bits */
USART2 ->CR1 |= USART_CR1_M;

/* select 1 stop bit */
USART2 ->CR2 &= ~(USART_CR2_STOP);

/* enable parity control */
USART2 ->CR1 |= USART_CR1_PCE;

/* select odd parity */
USART2 ->CR1 |= USART_CR1_PS;

/* set baud_rate = 115200 Bps
  * USARTDIV = fck / 16 * baud_rate
	           = 16000000 / (16 * 115200 ) = 8.68
	
	* DIV_Fraction = 16 * 0.68 = 10.88 = 11 = 0xB
	* DIV_Mantissa = 8 = 0x8
	
	*BRR = 0x8B   */
USART2 -> BRR = 0x8B;
}




void USART2_Enable (void){
/* Enable USART2*/
  USART2 ->CR1 |= USART_CR1_UE;
	
/* Enable TX */ 
	USART2 ->CR1 |= USART_CR1_TE;

/* Enable RX */ 
	USART2 ->CR1 |= USART_CR1_RE;

/* Enable reception buffer not empty flag interrpt */
	USART2 ->CR1 |= USART_CR1_RXNEIE;
	
/* Enable Parity error interrupt */ 
	USART2 ->CR1 |= USART_CR1_PEIE;

}


void USART2_IRQ_Callback(void){

/* check if	parity error detected */
	if((USART2 ->SR & USART_SR_PE) == USART_SR_PE )
	{
		while ((USART2 ->SR & USART_SR_RXNE) != USART_SR_RXNE){
			
	       /*wait for RXNE flag to be set*/ 
		}
		
	/*Read data register to clear parity error */
	USART2 ->DR;
	
	/*Set parity error */ 
		currentIRQstatus = USART2_PARITY_ERROR;
	}
	
	else {
		/* no parity error */
	}
	
	/*check USART reciever*/
	if ((USART2 ->SR & USART_SR_RXNE)== USART_SR_RXNE){
	
/* read character*/
RxChar = USART2 ->DR;

/*set IRQ status*/
	currentIRQstatus = USART2_CHAR_RECIEVED;

		
	}
	else{
		/* no data recieverd */
	}
	
}



void USART2_Process(void) {

	/*check error status*/
	switch (currentIRQstatus)
	{ 
		case USART2_PARITY_ERROR :
			/*transmit parity error*/
		strTransmit(Parity_error , sizeof(Parity_error));
		
		/*reset USART1 state*/
		currentState = USART2_IDLE;
		
		/*reset IRQ Status*/
		currentIRQstatus = USART2_NO_IRQ;
		break;
		
		
		case USART2_CHAR_RECIEVED:
			/*recieve string data*/
			strRecieve();
		
		  /* Reset IRQ status*/
		currentIRQstatus = USART2_NO_IRQ;
			break;
		
		
		case USART2_NO_IRQ: 
			break;
		
		default:
		break; }
	
		switch (currentState) {
		
			case USART2_IDLE :
				/*transmit data*/
				strTransmit(hello_world , sizeof(hello_world));
			
			/*go to the next state*/
			currentState = USART2_WAIT_FOR_RESPONCE;
			 break;
			
			case USART2_WAIT_FOR_RESPONCE:
				/*check if new message recieved*/
			if (0 != RxMessageLength){
			
				/*reset message length*/
				RxMessageLength = 0 ;
				
				/*go to the next state*/
				currentState = USART2_ASK_FOR_NAME;
			}
			else{
			/* nothing recieved yet*/
			}
		  	break;
			
			case USART2_ASK_FOR_NAME:
				/*transmit data*/
			strTransmit(ask_for_name, sizeof(ask_for_name));
			
			/*go to the next state*/
			currentState = USART2_WAIT_FOR_NAME;
			 break;
			
			case USART2_WAIT_FOR_NAME:
			/*check if new message recieved*/
      if (RxMessageLength != 0 ){
			
				/*transmit data*/
				strTransmit(hi , sizeof(hi));
				strTransmit(RxBuffer, RxMessageLength);
				strTransmit(ask_for_command, sizeof(ask_for_command));
				strTransmit(ask_for_command_ex, sizeof(ask_for_command_ex));
				
				/*reset message length*/
				RxMessageLength = 0;
				
				/*go to the next state */
				currentState = USART2_WAIT_FOR_COMMAND;
			}			
			else {
			/*nothing recieved yet*/
			}
		  	break;
			
			case USART2_WAIT_FOR_COMMAND :
				/*check if new message recieved*/
        if (RxMessageLength != 0 ){
				/*reset message length*/
					RxMessageLength = 0;
					
					/*string compare results*/
				strCmpReturnType isMatch_01 = STR_NOT_EQUAL;
				strCmpReturnType isMatch_02 = STR_NOT_EQUAL;
				strCmpReturnType isMatch_03 = STR_NOT_EQUAL;
				strCmpReturnType isMatch_04 = STR_NOT_EQUAL;
					
				isMatch_01 = strCmp (turn_on_green_led, RxBuffer, sizeof (turn_on_green_led ));
        isMatch_02 = strCmp (turn_on_red_led, RxBuffer ,sizeof (turn_on_red_led));
        isMatch_03 = strCmp (turn_off_green_led, RxBuffer ,sizeof (turn_off_green_led));
				isMatch_04 = strCmp (turn_off_red_led, RxBuffer , sizeof(turn_off_red_led));

					/*compare with turn on green led command */
				if ( STR_EQUAL == isMatch_01){
				/*turn on green led*/
					GPIOA -> ODR |= (1<<5);
				/*transmit data*/
				strTransmit(Done , sizeof (Done));
				}
        /*compare with turn on red led command*/
				else if ( STR_EQUAL == isMatch_02){
				/*turn on red led*/
				  GPIOA -> ODR |= (1<<6);

				/*transmit data*/
				strTransmit( Done, sizeof(Done));
				}
			  /*compare with turn off green led command*/
				else if ( STR_EQUAL == isMatch_03){
				/*turn off green led*/
					GPIOA -> ODR &= ~(1<<5);
				/*transmit data*/
				strTransmit( Done, sizeof(Done));
				}
					/*compare with turn off red led command */
				else if (STR_EQUAL == isMatch_04){
				/*turn off red led*/
					GPIOA -> ODR &= ~(1<<6);

				/*transmit data*/
				strTransmit(Done, sizeof(Done));
				}
        else {
				strTransmit(Wrong_command, sizeof(Wrong_command));
				}
			}
				else {
				/*do nothing , nothing recieved yet*/
				}
				break;
				
			default:
				break;
		}
}
