/*
 * bsp.c
 *
 *  Created on: 5 août 2017
 *      Author: Laurent
 */

#include "stm32f3xx.h"
#include "bsp.h"


void BSP_Console_Init()
{
	// Enable GPIOA clock
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

	// Configure PA2 and PA3 as Alternate function
	GPIOA->MODER &= ~(GPIO_MODER_MODER2 | GPIO_MODER_MODER3);
	GPIOA->MODER |=  (0x02 <<4U) | (0x02 <<6U);

	// Set PA2 and PA3 to AF7 (USART2)
	GPIOA->AFR[0] &= ~(0x0000FF00);
	GPIOA->AFR[0] |=  (0x00007700);

	// Enable USART2 clock
	RCC -> APB1ENR |= RCC_APB1ENR_USART2EN;

	// Clear USART2 configuration (reset state)
	// 8-bit, 1 start, 1 stop, CTS/RTS disabled
	USART2->CR1 = 0x00000000;
	USART2->CR2 = 0x00000000;
	USART2->CR3 = 0x00000000;

	// Baud Rate = 115200
	USART2->CR1 &= ~USART_CR1_OVER8;
	USART2->BRR = 0x0115;

	// Enable both Transmitter and Receiver
	USART2->CR1 |= USART_CR1_TE | USART_CR1_RE;

	// Enable USART2
	USART2->CR1 |= USART_CR1_UE;
}



void BSP_LED_Init()
{
	// Enable GPIOB clock
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

	// Configure PB3 as output
	GPIOB->MODER &= ~GPIO_MODER_MODER3;
	GPIOB->MODER |= (0x01 <<6U);

	// Configure PB3 as Push-Pull output
	GPIOB->OTYPER &= ~GPIO_OTYPER_OT_3;

	// Configure PB3 as High-Speed Output
	GPIOB->OSPEEDR &= ~GPIO_OSPEEDER_OSPEEDR3_Msk;
	GPIOB->OSPEEDR |= (0x03 <<GPIO_OSPEEDER_OSPEEDR3_Pos);

	// Disable PB3 Pull-up/Pull-down
	GPIOB->PUPDR &= ~GPIO_PUPDR_PUPDR3_Msk;

	// Set Initial State OFF
	GPIOB->BSRR = GPIO_BSRR_BR_3;
}

void BSP_LED_Toggle()
{
	GPIOB->ODR ^= GPIO_ODR_3;
}


