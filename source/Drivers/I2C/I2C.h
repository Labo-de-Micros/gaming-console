//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//	@file		I2C.h   										//
//	@brief		I2C communication protocol driver implementation//
//	@author		Grupo	4										//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

#ifndef _I2C_H_
#define _I2C_H_

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//							Headers								//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//		CONSTANT AND MACRO DEFINITIONS USING #DEFINE 		 	//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//			ENUMERATIONS AND STRUCTURES AND TYPEDEFS			//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

typedef void (* pfunc) (void);

typedef enum{
	I2C_BUS_BUSY,
	I2C_SLAVE_ERROR,
	I2C_NO_ERROR
}I2C_error_t;

typedef struct{
	pfunc callback;				// Callback for when the transfer is finished
	I2C_error_t error;			// Error saved of the transfer, important to check in the callback.
	uint8_t * data;				// Pointer to data buffer
	uint8_t data_size; 			// Amount of Bytes
}I2C_transcieve_t;

typedef enum {
	I2C_0, 
	I2C_1, 
	I2C_2
}I2C_channel_t;

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//			FUNCTION PROTOTYPES WITH GLOBAL SCOPE				//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

void I2C_init(I2C_channel_t channel);
/*****************************************************************
 * @brief Function to initialize I2C Driver module
 * @param channel: Channel of the I2C to initialize. I2C0, I2C1, I2C2
 ****************************************************************/

bool I2C_init_transcieve(uint8_t sl_address, uint8_t reg_address, I2C_transcieve_t * com, bool read);
/*****************************************************************
 * @brief Function to write a message from the I2C module
 * @param slave_address: I2C slave address of the module to communicate
 * 							with.
 * @param register_address: Register address of the module to communicate
 * 							with
 * @param i2c_com: Pointer to communication parameters.
 * @param read:  A boolean value to describe if the next transmission will
 * 				be a TX or a RX.
 * @returns: true if communication started, false otherwise.
 ****************************************************************/

bool I2C_is_transmitting(void);
/*****************************************************************
 * @brief Function to check if the I2C module is currently transmitting
 * 			a massage.
 * @returns - true if I2C module is transmitting, false otherwise.
 ****************************************************************/

#endif	// _I2C_H_ 
