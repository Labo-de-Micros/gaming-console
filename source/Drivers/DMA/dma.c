#include "dma.h"
#include "MK64F12.h"
#include "../FTM/ftm.h"
#include <stdlib.h>



//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//		                   Definitions	    					//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

#define DMA_AMOUNT_CHANNELS	16


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//		             Static function headers 					//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

static void dma_mux_init(dma_mux_conf_t config);
static void dma_change_erq_flag(int channel_number, bool value);
static void DMA_IRQHandler(uint8_t channel_number);


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//		                 Static variables   					//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

static dma_callback_t callbacks[DMA_AMOUNT_CHANNELS];


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//		                 External functions   					//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

void dma_init(){

	static bool done_already = false;

	if(!done_already){

	SIM_Type* sim = SIM;
	sim->SCGC6 |= SIM_SCGC6_DMAMUX_MASK;
	sim->SCGC7 |= SIM_SCGC7_DMA_MASK;

	for (unsigned int i = 0; i < DMA_AMOUNT_CHANNELS; i++) callbacks[i] = NULL;

	done_already = true;

    }

    return;
}


void dma_push_TCD_to_channel(uint8_t channel, dma_TCD_t tcd){
	memcpy(&(DMA0->TCD[channel]), &(tcd), sizeof(dma_TCD_t));
	return;
}

void dma_assoc_callback_to_channel(uint8_t channel, dma_callback_t callback){
	callbacks[channel]=callback;
	return;
}

bool dma_get_finished_transfer(int channel_number)
{
	bool finished = true;
	if(channel_number < DMA_AMOUNT_CHANNELS)
		finished = (DMA0->TCD[channel_number].CSR) & DMA_CSR_DONE_MASK;
	return finished;
}

void dma_enable_major_loop_irq(uint8_t channel_number, bool ie)
{
	DMA_Type * dma = DMA0;
	if(ie)
		dma->TCD[channel_number].CSR |= DMA_CSR_INTMAJOR_MASK;
	else
		dma->TCD[channel_number].CSR &= ~DMA_CSR_INTMAJOR_MASK;
    return;
}



//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//		                 Static functions   					//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

static void DMA_IRQHandler(uint8_t channel_number)
{
	DMA0->CINT |= 0;
	DMA_Type * dma = DMA0;
	if (callbacks[channel_number] != NULL) {
		callbacks[channel_number]();
	}
	dma->CINT |= DMA_CINT_CINT(channel_number);
    return;
}

void DMA0_IRQHandler(void)
{
	DMA_IRQHandler(0);
}

void DMA1_IRQHandler(void)
{
	DMA_IRQHandler(1);
}

void DMA2_IRQHandler(void)
{
	DMA_IRQHandler(2);
}

void DMA3_IRQHandler(void)
{
	DMA_IRQHandler(3);
}

void DMA4_IRQHandler(void)
{
	DMA_IRQHandler(4);
}

void DMA5_IRQHandler(void)
{
	DMA_IRQHandler(5);
}

void DMA6_IRQHandler(void)
{
	DMA_IRQHandler(6);
}

void DMA7_IRQHandler(void)
{
	DMA_IRQHandler(7);
}

void DMA8_IRQHandler(void)
{
	DMA_IRQHandler(8);
}

void DMA9_IRQHandler(void)
{
	DMA_IRQHandler(9);
}

void DMA10_IRQHandler(void)
{
	DMA_IRQHandler(10);
}

void DMA11_IRQHandler(void)
{
	DMA_IRQHandler(11);
}

void DMA12_IRQHandler(void)
{
	DMA_IRQHandler(12);
}

void DMA13_IRQHandler(void)
{
	DMA_IRQHandler(13);
}

void DMA14_IRQHandler(void)
{
	DMA_IRQHandler(14);
}

void DMA15_IRQHandler(void)
{
	DMA_IRQHandler(15);
}

static uint8_t _ftm_to_source_id(FTM_t ftm, FTMChannel_t channel)
{
  uint8_t ret = 20;
  
  switch (ftm)
  {
    case FTM_INSTANCE_0:  ret += channel; break;
    case FTM_INSTANCE_1:  ret += 8 + channel; break;
    case FTM_INSTANCE_2:  ret += 10 + channel; break;
    case FTM_INSTANCE_3:  ret += 12 + channel; break;
    default: break; 
  }
  
  return ret;
}