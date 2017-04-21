/* File:   pio_interrupt_demo2.c
   Author: Ran Bao (rba90@uclive.ac.nz)
   Date:   21 April 2017
   Descr:  An example showing how to trigger interrupt on pio.
*/
#include "pio.h"
#include "irq.h"
#include "target.h"
#include "pacer.h"

/* Define how fast ticks occur.  This must be faster than
   TICK_RATE_MIN.  */
enum {LOOP_POLL_RATE = 200};

/* Define LED flash rate in Hz.  */
enum {LED_FLASH_RATE = 2};


#ifndef WAKEUP_PIO
#define WAKEUP_PIO PA5_PIO
#endif


static void
pio_handler (void)
{
	// pio_irq_disable will stop PIO from generating interrupt signal
	pio_irq_disable(WAKEUP_PIO);

	// irq_disable will stop interrupt at specific port (for example, PORT_A)
	irq_disable (PIO_ID (WAKEUP_PIO));

	pio_output_high (LED2_PIO);
}


int
main (void)
{
	uint8_t flash_ticks;

	/* Configure LED PIO as output.  */
	pio_config_set (LED1_PIO, PIO_OUTPUT_LOW);
	pio_config_set (LED2_PIO, PIO_OUTPUT_LOW);

	// initialize WAKEUP_PIN as interrupt input, triggered on falling edge
	pio_irq_init(WAKEUP_PIO, PIO_IRQ_FALLING_EDGE, pio_handler);


	pacer_init (LOOP_POLL_RATE);
	flash_ticks = 0;

	while (1)
	{
	/* Wait until next clock tick.  */
	pacer_wait ();

	flash_ticks++;
	if (flash_ticks >= LOOP_POLL_RATE / (LED_FLASH_RATE * 2))
	{
		flash_ticks = 0;

		/* Toggle LED.  */
		pio_output_toggle (LED1_PIO);
	}
	}
}
