#include "stm32f3xx.h"
#include "main.h"
#include "bsp.h"
#include "delay.h"
#include "math.h"
#include "i2c.h"

static uint8_t SystemClock_Config(void);

int main(void)
{

	uint8_t		tx_data[2];
	uint8_t		rx_data[6];

	uint16_t		ax, ay, az, gx, gy, gz;

	float angle = 0;


	SystemClock_Config();


	BSP_LED_Init();
	BSP_I2C1_Init();
	BSP_Console_Init();

	my_printf("\r\n Robot Ready!\r\n");

	tx_data[1] = 0x00; tx_data[0] = 0x00;

	BSP_I2C1_Write(0x68, 0x1b, tx_data, 2);
	delay_ms(10);

	BSP_I2C1_Write(0x68, 0x1C, tx_data, 2);
	delay_ms(10);

	BSP_I2C1_Write(0x68, 0x6b, tx_data, 2);
	delay_ms(10);

	BSP_I2C1_Write(0x68, 0x6b, tx_data, 2);
	delay_ms(10);



	while(1)
	{
		BSP_I2C1_Read(0x68,0x43,rx_data,6);

		gx = (rx_data[0]<<8 | rx_data[1])/131;
		gy = (rx_data[2]<<8 | rx_data[3])/131;
		gz = (rx_data[4]<<8 | rx_data[5])/131;


		BSP_I2C1_Read(0x68,0x3B,rx_data,6);

		ax = (rx_data[0]<<8 | rx_data[1])/131;
		ay = (rx_data[2]<<8 | rx_data[3])/131;
		az = (rx_data[4]<<8 | rx_data[5])/131;

		//my_printf("gx = %d,gy = %d,gz = %d, ax = %d,ay = %d,az = %d\r\n",gx/131,gy/131,gz/131, ax/131,ay/131,az/131);

		//MahonyAHRSupdateIMU(gx, gy, gz, ax, ay, az);

		angle=0.98*(angle+(float)gy*0.01/131) + 0.02*atan2((double_t)ax,(double_t)az)*180/3;

		my_printf("angle = %d\r\n",(uint16_t)angle);

		delay_ms(10);


	}
}


/*
 * 	Clock configuration for the Nucleo STM32F303K8 board
 *
 * 	Default solder bridges configuration is for HSI (HSE is not connected)
 * 	You can change this by swapping solder bridges SB4 and SB6 (see Nucleo User manual)
 *
 * 	HSI clock configuration provides :
 * 	- SYSCLK, AHB	-> 64MHz
 * 	- APB2			-> 64MHz
 * 	- APB1			-> 32MHz (periph) 64MHz (timers)
 *
 * 	HSE clock configuration from ST-Link 8MHz MCO provides :
 * 	- SYSCLK, AHB	-> 72MHz
 * 	- APB2			-> 72MHz
 * 	- APB1			-> 36MHz (periph) 72MHz (timers)
 *
 * 	Select configuration by setting one of these symbol in your build configuration
 * 	- USE_HSI
 * 	- USE_HSE
 *
 */


#ifdef USE_HSI

static uint8_t SystemClock_Config()
{
	uint32_t	status;
	uint32_t	timeout = 0;

	// Start 8MHz HSI (it should be already started at power on)
	RCC->CR |= RCC_CR_HSION;

	// Wait until HSI is stable
	timeout = 1000;

	do
	{
		status = RCC->CR & RCC_CR_HSIRDY;
		timeout--;
	} while ((status == 0) && (timeout > 0));

	if (timeout == 0) return (1);	// HSI error


	// Configure Flash latency according to the speed (2WS)
	FLASH->ACR &= ~FLASH_ACR_LATENCY;
	FLASH->ACR |= 0x02 <<0;

	// Set HSI as PLL input
	RCC->CFGR |= RCC_CFGR_PLLSRC_HSI_DIV2;	// 4MHz from HSI

	// Configure the main PLL
	#define PLL_MUL		16					// 4MHz HSI to 64MHz
	RCC->CFGR |= (PLL_MUL-2) <<18;

	// Enable the main PLL
	RCC-> CR |= RCC_CR_PLLON;

	// Configure AHB/APB prescalers
	// AHB  Prescaler = /1	-> 64 MHz
	// APB1 Prescaler = /2  -> 32/64 MHz
	// APB2 Prescaler = /1  -> 64 MHz
	RCC->CFGR |= RCC_CFGR_HPRE_DIV1;
	RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;
	RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;

	// Wait until PLL is ready
	timeout = 1000;

	do
	{
		status = RCC->CR & RCC_CR_PLLRDY;
		timeout--;
	} while ((status == 0) && (timeout > 0));

	if (timeout == 0) return (2);	// PLL error


	// Select the main PLL as system clock source
	RCC->CFGR &= ~RCC_CFGR_SW;
	RCC->CFGR |= RCC_CFGR_SW_PLL;

	// Wait until PLL is switched on
	timeout = 1000;

	do
	{
		status = RCC->CFGR & RCC_CFGR_SWS;
		timeout--;
	} while ((status != RCC_CFGR_SWS_PLL) && (timeout > 0));

	if (timeout == 0) return (3);	// SW error

	// Update System core clock
	SystemCoreClockUpdate();
	return (0);
}

#endif


#ifdef USE_HSE

static uint8_t SystemClock_Config()
{
	uint32_t	status;
	uint32_t	timeout;

	// Start HSE
	RCC->CR |= RCC_CR_HSEBYP;
	RCC->CR |= RCC_CR_HSEON;

	// Wait until HSE is ready
	timeout = 1000;

	do
	{
		status = RCC->CR & RCC_CR_HSERDY;
		timeout--;
	} while ((status == 0) && (timeout > 0));

	if (timeout == 0) return (1); 	// HSE error


	// Configure Flash latency according to the speed (2WS)
	FLASH->ACR &= ~FLASH_ACR_LATENCY;
	FLASH->ACR |= 0x02 <<0;

	// Configure the main PLL
	#define PLL_MUL		9	// 8MHz HSE to 72MHz
	RCC->CFGR |= (PLL_MUL-2) <<18;

	// Set HSE as PLL input
	RCC->CFGR |= RCC_CFGR_PLLSRC_HSE_PREDIV;

	// Enable the main PLL
	RCC-> CR |= RCC_CR_PLLON;

	// Configure AHB/APB prescalers
	// AHB  Prescaler = /1	-> 72 MHz
	// APB1 Prescaler = /2  -> 36/72 MHz
	// APB2 Prescaler = /1  -> 72 MHz
	RCC->CFGR |= RCC_CFGR_HPRE_DIV1;
	RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;
	RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;

	// Wait until PLL is ready
	timeout = 1000;

	do
	{
		status = RCC->CR & RCC_CR_PLLRDY;
		timeout--;
	} while ((status == 0) && (timeout > 0));

	if (timeout == 0) return (2); 	// PLL error


	// Select the main PLL as system clock source
	RCC->CFGR &= ~RCC_CFGR_SW;
	RCC->CFGR |= RCC_CFGR_SW_PLL;

	// Wait until PLL is switched on
	timeout = 1000;

	do
	{
		status = RCC->CR & RCC_CR_PLLRDY;
		timeout--;
	} while ((status != RCC_CFGR_SWS_PLL) && (timeout > 0));

	if (timeout == 0) return (3); 	// SW error


	// Update System core clock
	SystemCoreClockUpdate();
	return (0);
}

#endif
