#include "dma.h"
#include "MK64F12.h"
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

void dma_set_config_channel(dma_conf_t config){
	if(config.dma_mux_conf.channel_number < DMA_AMOUNT_CHANNELS)
	{
		dma_mux_init(config.dma_mux_conf);

		DMA_Type * dma = DMA0;

		dma->SERQ |= DMA_SERQ_SERQ(config.dma_mux_conf.channel_number);
		dma_change_erq_flag(config.dma_mux_conf.channel_number, true);

		dma->TCD[config.dma_mux_conf.channel_number].SADDR = config.source_address;
		dma->TCD[config.dma_mux_conf.channel_number].DADDR = config.destination_address;

		dma->TCD[config.dma_mux_conf.channel_number].SOFF = config.source_offset;
		dma->TCD[config.dma_mux_conf.channel_number].DOFF = config.destination_offset;

		dma->TCD[config.dma_mux_conf.channel_number].ATTR = 0x00;
		dma->TCD[config.dma_mux_conf.channel_number].ATTR |= DMA_ATTR_SSIZE(config.source_data_transfer_size)| DMA_ATTR_DSIZE(config.destination_data_transfer_size);
	
		dma->TCD[config.dma_mux_conf.channel_number].NBYTES_MLNO = DMA_NBYTES_MLNO_NBYTES(config.nbytes);

		dma->TCD[config.dma_mux_conf.channel_number].CITER_ELINKNO = DMA_CITER_ELINKNO_CITER(config.citer);
		dma->TCD[config.dma_mux_conf.channel_number].BITER_ELINKNO = DMA_BITER_ELINKNO_BITER(config.citer);
		
        dma->TCD[config.dma_mux_conf.channel_number].SLAST = DMA_SLAST_SLAST(config.source_address_adjustment);
		dma->TCD[config.dma_mux_conf.channel_number].DLAST_SGA = DMA_DLAST_SGA_DLASTSGA(config.destination_address_adjustment);
		
		dma->TCD[config.dma_mux_conf.channel_number].CSR = 0X00;
		dma->TCD[config.dma_mux_conf.channel_number].CSR = DMA_CSR_BWC(0x00) |
				DMA_CSR_MAJORLINKCH(config.dma_mux_conf.channel_number) | DMA_CSR_MAJORELINK(0x00) | DMA_CSR_ESG(0x00) |
				DMA_CSR_DREQ(0x00) | DMA_CSR_INTHALF(0) | DMA_CSR_INTMAJOR(0) | DMA_CSR_START(0);

		dma->TCD[config.dma_mux_conf.channel_number].ATTR |= DMA_ATTR_SMOD(config.smod) | DMA_ATTR_DMOD(config.dmod);

		if (config.callback != NULL) {
			NVIC_ClearPendingIRQ(DMA0_IRQn);
			NVIC_EnableIRQ(DMA0_IRQn);//+config.dma_mux_conf.channel_number);
			callbacks[config.dma_mux_conf.channel_number] = config.callback;
		}

		if(config.major_loop_int_enable)
			dma->TCD[config.dma_mux_conf.channel_number].CSR = DMA_CSR_INTMAJOR_MASK;
			

			

	}
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

void dma0_enable_erq(){
	DMA0->ERQ = DMA_ERQ_ERQ0_MASK;
}


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//		                 Static functions   					//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

static void DMA_IRQHandler(uint8_t channel_number)
{
	DMA_Type * dma = DMA0;
	if (callbacks[channel_number] != NULL) {
		callbacks[channel_number]();
	}
	dma->CINT |= DMA_CINT_CINT(channel_number);
    return;
}

static void dma_mux_init(dma_mux_conf_t config){
	if(!(config.channel_number >= DMA_AMOUNT_CHANNELS)){
	    DMAMUX_Type* dma_mux = DMAMUX;
	    dma_mux->CHCFG[config.channel_number] = 0x00;
	    dma_mux->CHCFG[config.channel_number] = DMAMUX_CHCFG_ENBL(config.dma_enable) | DMAMUX_CHCFG_SOURCE(config.source);// | DMAMUX_CHCFG_TRIG(config.trigger_enable);
    }
    return;
}

static void dma_change_erq_flag(int channel_number, bool value){
	if(channel_number < DMA_AMOUNT_CHANNELS)
	{
		unsigned long newbit = value;
		uint32_t dma_channel_shifts[DMA_AMOUNT_CHANNELS] = {DMA_ERQ_ERQ0_SHIFT, DMA_ERQ_ERQ1_SHIFT, DMA_ERQ_ERQ2_SHIFT,
															DMA_ERQ_ERQ3_SHIFT, DMA_ERQ_ERQ4_SHIFT, DMA_ERQ_ERQ5_SHIFT,
															DMA_ERQ_ERQ6_SHIFT, DMA_ERQ_ERQ7_SHIFT, DMA_ERQ_ERQ8_SHIFT,
															DMA_ERQ_ERQ9_SHIFT, DMA_ERQ_ERQ10_SHIFT, DMA_ERQ_ERQ11_SHIFT,
															DMA_ERQ_ERQ12_SHIFT, DMA_ERQ_ERQ13_SHIFT, DMA_ERQ_ERQ14_SHIFT,
															DMA_ERQ_ERQ15_SHIFT };
		DMA0->ERQ ^= (-newbit ^ DMA0->ERQ) & dma_channel_shifts[channel_number];
	}
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

