//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//	@file		FXOS8700CQ.c   									//
//	@brief		FXOS8700CQ implementation                    	//
//	@author		Grupo	4										//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//							Headers								//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

#include "FXOS8700CQ.h"
#include "../I2C/I2C.h"
#include "../Timer/timer.h"
#include "../../board.h"
#include "MK64F12.h"
#include <stddef.h>

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//		CONSTANT AND MACRO DEFINITIONS USING #DEFINE 		 	//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

// FXOS8700CQ I2C Slave Address
//#define FXOS8700CQ_ADDRESS				0x1D
#define FXOS8700CQ_SLAVE_ADDRESS 		0x1D // FXOS8700CQ I2C address


#define FXOS8700CQ_DATA_PACK_LEN	  	  13 // Status plus 6 channels = 13 bytes

//FXOS8700CQ internal register addresses
#define FXOS8700CQ_STATUS_REG           0x00
#define FXOS8700CQ_WHOAMI_REG           0x0D
#define FXOS8700CQ_CTRL_REG1            0x2A
#define FXOS8700CQ_M_CTRL_REG1          0x5B
#define FXOS8700CQ_M_CTRL_REG2          0x5C


#define FXOS8700CQ_XYZ_DATA_CFG_REG     0x0E

#define FXOS8700CQ_OUT_X_MSB_REG        0x01
#define FXOS8700CQ_OUT_X_LSB_REG        0x02
#define FXOS8700CQ_OUT_Y_MSB_REG        0x03
#define FXOS8700CQ_OUT_Y_LSB_REG        0x04
#define FXOS8700CQ_OUT_Z_MSB_REG        0x05
#define FXOS8700CQ_OUT_Z_LSB_REG        0x06

#define FXOS8700CQ_PL_STATUS_REG        0x10
#define FXOS8700CQ_PL_CFG_REG           0x11
#define FXOS8700CQ_PL_COUNT_REG         0x12
#define FXOS8700CQ_PL_BF_ZCOMP_REG      0x13
#define FXOS8700CQ_PL_THS_REG           0x14

#define ACCEL_MG_LSB_4G (0.000488F)
#define SENSORS_GRAVITY_STANDARD (9.8F)
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

static tim_id_t timer_id;
static FXOS8700CQ_raw_data_t accelerometer_data;
static FXOS8700CQ_raw_data_t magnometer_data;
static I2C_transcieve_t i2c_com;
static uint8_t slave_address;
static uint8_t register_address;
static uint8_t buffer[FXOS8700CQ_DATA_PACK_LEN]; //Reading Buffer 
static FXOS8700CQ_mode_t FXOS8700CQ_mode;

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS W FILE LEVEL SCOPE//
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

void dummy(void){};
static void save_data (void);

static void handler();
/*****************************************************************
 * @brief FXOS8700CQ Handler, it performs the corresponding 
 * reading and saving of the accelerometer and magnometer data 
 ****************************************************************/


static FXOS8700CQ_error_t configuration();
/*****************************************************************
 * @brief Function that configures the FXOS8700CQ to enable the
 * reading of the accelerometer and magnometer data
 * @returns: Returns I2C communication errors 
 ****************************************************************/

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//					FUNCTION DEFINITIONS						//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

void FXOS8700CQ_init(FXOS8700CQ_mode_t mode){
/*****************************************************************
 * @brief Function to initialize accelerometer module
 ****************************************************************/
	I2C_init(I2C_0);
	FXOS8700CQ_mode = mode;
	i2c_com.data = buffer;
	i2c_com.error = I2C_NO_ERROR;
	slave_address = FXOS8700CQ_SLAVE_ADDRESS;
	configuration();
	timerInit();
	timer_id = timerGetId();
	timerStart(timer_id,TIMER_MS2TICKS(50),TIM_MODE_PERIODIC,handler);
	return;
}


static FXOS8700CQ_error_t configuration(){
/*****************************************************************
 * @brief Function that configures the FXOS8700CQ to enable the
 * reading of the accelerometer and magnometer data
 * @returns: Returns FXOS8700CQ errors 
 ****************************************************************/

	i2c_com.callback = dummy;
	i2c_com.data_size = 1;

	switch (FXOS8700CQ_mode)
	{

	case STANDBY:

		//Place FXOS8700CQ into standby
		register_address = FXOS8700CQ_CTRL_REG1;
		buffer[0] = 0x00;
		I2C_init_transcieve(slave_address,register_address, &i2c_com, false);
		while(I2C_is_transmitting());			
		break;

	case HYBRID_200:

		register_address = FXOS8700CQ_M_CTRL_REG1;
		buffer[0] = 0x1F;
		I2C_init_transcieve(slave_address,register_address, &i2c_com, false);
		while(I2C_is_transmitting());

		register_address = FXOS8700CQ_M_CTRL_REG2;
		buffer[0] = 0x20;
		I2C_init_transcieve(slave_address,register_address, &i2c_com, false);
		while(I2C_is_transmitting());

		register_address = FXOS8700CQ_XYZ_DATA_CFG_REG;
		buffer[0] = 0x01;		
		I2C_init_transcieve(slave_address,register_address, &i2c_com, false);
		while(I2C_is_transmitting());

		register_address = FXOS8700CQ_CTRL_REG1;
		buffer[0] = 0x0D;
		I2C_init_transcieve(slave_address,register_address, &i2c_com, false);
		while(I2C_is_transmitting());		
		break;

	default:

		register_address = FXOS8700CQ_CTRL_REG1;
		buffer[0] = 0x00;
		I2C_init_transcieve(slave_address,register_address, &i2c_com, false);
		while(I2C_is_transmitting());	
		break;

	}

	return FXOS8700CQ_NO_ERROR;
}


static void handler(void){
/*****************************************************************
 * @brief Accelerometer Handler, it performs the corresponding 
 * reading depending on the selected mode 
 ****************************************************************/
	register_address = FXOS8700CQ_STATUS_REG;
	i2c_com.data_size = FXOS8700CQ_DATA_PACK_LEN;

	switch (FXOS8700CQ_mode)
	{
	case STANDBY:
		i2c_com.callback = dummy;
		break;
	
	case HYBRID_200:
		i2c_com.callback = save_data;
		break;
	default:
		break;
	}

	if(!I2C_is_transmitting())
		I2C_init_transcieve(slave_address,register_address,&i2c_com, true);
	return;
}


static void save_data (void){
/*****************************************************************
 * @brief Function that saves accelerometer and magnometer data
 * in the corresponding structs of data 
 ****************************************************************/

	accelerometer_data.x = (int16_t)((buffer[1]  << 8) | buffer[2])	>> 2;
	accelerometer_data.y = (int16_t)((buffer[3]  << 8) | buffer[4])	>> 2;
	accelerometer_data.z = (int16_t)((buffer[5]  << 8) | buffer[6])	>> 2;


    accelerometer_data.x *= ACCEL_MG_LSB_4G * SENSORS_GRAVITY_STANDARD;
    accelerometer_data.y *= ACCEL_MG_LSB_4G * SENSORS_GRAVITY_STANDARD;
    accelerometer_data.z *= ACCEL_MG_LSB_4G * SENSORS_GRAVITY_STANDARD;


	magnometer_data.x    =			 (buffer[7]  << 8) | buffer[8];
	magnometer_data.y    =			 (buffer[9]  << 8) | buffer[10];
	magnometer_data.z    =			 (buffer[11] << 8) | buffer[12];
	return;
}


FXOS8700CQ_raw_data_t FXOS8700CQ_get_data(FXOS8700CQ_data_options_t data_option){
/*****************************************************************
 * @brief Function to get accelerometer or magnometer data
 * @param data_option: Data option can be accelerometer or magnometer
 * @returns FXOS8700CQ_raw_data_t: Struct with the x,y,z variables.
 ****************************************************************/

	if(data_option == ACCELEROMETER_DATA)
		return accelerometer_data;
	else 
		return magnometer_data;
}
