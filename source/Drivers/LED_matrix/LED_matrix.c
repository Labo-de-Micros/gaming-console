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


#define CNV_ON 		39 	// 0.8us
#define CNV_OFF 	22 	// 0.46us
#define CNV_ZERO 	0
#define MOD 		62	// 1.26us

#define FTM_CH 0
#define DMA_CH 0

#define ROW_SIZE 8
#define COL_SIZE 8

#define CANT_LEDS ROW_SIZE*COL_SIZE

//							   bits por color / colores / 		CnV			
#define MAT_SIZE ( CANT_LEDS * sizeof(uint8_t)   * 3 *   sizeof(uint16_t))+(1*2)


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

static void led_m_set_pixel_cnv(uint16_t *ptr, uint8_t brightness);
static uint8_t ftm_to_source_id(FTM_t ftm, FTMChannel_t channel);
static void restart_cb(void);
static void end_of_major_cb(void);

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//		                 Static variables   					//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

static int8_t timerid;
static led_m_brightness_t led_m_brightness;
static GRB_t led_matrix [CANT_LEDS];
static led_m_color_t color_matrix [CANT_LEDS];

static led_m_color_t
White = {255,255,255},
Black = {0,0,0},
Red = {255,0,0},
Green = {0,255,0},
Blue = {0,0,255},
Brown = {0x1b,0x3c,0},
Gray = {192,192,192},
Yellow = {255,255,0},
Crimson = {80,0,0},
Purple = {153,0,255};

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//		                 External functions   					//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

void led_m_set_pixel_color_rgb(uint8_t r, uint8_t g, uint8_t b, uint8_t row, uint8_t col)
{
	led_m_color_t color={r, g, b};
	led_m_set_pixel_color(color, row, col);
	
	return;
}

void led_m_set_pixel_color(led_m_color_t color, uint8_t row, uint8_t col)
{
	color_matrix[ROW_SIZE*row+col]=color;

	led_m_set_pixel_cnv(led_matrix[ROW_SIZE*row+col].R, color.R/led_m_brightness);
		
	led_m_set_pixel_cnv(led_matrix[ROW_SIZE*row+col].G, color.G/led_m_brightness);
		
	led_m_set_pixel_cnv(led_matrix[ROW_SIZE*row+col].B, color.B/led_m_brightness);

	return;
}

void led_m_set_brightness(led_m_brightness_t brightness)
{
	led_m_brightness=brightness;

	return;
}

void led_m_set_all_to_color(led_m_color_t color)
{
	for (int col = 1; col < ROW_SIZE; col++)
        for (int row = 1; row < COL_SIZE; row++)
			led_m_set_pixel_color(color, row, col);

	return;
}

void led_m_init()
{
	static bool done_already = false;
	dma_TCD_t _tcd;
	
	if(!done_already){

		led_m_brightness=BRIGHT_5;
		led_m_set_all_to_color(Yellow);

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

		_tcd.CITER_ELINKNO = DMA_CITER_ELINKNO_CITER(MAT_SIZE/2);	
		_tcd.BITER_ELINKNO = DMA_BITER_ELINKNO_BITER(MAT_SIZE/2);

		_tcd.SLAST = -MAT_SIZE;

		_tcd.DLAST_SGA = 0x00;

		_tcd.CSR = DMA_CSR_INTMAJOR_MASK;

		dma_push_TCD_to_channel(DMA_CH, _tcd);

		DMA0->ERQ = DMA_ERQ_ERQ0_MASK;

		dma_assoc_callback_to_channel(DMA_CH, end_of_major_cb);

		// Timer config

		timerInit();
		timerid = timerGetId();
		
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
		
		led_m_set_pixel_brightness(led_matrix[1].R, 255);
		led_m_set_pixel_brightness(led_matrix[1].G, 255);
		led_m_set_pixel_brightness(led_matrix[1].B, 255);
		
		done_already = true;
    }

	return;
}

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//		                 Static functions   					//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

static void led_m_set_pixel_cnv(uint16_t *ptr, uint8_t brightness)
{
	uint8_t index;
	
	for (index = 0; index<8; index++)
		ptr[index] = (brightness & (1<<index)) ? CNV_ON : CNV_OFF;
	
	return;
}

static void restart_cb(void)
{
	if(led_matrix[0].G[0] == CNV_ON)
		FTM_SetCounter (FTM0, 0, CNV_ON);
	else
		FTM_SetCounter (FTM0, 0, CNV_OFF);
	
	FTM_StartClock(FTM0);

	return;
}

static void end_of_major_cb(void)
{
	FTM_StopClock(FTM0);

	timerStart(timerid, 2, TIM_MODE_SINGLESHOT, restart_cb);

	return;
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
