//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//	@file		Encoder.h									    //
//	@brief		Encoder driver. 								//
//	@author		Grupo	4										//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////


#ifndef _ENCODER_H_
#define _ENCODER_H_

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//							Headers								//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

#include <stdint.h>
#include <stdbool.h>

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//		CONSTANT AND MACRO DEFINITIONS USING #DEFINE 		 	//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

#define ENC_MAX_BUFFER_EVENTS_LENGHT    50  //Maxima candidad de eventos que son posibles de guardar antes de sacarlos de la cola.
#if (ENC_MAX_BUFFER_EVENTS_LENGHT >= 256)
#error ENC_MAX_BUFFER_EVENTS_LENGHT cant be superior to 256, decrease its value or change the uint8_t varibales for its indexes.
#endif

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//			ENUMERATIONS AND STRUCTURES AND TYPEDEFS			//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

typedef uint8_t tim_id_t;
typedef void (*enc_callback_t)(void);
typedef enum {ENC_SINGLE_PRESS, ENC_DOUBLE_PRESS, ENC_LONG_PRESS, ENC_CLOCKWISE_TURN, ENC_COUNTER_CLOCKWISE_TURN, ENC_NO_EV}enc_events_t;

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//			FUNCTION PROTOTYPES WITH GLOBAL SCOPE				//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

void encoder_init(void);
/*****************************************************************
 * @brief: Initialize the encoder Driver
 * **************************************************************/

enc_events_t encoder_get_event(void);
/*****************************************************************
 * @brief: Function to get the events of the encoder. It saves the 
 *          coming events in order in an internal buffer FIFO type
 *          and it returns them in order. One muste check periodically
 *          this function for not to lose any event.
 * @return: Event arrived of type 'enc_events_t'. ENC_NO_EV will be 
 * 			returned if no event arrived.
 * **************************************************************/

/*void encoder_set_callback(void);
    // enc_callback_t ccw, 
    // enc_callback_t cw,
    // enc_callback_t click, 
    // enc_callback_t double_click, 
    // enc_callback_t long_click);
*****************************************************************
 * @brief: Set the callbacks corresponding to the different types
 *			of modes
 * @param ccw: Callback for Counter-Clockwise turn.
 * @param cw: Callback for Clockwise turn.
 * @param click: Callback for button pressed single time.
 * @param double_click: Callback for button pressed double time.
 * @param long_click: Callback for button long press.
 * **************************************************************
 * */

#endif	// _ENCODER_H_