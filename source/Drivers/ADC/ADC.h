//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//	@file		ADC.h   										//
//	@brief	    ADC module implementation for K64F				//
//	@author		Grupo	4										//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

#ifndef _ADC_H
#define _ADC_H

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//							Headers								//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

#include "MK64F12.h"
#include <stdbool.h>
#include <stdint.h>

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//		CONSTANT AND MACRO DEFINITIONS USING #DEFINE 		 	//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

#define ADC_MIN_DIGITAL_VALUE	0x0000
#define ADC_MAX_DIGITAL_VALUE	0xFFFF

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//			ENUMERATIONS AND STRUCTURES AND TYPEDEFS			//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

typedef enum {ADC_HARDWARE_TRIG, ADC_SOFTWARE_TRIG}ADC_trig_t;
typedef void (*ADC_callback_t)(void);

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//			FUNCTION PROTOTYPES WITH GLOBAL SCOPE				//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

void ADC_init(void);
/*****************************************************************
 * @brief Function to initialize the ADC module
 * @details Initializates ADC module with default settings:
 *  			- Interrupts disable
 *  			- Single ended conversion
 *  			- Input channel DADP0
 *  			- 16 bit conversion
 *  			- Software trigger
 ****************************************************************/

void ADC_select_trigger(ADC_trig_t trigger);
/*****************************************************************
 * @brief Function to select the trigger source for the ADC.
 * @param trigger Type of trigger to select.
 ****************************************************************/

void ADC_start_conversion(void);
/*****************************************************************
 * @brief Function to start an ADC conversion (only works if software
 * 			trigger is selected).
 ****************************************************************/

uint16_t ADC_get_data(void);
/*****************************************************************
 * @brief Function to get the conversion data.
 * @return uint16_t conversion value readed.
 ****************************************************************/

bool ADC_is_converting(void);
/*****************************************************************
 * @brief Function to check if the conversion process is active.
 * @return true if conversion process is active.
 ****************************************************************/

// void ADC_DMA_enable(bool enable);
// /*****************************************************************
//  * @brief Function to enable the DMA interrupts
//  * @param enable: if true, DMA interrupts will be enabled.
//  ****************************************************************/

// bool ADC_is_conversion_completed(void);
// /*****************************************************************
//  * @brief Function to check if the conversion has finished.
//  * @return true if conversion is completed
//  ****************************************************************/

// void ADC_enable_continous_conversion(bool enable);
// /*****************************************************************
//  * @brief Function to enable continous conversion.
//  * @param enable Boolean variable, when true continous conversion is enabled.
//  ****************************************************************/

// void ADC_set_callback(ADC_callback_t callback);
// /*****************************************************************
//  * @brief Configures a callback for the conversion completed interrupt
//  * @param callback Function pointer to the handler
//  ****************************************************************/

// void ADC_enable_interrupts(bool enable);
// /*****************************************************************
//  * @brief Enable / Disable interrupts
//  * @param ie enabled / disabled
//  ****************************************************************/

// uint32_t ADC_data_result_address(void);
// /*****************************************************************
//  * @brief ADC0 data register address
//  * @return returns a uint32_t with the ADC0 Result address
//  ****************************************************************/

// uint32_t ADC_get_SC1_address(void);
// /*****************************************************************
//  * @brief Method to get SC1 register address
//  * @return Return the address of the config register.
//  ****************************************************************/

#endif	// _ADC_H