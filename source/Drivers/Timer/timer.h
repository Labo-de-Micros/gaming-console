 /////////////////////////////////////////////////////////////////
 /////////////////////////////////////////////////////////////////
 //	@file		timer.h 									    //
 //	@brief		Timer driver. Advance Implementation			//
 //	@author		Grupo 4 										//
 /////////////////////////////////////////////////////////////////
 /////////////////////////////////////////////////////////////////

#ifndef _TIMER_H_
#define _TIMER_H_

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//							Headers								//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//		CONSTANT AND MACRO DEFINITIONS USING #DEFINE 		 	//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

#define TIMER_TICK_MS       1
#define TIMER_MS2TICKS(ms)  ((ms)/TIMER_TICK_MS)

#define TIMERS_MAX_CANT     16
#define TIMER_INVALID_ID    255

#if (TIMERS_MAX_CANT >= TIMER_INVALID_ID)
#error TIMERS_MAX_CANT must be lower than TIMER_INVALID_ID! In case you want TIMER_INVALID_ID > 255 you must change the data types!
#endif  // (TIMERS_MAX_CANT >= TIMER_INVALID_ID)

#if (TIMER_INVALID_ID > 255)
#error Change the data types for the Timers!
#endif  // (TIMER_INVALID_ID > 255)

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
//			ENUMERATIONS AND STRUCTURES AND TYPEDEFS			 //
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////

// Timer Modes
enum {TIM_MODE_SINGLESHOT, TIM_MODE_PERIODIC, CANT_TIM_MODES };

// Timer alias
typedef uint32_t ttick_t;
typedef uint8_t tim_id_t;
typedef void (*tim_callback_t)(void);

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//			VARIABLE PROTOTYPES WITH GLOBAL SCOPE  	  		    //
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//			FUNCTION PROTOTYPES WITH GLOBAL SCOPE				//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////


void timerInit(void);
/*****************************************************************
 * @brief Initialice timer and corresponding peripheral
 *****************************************************************/

tim_id_t timerGetId(void);
/*****************************************************************
 * @brief Request an timer
 * @return ID of the timer to use
 *****************************************************************/

void timerStart(tim_id_t id, ttick_t ticks, uint8_t mode, tim_callback_t callback);
/*****************************************************************
 * @brief Begin to run a new timer
 * @param id ID of the timer to start
 * @param ticks time until timer expires, in ticks
 * @param mode SINGLESHOT or PERIODIC
 * @param callback Function to be call when timer expires
 *****************************************************************/

void timerStop(tim_id_t id);
/*****************************************************************
 * @brief Finish to run a timer
 * @param id ID of the timer to stop
 *****************************************************************/

bool timerExpired(tim_id_t id);
/*****************************************************************
 * @brief Verify if a timer has run timeout
 * @param id ID of the timer to check for expiration
 * @return 1 = timer expired
 *****************************************************************/

void timerDelay(ttick_t ticks);
/*****************************************************************
 * @brief Wait the specified time. Use internal timer
 * @param ticks time to wait in ticks
 *****************************************************************/

bool timerRunning(tim_id_t id);
/*****************************************************************
 * @brief: Funciton to check if a timer is currently running.
 * @param id: ID of the timer to check.
 * @returns: true if the timer is running, false if it is not.
 *****************************************************************/

#endif // _TIMER_H_
