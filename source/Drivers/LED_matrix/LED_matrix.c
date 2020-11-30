#include "LED_matrix.h"

#include "../DMA/dma.h"
#include "../FTM/ftm.h"
#include "../Timer/timer.h"

#define CNV_ON 39 //39 ticks -> 0.8us
#define CNV_OFF 22 //22 ticks -> 0.46us
#define CNV_ZERO 0
#define MOD 62//62ticks ->1.26us

#define CANT_LEDS 64
#define CANT_LEDS_ZERO 0

#define MAT_SIZE ((CANT_LEDS+CANT_LEDS_ZERO)*8*3*2)+(1*2) //(64 LEDS+10LEDS en zero para reset) * 8BITS * 3 COLORES * 2bytes (CNV son 16 bits)
#define ROW_SIZE 8

typedef void (*Led_m_callback_t)(void);

typedef enum {RED, GREEN, BLUE} led_color;

typedef struct
{
	uint16_t G[8]; //Un array de 8 elementos de 16 bits.
	uint16_t R[8];
	uint16_t B[8];
}GRB_t;


static int8_t timerid;
static GRB_t led_matrix [CANT_LEDS+CANT_LEDS_ZERO];

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
	uint16_t* matrix_ptr=(uint16_t*)(&led_matrix);
	uint32_t matrix_size=MAT_SIZE;
	Led_m_callback_t _callback=dma_cb;

    dma_init();
    dma_conf_t config;
    
	timerInit();
	timerid = timerGetId();
	timerStart(timerid, 2, TIM_MODE_SINGLESHOT, tim_cb);
	timerStop(timerid);

    config.dma_mux_conf.channel_number=0;
	config.dma_mux_conf.dma_enable=true;
	config.dma_mux_conf.trigger_enable=true; // doubt
	config.dma_mux_conf.source=20;

	/// ============= INIT TCD0 ===================//
	/* Set memory address for source and destination. */
	config.source_address = (uint32_t)(matrix_ptr);				   //List of Duties

	//DMA_TCD0_DADDR = (uint32_t)(destinationBuffer);
	config.destination_address = (uint32_t)(&(FTM0->CONTROLS[0].CnV));  // To change FTM Duty

    /* Set an offset for source and destination address. */
	config.source_offset =0x02; // Source address offset of 2 bytes per transaction.
	config.destination_offset =0x00; // Destination address offset is 0. (Siempre al mismo lugar)

	/* Set source and destination data transfer size to 16 bits (CnV is 2 bytes wide). */
	config.source_data_transfer_size=1;
    config.destination_data_transfer_size=1;

	/*Number of bytes to be transfered in each service request of the channel.*/
	config.nbytes = 0x02;

	/* Current major iteration count (5 iteration of 1 byte each one). */

	//DMA_TCD0_CITER_ELINKNO = DMA_CITER_ELINKNO_CITER(0x05);
	//DMA_TCD0_BITER_ELINKNO = DMA_BITER_ELINKNO_BITER(0x05);

	/* Autosize for Current major iteration count */

	config.citer=matrix_size/2;	// div 2 //(sizeof(sourceBuffer)/sizeof(sourceBuffer[0]))
	
	//DMA_TCD0_SLAST = 0x00;
	//DMA_TCD0_SLAST = -5*sizeof(uint16_t);
	//DMA_TCD0_SLAST = -((sizeof(sourceBuffer)/sizeof(sourceBuffer[0])*sizeof(uint16_t)));

	/* Autosize SLAST for Wrap Around. This value is added to SADD at the end of Major Loop */
	config.source_address_adjustment = -matrix_size;

	/* DLASTSGA DLAST Scatter and Gatter */
	config.destination_address_adjustment = 0x00;

	/* Setup control and status register. */
	config.major_loop_int_enable=true;	//Enable Major Interrupt.

    config.callback=_callback;

	dma_set_config_channel(config);

	FTM_Init(FTM0);
	FTM_SetModulus(FTM0,MOD);
	FTM_SetPulseWidthModulationLogic(FTM0,0,FTM_lAssertedHigh);
	FTM_SetCounter (FTM0,0,CNV_OFF);
	FTM_SetWorkingMode (FTM0, 0, FTM_mPulseWidthModulation);
	FTM_SetPSC(FTM0, FTM_PSC_x1);

	FTM_StartClock(FTM0);

	return;
}
