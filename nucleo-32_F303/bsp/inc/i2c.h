/*
 * i2c.h
 *
 *  Created on: 9 sept. 2019
 *      Author: cyril
 */

#ifndef BSP_INC_I2C_H_
#define BSP_INC_I2C_H_

#define 	WHO_AM_I     0x0F

#define     CTRL_REG1    0x20
#define     CTRL_REG2    0x21
#define     CTRL_REG3    0x22
#define     CTRL_REG4    0x23
#define     CTRL_REG5    0x24

#define     STATUS_REG   0x27
#define     OUT_X_L      0x28
#define     OUT_X_H      0x29
#define     OUT_Y_L      0x2A
#define     OUT_Y_H      0x2B
#define     OUT_Z_L      0x2C
#define     OUT_Z_H      0x2D
#define     TEMP_OUT_L   0x2E
#define     TEMP_OUT_H   0x2F
#define     INT_CFG      0x30
#define     INT_SRC      0x31
#define     INT_THS_L    0x32
#define     INT_THS_H    0x33

void BSP_I2C1_Init(void);
uint8_t	BSP_I2C1_Read( uint8_t device_address,uint8_t register_address,uint8_t *buffer,uint8_t nbytes );
uint8_t	BSP_I2C1_Write( uint8_t device_address,uint8_t register_address,uint8_t *buffer, uint8_t nbytes );

#endif /* BSP_INC_I2C_H_ */
