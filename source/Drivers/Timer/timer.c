//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//	@file		timer.h											//
//	@brief		Timer driver. Advance Implementation			//
//	@author		Grupo 4											//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//							Headers								//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

#include "./timer.h"
#include "../SysTick/SysTick.h"

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//		CONSTANT AND MACRO DEFINITIONS USING #DEFINE 			//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

#if TIMER_TICK_MS != (1000U/SYSTICK_ISR_FREQUENCY_HZ)
#error Las frecuencias no coinciden!!
#endif // TIMER_TICK_MS != (1000U/SYSTICK_ISR_FREQUENCY_HZ)

#define TIMER_DEVELOPMENT_MODE    0
#define TIMER_ID_INTERNAL   0


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//			ENUMERATIONS AND STRUCTURES AND TYPEDEFS	  		//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

typedef struct {
	ttick_t             period;
	ttick_t             cnt;
	tim_callback_t      callback;
	uint8_t             mode        : 1;
	uint8_t             running     : 1;
	uint8_t             expired     : 1;
	uint8_t             unused      : 5;
} timer_t;

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//					VARIABLE PROTOTYPES							//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

static timer_t timers[TIMERS_MAX_CANT];
static tim_id_t timers_cant = TIMER_ID_INTERNAL+1;

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS W FILE LEVEL SCOPE//
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

static void timer_isr(void);

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//					FUNCTION DEFINITIONS						//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

void timerInit(void){
/*****************************************************************
 * @brief Initialize timer and corresponding peripheral
 *****************************************************************/
    static bool yaInit = false;
    if (yaInit)
        return;
    SysTick_Init(timer_isr); // init peripheral
    yaInit = true;
		return;
}

tim_id_t timerGetId(void){
/*****************************************************************
 * @brief Request an timer
 * @return ID of the timer to use
 *****************************************************************/
#ifdef TIMER_DEVELOPMENT_MODE
    if (timers_cant >= TIMERS_MAX_CANT)
        return TIMER_INVALID_ID;
    else
#endif // TIMER_DEVELOPMENT_MODE
    {
        return timers_cant++;
    }
}

void timerStart(tim_id_t id, ttick_t ticks, uint8_t mode, tim_callback_t callback){
/*****************************************************************
 * @brief Begin to run a new timer
 * @param id ID of the timer to start
 * @param ticks time until timer expires, in ticks
 * @param mode SINGLESHOT or PERIODIC
 * @param callback Function to be call when timer expires
 *****************************************************************/
#ifdef TIMER_DEVELOPMENT_MODE
    if ((id < timers_cant) && (mode < CANT_TIM_MODES))
#endif // TIMER_DEVELOPMENT_MODE
    {
				timers[id].running = false;
				timers[id].period = ticks;
				timers[id].cnt = 0;
				timers[id].mode = mode;
				timers[id].expired = false;
				timers[id].callback = callback;
				timers[id].running = true;
    }
		return;
}

void timerStop(tim_id_t id){
/*****************************************************************
 * @brief Finish to run a timer
 * @param id ID of the timer to stop
 *****************************************************************/
	timers[id].running = false;
	timers[id].period = 0;
	timers[id].cnt = 0;
	timers[id].expired = true;
	timers[id].callback = NULL;
	return;
}

bool timerExpired(tim_id_t id){
/*****************************************************************
 * @brief Verify if a timer has run timeout
 * @param id ID of the timer to check for expiration
 * @return 1 = timer expired
 *****************************************************************/
	if(timers[id].mode == TIM_MODE_SINGLESHOT && timers[id].expired)
		return true;
	else
		return false;
}

bool timerRunning(tim_id_t id){
/*****************************************************************
 * @brief: Funciton to check if a timer is currently running.
 * @param id: ID of the timer to check.
 * @returns: true if the timer is running, false if it is not.
 *****************************************************************/
	return timers[id].running;
}

void timerDelay(ttick_t ticks){
/*****************************************************************
 * @brief Wait the specified time. Use internal timer
 * @param ticks time to wait in ticks
 *****************************************************************/
    timerStart(TIMER_ID_INTERNAL, ticks, TIM_MODE_SINGLESHOT, NULL);
    while (!timerExpired(TIMER_ID_INTERNAL)){;}
		return;
}

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//					LOCAL FUNCTION DEFINITIONS					//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

static void timer_isr(void){
/*****************************************************************
 * @brief: Systick ISR function that runs at every tick and does the
 * 			logic for every timer started on this driver.
 *****************************************************************/
	for(tim_id_t id = 0 ; id <= timers_cant ; id++){
	// Does the logic for every timer activated.
		if(timers[id].running && timers[id].expired == false){
		//Check if the timer is currently running to perform the logic.
			if(timers[id].cnt < timers[id].period)
			//If the timer doesnt expired, increase its counter
				timers[id].cnt += 1;
			else if(timers[id].mode == TIM_MODE_PERIODIC && timers[id].cnt >= timers[id].period){
			//If the timer counter is superior to its period, call the callback and set the counter on 0.
				timers[id].cnt = 0;
				timers[id].expired = true;
				if(timers[id].callback != NULL)
					timers[id].callback();
				timers[id].expired = false;
			}
			else if(timers[id].mode == TIM_MODE_SINGLESHOT && timers[id].cnt >= timers[id].period){
			//If the timer counter is superior to its period, call the callback, set the counter on 0 and deactivate the timer.
				timers[id].expired = true;
				if(timers[id].callback != NULL)
					timers[id].callback();
				timers[id].running = false;
			}
		}
	}
	return;
}
