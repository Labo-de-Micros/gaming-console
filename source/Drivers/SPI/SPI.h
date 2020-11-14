//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//	@file		SPI.h   										//
//	@brief		SPI communication protocol driver implementation//
//	@author		Grupo	4										//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

#ifndef SPI_H_
#define SPI_H_

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//							Headers								//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

#include <stdint.h>
#include <stdbool.h>

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

typedef enum{
    SPI_0,
    SPI_1,
    SPI_2
}SPI_module_t

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//			FUNCTION PROTOTYPES WITH GLOBAL SCOPE				//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

void SPI_init(SPI_module_t module);
/*****************************************************************
 * @brief Function to initialize the SPI comunication protocol
 *          driver
 * @param module: Module of the SPI from K64F to utilize, SPI0, SPI1 or
 *                  SPI2.
 ****************************************************************/

uint8_t SPI_transcieve(uint8_t * data2send, uint8_t size, uint8_t * recivedData);
/*****************************************************************
 * @brief Function to send data over the SPI protocol. This function
 *          is a blocking one
 * @param data2send An array containing the data to send over this
 *                  protocol.
 * @param size Size of the array containing the data.
 * @param recivedData Array containing the data received from the slave.
 * @returns The amount of data stored in receivedData.
 ****************************************************************/

#endif /* SPI_DRIVER_H_ */
