
#include "delay.h"

/* Basic delay function */

void delay_ms(uint32_t delay)
{
	uint32_t	i;
	for(i=0; i<(delay*4200); i++);		// Tuned for ms on STM32F303 at 64MHz
}
