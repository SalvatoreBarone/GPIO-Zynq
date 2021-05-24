#ifndef __XIL_GPIO_H__
#define __XIL_GPIO_H__

#include <inttypes.h>

#define XGPIO_GIE_GINTR_ENABLE_MASK	0x80000000

#define GPIO_DATA_OFFSET 	0x00
#define GPIO_TRI_OFFSET 	0x04
#define GPIO_READ_OFFSET	0x08
#define XGPIO_GIE_OFFSET	0x11C /**< Glogal interrupt enable register */
#define XGPIO_ISR_OFFSET	0x120 /**< Interrupt status register */
#define XGPIO_IER_OFFSET	0x128 /**< Interrupt enable register */

#define GLOBAL_INTR_ENABLE XGPIO_GIE_GINTR_ENABLE_MASK
#define CHANNEL1_INTR_ENABLE 0x00000001
#define CHANNEL2_INTR_ENABLE 0x00000002

#define GLOBAL_INTR_DISABLE   0x00000000
#define CHANNEL1_INTR_DISABLE 0x00000000
#define CHANNEL2_INTR_DISABLE 0x00000000

#define CHANNEL1_ACK 0x01
#define CHANNEL2_ACK 0x02

#define GPIO_MAP_SIZE 0x10000	//spazio di indirizzamento del device



int XilGpio_Global_Interrupt(uint32_t* baseAddress, uint32_t mask);
int XilGpio_Channel_Interrupt(uint32_t* baseAddress, uint32_t mask);
int XilGpio_Ack_Interrupt(uint32_t* baseAddress, uint32_t channel);

#endif

