/* File:   adc_usb_cdc_test2.c
   Author: M. P. Hayes, UCECE
   Date:   14 May 2015
   Descr:  This triggers ADC conversions using the a TC channel.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "usb_cdc.h"
#include "pio.h"
#include "adc.h"
#include "tc.h"
#include "sys.h"
#include "pacer.h"


#define PACER_RATE 2

#define GENERAL_LED_PIO  LED1_PIO
#define USB_LED_PIO  LED2_PIO

#define ADC_SAMPLES 8
#define ADC_CHANNEL 4

#define IADC_CLOCK_FREQ (F_CPU / 4)
#define IADC_SAMPLE_CLOCKS 24
#define IADC_SAMPLE_FREQ (IADC_CLOCK_FREQ / IADC_SAMPLE_CLOCKS)

#define IADC_BITS 12
#define IADC_RANGE BIT(IADC_BITS)
#define IADC_MASK (IADC_RANGE - 1)
#define IADC_SHIFT 0
#define IADC_RAW_TO_INT(ADC_RAW) (((int)(((uint16_t)(ADC_RAW) >> IADC_SHIFT) & IADC_MASK) - (IADC_RANGE / 2)) * 16)


static const adc_cfg_t adc_cfg =
{
    .bits = 12,
    .channel = ADC_CHANNEL,
    .trigger = ADC_TRIGGER_TC1,
    .clock_speed_kHz = IADC_CLOCK_FREQ / 1000
};


static const tc_cfg_t tc_cfg =
{
    /* The PIO needs to be specified to select the TC channel
       but it is not driven with TC_MODE_ADC.  */
    .pio = PA15_PIO,
    .mode = TC_MODE_ADC,
    .period = TC_PERIOD_DIVISOR (IADC_SAMPLE_FREQ, 2),
    .prescale = 2
};

        
typedef struct
{
    adc_t adc;
    tc_t tc;
} iadc_t;


static iadc_t iadc_dev;


/* Initialize the ADC and TC to trigger the ADC.  */
int
iadc_init (void)
{
    /* Generate sampling clock.  */
    iadc_dev.tc = tc_init (&tc_cfg);
    if (! iadc_dev.tc)
        return 0;

    iadc_dev.adc = adc_init (&adc_cfg);
    if (! iadc_dev.adc)
        return 0;

    tc_start (iadc_dev.tc);

    return 1;
}


uint16_t
iadc_read_raw (int16_t *data, uint16_t bytes)
{
    return adc_read (iadc_dev.adc, data, bytes);
}


uint16_t
iadc_read (int16_t *data, uint16_t bytes)
{
    uint16_t samples;
    uint16_t i;

    bytes = iadc_read_raw (data, bytes);

    samples = bytes / sizeof (data[0]);

    for (i = 0; i < samples; i++)
    {
        /* Sign-extend 12 bit data and pretend to be 16 bits.  */
        data[i] = IADC_RAW_TO_INT(data[i]);
    }

    return bytes;
}



int main (void)
{
    usb_cdc_t usb_cdc;
    int count = 0;

    pio_config_set (USB_LED_PIO, PIO_OUTPUT_LOW);                
    pio_config_set (GENERAL_LED_PIO, PIO_OUTPUT_HIGH);                

    iadc_init ();

    usb_cdc = usb_cdc_init ();
    
    sys_redirect_stdin ((void *)usb_cdc_read, usb_cdc);
    sys_redirect_stdout ((void *)usb_cdc_write, usb_cdc);
    sys_redirect_stderr ((void *)usb_cdc_write, usb_cdc);

    /* Wait until USB configured.  */
    while (! usb_cdc_update ())
        continue;
    pio_config_set (USB_LED_PIO, PIO_OUTPUT_HIGH);                

    pacer_init (PACER_RATE);
    while (1)
    {
        int i;
        int16_t buffer[ADC_SAMPLES];

        pacer_wait ();

        /* Check if USB disconnected.  */
        pio_output_set (USB_LED_PIO, usb_cdc_update ());

        pio_output_toggle (GENERAL_LED_PIO);

        iadc_read (buffer, sizeof (buffer));

        printf ("*** %4d *** \n", count);
        count++;
        for (i = 0; i < ADC_SAMPLES; i++)
            printf ("%3d: %d\n", i, buffer[i]);
    }
}
