//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//	@file		board.h   										//
//	@brief		Board management.								//
//	@author		Grupo	4										//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

#ifndef _BOARD_H_
#define _BOARD_H_

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//							Headers								//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

#include "./Drivers/GPIO/gpio.h"

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//		CONSTANT AND MACRO DEFINITIONS USING #DEFINE 		 	//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

// On Board User LED



// #define INTERRUPT_PIN	PORTNUM2PIN(PC,6)		//PTC5
// #define DEBUG_PIN       PORTNUM2PIN(PE,25)		//PTE25
// #define SIGNAL_PIN		PORTNUM2PIN(PC,12)		//PTC12

#define LED_ACTIVE			LOW

// On Board User Switches
#define PIN_SW2         PORTNUM2PIN(PC,6)		// PTC6
#define PIN_SW3         PORTNUM2PIN(PA,4)		// PTA4

// SPI
#define SPI_PIN_SCK	    PORTNUM2PIN(PD,1) 		// PTD1
#define SPI_PIN_SOUT    PORTNUM2PIN(PD,2) 		// PTD2
#define SPI_PIN_SIN		PORTNUM2PIN(PD,3) 		// PTD3
#define SPI_PIN_PCS0    PORTNUM2PIN(PC,4) 		// PTC4

#define I2C_SDA			PORTNUM2PIN(PE,25) 		// PTE25
#define I2C_SCL			PORTNUM2PIN(PE,24) 		// PTE24

#define UART0_TX_PIN 17 //PTB17
#define UART0_RX_PIN 16 //PTB16
#define UART3_TX_PIN 17 //PTB11
#define UART3_RX_PIN 16 //PTB10

#define MC74HC589A_SHIFT_PIN    PORTNUM2PIN(PB,9)   // PTB9
#define MC74HC589A_LATCH_PIN    PORTNUM2PIN(PA,1)   // PTA1


#endif // _BOARD_H_
