/***************************************************************************//**
  @file     board.h
  @brief    Board management
  @author   Nicolás Magliola
 ******************************************************************************/

#ifndef _BOARD_H_
#define _BOARD_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "gpio.h"


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

/***** BOARD defines **********************************************************/

// On Board User LEDs
#define PIN_LED_RED     PORTNUM2PIN(PB,22) 	// PTB22
#define PIN_LED_GREEN   PORTNUM2PIN(PB,26)	// PTB26
#define PIN_LED_BLUE    PORTNUM2PIN(PB,21) 	// PTB21



#define INTERRUPT_PIN	  PORTNUM2PIN(PC,6)		//PTC5
#define DEBUG_PIN       PORTNUM2PIN(PE,25)	//PTE25
#define SIGNAL_PIN		  PORTNUM2PIN(PC,12)	//PTC12

#define LED_ACTIVE			LOW

// On Board User Switches
#define PIN_SW2         PORTNUM2PIN(PC,6)		// PTC6
#define PIN_SW3         PORTNUM2PIN(PA,4)		// PTA4

/*******************************************************************************
 ******************************************************************************/

#endif // _BOARD_H_
