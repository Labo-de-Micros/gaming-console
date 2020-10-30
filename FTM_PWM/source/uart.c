/*
 * uart.c
 *
 *  Created on: 26 oct. 2020
 *      Author: marth
 */


#include "MK64F12.h"
#include "uart.h"
#include "gpio.h"
//#include "board.h"
#include <stdbool.h>




//PCR congfig

void PCR_config(uint32_t port_name, uint32_t pin, uint32_t mux_value);

static PORT_Type* portPtrs[] = PORT_BASE_PTRS;


void PCR_config(uint32_t port_name, uint32_t pin, uint32_t mux_value)
{
	uint132_t pin = PORTNUM2PIN(port_name,pin_value);
	PORT_Type *port = portPtrs[PIN2PORT(pin)];
	uint32_t num = PIN2NUM(pin);
	port->PCR[num] =0x00; // fuerzo a valer 0 el PCR
	port->PCR[num] |= PORT_PCR_MUX(value); // activo el modo en el que quiero utilizar ese pin
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
		break;
	case UART_1:
		SIM->SCGC4 |= SIM_SCGC4_UART1_MASK;
		break;
	case UART_2:
		SIM->SCGC4 |= SIM_SCGC4_UART2_MASK;
		break;
	case UART_3:
		SIM->SCGC4 |= SIM_SCGC4_UART3_MASK;
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

	//configuro los pins a utilizar en este caso del UART0
	PORT_config(PORTB,UART0_RX_PIN,3);
	PORT_config(PORTB,UART0_TX_PIN,3);

	//seteo el baudrate
	uint16_t sbr, brfa;
	uint32_t clock;

	clock= __CORE_CLOCK__;	//cambiar si no se usa el UART0 o UART1

	sbr= clock/(config.baudrate << 4);
	brfa= (clock << 1) / config.baudrate - (sbr << 5);

	UART0->BDH=UART_BDH_SBR(sbr >> 8);
	UART0->BDL=UART_BDL_SBR(sbr);

	//termino el seteo del baudrate
	//habilito para que pueda enviar y transmitir

	UART0->C2=UART_C2_TE_MASK|UART_C2_RE_MASK;

}


void UART_send_data(unsigned char tx_data )
{
	uint8_t s1 = UART0->S1;
	while(((s1) & UART_S1_TDRE_MASK)) == 0);

}


