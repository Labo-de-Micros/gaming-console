<<<<<<< HEAD
#include "FTM.h"
#include "../GPIO/gpio.h"

void OVF_Init(void);
void OVF_ISR(void);
=======
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
//	@file		FTM.c    									   //
//	@brief		FTM driver. Advance Implementation		       //
//	@author		Grupo 4 									   //
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//							Headers								//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

#include "FTM.h"
#include "../GPIO/gpio.h"
#include "../../board.h"

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//		CONSTANT AND MACRO DEFINITIONS USING #DEFINE 			//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//			ENUMERATIONS AND STRUCTURES AND TYPEDEFS	  		//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//						STATIC VARIABLES						//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

//uint16_t PWM_modulus = 1000-1;
//uint16_t PWM_duty    = 300;//5000-1;
static PWM_callback_t PWM_ISR;

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS W FILE LEVEL SCOPE//
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//					FUNCTION DEFINITIONS						//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
>>>>>>> 27f5ad44e42e788cc92d7514b27678730d2fdccb

/* FTM0 fault, overflow and channels interrupt handler*/
__ISR__ FTM0_IRQHandler(void)
{
	PWM_ISR();
}

// void PWM_ISR (void)
// {

// 	callBack();
// 	//FTM_ClearOverflowFlag (FTM0);
// 	//FTM_ClearInterruptFlag(FTM0,FTM_CH_0);
// 	//gpioToggle(TEST);
// 	//set_DutyPWM(FTM0, 0, percent);
// 	//percent +=10;
// 	//percent= percent%100;
// 	//FTM_SetCounter(FTM0, 0, PWM_duty++);  //change DC
// 	//GPIO_Toggle(PTC, 1 << 8);			  //GPIO pin PTC8
// 	//PWM_duty %= PWM_modulus;
// }


void FTM_Init(FTM_t ftm, PWM_callback_t PWM_callback){
	if(ftm == FTM0){
		SIM->SCGC6 |= SIM_SCGC6_FTM0_MASK;
		NVIC_EnableIRQ(FTM0_IRQn);
		FTM0->PWMLOAD = FTM_PWMLOAD_LDOK_MASK | 0x0F;
	}
	else if(ftm == FTM1){
		SIM->SCGC6 |= SIM_SCGC6_FTM1_MASK;
		NVIC_EnableIRQ(FTM1_IRQn);
		FTM1->PWMLOAD = FTM_PWMLOAD_LDOK_MASK | 0x0F;
	}
	else if (ftm == FTM2){
		SIM->SCGC6 |= SIM_SCGC6_FTM2_MASK;
		SIM->SCGC3 |= SIM_SCGC3_FTM2_MASK;
		NVIC_EnableIRQ(FTM2_IRQn);
		FTM2->PWMLOAD = FTM_PWMLOAD_LDOK_MASK | 0x0F;
	}
	else if (ftm == FTM3){
		SIM->SCGC3 |= SIM_SCGC3_FTM3_MASK;
		NVIC_EnableIRQ(FTM3_IRQn);
		FTM3->PWMLOAD = FTM_PWMLOAD_LDOK_MASK | 0x0F;
	}
	//PWM_Init(10000-1,FTM_PSC_x32,70);
	PWM_ISR = PWM_callback;
	return;
}


// void PWM_Init (uint16_t modulus, FTM_Prescal_t prescaler, uint16_t duty)
// {

// 	PWM_modulus=modulus;
// 	PWM_duty=duty;
// 	//seteo los contadores

<<<<<<< HEAD
void FTM_Init (void)
{
	SIM->SCGC6 |= SIM_SCGC6_FTM0_MASK;
	SIM->SCGC6 |= SIM_SCGC6_FTM1_MASK;
	SIM->SCGC6 |= SIM_SCGC6_FTM2_MASK;
	SIM->SCGC3 |= SIM_SCGC3_FTM2_MASK;
	SIM->SCGC3 |= SIM_SCGC3_FTM3_MASK;
=======
// 	FTM0->CNTIN = 0X00;
// 	FTM0->MOD = FTM_MOD_MOD(PWM_modulus);

// 	PORTC->PCR[PIN2NUM(PORTNUM2PIN(PC,1))]=0x00;
// 	PORTC->PCR[PIN2NUM(PORTNUM2PIN(PC,1))] |= PORT_PCR_DSE(1);
// 	PORTC->PCR[PIN2NUM(PORTNUM2PIN(PC,1))] |= PORT_PCR_MUX(4);
// 	PORTC->PCR[PIN2NUM(PORTNUM2PIN(PC,1))] |= PORT_PCR_IRQC(0);

// 	//	PORT_Configure2 (PORTC,1,UserPCR);

// 	FTM0->CNT = 0X00;
>>>>>>> 27f5ad44e42e788cc92d7514b27678730d2fdccb


// 	FTM_SetPrescaler(FTM0, prescaler);
// 	FTM_SetModulus(FTM0, PWM_modulus);
// 	FTM_SetOverflowMode(FTM0, true);
// 	//FTM_SetInterruptMode (FTM0,FTM_CH_0, true);
// 	FTM_SetWorkingMode(FTM0, 0, FTM_mPulseWidthModulation);			// MSA  / B
// 	FTM_SetPulseWidthModulationLogic(FTM0, 0, FTM_lAssertedHigh);   // ELSA / B
// 	set_DutyPWM(FTM0, 0, PWM_duty);
// 	//FTM_SetCounter(FTM0, 0, PWM_duty);
// 	//FTM_StartClock(FTM0);
// }


// funciones de timer utilizando pwm
// void pwm_start_timer(uint16_t ticks,uint16_t duty_cycle,FTM_callback_t callback) //tener en cuenta que el tiempo se obtiene como ticks*preescaler/Sysclock == T
// {
// 	FTM_StopClock(FTM0);
// 	FTM_ClearOverflowFlag(FTM0);
// 	FTM_SetModulus(FTM0,ticks);
// 	set_DutyPWM(FTM0,0,duty_cycle);

// 	FTM_StartClock(FTM0);

// }




// Setters

void FTM_SetPrescaler (FTM_t ftm, FTM_Prescal_t data)
{
	ftm->SC = (ftm->SC & ~FTM_SC_PS_MASK) | FTM_SC_PS(data);
}

void FTM_SetModulus (FTM_t ftm, FTMData_t data)
{
	ftm->CNTIN = 0X00;
	ftm->CNT = 0X00;
	ftm->MOD = FTM_MOD_MOD(data);
}

FTMData_t FTM_GetModulus (FTM_t ftm)
{
	return ftm->MOD & FTM_MOD_MOD_MASK;
}

void FTM_StartClock (FTM_t ftm)
{
	ftm->SC |= FTM_SC_CLKS(0x01);
}

void FTM_StopClock (FTM_t ftm)
{
	ftm->SC &= ~FTM_SC_CLKS(0x01);
}

void FTM_SetOverflowMode (FTM_t ftm, bool mode)
{
	ftm->SC = (ftm->SC & ~FTM_SC_TOIE_MASK) | FTM_SC_TOIE(mode);
}

bool FTM_IsOverflowPending (FTM_t ftm)
{
	return ftm->SC & FTM_SC_TOF_MASK;
}

void FTM_ClearOverflowFlag (FTM_t ftm)
{
	ftm->SC &= ~FTM_SC_TOF_MASK;
}

void FTM_SetWorkingMode (FTM_t ftm, FTMChannel_t channel, FTMMode_t mode)
{
	ftm->CONTROLS[channel].CnSC = (ftm->CONTROLS[channel].CnSC & ~(FTM_CnSC_MSB_MASK | FTM_CnSC_MSA_MASK)) |
			                      (FTM_CnSC_MSB((mode >> 1) & 0X01) | FTM_CnSC_MSA((mode >> 0) & 0X01));
}

FTMMode_t FTM_GetWorkingMode (FTM_t ftm, FTMChannel_t channel)
{
	return (ftm->CONTROLS[channel].CnSC & (FTM_CnSC_MSB_MASK | FTM_CnSC_MSA_MASK)) >> FTM_CnSC_MSA_SHIFT;
}

void FTM_SetInputCaptureEdge (FTM_t ftm, FTMChannel_t channel, FTMEdge_t edge)
{
	ftm->CONTROLS[channel].CnSC = (ftm->CONTROLS[channel].CnSC & ~(FTM_CnSC_ELSB_MASK | FTM_CnSC_ELSA_MASK)) |
				                  (FTM_CnSC_ELSB((edge >> 1) & 0X01) | FTM_CnSC_ELSA((edge >> 0) & 0X01));
}

FTMEdge_t FTM_GetInputCaptureEdge (FTM_t ftm, FTMChannel_t channel)
{
	return (ftm->CONTROLS[channel].CnSC & (FTM_CnSC_ELSB_MASK | FTM_CnSC_ELSA_MASK)) >> FTM_CnSC_ELSA_SHIFT;
}

void FTM_SetOutputCompareEffect (FTM_t ftm, FTMChannel_t channel, FTMEffect_t effect)
{
	ftm->CONTROLS[channel].CnSC = (ftm->CONTROLS[channel].CnSC & ~(FTM_CnSC_ELSB_MASK | FTM_CnSC_ELSA_MASK)) |
				                  (FTM_CnSC_ELSB((effect >> 1) & 0X01) | FTM_CnSC_ELSA((effect >> 0) & 0X01));
}

FTMEffect_t FTM_GetOutputCompareEffect (FTM_t ftm, FTMChannel_t channel)
{
	return (ftm->CONTROLS[channel].CnSC & (FTM_CnSC_ELSB_MASK | FTM_CnSC_ELSA_MASK)) >> FTM_CnSC_ELSA_SHIFT;
}

void FTM_SetPulseWidthModulationLogic (FTM_t ftm, FTMChannel_t channel, FTMLogic_t logic)
{
	ftm->CONTROLS[channel].CnSC = (ftm->CONTROLS[channel].CnSC & ~(FTM_CnSC_ELSB_MASK | FTM_CnSC_ELSA_MASK)) |
				                  (FTM_CnSC_ELSB((logic >> 1) & 0X01) | FTM_CnSC_ELSA((logic >> 0) & 0X01));
}

FTMLogic_t FTM_GetPulseWidthModulationLogic (FTM_t ftm, FTMChannel_t channel)
{
	return (ftm->CONTROLS[channel].CnSC & (FTM_CnSC_ELSB_MASK | FTM_CnSC_ELSA_MASK)) >> FTM_CnSC_ELSA_SHIFT;
}

void FTM_SetCounter (FTM_t ftm, FTMChannel_t channel, FTMData_t data)
{
	ftm->CONTROLS[channel].CnV = FTM_CnV_VAL(data);
}

FTMData_t FTM_GetCounter (FTM_t ftm, FTMChannel_t channel)
{
	return ftm->CONTROLS[channel].CnV & FTM_CnV_VAL_MASK;
}

void FTM_SetInterruptMode (FTM_t ftm, FTMChannel_t channel, bool mode)
{
	ftm->CONTROLS[channel].CnSC = (ftm->CONTROLS[channel].CnSC & ~FTM_CnSC_CHIE_MASK) | FTM_CnSC_CHIE(mode);
}

bool FTM_IsInterruptPending (FTM_t ftm, FTMChannel_t channel)
{
	return ftm->CONTROLS[channel].CnSC & FTM_CnSC_CHF_MASK;
}

void FTM_ClearInterruptFlag (FTM_t ftm, FTMChannel_t channel)
{
	ftm->CONTROLS[channel].CnSC &= ~FTM_CnSC_CHF_MASK;
}

// void set_DutyPWM(FTM_t ftm,FTMChannel_t Chn, uint16_t  percent)
// {
// 	//double duty_per=(percent/100.0)*(PWM_modulus+1);
// 	PWM_duty=percent*(PWM_modulus+1)/100;
// 	FTM_SetCounter(ftm, Chn, PWM_duty);
// }





