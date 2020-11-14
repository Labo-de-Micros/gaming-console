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



// FXOS8700CQ internal register addresses 
#define FXOS8700CQ_STATUS       0x00 
#define FXOS8700CQ_WHOAMI       0x0D 
#define FXOS8700CQ_XYZ_DATA_CFG 0x0E 
#define FXOS8700CQ_CTRL_REG1    0x2A 
#define FXOS8700CQ_M_CTRL_REG1  0x5B 
#define FXOS8700CQ_M_CTRL_REG2  0x5C 
#define FXOS8700CQ_WHOAMI_VAL   0xC7



#define OUT_X_MSB   0x01
#define OUT_X_LSB   0x02
#define OUT_Y_MSB   0x03
#define OUT_Y_LSB   0x04
#define OUT_Z_MSB   0x05
#define OUT_Z_LSB   0x06

#define FXOS8700CQ_READ_LEN 13
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

static read_data r_data;

I2C_COM_CONTROL i2c_com;

uint8_t Buffer[FXOS8700CQ_READ_LEN];

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS W FILE LEVEL SCOPE//
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

void dummy(void){};
void accel_magn_dataread(void);
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//					FUNCTION DEFINITIONS						//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

void App_Init (void){
	I2C_init(I2C_0);
	timerInit();
	r_data.error = I2C_OK;
	return;
}

void App_Run (void){
    
    i2c_com.slave_address = MAGN_ADRESS_SLAVE;
	i2c_com.data_size = 1;
	i2c_com.data = Buffer;
	i2c_com.callback = dummy;

	i2c_com.register_address = FXOS8700CQ_CTRL_REG1;
	i2c_com.data[0] = 0x00;
	I2C_init_transcieve(&i2c_com, false);
	timerDelay(TIMER_MS2TICKS(100));

	i2c_com.register_address = FXOS8700CQ_M_CTRL_REG1;
	i2c_com.data[0] = 0x1F;
	I2C_init_transcieve(&i2c_com, false);
	timerDelay(TIMER_MS2TICKS(100));

	i2c_com.register_address = FXOS8700CQ_M_CTRL_REG2;
	i2c_com.data[0] = 0x20;
	I2C_init_transcieve(&i2c_com, false);
	timerDelay(TIMER_MS2TICKS(100));

	i2c_com.register_address = FXOS8700CQ_XYZ_DATA_CFG;
	i2c_com.data[0] = 0x01;
	I2C_init_transcieve(&i2c_com, false);
	timerDelay(TIMER_MS2TICKS(100));
	while(true){
		accel_magn_dataread();
	}
	return;
}

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//					LOCAL FUNCTION DEFINITIONS					//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////


void accel_magn_dataread(void){

    i2c_com.register_address = OUT_X_MSB;
    i2c_com.data_size = false;
    I2C_init_transcieve(&i2c_com,true);
	timerDelay(TIMER_MS2TICKS(100));

    if(i2c_com.fault == I2C_NO_FAULT){
    	r_data.pAccelData.x = (int16_t)(((Buffer[0] << 8) | Buffer[1]))>> 2;
		r_data.pAccelData.y = (int16_t)(((Buffer[2] << 8) | Buffer[3]))>> 2;
		r_data.pAccelData.z = (int16_t)(((Buffer[4] << 8) | Buffer[5]))>> 2;

        // copy the magnetometer byte data into 16 bit words
        //r_data->pMagnData.x = (int16_t)(Buffer[7] << 8) | Buffer[8];
        //r_data->pMagnData.y = (int16_t)(Buffer[9] << 8) | Buffer[10];
        //r_data->pMagnData.z = (int16_t)(Buffer[11] << 8) | Buffer[12];

        r_data.error = I2C_OK;
    }
    else
        r_data.error = I2C_ERROR;

	return;
}
