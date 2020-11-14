//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//	@file		I2C.h   										//
//	@brief		I2C communication protocol driver implementation//
//	@author		Grupo	4										//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//							Headers								//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

#include "I2C.h"
#include "../../board.h"
#include "MK64F12.h"
#include <stddef.h>

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//		CONSTANT AND MACRO DEFINITIONS USING #DEFINE 		 	//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

#define I2C_START_SIGNAL			(i2c->C1 |= I2C_C1_MST_MASK) 	// Generates STARTF
#define I2C_STOP_SIGNAL				(i2c->C1 &= ~I2C_C1_MST_MASK)	// Generates STOPF
#define I2C_REPEAT_START_SIGNAL		(i2c->C1 |= I2C_C1_RSTA_MASK) 	// Generates RSTARTF
#define I2C_WRITE_BYTE(data)		(i2c->D = data) 				// Write byte to transfer
#define I2C_READ_BYTE				(i2c->D)						// Read current byte
#define I2C_GET_IRQ_FLAG			(i2c->S & I2C_S_IICIF_MASK)		// Get IICIF interrupt flag
#define I2C_CLEAR_IRQ_FLAG			(i2c->S |= I2C_S_IICIF_MASK)	// Clear interrupt flag
#define I2C_GET_RX_ACK				(i2c->S & I2C_S_RXAK_MASK)		// Get RXAK 
#define I2C_SET_RX_MODE				(i2c->C1 &= ~I2C_C1_TX_MASK)	// Set RX mode (TX in 0)
#define I2C_SET_TX_MODE				(i2c->C1 |= I2C_C1_TX_MASK)		// Set TX mode (TX in 1)
#define I2C_SET_NACK				(i2c->C1 |= I2C_C1_TXAK_MASK)	// Set NACK
#define I2C_CLEAR_NACK				(i2c->C1 &= ~I2C_C1_TXAK_MASK)	// Clear NACK
#define I2C_CHECK_BUS				(i2c->S & I2C_S_BUSY_MASK)
#define I2C_GET_TCF					(i2c->S & I2C_S_TCF_MASK)		// Get Transfer Complete Flag.
#define BUS_BUSY 					1

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//			ENUMERATIONS AND STRUCTURES AND TYPEDEFS	  		//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

typedef enum{	//State defined on the state machine diagram on https://www.nxp.com/docs/en/application-note/AN4803.pdf
	I2C_STATE_NONE,
	I2C_STATE_WRITE_DATA,
	I2C_STATE_WRITE_DEV_ADDRESS_W,
	I2C_STATE_WRITE_DEV_ADDRESS_R,
	I2C_STATE_WRITE_REG_ADDRESS,
	I2C_STATE_READ_DUMMY_DATA,
	I2C_STATE_READ_DATA,
}I2C_STATE;

typedef enum{	// Modes of transfer availables
	I2C_MODE_READ,
	I2C_MODE_WRITE,
}I2C_MODE;

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//						STATIC VARIABLES						//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

static I2C_Type* i2cPtrs [] = I2C_BASE_PTRS;
static uint32_t simMasks[] = {SIM_SCGC4_I2C0_MASK, SIM_SCGC4_I2C1_MASK, SIM_SCGC1_I2C2_MASK};
static IRQn_Type i2c_irqs[] = I2C_IRQS;
static I2C_Type* i2c;

static I2C_transcieve_t * i2c_com;
static I2C_STATE state;
static I2C_MODE mode;
static uint8_t data_index;
static uint8_t slave_address;
static uint8_t register_address;

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS W FILE LEVEL SCOPE//
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

static void end_transcieve(I2C_error_t error);
/*****************************************************************
 * @brief Function to end the transfer between the I2C module
 * @param error: Reason for it to finalize the transaction, if 
 * 				error == I2C_NO_ERROR the transaction finalized 
 * 				correctly
 ****************************************************************/

static void handler(void);
/*****************************************************************
 * @brief IRQ Handler for the I2C0,I2C1, I2C2 module, it performs 
 * 			the corresponding state machine for the selected mode of 
 * 			communication.
 ****************************************************************/

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//					FUNCTION DEFINITIONS						//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

void I2C_init(I2C_channel_t channel){
/*****************************************************************
 * @brief Function to initialize I2C Driver module
 * @param channel: Channel of the I2C to initialize. I2C0, I2C1, I2C2
 ****************************************************************/
	SIM_Type* sim_ptr = SIM;
	PORT_Type * portsPtrs[] = PORT_BASE_PTRS;
	i2c = i2cPtrs[channel];
	PORT_Type * port_SDA = portsPtrs[PIN2PORT(I2C_SDA)];
	uint32_t pin_SDA = PIN2NUM(I2C_SDA);
	PORT_Type * port_SCL = portsPtrs[PIN2PORT(I2C_SCL)];
	uint32_t pin_SCL = PIN2NUM(I2C_SCL);

	sim_ptr->SCGC5 |= SIM_SCGC5_PORTE_MASK;		// Enable clock gating for the ports
	if (channel == I2C_2)						// Enable clock gating for the I2C module
		sim_ptr->SCGC1 |= simMasks[channel];
	else
		sim_ptr->SCGC4 |= simMasks[channel];

	// Control Register
	i2c->C1 = 0x00; 							// Initialize the control register module in 0
	i2c->C1 |= I2C_C1_IICEN_MASK; 				// Enable the I2C moudle operation
	i2c->C1 |= I2C_C1_IICIE_MASK; 				// Enables I2C interrupts
	//i2c->S = I2C_S_TCF_MASK | I2C_S_IICIF_MASK; //VER SI SE PUEDE BORRARRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR
	
	// Frequency Divider Register
	i2c->F = I2C_F_MULT(0) | I2C_F_ICR(0); 		// Set the Multiple Factor in 1(00) and the ClockRate prescaler in 00
	// Pins configuration
	port_SDA->PCR[pin_SDA] |= PORT_PCR_MUX(5); 	// Set I2C alternative
	port_SDA->PCR[pin_SDA] |= PORT_PCR_ODE_MASK;// Set open drain enable (See page 1549 Reference manual.)
	port_SCL->PCR[pin_SCL] |= PORT_PCR_MUX(5);	// Set I2C alternative
	port_SCL->PCR[pin_SCL] |= PORT_PCR_ODE_MASK;// Set open drain enable

	NVIC_EnableIRQ(i2c_irqs[channel]);			// Enable all interrupts from I2C
	return;
}

bool I2C_init_transcieve(uint8_t sl_address, uint8_t reg_address, I2C_transcieve_t * com, bool read){
/*****************************************************************
 * @brief Function to write a message from the I2C module
 * @param slave_address: I2C slave address of the module to communicate
 * 							with.
 * @param register_address: Register address of the module to communicate
 * 							with
 * @param i2c_comm: Pointer to communication parameters.
 * @returns: true if communication started, false otherwise.
 ****************************************************************/
	bool started = false;
	if(I2C_CHECK_BUS != BUS_BUSY){
		i2c_com = com;					// Save the I2C data.
		slave_address = sl_address;
		register_address = reg_address;
		data_index = 0;

		i2c_com->error = I2C_NO_ERROR;
		state =  I2C_STATE_WRITE_REG_ADDRESS;

		if(read)
			mode =  I2C_MODE_READ;
		else
			mode =  I2C_MODE_WRITE;

		I2C_SET_TX_MODE;									// Set Transmit Mode
		I2C_START_SIGNAL;			 						// Set Master Mode, sends a STARTF to the I2C
		I2C_WRITE_BYTE((slave_address << 1) & 0b11111110);	// Write address slave to the bus with the R/W bit in 0.
		started = true;
	}
	return started;
}

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//					LOCAL FUNCTION DEFINITIONS					//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

static void end_transcieve(I2C_error_t error){
/*****************************************************************
 * @brief Function to end the transfer between the I2C module
 * @param error: Reason for it to finalize the transaction, if 
 * 				error == I2C_NO_ERROR the transaction finalized 
 * 				correctly
 ****************************************************************/
	I2C_STOP_SIGNAL;				// Send a STOP Signal.(MST is set to 0 again.)
	if(i2c_com->error == I2C_NO_ERROR)
		i2c_com->error = error;		// Save the error only if there was no error before
	if ((mode == I2C_MODE_READ) & (i2c_com->error == I2C_NO_ERROR)){
									// If there was no error and was reading, send a NACK
		I2C_CLEAR_NACK;
		i2c_com->data[data_index] = I2C_READ_BYTE;	// Read last byte.
	}
	state = I2C_STATE_NONE;			// Set state to NONE, in case

	if(i2c_com->callback != NULL)
		i2c_com->callback();		// Call the callback anouncing that the transmission ended.
	return;
}

static void handler(void){
/*****************************************************************
 * @brief IRQ Handler for the I2C0,I2C1, I2C2 module, it performs 
 * 			the corresponding state machine for the selected mode of 
 * 			communication.
 ****************************************************************/
	I2C_CLEAR_IRQ_FLAG;					// Clear interrupt flag.
	if(mode == I2C_MODE_READ){			// Read mode state machine
		switch(state){
			case I2C_STATE_WRITE_REG_ADDRESS:
				if(I2C_GET_RX_ACK == 0){ // ACK arrived
					state = I2C_STATE_WRITE_DEV_ADDRESS_R;
					I2C_WRITE_BYTE(register_address);
				}
				else
					end_transcieve(I2C_SLAVE_ERROR);
				break;
			case I2C_STATE_WRITE_DEV_ADDRESS_R:
				if(I2C_GET_RX_ACK == 0){ // ACK arrived
					state = I2C_STATE_READ_DUMMY_DATA;
					I2C_REPEAT_START_SIGNAL;
					I2C_WRITE_BYTE((slave_address << 1) | 0b00000001);
				}
				else
					end_transcieve(I2C_SLAVE_ERROR);
				break;
			case I2C_STATE_READ_DUMMY_DATA:
				if(I2C_GET_RX_ACK == 0){ // ACK arrived
					state = I2C_STATE_READ_DATA;
					I2C_SET_RX_MODE;
					if(data_index == i2c_com->data_size-1){ 
						I2C_SET_NACK;
					}
					uint8_t dummy_data = I2C_READ_BYTE;
				}
				else
					end_transcieve(I2C_SLAVE_ERROR);
				break;
			case I2C_STATE_READ_DATA:
				if(data_index == i2c_com->data_size-1)
					end_transcieve(I2C_NO_ERROR);
				else {
					if(data_index == i2c_com->data_size-2)
						I2C_SET_NACK;
					i2c_com->data[data_index] = I2C_READ_BYTE;
					data_index++;
				}
				break;
			default:
				break;
		}
	}
	else {	// Write mode state machine
		switch(state){
			case I2C_STATE_WRITE_REG_ADDRESS:
				if(I2C_GET_RX_ACK == 0){
					I2C_WRITE_BYTE(register_address);
					state = I2C_STATE_WRITE_DATA;
				}
				else
					end_transcieve(I2C_SLAVE_ERROR);
				break;
			case I2C_STATE_WRITE_DATA:
				if(I2C_GET_RX_ACK == 0){
					if(data_index == i2c_com->data_size)
						end_transcieve(I2C_NO_ERROR);
					else {
						I2C_WRITE_BYTE(i2c_com->data[data_index]);
						data_index++;
					}
				}
				else
					end_transcieve(I2C_SLAVE_ERROR);
			default:
				break;
		}
	}
	return;
}

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//							HANDLERS							//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

void I2C0_IRQHandler(void){
/*****************************************************************
 * @brief IRQ Handler for the I2C0 module, it performs the corresponding
 * 			state machine for the selected mode of communication.
 ****************************************************************/
	handler();
	return;
}

void I2C1_IRQHandler(void){
/*****************************************************************
 * @brief IRQ Handler for the I2C1 module, it performs the corresponding
 * 			state machine for the selected mode of communication.
 ****************************************************************/
	handler();
	return;
}

void I2C2_IRQHandler(void){
/*****************************************************************
 * @brief IRQ Handler for the I2C2 module, it performs the corresponding
 * 			state machine for the selected mode of communication.
 ****************************************************************/
	handler();
	return;
}
