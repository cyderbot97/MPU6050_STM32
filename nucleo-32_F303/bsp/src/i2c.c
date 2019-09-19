/*
 * i2c.c
 *
 *  Created on: 9 sept. 2019
 *      Author: cyril
 */

#include "stm32f3xx.h"

/*
 * I2C Function
 */
uint8_t	BSP_I2C1_Write( uint8_t device_address,
                        uint8_t register_address,
                        uint8_t *buffer, uint8_t nbytes )
{
	uint32_t 	timeout;	// Flag waiting timeout
	uint8_t		n;		// Loop counter

	// Set device address
	I2C1->CR2 &= ~I2C_CR2_SADD_Msk;
	I2C1->CR2 |= ((device_address <<1U) <<I2C_CR2_SADD_Pos);

	// Set I2C in Write mode
	I2C1->CR2 &= ~I2C_CR2_RD_WRN;

	// Transfer NBYTES, with AUTOEND
	I2C1->CR2 &= ~I2C_CR2_NBYTES;
	I2C1->CR2 |= ((nbytes+1) <<16U);
	I2C1->CR2 &= ~I2C_CR2_AUTOEND;

	// Clear STOPF flag
	I2C1->ICR |= I2C_ICR_STOPCF;

	// Start I2C transaction
	I2C1->CR2 |= I2C_CR2_START;

	// Wait for TXIS with timeout
	timeout = 100000;
	while (((I2C1->ISR) & I2C_ISR_TXIS) != I2C_ISR_TXIS)
	{
		timeout--;
		if (timeout == 0) return 1;
	}

	// Send register address
	I2C1->TXDR = register_address;

	n = nbytes;

	while(n>0)
	{
		// Wait for TXIS with timeout
		timeout = 100000;
		while (((I2C1->ISR) & I2C_ISR_TXIS) != I2C_ISR_TXIS)
		{
			timeout--;
			if (timeout == 0) return 2;
		}

		// Send data
		I2C1->TXDR = *buffer;
		buffer++;
		n--;
	}

	// Wait for STOPF with timeout
	timeout = 100000;
	while (((I2C1->ISR) & I2C_ISR_STOPF) != I2C_ISR_STOPF)
	{
		timeout--;
		if (timeout == 0) return 3;
	}

	// Return success
	return 0;
}
uint8_t	BSP_I2C1_Read( uint8_t device_address,
                       uint8_t register_address,
                       uint8_t *buffer,
                       uint8_t nbytes )
{
	uint32_t 	timeout;	// Flag waiting timeout
	uint8_t		n;			// Loop counter

	// Set device address
	I2C1->CR2 &= ~I2C_CR2_SADD_Msk;
	I2C1->CR2 |= ((device_address <<1U) <<I2C_CR2_SADD_Pos);

        // Set I2C in Write mode
	I2C1->CR2 &= ~I2C_CR2_RD_WRN;

	// Transfer NBYTES = 1, no AUTOEND
	I2C1->CR2 &= ~I2C_CR2_NBYTES;
	I2C1->CR2 |= (1 <<16U);
	I2C1->CR2 &= ~I2C_CR2_AUTOEND;

	// Start I2C transaction
	I2C1->CR2 |= I2C_CR2_START;

	// Wait for TXIS with timeout
	timeout = 100000;
	while (((I2C1->ISR) & I2C_ISR_TXIS) != I2C_ISR_TXIS)
	{
		timeout--;
		if (timeout == 0) return 1;
	}

	// Send Register address
	I2C1->TXDR = register_address;

	// Wait for TC with timeout
	timeout = 100000;
	while (((I2C1->ISR) & I2C_ISR_TC) != I2C_ISR_TC)
	{
		timeout--;
		if (timeout == 0) return 2;
	}

	// Set I2C in Read mode
	I2C1->CR2 |= I2C_CR2_RD_WRN;

	// Transfer NBYTES, no AUTOEND
	I2C1->CR2 &= ~I2C_CR2_NBYTES;
	I2C1->CR2 |= (nbytes <<16U);
	I2C1->CR2 &= ~I2C_CR2_AUTOEND;

	// Re-Start transaction
	I2C1->CR2 |= I2C_CR2_START;

	n = nbytes;

	while (n>0)
	{
		// Wait for RXNE with timeout
		timeout = 100000;
		while (((I2C1->ISR) & I2C_ISR_RXNE) != I2C_ISR_RXNE)
		{
			timeout--;
			if (timeout == 0) return 3;
		}

		// Store data into buffer
		*buffer = I2C1->RXDR;
		buffer++;
		n--;
	}

	// Generate STOP condition
	I2C1->CR2 |= I2C_CR2_STOP;

	// Wait for STOPF with timeout
	timeout = 100000;
	while (((I2C1->ISR) & I2C_ISR_STOPF) != I2C_ISR_STOPF)
	{
		timeout--;
		if (timeout == 0) return 4;
	}

	// Return success
	return 0;
}
void BSP_I2C1_Init()
{


	// Enable GPIOB clock
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

	// Configure PB6 PB7 as AF mode
	GPIOB->MODER &= ~(GPIO_MODER_MODER6 | GPIO_MODER_MODER7);
	GPIOB->MODER |= (0X02 << GPIO_MODER_MODER6_Pos) | (0X02 << GPIO_MODER_MODER7_Pos);

	// Connect to I2C1 (AF4)
	GPIOB->AFR[0] &= ~(0xFF000000);
	GPIOB->AFR[0] |=   0x44000000;

	// Setup Open-Drain
	GPIOB->OTYPER |= GPIO_OTYPER_OT_6 | GPIO_OTYPER_OT_7;

	// Select SYSCLK as I2C1 clock (48MHz)
	RCC->CFGR3 |= RCC_CFGR3_I2C1SW;

	// Enable I2C1 clock
	RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

	// Make sure I2C1 is disabled
	I2C1->CR1 &= ~I2C_CR1_PE;

	// Reset I2C1 Configuration to default values
	I2C1->CR1 	  = 0x00000000;
	I2C1->CR2 	  = 0x00000000;
	I2C1->TIMINGR = 0x00000000;

	// Configure timing for 100kHz, 50% duty cycle
	I2C1->TIMINGR |= ((4 -1) <<I2C_TIMINGR_PRESC_Pos); // Clock prescaler /4 -> 12MHz
	I2C1->TIMINGR |= (60 	 <<I2C_TIMINGR_SCLH_Pos);  // High half-period = 5µs
	I2C1->TIMINGR |= (60     <<I2C_TIMINGR_SCLL_Pos);  // Low  half-period = 5µs

	// Enable I2C1
	I2C1->CR1 |= I2C_CR1_PE;
}
