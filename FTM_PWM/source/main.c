/*
 * main.c
 *
 *  Created on: May 1, 2015--
 *      Author: Juan Pablo VEGA - Laboratorio de Microprocesadores
 */

#include "hardware.h"
//#include "PORT.h"
//#include "GPIO.h"
#include "FTM.h"


#define __FOREVER__ 	for(;;)



int main (void)
{


 	 	 	 	hw_Init ();
 	 	 	 	PORT_Init();
 	 	 		FTM_Init(FTM0);


 	 	// 		hw_DisableInterrupts();

 	 	 		__FOREVER__;

			// Enable interrupts
			//hw_EnableInterrupts();






}

