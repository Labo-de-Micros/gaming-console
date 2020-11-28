//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//	@file		accelerometer.c   								//
//	@brief		Accelerometer implementation                    //
//	@author		Grupo	4										//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//							Headers								//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

#include "accelerometer.h"
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

#define ACCELEROMETER_ADDRESS			0x1D
#define ACCELEROMETER_SLAVE_ADDRESS 	0x1D // FXOS8700CQ I2C address
#define ACCELEROMETER_DATA_PACK_LEN		  13 // Status plus 6 channels = 13 bytes


//FXOS8700CQ internal register addresses
#define ACCELEROMETER_STATUS 			0x00
#define ACCELEROMETER_WHOAMI 			0x0D
#define ACCELEROMETER_XYZ_DATA_CFG 		0x0E
#define ACCELEROMETER_CTRL_REG1 		0x2A
#define ACCELEROMETER_M_CTRL_REG1 		0x5B
#define ACCELEROMETER_M_CTRL_REG2 		0x5C
#define ACCELEROMETER_WHOAMI_VAL 		0xC7

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
static accelerometer_raw_data_t accelerometer_data;
static accelerometer_raw_data_t magnometer_data;
static I2C_transcieve_t i2c_com;
static uint8_t slave_address;
static uint8_t register_address;
static uint8_t buffer[ACCELEROMETER_DATA_PACK_LEN]; //Reading Buffer 

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS W FILE LEVEL SCOPE//
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
void dummy(void){};
static void save_data (void);

static void handler();
/*****************************************************************
 * @brief Accelerometer Handler, it performs the corresponding 
 * reading and saving of the accelerometer and magnometer data 
 ****************************************************************/


static accelerometer_error_t configuration();
/*****************************************************************
 * @brief Function that configures the accelerometer to enable the
 * reading of the accelerometer and magnometer data
 * @returns: Returns I2C communication errors 
 ****************************************************************/

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//					FUNCTION DEFINITIONS						//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

void accelerometer_init(){
/*****************************************************************
 * @brief Function to initialize accelerometer module
 ****************************************************************/

	I2C_init(I2C_0);
	configuration();
	i2c_com.error = I2C_NO_ERROR;

	timerInit();
	timer_id = timerGetId();
	timerStart(timer_id,TIMER_MS2TICKS(1000),TIM_MODE_PERIODIC,handler);
	return;
}


static accelerometer_error_t configuration(){
/*****************************************************************
 * @brief Function that configures the accelerometer to enable the
 * reading of the accelerometer and magnometer data
 * @returns: Returns accelerometer errors 
 ****************************************************************/

	//Secuencia de configuracion del acelerometro
	slave_address = ACCELEROMETER_SLAVE_ADDRESS;
	i2c_com.callback = dummy;
	i2c_com.data_size = 1;

	register_address = ACCELEROMETER_CTRL_REG1;
	i2c_com.data = buffer;
	buffer[0] = 0x00;
	while (!I2C_init_transcieve(slave_address,register_address, &i2c_com, false));

	register_address = ACCELEROMETER_CTRL_REG1;
	buffer[0] = 0x1F;

	while(!I2C_init_transcieve(slave_address,register_address, &i2c_com, false));

	register_address = ACCELEROMETER_M_CTRL_REG2;
	buffer[0] = 0x20;

	while(!I2C_init_transcieve(slave_address,register_address, &i2c_com, false));

	register_address = ACCELEROMETER_XYZ_DATA_CFG;
	buffer[0] = 0x01;
	
	while(!I2C_init_transcieve(slave_address,register_address, &i2c_com, false));

	register_address = ACCELEROMETER_CTRL_REG1;
	buffer[0] = 0x0D;

	while(!I2C_init_transcieve(slave_address,register_address, &i2c_com, false));

	return ACCELEROMETER_NO_ERROR;
}


static void handler(void){
/*****************************************************************
 * @brief Accelerometer Handler, it performs the corresponding 
 * reading and saving of the accelerometer and magnometer data 
 ****************************************************************/

	slave_address = ACCELEROMETER_SLAVE_ADDRESS;
	register_address = ACCELEROMETER_STATUS;
	i2c_com.callback = save_data;
	i2c_com.data_size = ACCELEROMETER_DATA_PACK_LEN;
	i2c_com.data = buffer;

	I2C_init_transcieve(slave_address,register_address,&i2c_com, true);
}


static void save_data (void){
/*****************************************************************
 * @brief Function that saves accelerometer and magnometer data
 * in the corresponding structs of data 
 ****************************************************************/

	accelerometer_data.x = (int16_t)((buffer[1]  << 8) | buffer[2])	>> 2;
	accelerometer_data.y = (int16_t)((buffer[3]  << 8) | buffer[4])	>> 2;
	accelerometer_data.z = (int16_t)((buffer[5]  << 8) | buffer[6])	>> 2;

	magnometer_data.x    =			 (buffer[7]  << 8) | buffer[8];
	magnometer_data.y    =			 (buffer[9]  << 8) | buffer[10];
	magnometer_data.z    =			 (buffer[11] << 8) | buffer[12];
}


accelerometer_raw_data_t accelerometer_get_data(accelerometer_data_options_t data_option){
/*****************************************************************
 * @brief Function to get accelerometer or magnometer data
 * @param data_option: Data option can be accelerometer or magnometer
 * @returns accelerometer_raw_data_t: Struct with the x,y,z variables.
 ****************************************************************/

	if(data_option == ACCELEROMETER_ACCEL_DATA)
		return accelerometer_data;
	else 
		return magnometer_data;
}
