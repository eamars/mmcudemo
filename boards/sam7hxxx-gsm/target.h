/** @file   target.h
    @author M. P. Hayes, UCECE
    @date   02 June 2007
    @brief 
*/
#ifndef TARGET_H
#define TARGET_H

#include "mat91lib.h"

/* CPU clocks  */
#define F_XTAL 18432000.0
#define CPU_PLL_MUL 125
#define CPU_PLL_DIV 24
#define CPU_USB_DIV 2
/* 96 MHz  */
#define F_PLL (F_XTAL / CPU_PLL_DIV * CPU_PLL_MUL)
/* 48 MHz  */
#define F_CPU (F_PLL / 2)


/* GSM modem  */
#define GSM_ONOFF_PIO PA8_PIO
#define GSM_PWRMON_PIO PA9_PIO
#define GSM_RESET_PIO PA10_PIO

#define GSM_DEBUG_PIO PA11_PIO

#endif /* TARGET_H  */
