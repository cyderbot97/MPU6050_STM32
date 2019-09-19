/*
 * i2c.h
 *
 *  Created on: 9 sept. 2019
 *      Author: cyril
 */

#ifndef BSP_INC_I2C_H_
#define BSP_INC_I2C_H_

#include "main.h"

void BSP_I2C1_Init(void);
uint8_t	BSP_I2C1_Read( uint8_t device_address,uint8_t register_address,uint8_t *buffer,uint8_t nbytes );
uint8_t	BSP_I2C1_Write( uint8_t device_address,uint8_t register_address,uint8_t *buffer, uint8_t nbytes );

#endif /* BSP_INC_I2C_H_ */
