/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include "board.h"
#include "gpio.h"
#include "./Drivers/SysTick/SysTick.h"
#include "uart.h"
#include"./Drivers/Timer/timer.h"
//#define DEBUG_OSCILLOSCOPE
/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
#define BLINK_FREQ_HZ 10U
#if SYSTICK_ISR_FREQUENCY_HZ % (2*BLINK_FREQ_HZ) != 0
#warning BLINK cannot implement this exact frequency.
		Using floor(SYSTICK_ISR_FREQUENCY_HZ/BLINK_FREQ_HZ/2) instead.
#endif
/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/* Función que se llama 1 vez, al comienzo del programa */
void App_Init (void){
	//gpioMode(PIN_LED_GREEN, OUTPUT);
	//gpioMode(PIN_LED_BLUE, OUTPUT);
	//SysTick_Init(systick_callback);
	//gpioMode(DEBUG_PIN, OUTPUT);
	//gpioMode(PIN_LED_BLUE, OUTPUT);
	//gpioMode(INTERRUPT_PIN, INPUT_PULLDOWN);
	//gpioIRQ (INTERRUPT_PIN, GPIO_IRQ_MODE_RISING_EDGE, irq_callback);

 	timerInit();
// 	FTM_Init(FTM0);
 	uart_cfg_t uart_test;
 	uart_test.baudrate=9600;
 	uart_init(UART_3, uart_test);




	return;
}

/* Función que se llama constantemente en un ciclo infinito */
void App_Run (void){
	char word[]="A";
	//char word[]="Tuvi";
	//UART_send_data('A');
	upload_word(UART_3,word,1);
	timerDelay(TIMER_MS2TICKS(500));
	while (1) {

		//timerDelay(TIMER_MS2TICKS(300));
		download_word();


	}
	return;
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
