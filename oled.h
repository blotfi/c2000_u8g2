/* 
 * File:   oled.h
 * Author: baghli
 *
 * Created on 3 juin 2017, 16:02
 */
#include <stdint.h>
#include "F2802x_Device.h"     // F2806x Headerfile Include File
#include "F2802x_I2c_defines.h"              // Macros used for I2C examples.

typedef uint8_t	Uint8;

#include <u8g2.h>

#ifndef OLED_H
#define	OLED_H

//Uint16 I2CA_WriteData(struct I2CMSG *msg);
//Uint16 I2CA_ReadData(struct I2CMSG *msg);
//__interrupt void i2c_int1a_isr(void);

#define I2C_SLAVE_ADDR       0x3c	// old: 0x50 for EEPROM

//-----------------------------------------------------------------------------
// I2C OLED Routines

//#define I2C1_SCL_TRIS	TRISBbits.TRISB8
//#define I2C1_SDA_TRIS	TRISBbits.TRISB9
//#define I2C1_SCL_LAT	LATBbits.LATB8
//#define I2C1_SDA_LAT	LATBbits.LATB9

#define kI2C_100KHZ     (0x00)
#define kI2C_400KHZ     (0x02)

uint8_t u8x8_gpio_and_delay_c2000(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
uint8_t u8x8_byte_i2c_c2000(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
extern u8g2_t u8g2;

void InitI2C();
//void InitI2CGpio();
//interrupt void i2c_int2a_isr(void);

void i2c_Reset();

void oledCommand( Uint8 ch );
void oledDisplayOffset( Uint8 offset );
void oledData( Uint8 data );
void oledGotoYX(unsigned char Row, unsigned char Column);
void oledPutChar( char ch );
void oledPutHexa( unsigned int Var );
void oledPutDec( unsigned int Var );
void oledPrint( char *s );
void oledClear();
void oledInit();
void __delay_ms(unsigned long t);
void i2c_Xfer(
	Uint8 address,
	Uint8* wdata, int wsize,
	Uint8* rdata, int rsize );
#endif	/* OLED_H */

