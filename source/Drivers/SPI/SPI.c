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

#define SPI_DRIVER_ALT		2                       // Alternativa SPI K64
#define SPI_DRIVER_INPUT	1
#define SPI_DRIVER_BR       5
#define SPI_DRIVER_PBR      1

// PASARLO A BOARD.H
#define SPI_PIN_PCS0    PORTNUM2PIN(PB,22) 	// PTB22
#define SPI_PIN_SCK	    PORTNUM2PIN(PB,22) 	// PTB22
#define SPI_PIN_SIN		PORTNUM2PIN(PB,22) 	// PTB22
#define SPI_PIN_SOUT    PORTNUM2PIN(PB,22) 	// PTB22
#define _DEBUG_SPI_

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

static bool trasmiting=false;
static SPI_Type *SPI_driver_SPI[] = {SPI0, SPI1, SPI2};
// static PORT_Type * ports[] = PORT_BASE_PTRS;
// static uint32_t sim_port[] = {SIM_SCGC5_PORTA_MASK, SIM_SCGC5_PORTB_MASK, SIM_SCGC5_PORTC_MASK, SIM_SCGC5_PORTD_MASK, SIM_SCGC5_PORTE_MASK};

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

static void mcr_setup(void);
/*****************************************************************
 * @brief Function to configure the MCR register.
 ****************************************************************/

static void ctar_setup(void);
/*****************************************************************
 * @brief Function to configure the CTAR register.
 ****************************************************************/

static void startTrasmissionReception(void);

static void stopTrasmissionReception(void);

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//					FUNCTION DEFINITIONS						//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

void SPI_driver_init (void){
/*****************************************************************
 * @brief Function to initialize the SPI comunication protocol
 *          driver
 ****************************************************************/
	static bool ya_init = false;
	if(!ya_init){
		port_init();						// Inicializo los puertos
		SIM->SCGC6 |= SIM_SCGC6_SPI0(1); 	// Habilito el Clock Gating
		mcr_setup();						// Configuro el MCR register
		ctar_setup();						// Configuro el CTAR register
		ya_init = true;
	}
    return;
}

uint8_t SPI_driver_sendReceive(uint8_t * data2end, uint8_t size,uint8_t * recivedData){
/*****************************************************************
 * @brief Function to send data over the SPI protocol. This function
 *          is a blocking one
 * @param data2send An array containing the data to send over this
 *                  protocol.
 * @param size Size of the array containing the data.
 * @param recivedData Array containing the data received from the slave.
 * @returns The amount of data stored in receivedData.
 ****************************************************************/

	static uint32_t pushrInicial=SPI_PUSHR_PCS(1)|SPI_PUSHR_CONT(1); //cont 1
	static uint32_t pushrFinal=SPI_PUSHR_PCS(1)|SPI_PUSHR_CONT(0)|SPI_PUSHR_EOQ(1);//cont0 eoq
	
	uint32_t pushr2send=0;
	uint8_t dataRecived=0;
	// Bajar todos los flags del sr
	for(int i = 0 ; i < size ; i++){
		if((i+1) == size){// Estoy en el ultimo dato a mandar
			pushr2send=pushrFinal|SPI_PUSHR_TXDATA(data2end[i]);
		}
		else{ // No es el ultimo dato que mando
			pushr2send=pushrInicial|SPI_PUSHR_TXDATA(data2end[i]);
		}


		stopTrasmissionReception();
		SPI_driver_SPI[0]->SR=(SPI_driver_SPI[0]->SR & (~SPI_SR_TCF_MASK) ) | SPI_SR_TCF(1) ; // Reinicio el tcf
		SPI_driver_SPI[0]->PUSHR=pushr2send;
		startTrasmissionReception();


		while(!(SPI_driver_SPI[0]->SR & SPI_SR_TCF_MASK)); // Espero a que se envie el frame

		SPI_driver_SPI[0]->SR= (SPI_driver_SPI[0]->SR & ~SPI_SR_TCF_MASK)| SPI_SR_TCF_MASK;

		if(recivedData != NULL){
			if(SPI_driver_SPI[0]->SR & SPI_SR_RXCTR_MASK){
				recivedData[dataRecived]=SPI_driver_SPI[0]->POPR;
				dataRecived++;
			}
		}
		else{
			SPI_driver_SPI[0]->POPR;
		}
	}
	//SPI_driver_SPI[0]->PUSHR|=SPI_PUSHR_PCS(1);
	//SPI_driver_SPI[0]->PUSHR=(SPI_driver_SPI[0]->PUSHR&(~(SPI_PUSHR_TXDATA_MASK | SPI_PUSHR_PCS_MASK)))| SPI_PUSHR_TXDATA(data2end[i])|SPI_PUSHR_PCS(1);
	//SPI_driver_SPI[0]->PUSHR=(SPI_driver_SPI[0]->PUSHR&(~(SPI_PUSHR_TXDATA_MASK)))| SPI_PUSHR_TXDATA(data2end[i]);
	return dataRecived;
}

bool SPI_driver_dataSended(void){
/*****************************************************************
 * @brief Function to check if the data was sended.
 * @returns true if the data was sended, false otherwise.
 ****************************************************************/
	if(trasmiting){
		if(SPI_driver_SPI[0]->SR & SPI_SR_TCF_MASK){
			SPI_driver_SPI[0]->SR = SPI_SR_TCF_MASK;	// Borro el flag
			trasmiting=false;
		}
	}
	return !trasmiting;
}

bool SPI_driver_availableDataRecived(void){
/*****************************************************************
 * @brief Function to check if data was received.
 * @returns true if data was received, false otherwise.
 ****************************************************************/
	return true;	//Por ahora siempre devuelve true! Checkear esto.
}

uint8_t SPI_driver_getData(uint8_t * dataRecived){
/*****************************************************************
 * @brief Function to get the data received.
 * @param dataReceived pointer to an array for loading the data.
 * @returns the lenght of the data received.
 ****************************************************************/
	uint8_t amauntOfData=0;
	if(trasmiting){
		return 0;
	}
	amauntOfData=(SPI_driver_SPI[0]->SR & SPI_SR_RXCTR_MASK)>>SPI_SR_RXCTR_SHIFT;
	if(amauntOfData != 0){
		for(int i=0 ; i < amauntOfData ; i++){
			dataRecived[i]=SPI_driver_SPI[0]->POPR;
		}
	}

	return amauntOfData;
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
    spi_pin_t sck	= {.port=PIN2PORT(SPI_PIN_SCK), .pin=PIN2NUM(SPI_PIN_SCK)};
    spi_pin_t sin	= {.port=PIN2PORT(SPI_PIN_SIN), .pin=PIN2NUM(SPI_PIN_SIN)};
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
		//Clear and set MUX field
		port_ptr->PCR[spi_pin.pin] &= ~PORT_PCR_MUX_MASK;
		port_ptr->PCR[spi_pin.pin] |= PORT_PCR_MUX(SPI_DRIVER_ALT);
		//Lock PIN.
		port_ptr->PCR[spi_pin.pin] |= PORT_PCR_LK(1);
		//TODO: Veriicar que la configuracion de Lock sea compatible.
	}
	return;
}

static void mcr_setup(void){
/*****************************************************************
 * @brief Function to configure the MCR register.
 ****************************************************************/
	//DCONF. SPI
	SPI0->MCR &= ~SPI_MCR_DCONF(0b11); 	//Pongo el DCONF en 00 para configurar el SPI
    //MSTR. Master Mode
    SPI0->MCR |= SPI_MCR_MSTR(1);		//Lo configuro en modo Master
	//CONT_SCKE. Continuous SCK enabled.
    SPI0->MCR |= SPI_MCR_CONT_SCKE(1);	//Activo el Clock Continuo

#ifdef _DEBUG_SPI_
	//FRZ. Halt serial transfers in debug mode
	SPI0->MCR |= SPI_MCR_FRZ(1);
#endif	// _DEBUG_SPI_

	//MTFE Disbaled
	SPI0->MCR |= SPI_MCR_MTFE(0);
	//PCSIS. PCSx active LOW.
    SPI0->MCR |= SPI_MCR_PCSIS(1);
    //MDIS. Disable module clock
    SPI0->MCR &= ~SPI_MCR_MDIS(1);
    //CLR_RXF. Flush RX FIFO & clear RX Counter
    SPI0->MCR |= SPI_MCR_CLR_RXF(1);
    //CLR_TXF. Flush TX FIFO & clear TX Counter
    SPI0->MCR |= SPI_MCR_CLR_TXF(1);
	return;
}

static void ctar_setup(void){
/*****************************************************************
 * @brief Function to configure the CTAR register.
 ****************************************************************/
	//PCSSCK. PCS to SCK Prescaler value is 1.
	SPI0->CTAR[0] &= ~SPI_CTAR_PCSSCK(0b11);
	//FMSZ. Frame Size = 1Byte = 8bits.
	SPI0->CTAR[0] &= ~SPI_CTAR_FMSZ_MASK;
	SPI0->CTAR[0] |= SPI_CTAR_FMSZ(7);
	//BR.
	SPI0->CTAR[0] &= ~SPI_CTAR_BR_MASK;
	SPI0->CTAR[0] |= SPI_CTAR_BR(0b1111);
	/*
	SPI_driver_SPI[0]->CTAR[0] = SPI_CTAR_CPOL(0) |  \
									SPI_CTAR_CPHA(0)| \
									SPI_CTAR_PBR(SPI_DRIVER_PBR) | \
									SPI_CTAR_BR(SPI_DRIVER_BR) | \
									SPI_CTAR_FMSZ(8-1)| \
									SPI_CTAR_ASC(SPI_DRIVER_BR-3) | \
									SPI_CTAR_PASC(SPI_DRIVER_PBR) | \
									SPI_CTAR_CSSCK(SPI_DRIVER_BR-3) | \
									SPI_CTAR_PCSSCK(SPI_DRIVER_PBR) |
									SPI_CTAR_PDT(0)|\
									SPI_CTAR_DT(0);
	*/
	return;
}

static void startTrasmissionReception(void){
	SPI_driver_SPI[0]->MCR =(SPI_driver_SPI[0]->MCR & (~SPI_MCR_HALT_MASK)) | SPI_MCR_HALT(0);
    return;
}

static void stopTrasmissionReception(void){
	SPI_driver_SPI[0]->MCR =(SPI_driver_SPI[0]->MCR & (~SPI_MCR_HALT_MASK))| SPI_MCR_HALT(1);
    return;
}
