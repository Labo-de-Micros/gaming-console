//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//	@file		Encoder.c										//
//	@brief		Encoder driver. Advance Implementation			//
//	@author		Grupo	4										//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//							Headers								//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

#include <stddef.h>
#include "./encoder.h"
#include "../GPIO/gpio.h"
#include "../../board.h"
#include "../Timer/timer.h"
#include "../Button/button.h"

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//		CONSTANT AND MACRO DEFINITIONS USING #DEFINE 			//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

#define ENCODER_TIME_TIMER	1

#if ENCODER_TIME_TIMER >= 5
#warning ENCODER_TIME_TIMER too High! Check if that´s ok, maybe when you turn the encoder too fast it wont work!.
#endif

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//			ENUMERATIONS AND STRUCTURES AND TYPEDEFS	  		//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

typedef enum{
	ENCODER_IDLE_ST,

	ENCODER_CASE_1_0_ST,
	ENCODER_CASE_1_1_ST,
	ENCODER_CASE_1_2_ST,
	ENCODER_CASE_1_3_ST,

	ENCODER_CASE_2_0_ST,
	ENCODER_CASE_2_1_ST,
	ENCODER_CASE_2_2_ST,
	ENCODER_CASE_2_3_ST,

	ENCODER_ERROR_ST
}encoder_states_t;

typedef enum{
	ENCODER_B_NEGATIVE_EDGE_EV,
	ENCODER_B_POSITIVE_EDGE_EV,
	ENCODER_A_NEGATIVE_EDGE_EV,
	ENCODER_A_POSITIVE_EDGE_EV,
	ENCODER_TIMER_FINISHED_EV,
	ENCODER_NO_EV
}encoder_events_t;

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//						STATIC VARIABLES						//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

static tim_id_t encoder_timer;
static button_id_t encoder_button;
static encoder_states_t current_state;
static enc_events_t events_buffer[ENC_MAX_BUFFER_EVENTS_LENGHT];
static uint8_t buffer_index_push = 0;
static uint8_t buffer_index_pull = 0;
//static enc_callback_t  callback_ccw;
//static enc_callback_t  callback_cw;
//static enc_callback_t  callback_click;
//static enc_callback_t  callback_double;
//static enc_callback_t  callback_long;


////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS W FILE LEVEL SCOPE//
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

static void callback_A(void);
/*****************************************************************
 * @brief: Callback called by both edges of A encoder pin. It calls
 * 			the state machine handler with the corresponding event.
 * **************************************************************/

static void callback_B(void);
/*****************************************************************
 * @brief: Callback called by both edges of B encoder pin. It calls
 * 			the state machine handler with the corresponding event.
 * **************************************************************/

static void callback_timer(void);
/*****************************************************************
 * @brief: Callback called when the singleshot timer of the state
 * 			machine finishes. It calls the stame machine with the 
 * 			timer finished event.
 * **************************************************************/

static void encoder_state_machine(encoder_events_t ev);
/*****************************************************************
 * @brief: Encoder state machines, it determines which state to go
 * 			whenever an event arrives. When the correct moment arrives
 * 			it calls the clockwise or conter-clockwise callbacks.
 * **************************************************************/

static void push_event(enc_events_t ev);
/*****************************************************************
 * @brief: Pushes an event to the buffer.
 * **************************************************************/

static enc_events_t pull_ev(void);
/*****************************************************************
 * @brief: Pulls an event from the buffer, removing it.
 * **************************************************************/

static void click_callback(void);
/*****************************************************************
 * @brief: Callback for a single press in the encoder. It pushes an
 * 			event to the event buffer.
 * **************************************************************/

static void double_click_callback(void);
/*****************************************************************
 * @brief: Callback for a double press in the encoder. It pushes an
 * 			event to the event buffer.
 * **************************************************************/

static void long_click_callback(void);
/*****************************************************************
 * @brief: Callback for a long press in the encoder. It pushes an
 * 			event to the event buffer.
 * **************************************************************/

static void reset_buffer(void);
/*****************************************************************
 * @brief: Reset the buffer to ENC_NO_EV in the entire array.
 * **************************************************************/


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//					FUNCTION DEFINITIONS						//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

void encoder_init(void){
/*****************************************************************
 * @brief: Initialize the encoder Driver
 * **************************************************************/
	static bool yaInit = false;
	if (yaInit)
		return;
	timerInit();
	encoder_timer = timerGetId();
	gpioMode(ENCODER_PIN_A, INPUT_PULLUP);
	gpioMode(ENCODER_PIN_B, INPUT_PULLUP);
	gpioIRQ (ENCODER_PIN_A, GPIO_IRQ_MODE_BOTH_EDGES, callback_A);
	gpioIRQ (ENCODER_PIN_B, GPIO_IRQ_MODE_BOTH_EDGES, callback_B);
	button_init();
	reset_buffer();
	encoder_button = button_get_new_button(ENCODER_PIN_SW, INPUT_PULLUP);
	current_state = ENCODER_IDLE_ST;
	button_configure(encoder_button, click_callback,long_click_callback,double_click_callback);
	yaInit = true;
	return;
}

enc_events_t encoder_get_event(void){
/*****************************************************************
 * @brief: Function to get the events of the encoder. It saves the 
 *          coming events in order in an internal buffer FIFO type
 *          and it returns them in order. One muste check periodically
 *          this function for not to lose any event.
 * @return: Event arrived of type 'enc_events_t'. ENC_NO_EV will be 
 * 			returned if no event arrived.
 * **************************************************************/
	return pull_ev();
}

/*void encoder_set_callback(){//enc_callback_t  ccw, enc_callback_t  cw, enc_callback_t  click, enc_callback_t double_click, enc_callback_t long_click){
*****************************************************************
 * @brief: Set the callbacks corresponding to the different types
 *			of modes
 * @param ccw: Callback for Counter-Clockwise turn.
 * @param cw: Callback for Clockwise turn.
 * @param click: Callback for button pressed single time.
 * @param double_click: Callback for button pressed double time.
 * @param long_click: Callback for button long press.
 * **************************************************************
	//callback_ccw=ccw;
	//callback_cw=cw;
	//callback_click=click;
	//callback_double=double_click;
	//callback_long=long_click;
	//configure_button(encoder_button, callback_click,callback_long,callback_double);
	return;
}*/

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//					LOCAL FUNCTION DEFINITIONS					//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

static void callback_B(void){
/*****************************************************************
 * @brief: Callback called by both edges of B encoder pin. It calls
 * 			the state machine handler with the corresponding event.
 * **************************************************************/
	if(gpioRead(ENCODER_PIN_B))
		encoder_state_machine(ENCODER_B_POSITIVE_EDGE_EV);
	else
		encoder_state_machine(ENCODER_B_NEGATIVE_EDGE_EV);
	return;
}

static void callback_A(void){
/*****************************************************************
 * @brief: Callback called by both edges of A encoder pin. It calls
 * 			the state machine handler with the corresponding event.
 * **************************************************************/
	if(gpioRead(ENCODER_PIN_A))
		encoder_state_machine(ENCODER_A_POSITIVE_EDGE_EV);
	else
		encoder_state_machine(ENCODER_A_NEGATIVE_EDGE_EV);
	return;
}

static void callback_timer(void){
/*****************************************************************
 * @brief: Callback called when the singleshot timer of the state
 * 			machine finishes. It calls the stame machine with the 
 * 			timer finished event.
 * **************************************************************/
	encoder_state_machine(ENCODER_TIMER_FINISHED_EV);
	return;
}

static void encoder_state_machine(encoder_events_t ev){
/*****************************************************************
 * @brief: Encoder state machines, it determines which state to go
 * 			whenever an event arrives. When the correct moment arrives
 * 			it calls the clockwise or conter-clockwise callbacks.
 * **************************************************************/
	switch(current_state){
		case ENCODER_IDLE_ST:
			if(ev == ENCODER_A_NEGATIVE_EDGE_EV){
				current_state = ENCODER_CASE_1_0_ST;
			}
			else if(ev == ENCODER_B_NEGATIVE_EDGE_EV){
				current_state = ENCODER_CASE_2_0_ST;
			}
			break;
		case ENCODER_CASE_1_0_ST:
			if(ev == ENCODER_B_NEGATIVE_EDGE_EV){
				current_state = ENCODER_CASE_1_1_ST;
			}
			break;
		case ENCODER_CASE_1_1_ST:
			if(ev == ENCODER_A_POSITIVE_EDGE_EV){
				current_state = ENCODER_CASE_1_2_ST;
			}
			break;
		case ENCODER_CASE_1_2_ST:
			if(ev == ENCODER_B_POSITIVE_EDGE_EV){
				gpioDisableIRQ(ENCODER_PIN_B);
				timerStart(encoder_timer,ENCODER_TIME_TIMER,TIM_MODE_SINGLESHOT,callback_timer);
				current_state = ENCODER_CASE_1_3_ST;
			}
			break;
		case ENCODER_CASE_1_3_ST:
			if(ev == ENCODER_TIMER_FINISHED_EV){
				//callback_ccw();
				push_event(ENC_COUNTER_CLOCKWISE_TURN);
				current_state = ENCODER_IDLE_ST;
				gpioIRQ (ENCODER_PIN_B, GPIO_IRQ_MODE_BOTH_EDGES, callback_B);
			}
			break;
		case ENCODER_CASE_2_0_ST:
			if(ev == ENCODER_A_NEGATIVE_EDGE_EV){
				current_state = ENCODER_CASE_2_1_ST;
			}
			break;
		case ENCODER_CASE_2_1_ST:
			if(ev == ENCODER_B_POSITIVE_EDGE_EV){
				current_state = ENCODER_CASE_2_2_ST;
			}
			break;
		case ENCODER_CASE_2_2_ST:
			if(ev == ENCODER_A_POSITIVE_EDGE_EV){
				gpioDisableIRQ(ENCODER_PIN_A);
				timerStart(encoder_timer,ENCODER_TIME_TIMER,TIM_MODE_SINGLESHOT,callback_timer);
				current_state = ENCODER_CASE_2_3_ST;
			}
			break;
		case ENCODER_CASE_2_3_ST:
			if(ev == ENCODER_TIMER_FINISHED_EV){
				//callback_cw();
				push_event(ENC_CLOCKWISE_TURN);
				current_state = ENCODER_IDLE_ST;
				gpioIRQ (ENCODER_PIN_A, GPIO_IRQ_MODE_BOTH_EDGES, callback_A);
			}
			break;
		default:
			current_state = ENCODER_ERROR_ST;
			break;
	}
	return;
}

static void push_event(enc_events_t ev){
/*****************************************************************
 * @brief: Pushes an event to the buffer.
 * **************************************************************/
	events_buffer[buffer_index_push] = ev;	//Pusheo evento
	buffer_index_push++;					//Incremento el indice
	if(buffer_index_push>=ENC_MAX_BUFFER_EVENTS_LENGHT)
		buffer_index_push = 0;				// Si el indice es mayor al largo del buffer seteo el indice en 0 
											// porque es un buffer circular.
	return;
}

static enc_events_t pull_ev(void){
/*****************************************************************
 * @brief: Pulls an event from the buffer, removing it.
 * **************************************************************/
	enc_events_t temp = events_buffer[buffer_index_pull];	//Guardo el ultimo evento recibido
	if(temp == ENC_NO_EV)	//Si no hubo evento, salgo de la funcion.
		return temp;
	events_buffer[buffer_index_pull] = ENC_NO_EV;	//caso contrario, seteo ENC_NO_EV y incremento el indice actual
	buffer_index_pull++;							//teniendo en cuenta el buffer circular.
	if(buffer_index_pull >= ENC_MAX_BUFFER_EVENTS_LENGHT)
		buffer_index_pull = 0;
	return temp;
}

static void click_callback(void){
/*****************************************************************
 * @brief: Callback for a single press in the encoder. It pushes an
 * 			event to the event buffer.
 * **************************************************************/
	push_event(ENC_SINGLE_PRESS);	//Pusheo el evento de click.
	return;
}

static void double_click_callback(void){
/*****************************************************************
 * @brief: Callback for a double press in the encoder. It pushes an
 * 			event to the event buffer.
 * **************************************************************/
	push_event(ENC_DOUBLE_PRESS);	//Pusheo un evento de doble click.
	return;
}

static void long_click_callback(void){
/*****************************************************************
 * @brief: Callback for a long press in the encoder. It pushes an
 * 			event to the event buffer.
 * **************************************************************/
	push_event(ENC_LONG_PRESS);	//Pusheo un evento de long press.
	return;
}

static void reset_buffer(void){
/*****************************************************************
 * @brief: Reset the buffer to ENC_NO_EV in the entire array.
 * **************************************************************/
	for(uint8_t i=0; i < ENC_MAX_BUFFER_EVENTS_LENGHT; i++)	//Seteo todo el buffer en ENC_NO_EV.
		events_buffer[i] = ENC_NO_EV;
	return;
}
