#include "xil_gpio.h"

#include <stdio.h>

int XilGpio_Global_Interrupt(uint32_t* baseAddress, uint32_t mask)
{
	baseAddress[XGPIO_GIE_OFFSET/4]=mask;	
	return 1;
}

int XilGpio_Channel_Interrupt(uint32_t* baseAddress, uint32_t mask)
{
	baseAddress[XGPIO_IER_OFFSET/4] = mask;
	return 1;
}

int XilGpio_Ack_Interrupt(uint32_t* baseAddress, uint32_t channel)
{
	// Toggle-On-Write (TOW) access toggles the status of the bit when
	// a value of 1 is written to the corresponding bit.

	baseAddress[XGPIO_ISR_OFFSET/4] = channel;

	return 1;
}

