/*
 * bls.c
 *
 *  Created on: 12/12/2016
 *      Author: david.villafaina
 */


#include "xparameters.h"
#include "xgpio.h"
#include "board_configuration.h"
#include "string.h"
#include "display.h"
#include "time.h"
#include "sys/time.h"

/* Only needed for SDK versions before 2015
 *#include "xutil.h"
 *====================================================
 */

int read_button(void)
{
	/* Pointer and variable declaration */
	XGpio buttons;
	int psb_check = 0;

	/* Pointer initialization */
	/* Check out your own XPAR ID symbol name declared in xparameters.h */
	/* The format is always XPAR_<NAME_IN_VIVADO>_DEVICE_ID */
	XGpio_Initialize(&buttons, XPAR_BUTTONS_DEVICE_ID);
	XGpio_SetDataDirection(&buttons, 1, 0xffffffff);

	/* Read value of buttons */
	psb_check = XGpio_DiscreteRead(&buttons, 1);
	//xil_printf("Button pushed: %x\r\n", psb_check);
	return psb_check;
}

int read_switches (void)
{

    /* Pointer and variable declaration */
    XGpio switches;
	int switches_check = 0;

   /* Pointer initialization */
   /* Check out your own XPAR ID symbol name declared in xparameters.h */
   /* The format is always XPAR_<NAME_IN_VIVADO>_DEVICE_ID */
   XGpio_Initialize(&switches, XPAR_SWITCHES_DEVICE_ID);
   XGpio_SetDataDirection(&switches, 1, 0xffffffff);

   /* Read value of switches */
   switches_check = XGpio_DiscreteRead(&switches, 1);
   //xil_printf("Switch moved: %x\r\n", switches_check);
   return switches_check;
}

void play_game(int recv_seq[])
{

   /* Pointer and variable declaration */
   XGpio leds;
   int i, led_value;
//   int  myTime = 0;
   /* Pointer initialization */
   /* Check out your own XPAR ID symbol name declared in xparameters.h */
   /* The format is always XPAR_<NAME_IN_VIVADO>_DEVICE_ID */
   XGpio_Initialize(&leds, XPAR_LEDS_DEVICE_ID);
   XGpio_SetDataDirection(&leds, 1, 0x00000000);

   /* Write value of sequence to leds */
   xil_printf("\r\n\r\n");
   xil_printf("PLAYING...");
   xil_printf("\r\n\r\n");
   update_leds2go(0);
   for(i = 0; i<20; i++){
	   led_value = sequence_to_led_mapping(recv_seq[i]);
	   XGpio_DiscreteWrite(&leds, 1, led_value); /* Leds associated with the sequence of 20 integers */
	   while(read_switches() != led_value);
//	   { /* Check the switch moved is the one associated to that led */
//		   myTime = myTime + 1;
//	   }
	   update_leds2go(i+1);
   }
   Xil_Out32(XPAR_LEDS_BASEADDR,0x00);
   sleep(3);
   clear();
//   xil_printf("My Time is: %d \r\n", myTime);
//   return myTime;
   return;
}



