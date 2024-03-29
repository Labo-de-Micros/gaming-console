/***************************************************************************//**
  @file     UART.c
  @brief    UART Driver for K64F
  @author   Grupo 4 Dieguez-Cruz-Fogg-Martorell
 ******************************************************************************/

#ifndef _UART_H_
#define _UART_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include "hardware.h"


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define UART_CANT_IDS   1
#define __CORE_CLOCK__ 	100000000U
#define BUFFER_SIZE 100

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef struct {
  uint32_t baudrate;
  uint8_t length;
  uint8_t parity_bit;
  uint8_t stop_bit;
} uart_cfg_t;

typedef enum{ UART_0, UART_1, UART_2, UART_3, UART_4, UART_5} UART_type_t;
/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief Initialize UART driver
 * @param id UART's number
 * @param config UART's configuration (baudrate, parity, etc.)
*/
void uart_init (UART_type_t id, uart_cfg_t config);

/**
 * @brief Check if a new byte was received
 * @param id UART's number
 * @return A new byte has being received
*/
uint8_t uartIsRxMsg(uint8_t id);

/**
 * @brief Check how many bytes were received
 * @param id UART's number
 * @return Quantity of received bytes
*/
uint8_t uartGetRxMsgLength(uint8_t id);

/**
 * @brief Read a received message. Non-Blocking
 * @param id UART's number
 * @param msg Buffer to paste the received bytes
 * @param cant Desired quantity of bytes to be pasted
 * @return Real quantity of pasted bytes
*/
uint8_t uartReadMsg(uint8_t id, char* msg, uint8_t cant);

/**
 * @brief Write a message to be transmitted. Non-Blocking
 * @param id UART's number
 * @param msg Buffer with the bytes to be transfered
 * @param cant Desired quantity of bytes to be transfered
 * @return Real quantity of bytes to be transfered
*/
uint8_t uartWriteMsg(uint8_t id, const char* msg, uint8_t cant);

/**
 * @brief Check if all bytes were transfered
 * @param id UART's number
 * @return All bytes were transfered
*/
uint8_t uartIsTxMsgComplete(uint8_t id);

void UART_send_data(unsigned char tx_data );
//envia una palabra por uart
//es bloqueante

unsigned char UART_Recieve_Data(void);
//funcion que se queda escuchando hasta que le llega un mensaje a traves de aurt


void upload_word(UART_type_t id,char * word, uint16_t can);
//copia el mensaje al buffer y activa las interrupciones para mandarlo



bool download_word(int len);
// se fija si llego un mensaje de largo len y lo copia a un arreglo externo al buffer
/*******************************************************************************
 ******************************************************************************/

char * get_word_down_ptr(void);

#endif // _UART_H_
