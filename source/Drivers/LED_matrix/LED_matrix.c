#include "LED_matrix.h"
#include "MK64F12.h"
#include "../DMA/dma.h"
#include "../FTM/ftm.h"
#include "../Timer/timer.h"


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//		CONSTANT AND MACRO DEFINITIONS USING #DEFINE 		 	//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////


#define CNV_ON 		39 	//39 ticks -> 0.8us
#define CNV_OFF 	22 	//22 ticks -> 0.46us
#define CNV_ZERO 	0
#define MOD 		62	//62ticks ->1.26us

#define FTM_CH 0
#define DMA_CH 0

#define CANT_LEDS 64
#define CANT_LEDS_ZERO 0

#define MAT_SIZE ((CANT_LEDS+CANT_LEDS_ZERO)*8*3*2)+(1*2) //(64 LEDS+10LEDS en zero para reset) * 8BITS * 3 COLORES * 2bytes (CNV son 16 bits)
#define ROW_SIZE 8

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//			ENUMERATIONS AND STRUCTURES AND TYPEDEFS	  		//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

typedef void (*Led_m_callback_t)(void);

typedef enum {RED, GREEN, BLUE} led_color;

typedef struct
{
	uint16_t G[8]; 
	uint16_t R[8];
	uint16_t B[8];
}GRB_t;

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//		             Static function headers 					//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

static uint8_t ftm_to_source_id(FTM_t ftm, FTMChannel_t channel);
static void tim_cb(void);
static void dma_cb(void);

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//		                 Static variables   					//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

static int8_t timerid;
static GRB_t led_matrix [CANT_LEDS+CANT_LEDS_ZERO];

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//		                 External functions   					//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////


void led_m_set_pixel_brightness(uint16_t *ptr, uint8_t brightness)
{
	uint8_t i;
	for (i = 0; i<8; i++){
		ptr[i] = (brightness & (1<<i)) ? CNV_ON : CNV_OFF;
	}
	return;
}

void led_m_set_pixel(uint8_t color, uint8_t brightness, uint8_t row, uint8_t col)
{
	switch(color)
	{
	case GREEN:
		led_m_set_pixel_brightness(led_matrix[ROW_SIZE*row+col].G, brightness);
		break;
	case RED:
		led_m_set_pixel_brightness(led_matrix[ROW_SIZE*row+col].R, brightness);
		break;
	case BLUE:
		led_m_set_pixel_brightness(led_matrix[ROW_SIZE*row+col].B, brightness);
		break;
	default: 
		break;
	}
	return;
}

void led_m_init()
{
	static bool done_already = false;
	dma_TCD_t _tcd;
	
	if(!done_already){

		uint8_t i;

		for(i = 0; i < CANT_LEDS+CANT_LEDS_ZERO; i++)
		{
			if(i < CANT_LEDS)
			{
				led_m_set_pixel_brightness(led_matrix[i].R, 255);
				led_m_set_pixel_brightness(led_matrix[i].G, 255);
				led_m_set_pixel_brightness(led_matrix[i].B, 255);
			}
		}

		// DMA config

		dma_init();

		DMAMUX->CHCFG[DMA_CH] = DMAMUX_CHCFG_ENBL(1) | DMAMUX_CHCFG_TRIG(0) | DMAMUX_CHCFG_SOURCE(ftm_to_source_id(FTM0, FTM_CH));

		NVIC_ClearPendingIRQ(DMA0_IRQn);
		NVIC_EnableIRQ(DMA0_IRQn);

		_tcd.SADDR = (uint32_t)((uint16_t*)(&led_matrix));
		_tcd.DADDR = (uint32_t)(&(FTM0->CONTROLS[FTM_CH].CnV));

		_tcd.SOFF =0x02;
		_tcd.DOFF =0x00;

		_tcd.ATTR = DMA_ATTR_SSIZE(1) | DMA_ATTR_DSIZE(1);

		_tcd.NBYTES_MLNO = 0x02;

		_tcd.CITER_ELINKNO = DMA_CITER_ELINKNO_CITER(MAT_SIZE/2);	// div 2 //(sizeof(sourceBuffer)/sizeof(sourceBuffer[0]))
		_tcd.BITER_ELINKNO = DMA_BITER_ELINKNO_BITER(MAT_SIZE/2);  // div 2

		_tcd.SLAST = -MAT_SIZE;

		_tcd.DLAST_SGA = 0x00;

		_tcd.CSR = DMA_CSR_INTMAJOR_MASK;

		dma_push_TCD_to_channel(DMA_CH, _tcd);

		DMA0->ERQ = DMA_ERQ_ERQ0_MASK;

		dma_assoc_callback_to_channel(DMA_CH, dma_cb);

		led_m_set_pixel_brightness(led_matrix[1].R, 255);
		led_m_set_pixel_brightness(led_matrix[1].G, 255);
		led_m_set_pixel_brightness(led_matrix[1].B, 255);

		timerInit();
		timerid = timerGetId();
		timerStart(timerid, 2, TIM_MODE_SINGLESHOT, tim_cb);
		timerStop(timerid);

		// Port config

		SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
		SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
		SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
		SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
		SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
	
		PORTA->ISFR = PORT_ISFR_ISF_MASK;
		PORTB->ISFR = PORT_ISFR_ISF_MASK;
		PORTC->ISFR = PORT_ISFR_ISF_MASK;
		PORTD->ISFR = PORT_ISFR_ISF_MASK;
		PORTE->ISFR = PORT_ISFR_ISF_MASK;
	
		NVIC_EnableIRQ(PORTA_IRQn);
		NVIC_EnableIRQ(PORTB_IRQn);
		NVIC_EnableIRQ(PORTC_IRQn);
		NVIC_EnableIRQ(PORTD_IRQn);
		NVIC_EnableIRQ(PORTE_IRQn);
		
		// FTM config

		FTM_Init(FTM0);
		FTM_SetModulus(FTM0,MOD);

		FTM_SetWorkingMode (FTM0, 0, FTM_mPulseWidthModulation);
		FTM_SetPulseWidthModulationLogic(FTM0,0,FTM_lAssertedHigh);
		FTM_SetInterruptMode (FTM0, 0, 1);
		FTM_SetDMA(FTM0, 0, 1);
		FTM_SetCounter (FTM0,0,CNV_OFF);
		FTM_SetPSC(FTM0, FTM_PSC_x1);

		FTM_StartClock(FTM0);
		
		done_already = true;
    }
	return;
}


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//		                 Static functions   					//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////


static void tim_cb(void)
{
	if(led_matrix[0].G[0] == CNV_ON)
		FTM_SetCounter (FTM0, 0, CNV_ON);
	else
		FTM_SetCounter (FTM0, 0, CNV_OFF);
	FTM_StartClock(FTM0);
}

static void dma_cb(void)
{
	FTM_StopClock(FTM0);
	timerStart(timerid, 2, TIM_MODE_SINGLESHOT, tim_cb);
}

static uint8_t ftm_to_source_id(FTM_t ftm, FTMChannel_t channel)
{
 	uint8_t ret = 20;

	if(ftm==FTM0)  ret += channel;
	if(ftm==FTM1)  ret += 8 + channel;
	if(ftm==FTM2)  ret += 10 + channel;
	if(ftm==FTM3)  ret += 12 + channel;

  	return ret;
}
