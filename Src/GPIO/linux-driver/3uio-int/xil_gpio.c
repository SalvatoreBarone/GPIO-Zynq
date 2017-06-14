#include "xil_gpio.h"

int XilGpio_Global_Interrupt(long* baseAddress, unsigned long mask)
{
	baseAddress[XGPIO_GIE_OFFSET/4]=mask;	
	return 1;
}

int XilGpio_Channel_Interrupt(long* baseAddress, unsigned long mask)
{
	baseAddress[XGPIO_IER_OFFSET/4] = mask;
	return 1;
}

int XilGpio_Ack_Interrupt(long* baseAddress, int channel)
{
	baseAddress[XGPIO_ISR_OFFSET/4] = 0xFFFFFFFF;	
	return 1;
}

