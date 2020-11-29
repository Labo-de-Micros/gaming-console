//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//	@file		FXOS8700CQ.h   								//
//	@brief		FXOS8700CQ implementation                    //
//	@author		Grupo	4										//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

#ifndef _FXOS8700CQ_H_
#define _FXOS8700CQ_H_

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
	FXOS8700CQ_CONFIG_ERROR,
    FXOS8700CQ_READ_ERROR,
	FXOS8700CQ_NO_ERROR
}FXOS8700CQ_error_t;

typedef enum{
    STANDBY,
	HYBRID_200
}FXOS8700CQ_mode_t;


typedef enum {
    ACCELEROMETER_DATA, 
    MAGNOMETER_DATA
} FXOS8700CQ_data_options_t;

typedef struct {
	int16_t x;
	int16_t y;
	int16_t z;
} FXOS8700CQ_raw_data_t;

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//			FUNCTION PROTOTYPES WITH GLOBAL SCOPE				//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

void FXOS8700CQ_init(FXOS8700CQ_mode_t mode);
/*****************************************************************
 * @brief Function to initialize accelerometer module
 ****************************************************************/

FXOS8700CQ_raw_data_t FXOS8700CQ_get_data(FXOS8700CQ_data_options_t data_option);

/*****************************************************************
 * @brief Function to get accelerometer or magnometer data
 * @param data_option: Data option can be accelerometer or magnometer
 * @returns FXOS8700CQ_raw_data_t: Struct with the x,y,z variables.
 ****************************************************************/

#endif // _FXOS8700CQ_H_ 