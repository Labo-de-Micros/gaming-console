//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//	@file		SysTick.c										//
//	@brief		SysTick driver implementation					//
//	@author		Grupo	4										//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//							Headers								//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

#include "board.h"
#include "MK64F12.h"
#include <stdio.h>
#include "./SPI.h"

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//		CONSTANT AND MACRO DEFINITIONS USING #DEFINE 		 	//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

#define SPI_DRIVER_ALT			2 	// Alternativa SPI K64F
#define SPI_STOP_TRANSMISSION	(SPI_driver_SPI[module_index]->MCR =(( SPI_driver_SPI[module_index]->MCR & (~SPI_MCR_HALT_MASK)) | SPI_MCR_HALT(1)))
#define SPI_START_TRANSMISSION	(SPI_driver_SPI[module_index]->MCR =(( SPI_driver_SPI[module_index]->MCR & (~SPI_MCR_HALT_MASK)) | SPI_MCR_HALT(0)))
#define SPI_RESTART_TCF			(SPI_driver_SPI[module_index]->SR  =(( SPI_driver_SPI[module_index]->SR  & (~SPI_SR_TCF_MASK)  ) | SPI_SR_TCF(1)))
#define SPI_GET_TCF				(SPI_driver_SPI[module_index]->SR & SPI_SR_TCF_MASK) //No se si va un AND aca
#define SPI_GET_RXCTR			(SPI_driver_SPI[module_index]->SR & SPI_SR_RXCTR_MASK)
#define SPI_GET_POPR			(SPI_driver_SPI[module_index]->POPR)
#define SPI_RESET_PUSHR			(SPI_driver_SPI[module_index]->PUSHR = 0)
#define SPI_SET_TXRXS			(SPI_driver_SPI[module_index]->SR |= SPI_SR_TXRXS(1))
#define SPI_SET_CTCNT			(SPI_driver_SPI[module_index]->PUSHR |= SPI_PUSHR_CTCNT(1))

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//			ENUMERATIONS AND STRUCTURES AND TYPEDEFS	  		//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

typedef struct{
	int port;
	int pin;
}spi_pin_t;

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//						STATIC VARIABLES						//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

static SPI_Type *SPI_driver_SPI[] = {SPI0, SPI1, SPI2};
static uint8_t module_index;

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS W FILE LEVEL SCOPE//
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

static void port_init(void);
/*****************************************************************
 * @brief Function to initialize the SPI pins.
 ****************************************************************/

static void port_setup(spi_pin_t spi_pin);
/*****************************************************************
 * @brief Function to initialize individual SPI pins
 * @param spi_pin The pin to configure.
 ****************************************************************/

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//					FUNCTION DEFINITIONS						//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

void SPI_init(SPI_module_t module){
/*****************************************************************
 * @brief Function to initialize the SPI comunication protocol
 *          driver
 * @param module: Module of the SPI from K64F to utilize, SPI0, SPI1 or
 *                  SPI2.
 ****************************************************************/
	static bool ya_init = false;
	if(!ya_init){
		module_index = module;
		port_init();// Port initialization

		{// Clock Gating
			if(module == SPI_0)
				SIM->SCGC6 |= SIM_SCGC6_SPI0(1);
			else if (module == SPI_1)
				SIM->SCGC6 |= SIM_SCGC6_SPI1(1);
			else if (module == SPI_2)
				SIM->SCGC6 |= SIM_SCGC3_SPI2(1);
		}

		{// MCR Configuration
			SPI_driver_SPI[module_index]->MCR &= ~SPI_MCR_DCONF(0b11); 	//Pongo el DCONF en 00 para configurar el SPI
			SPI_driver_SPI[module_index]->MCR |= SPI_MCR_MSTR(1);		//Lo configuro en modo Master
			SPI_driver_SPI[module_index]->MCR |= SPI_MCR_CONT_SCKE(1);	//Activo el Clock Continuo
			SPI_driver_SPI[module_index]->MCR |= SPI_MCR_MTFE(0);
			SPI_driver_SPI[module_index]->MCR |= SPI_MCR_PCSIS(1);
			SPI_driver_SPI[module_index]->MCR &= ~SPI_MCR_MDIS(1);
			SPI_driver_SPI[module_index]->MCR |= SPI_MCR_CLR_RXF(1);
			SPI_driver_SPI[module_index]->MCR |= SPI_MCR_CLR_TXF(1);
		}

		{// CTAR configuration
			SPI_driver_SPI[module_index]->CTAR[0] &= ~SPI_CTAR_PCSSCK(0b11);
			SPI_driver_SPI[module_index]->CTAR[0] |= SPI_CTAR_CPOL(0);
			SPI_driver_SPI[module_index]->CTAR[0] |= SPI_CTAR_CPHA(0);
			SPI_driver_SPI[module_index]->CTAR[0] |= SPI_CTAR_FMSZ(7);
			SPI_driver_SPI[module_index]->CTAR[0] |= SPI_CTAR_PBR(0b00);
			SPI_driver_SPI[module_index]->CTAR[0] |= SPI_CTAR_BR(0b1000);
		}

		ya_init = true;
	}
    return;
}

uint8_t SPI_transcieve(uint8_t * data2end, uint8_t size, uint8_t * recived_data){
/*****************************************************************
 * @brief Function to send data over the SPI protocol. This function
 *          is a blocking one
 * @param data2send An array containing the data to send over this
 *                  protocol.
 * @param size Size of the array containing the data.
 * @param recivedData Array containing the data received from the slave.
 * @returns The amount of data stored in receivedData.
 ****************************************************************/	
	uint8_t i;
	SPI_SET_TXRXS;
	for(i = 0 ; i < size ; i++){
		SPI_STOP_TRANSMISSION; 	// STOP Trasmission (HALT 1)
		{
			// Scope only for undestanding my code.
			SPI_RESTART_TCF;
			// Set PUSHR
			if((i+1) != size)												// Data to send, not last.
				SPI_driver_SPI[module_index]->PUSHR = SPI_PUSHR_PCS(1) | SPI_PUSHR_CONT(1) | SPI_PUSHR_TXDATA(data2end[i]);	// CONT 1
			else															// Last data to send
				SPI_driver_SPI[module_index]->PUSHR = SPI_PUSHR_PCS(1) | SPI_PUSHR_CONT(0) | SPI_PUSHR_EOQ(1) | SPI_PUSHR_TXDATA(data2end[i]);// CONT 0 | EOQ 1
		}
		SPI_START_TRANSMISSION;	// START Trasmission (HALT 0)

		while(!SPI_GET_TCF);	// Wait for frame to send. (Wait until TCF == 1)

		if(recived_data != NULL && SPI_GET_RXCTR)
			recived_data[i] = SPI_GET_POPR;
		else if(SPI_GET_RXCTR) // Nowhere to save the received data, only pop the data from the register.
			SPI_GET_POPR;
	}
	return i;
}

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//					LOCAL FUNCTION DEFINITIONS					//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

static void port_init(void){
/*****************************************************************
 * @brief Function to initialize the SPI pins.
 ****************************************************************/
    spi_pin_t pcs0 	= {.port=PIN2PORT(SPI_PIN_PCS0), .pin=PIN2NUM(SPI_PIN_PCS0)};	// Esto no esta checkeado!
    spi_pin_t sck	= {.port=PIN2PORT(SPI_PIN_SCK) , .pin=PIN2NUM(SPI_PIN_SCK) };
    spi_pin_t sin	= {.port=PIN2PORT(SPI_PIN_SIN) , .pin=PIN2NUM(SPI_PIN_SIN) };
    spi_pin_t sout	= {.port=PIN2PORT(SPI_PIN_SOUT), .pin=PIN2NUM(SPI_PIN_SOUT)};
    port_setup(pcs0);
    port_setup(sck);
    port_setup(sin);
    port_setup(sout);
	return;
}

static void port_setup(spi_pin_t spi_pin){
/*****************************************************************
 * @brief Function to initialize individual SPI pins
 * @param spi_pin The pin to configure.
 ****************************************************************/
	PORT_Type * port_ptr;
	switch(spi_pin.port){
		case PA:
			port_ptr = PORTA;
			break;
		case PB:
			port_ptr = PORTB;
			break;
		case PC:
			port_ptr = PORTC;
			break;
		case PD:
			port_ptr = PORTD;
			break;
		case PE:
			port_ptr = PORTE;
			break;
		default:
			port_ptr = NULL;
			break;
	}

	if(port_ptr != NULL){
		port_ptr->PCR[spi_pin.pin] &= ~PORT_PCR_MUX_MASK;
		port_ptr->PCR[spi_pin.pin] |= PORT_PCR_MUX(SPI_DRIVER_ALT);
		port_ptr->PCR[spi_pin.pin] |= PORT_PCR_LK(1);
	}
	return;
}
