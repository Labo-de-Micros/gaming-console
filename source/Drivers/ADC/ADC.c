//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//	@file		ADC.c											//
//	@brief	    ADC module implementation for K64F				//
//	@author		Grupo	4										//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//							Headers								//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

#include "ADC.h"
#include "MK64F12.h"
#include <stdlib.h>

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//		CONSTANT AND MACRO DEFINITIONS USING #DEFINE 		 	//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

#define SET_SC1_DIFF(x)		(ADC0->SC1[0] |= ADC_SC1_DIFF(x))
#define SET_SC1_ADCH(x)		(ADC0->SC1[0] |= ADC_SC1_ADCH(x))
#define SET_SC1_AIEN(x)		(ADC0->SC1[0] |= ADC_SC1_AIEN(x))
#define SET_CFG1_MODE(x)	(ADC0->CFG1   |= ADC_CFG1_MODE(x))
#define SET_CFG1_ADICLK(x)	(ADC0->CFG1 |= ADC_CFG1_ADICLK(x))
#define SET_CFG1_ADIV(x)	(ADC0->CFG1 |= ADC_CFG1_ADIV(x))
#define START_CALIBRATION	(ADC0->SC3 |= ADC_SC3_CAL_MASK)


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//			ENUMERATIONS AND STRUCTURES AND TYPEDEFS	  		//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//						STATIC VARIABLES						//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

static ADC_callback_t conv_completed_callback = NULL;
static ADC_Type * adc;

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//					FUNCTION DEFINITIONS						//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

void ADC_init(void){
/*****************************************************************
 * @brief Function to initialize the ADC module
 * @details Initializates ADC module with default settings:
 *  			- Interrupts disable
 *  			- Single ended conversion
 *  			- Input channel DADP0
 *  			- 16 bit conversion
 *  			- Max sample freq =~ 200kHz
 *  			- Continous Conversion
 *  			- Software trigger
 ****************************************************************/
	static bool ya_init = false;
	if(ya_init)
		return;
	ya_init = true;
	adc = ADC0;
	SIM->SCGC6 |= SIM_SCGC6_ADC0_MASK;		// Clock Gating.
	
	{	// SC1 Register configuration
		SET_SC1_DIFF(0);						// Single ended conversion
		SET_SC1_ADCH(0b00000);					// Select input channel as DADP0
		SET_SC1_AIEN(1);						// Enable COCO Interrupt (COnversion COmpleted Interrupts)
	}

	{	// CFG1 Register Configuration
		SET_CFG1_MODE(0b11);					// Set mode 11 -> 16-bit conversion
		SET_CFG1_ADICLK(0b00);					// Select Bus Clock
		SET_CFG1_ADIV(0b00);					// Select Divide ratio to 1.
	}

	{	// SC3 Register configuration
		START_CALIBRATION;
		while(ADC0->SC3 & ADC_SC3_CALF_MASK);	// Wait for calibration to finish without failure
		ADC0->SC3 |= ADC_SC3_AVGE_MASK;			// Enable Hardware Average
		ADC0->SC3 |= ADC_SC3_AVGS(0b00);		// Set Hardware average to 4 conversions.
	}
	
	ADC_select_trigger(ADC_SOFTWARE_TRIG);
	// // Enable Interrupts
	// NVIC_ClearPendingIRQ(ADC0_IRQn);
	// NVIC_EnableIRQ(ADC0_IRQn);
	return;
}


bool ADC_is_converting(void){
/*****************************************************************
 * @brief Function to check if the conversion process is active.
 * @return true if conversion process is active.
 ****************************************************************/
	return (ADC0->SC2 & ADC_SC2_ADACT_MASK);
}

void ADC_select_trigger(ADC_trig_t trigger){
/*****************************************************************
 * @brief Function to select the trigger source for the ADC.
 * @param trigger Type of trigger to select.
 ****************************************************************/
	if (trigger == ADC_HARDWARE_TRIG)
		ADC0->SC2 |= ADC_SC2_ADTRG_MASK;
	else if (trigger == ADC_SOFTWARE_TRIG)
		ADC0->SC2 &= ~ADC_SC2_ADTRG_MASK;
	return;
}

uint16_t ADC_get_data(void){
/*****************************************************************
 * @brief Function to get the conversion data.
 * @return uint16_t conversion value readed.
 ****************************************************************/
	return (uint16_t)ADC0->R[0];
}

void ADC_start_conversion(void){
/*****************************************************************
 * @brief Function to start an ADC conversion (only works if software
 * 			trigger is selected).
 ****************************************************************/
	/* Input channel select */
	ADC0->SC1[0] &= ~ADC_SC1_ADCH_MASK;
	return;
}

// void ADC_DMA_enable(bool enable){
// /*****************************************************************
//  * @brief Function to enable the DMA interrupts
//  * @param enable: if true, DMA interrupts will be enabled.
//  ****************************************************************/
// 	if(enable)
// 		ADC0->SC2 = ADC_SC2_DMAEN(1);	// DMA Enable = 1
// 	else
// 		ADC0->SC2 = ADC_SC2_DMAEN(0);	// DMA Enable = 0
// 	return;
// }

// bool ADC_is_conversion_completed(void){
// /*****************************************************************
//  * @brief Function to check if the conversion has finished.
//  * @return true if conversion is completed
//  ****************************************************************/
// 	return (ADC0->SC1[0] & ADC_SC1_COCO_MASK);
// }

// uint32_t ADC_data_result_address(void){
// /*****************************************************************
//  * @brief ADC0 data register address
//  * @return returns a uint32_t with the ADC0 Result address
//  ****************************************************************/
// 	uint32_t ret = (uint32_t)&(ADC0->R[0]);
// 	return ret;
// }

// uint32_t ADC_get_SC1_address(void){
// /*****************************************************************
//  * @brief Method to get SC1 register address
//  * @return Return the address of the config register.
//  ****************************************************************/
// 	return (uint32_t)&(ADC0->SC1[0]);
// }

// void ADC_enable_continous_conversion(bool enable){
// /*****************************************************************
//  * @brief Function to enable continous conversion.
//  * @param enable Boolean variable, when true continous conversion is enabled.
//  ****************************************************************/
// 	if (enable)
// 		ADC0->SC3 |= ADC_SC3_ADCO_MASK;
// 	else
// 		ADC0->SC3 &= ~ADC_SC3_ADCO_MASK;
// 	return;
// }

// void ADC_set_callback(ADC_callback_t callback){
// /*****************************************************************
//  * @brief Configures a callback for the conversion completed interrupt
//  * @param callback Function pointer to the handler
//  ****************************************************************/
// 	conv_completed_callback = callback;
// 	return;
// }

// void ADC_enable_interrupts(bool enable){
// /*****************************************************************
//  * @brief Enable / Disable interrupts
//  * @param ie enabled / disabled
//  ****************************************************************/
// 	if(enable)
// 		NVIC_EnableIRQ(ADC0_IRQn);
// 	else
// 		NVIC_DisableIRQ(ADC0_IRQn);
// }

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//							HANDLERS							//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

// void ADC0_IRQHandler(void){
// 	if(conv_completed_callback != NULL)
// 		conv_completed_callback();
// 	return;
// }
