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

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//			FUNCTION PROTOTYPES WITH GLOBAL SCOPE				//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

void SPI_init(void);
/*****************************************************************
 * @brief Function to initialize the SPI comunication protocol
 *          driver
 ****************************************************************/

uint8_t SPI_sendReceive(uint8_t * data2send, uint8_t size, uint8_t * recivedData);
/*****************************************************************
 * @brief Function to send data over the SPI protocol. This function
 *          is a blocking one
 * @param data2send An array containing the data to send over this
 *                  protocol.
 * @param size Size of the array containing the data.
 * @param recivedData Array containing the data received from the slave.
 * @returns The amount of data stored in receivedData.
 ****************************************************************/

bool SPI_dataSended(void);
/*****************************************************************
 * @brief Function to check if the data was sended.
 * @returns true if the data was sended, false otherwise.
 ****************************************************************/

bool SPI_availableDataRecived(void);
/*****************************************************************
 * @brief Function to check if data was received.
 * @returns true if data was received, false otherwise.
 ****************************************************************/

uint8_t SPI_getData(uint8_t * dataRecived);
/*****************************************************************
 * @brief Function to get the data received.
 * @param dataReceived pointer to an array for loading the data.
 * @returns the lenght of the data received.
 ****************************************************************/



#endif /* SPI_DRIVER_H_ */
