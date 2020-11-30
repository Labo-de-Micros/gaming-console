/*
 * uart.c
 *
 *  Created on: 26 oct. 2020
 *      Author: marth
 */


#include "MK64F12.h"
#include "./uart.h"
#include "../GPIO/gpio.h"
#include "../../board.h"
#include <stdbool.h>

static uint8_t rx_data;


//IRQ_RX_HANDLER

#define BUFFER_SIZE 100
#define TERMINADOR	'\0'


static UART_Type *  uarts_ptr[]= UART_BASE_PTRS;

uint8_t buffer_send[BUFFER_SIZE];
uint8_t buffer_recived[BUFFER_SIZE];

static uint16_t first_recived=0; 		//puntero al principio del buffer de mensajes recibidos
static uint16_t last_recived=0;			//puntero al final del buffer de mensajes recibidos

static	uint16_t first_2_send=0;		//puntero al principio del buffer para enviar
static 	uint16_t last_2_send=0;			//puntero a la cola del buffer para enviar

static bool rx_flag=false;
char word_down[BUFFER_SIZE];

//PCR congfig

void PCR_config(uint32_t port_name, uint32_t pin, uint32_t mux_value);

static PORT_Type* portPtrs[] = PORT_BASE_PTRS;


void PCR_config(uint32_t port_name, uint32_t pin2use, uint32_t mux_value)
{
	uint32_t pin = PORTNUM2PIN(port_name,pin2use);
	PORT_Type *port = portPtrs[PIN2PORT(pin)];
	uint32_t num = PIN2NUM(pin);
	port->PCR[num] =0x00; // fuerzo a valer 0 el PCR
	port->PCR[num] |= PORT_PCR_MUX(mux_value); // activo el modo en el que quiero utilizar ese pin
	port->PCR[num] &= ~PORT_PCR_IRQC_MASK;
	port->PCR[num] |= PORT_PCR_IRQC(0b0000);
}



void uart_init (UART_type_t id, uart_cfg_t config)
{
	//activo el clock-gating del uart a utilizar

	switch (id)
	{
	case UART_0:
		SIM->SCGC4 |= SIM_SCGC4_UART0_MASK;
		//configuro los pins a utilizar en este caso del UART0

		PCR_config(PB,UART0_RX_PIN,3);
		PCR_config(PB,UART0_TX_PIN,3);

		//habilito las interrupciones
		NVIC_EnableIRQ(UART0_RX_TX_IRQn);

		break;
	case UART_1:
		SIM->SCGC4 |= SIM_SCGC4_UART1_MASK;
		break;
	case UART_2:
		SIM->SCGC4 |= SIM_SCGC4_UART2_MASK;
		break;
	case UART_3:
		SIM->SCGC4 |= SIM_SCGC4_UART3_MASK;

		PCR_config(PC,UART3_RX_PIN,3);
		PCR_config(PC,UART3_TX_PIN,3);

		NVIC_EnableIRQ(UART3_RX_TX_IRQn);

		break;
	case UART_4:
		SIM->SCGC1 |= SIM_SCGC1_UART4_MASK;
		break;
	case UART_5:
		SIM->SCGC1 |= SIM_SCGC1_UART5_MASK;	
		break;
	default:
		break;
	}


	//seteo el baudrate
	uint16_t sbr, brfa;
	uint32_t clock;

	if( (id==UART_0 || id == UART_1) ){
		clock= __CORE_CLOCK__;	//cambiar si no se usa el UART0 o UART1
	}
	else
	{
		clock= __CORE_CLOCK__ >> 1;	//por si utilizo otro canal de UART qque no sea el 1 u 0
	}


	sbr= clock/(config.baudrate << 4);
	brfa= (clock << 1) / config.baudrate - (sbr << 5);

	uarts_ptr[id]->BDH=UART_BDH_SBR(sbr >> 8);
	uarts_ptr[id]->BDL=UART_BDL_SBR(sbr);

	//termino el seteo del baudrate

	//configuro C1 para el largo de la palabra, paridad, stop_bit
	uarts_ptr[id]->C1=0x01;	//length 8, no parity, 1 stop_bit


	uarts_ptr[id]->C2 &= ~(UART_C2_TE_MASK | UART_C2_RE_MASK );


	//habilito para que pueda enviar y transmitir
	//solo habilito las interrupciones al recbir ya que yo decido cuando transmitir
	uarts_ptr[id]->TWFIFO=0; //esto es para el buffer fifo

	uarts_ptr[id]->C2=UART_C2_TE_MASK|UART_C2_RE_MASK |  UART_C2_RIE_MASK;
	//
	uarts_ptr[id]->S2 &= ~(0x06); // MSBF = 0, BRK13 = 0

}


void UART_send_data(unsigned char tx_data )
{
	//uint8_t s1 = UART0->S1;
	while(((UART0->S1) & UART_S1_TDRE_MASK) == 0);
	UART0->D = tx_data;

}

unsigned char UART_Recieve_Data(void)
{
	while(((UART0->S1)& UART_S1_RDRF_MASK) ==0); // Espero recibir un caracter
	return(UART0->D); //Devuelvo el caracter recibido
}

__ISR__ UART0_RX_TX_IRQHandler (void)
{
	unsigned char s1;

	s1=UART0->S1;			// Dummy read to clear status register

	if( s1 & UART_S1_RDRF_MASK)		//cuando recibo transmisiones que son de mas de una palabra
	{
		rx_data=UART0->D;
		buffer_recived[last_recived]=rx_data;
		last_recived = (last_recived + 1) % BUFFER_SIZE;
		rx_flag=true;


	}
	else if( s1 & UART_S1_TDRE_MASK)
	{
		UART0->D=buffer_send[first_2_send];
		first_2_send = (first_2_send + 1) % BUFFER_SIZE;
		if(first_2_send == last_2_send)
		{
			UART0->C2 &= ~UART_C2_TIE_MASK;			//deshabilito la interrupcion asi dejo de enviar
		}

	}
	//rx_flag=true;

	//UART_send_data(rx_data+1);

}


__ISR__ UART3_RX_TX_IRQHandler (void)
{
	unsigned char s1;

	s1=UART3->S1;			// Dummy read to clear status register

	if( s1 & UART_S1_RDRF_MASK)		//cuando recibo transmisiones que son de mas de una palabra
	{
		rx_data=UART3->D;
		buffer_recived[last_recived]=rx_data;
		last_recived = (last_recived + 1) % BUFFER_SIZE;
		rx_flag=true;


	}
	else if( s1 & UART_S1_TDRE_MASK)
	{
		UART3->D=buffer_send[first_2_send];
		first_2_send = (first_2_send + 1) % BUFFER_SIZE;
		if(first_2_send == last_2_send)
		{
			UART3->C2 &= ~UART_C2_TIE_MASK;			//deshabilito la interrupcion asi dejo de enviar
		}

	}
	//rx_flag=true;

	//UART_send_data(rx_data+1);

}





void upload_word(UART_type_t id,char * word, uint16_t can){

	uint8_t temp=0;

	while( ((word[temp]) != TERMINADOR) && (temp < can)){

		buffer_send[last_2_send]= word[temp];
		temp++;
		last_2_send= (last_2_send + 1) % BUFFER_SIZE;
	}
	uarts_ptr[id]->C2 |= UART_C2_TIE_MASK;

}

void download_word(int len){

	uint8_t i = 0;

	if(rx_flag){

		while(( first_recived != last_recived) && (i < len)) 	//hasta que igualo los punteros o hasta la cantidad de palabras deseadas
		{
			word_down[i]=buffer_recived[first_recived];
			first_recived= (first_recived + 1) % BUFFER_SIZE;
			i++;
		}

		//upload_word(UART_3,word_down,i); esto es solo si queres transmitir lo que queres enviar
		rx_flag=false;
	}


}
