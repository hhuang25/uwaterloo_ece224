/*
 * lab1_phase1.c
 *
 *  Created on: 2013-09-27
 *      Author: h53huang
 */



// Libraries required
#include "alt_types.h"  // define types used by Altera code, e.g. alt_u8
#include <stdio.h>
#include <unistd.h>
#include "system.h"  // constants such as the base addresses of any PIOs
                     // defined in your hardware
#include "sys/alt_irq.h"  // required when using interrupts
#include <io.h>

static void timerfunction (void);
static void timerfunction2 (void);

// declare global variable of LED state, initially all LEDs should be off
volatile alt_u8 led_state = (alt_u8)0x00;
volatile alt_u8 switches = (alt_u8)0x00;
volatile alt_u8 buttons = (alt_u8)0x00;
volatile alt_u8 seven_seg = (alt_u8)0x00;

volatile alt_u8 count_flag = (alt_u8)0x9;
volatile alt_u8 count_flag2 = (alt_u8)0x9;
volatile alt_u8 button1_pressed = (alt_u8)0x00;
volatile alt_u8 button2_pressed = (alt_u8)0x00;

//--------------------------TIMER--------------------------------------------
#ifdef TIMER_0_BASE  // only compile this code if there is a TIMER_0_BASE
static void timer_ISR(void* context, alt_u32 id)
{

	// acknowledge the interrupt by clearing the TO bit in the status register
   IOWR(TIMER_0_BASE, 0, 0x0);

   // set the flag with a non zero value
   if(button1_pressed == 0x1)
   {
	   count_flag = count_flag - 1;
	   timerfunction();
   }
   if (button2_pressed == 0x2){
	   count_flag2 = count_flag2 - 1;
	   timerfunction2();
   }
   if(count_flag == 0x0)
   {
	   count_flag = 0x9;
	   button1_pressed = 0x00;
	   IOWR(LED_PIO_BASE, 0, 0x00);
   }
   if(count_flag2 == 0x0)
   {
	   count_flag2 = 0x9;
	   button2_pressed = 0x00;
	   IOWR(SEVEN_SEG_RIGHT_PIO_BASE, 0, 0xFF);
   }
   if((count_flag == 0x9) && (count_flag2 == 0x9))
   {
	   // initialize timer control - stop bit, start timer, run continuously, enable interrupts
	   IOWR(TIMER_0_BASE, 1, 0x8);

   }
}
#endif

//--------------------------Button-------------------------------------------
#ifdef BUTTON_PIO_BASE
#ifdef LED_PIO_BASE
#ifdef SWITCH_PIO_BASE
static void button_ISR(void* context, alt_u32 id)
{

   /* get value from edge capture register and mask off all bits except
      the 4 least significant */
   buttons = IORD(BUTTON_PIO_BASE, 3) & 0x3;
   if(button1_pressed == 0x00)
   {
	   button1_pressed = buttons & 0x1;
   }
   if(button2_pressed == 0x00)
   {
	   button2_pressed = buttons & 0x2;
   }

   //read the state of the switch
   //after read if 0 dont turn on led if 1 turn on led
   //led state should equal ones for the leds we want to turn on
   switches = IORD(SWITCH_PIO_BASE, 0) & 0xFF;

   if(button1_pressed == 0x1 && buttons == 0x01)
   {
	   led_state = switches;
	   count_flag = 0x9;
   }
   if(button2_pressed == 0x2 && buttons == 0x02)
   {
	   seven_seg = switches;
	   count_flag2 = 0x9;
   }
   /* write current state to LED PIO */
   //IOWR(LED_PIO_BASE, 0, led_state);


   /* reset edge capture register to clear interrupt */
   IOWR(BUTTON_PIO_BASE, 3, 0x0);

#ifdef TIMER_0_BASE

   // initialize timer control - start timer, run continuously, enable interrupts
   IOWR(TIMER_0_BASE, 1, 0x7);

#endif

}
#endif
#endif
#endif

//---------------------------------timerfunctions----------------------------------
static void timerfunction()
{

#ifdef TIMER_0_BASE

	/* write current state to LED PIO */
	IOWR(LED_PIO_BASE, 0, led_state & 0x1);
	led_state = led_state >> 1;


#endif
}

static void timerfunction2()
{
#ifdef TIMER_0_BASE

	/* write current state to Seven Seg */
	if (seven_seg & 0x1 == 1)
	{
		IOWR(SEVEN_SEG_RIGHT_PIO_BASE, 0, 0xCF);
	}
	else
	{
		IOWR(SEVEN_SEG_RIGHT_PIO_BASE, 0, 0x81);
	}
	seven_seg = seven_seg >> 1;


#endif

}

//----------------------------------main--------------------------------------------
/*
int main(void)
{

#ifdef BUTTON_PIO_BASE
   //initialize the button PIO

   //direction is input only

   //set up the interrupt vector
  alt_irq_register( BUTTON_PIO_IRQ, (void*)0, button_ISR );

   //reset the edge capture register by writing to it (any value will do)
  IOWR(BUTTON_PIO_BASE, 3, 0x0);

   //enable interrupts for all four buttons
  IOWR(BUTTON_PIO_BASE, 2, 0xf);

#endif

#ifdef LED_PIO_BASE
   //initially turn off all LEDs
  IOWR(LED_PIO_BASE, 0, led_state);

#endif

#ifdef TIMER_0_BASE
  alt_u32 timerPeriod;
  // calculate timer period for 1 seconds
  timerPeriod = 1 * TIMER_0_FREQ;

  // initialize timer interrupt vector
  alt_irq_register(TIMER_0_IRQ, (void*)0, timer_ISR);

  // initialize timer period
  IOWR(TIMER_0_BASE, 2, (alt_u16)timerPeriod);
  IOWR(TIMER_0_BASE, 3, (alt_u16)(timerPeriod >> 16));

  // clear timer interrupt bit in status register
  IOWR(TIMER_0_BASE, 0, 0x0);

#endif
   //output intial message
  printf("\n\nPush a button to turn on an LED\n");

  while( 1 )
  {
  }

  return(0);
}
*/
/**********************************************************************
  Copyright(c) 2007 C.C.W. Hulls, P.Eng., Students, staff, and faculty
  members at the University of Waterloo are granted a non-exclusive right
  to copy, modify, or use this software for non-commercial teaching,
  learning, and research purposes provided the author(s) are acknowledged.
**********************************************************************/
