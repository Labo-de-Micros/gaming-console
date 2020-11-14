//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//							Headers								//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

#include "./Drivers/Timer/timer.h"
#include "./Drivers/I2C/I2C.h"

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//		CONSTANT AND MACRO DEFINITIONS USING #DEFINE 		 	//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

#define BUFFER_SIZE			10
#define MAGN_ADRESS_SLAVE	0x1D
#define TEST_ADDRESS_W		0x26
#define TEST_ADDRESS_R		0x26

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//			ENUMERATIONS AND STRUCTURES AND TYPEDEFS	  		//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

typedef void (* callbackp) (void);

typedef enum { I2C_ERROR,  I2C_OK} I2C_FAIL;

typedef struct{
    int16_t x;
    int16_t y;
    int16_t z;
}SRAWDATA;

typedef struct{
    SRAWDATA pMagnData;
    SRAWDATA pAccelData;
    I2C_FAIL error;
}read_data;

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//						STATIC VARIABLES						//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

static read_data * r_data;
uint8_t Buffer[BUFFER_SIZE];
I2C_transcieve_t i2c_com;
read_data data;


////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS W FILE LEVEL SCOPE//
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

void dummy(void){};

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//					FUNCTION DEFINITIONS						//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

void App_Init (void){
	I2C_init(I2C_0);
	timerInit();
	return;
}

void App_Run (void){
    i2c_com.data = Buffer;
    i2c_com.error = I2C_NO_ERROR;
    
	i2c_com.data[0] = 0xAB;
	i2c_com.data_size = 1;
	I2C_init_transcieve(MAGN_ADRESS_SLAVE, TEST_ADDRESS_W, &i2c_com, false);
	timerDelay(TIMER_MS2TICKS(100));

	i2c_com.data[0] = 0xFF;
    i2c_com.data_size = 1;
    I2C_init_transcieve(MAGN_ADRESS_SLAVE, TEST_ADDRESS_R, &i2c_com, true);
    timerDelay(TIMER_MS2TICKS(1000));
	return;
}

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//					LOCAL FUNCTION DEFINITIONS					//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

