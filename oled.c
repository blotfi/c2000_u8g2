/////////////////////////////////////////////////////////////////////////
//
// I2C Routines
//
//Modified by L. BAGHLI 03/2017
// Help
// d:\users\baghli\orcad\gyro\C2000_IMU6050\3583.F28335 - I2C.pdf
// https://e2e.ti.com/support/microcontrollers/c2000/f/171/t/416428
//
// Included Files
//

//#include <stddef.h>
//#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include <math.h>
//#include <ctype.h>
#include <stdint.h>
//#include <stdbool.h>

#include "F2802x_PieCtrl.h"
#include "F2802x_I2c.h"
#include "DSP28x_Project.h"     // Device Headerfile and Examples Include File
#include "F2802x_Device.h"     // F2802x Headerfile Include File

#include "oled.h"
// USER_SYSTEM_FREQ_MHz is 60 MHz

u8g2_t u8g2; // a structure which will contain all the data for one display

//-----------------------------------------------------------------------------
uint8_t u8x8_gpio_and_delay_c2000(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
  switch(msg)
  {
    case U8X8_MSG_GPIO_AND_DELAY_INIT:	// called once during init phase of u8g2/u8x8
      break;							// can be used to setup pins
    case U8X8_MSG_DELAY_NANO:			// delay arg_int * 1 nano second
      DSP28x_usDelay(arg_int/1000);
      break;
    case U8X8_MSG_DELAY_100NANO:		// delay arg_int * 100 nano seconds
      DSP28x_usDelay(arg_int/10);
      break;
    case U8X8_MSG_DELAY_10MICRO:		// delay arg_int * 10 micro seconds
      DSP28x_usDelay(arg_int*10);
      break;
    case U8X8_MSG_DELAY_MILLI:			// delay arg_int * 1 milli second
      while( arg_int-- > 0 )
         DSP28x_usDelay(1000);

      break;
    case U8X8_MSG_DELAY_I2C:				// arg_int is the I2C speed in 100KHz, e.g. 4 = 400 KHz
      break;							// arg_int=1: delay by 5us, arg_int = 4: delay by 1.25us
  }
}
//-----------------------------------------------------------------------------
uint8_t u8x8_byte_i2c_c2000(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
  static uint8_t buffer[32];		/* u8g2/u8x8 will never send more than 32 bytes */
  static uint8_t buf_idx;
  uint8_t *data;

  switch(msg)
  {
    case U8X8_MSG_BYTE_SEND:
      data = (uint8_t *)arg_ptr;
      while( arg_int > 0 )
      {
	buffer[buf_idx++] = *data;
	data++;
	arg_int--;
      }
      break;
    case U8X8_MSG_BYTE_INIT:
      /* init i2c communication */
      break;
    case U8X8_MSG_BYTE_SET_DC:
      /* ignored for i2c */
      break;
    case U8X8_MSG_BYTE_START_TRANSFER:
      buf_idx = 0;
      break;
    case U8X8_MSG_BYTE_END_TRANSFER:
      i2c_Xfer( u8x8_GetI2CAddress(u8x8), buffer, buf_idx, NULL, 0 );
      break;
    default:
      return 0;
  }
  return 1;
}
//-----------------------------------------------------------------------------
//---------------------------------------------Delay
void __delay_ms(unsigned long t)
{
	unsigned long i;
	for (i=0;i<t;i++)
		DSP28x_usDelay(1000);	//usDelay
}
//-----------------------------------------------------------------------------
// InitI2C - This function initializes the I2C to a known state.
void InitI2C()
{
    // Initialize I2C-A
/*
    I2caRegs.I2CSAR = I2C_SLAVE_ADDR;		// Slave address - OLED control code
// ( I2C_SLAVE_ADDR << 1 ) | ( hasWrite ? 0x00 : 0x01 );
    I2caRegs.I2CPSC.all = 6;		// Prescaler - need 7-12 Mhz on module clk
    I2caRegs.I2CCLKL = 10;			// NOTE: must be non zero
    I2caRegs.I2CCLKH = 5;			// NOTE: must be non zero
    I2caRegs.I2CIER.all = 0x24;		// Enable SCD & ARDY interrupts
    // Take I2C out of reset. Stop I2C when suspended
    I2caRegs.I2CMDR.all = 0x0020;

    I2caRegs.I2CFFTX.all = 0x6000;	// Enable FIFO mode and TXFIFO
    I2caRegs.I2CFFRX.all = 0x2040;	// Enable RXFIFO, clear RXFFINT,

	i2c1Reset();
//	i2c1Enable( kI2C_400KHZ );
*/
   I2caRegs.I2CMDR.all = 0x0000;
   I2caRegs.I2CSAR = I2C_SLAVE_ADDR;		// Slave address
   // 90 MHz/( (8+1)(10+5 + 5+5)) = 400 kHz
   // 60 MHz/( (5+1)(10+5 + 5+5)) = 400 kHz
   //
   // I2CCLK = SYSCLK/(I2CPSC+1)
   I2caRegs.I2CPSC.all = 5;		    		// Prescaler - need 7-12 Mhz on module clk
   I2caRegs.I2CCLKL = 10;					// NOTE: must be non zero
   I2caRegs.I2CCLKH = 5;					// NOTE: must be non zero

   //I2caRegs.I2CIER.all = 0x24;			// Enable SCD & ARDY interrupts  0010 0100
   I2caRegs.I2CIER.all = 0x00;				// No interrupt
//   I2caRegs.I2CIER.bit.AAS = 0;				// Addressed as slave interrupt enable bit
//   I2caRegs.I2CIER.bit.SCD = 1;				// Stop condition detected interrupt enable bit
//   I2caRegs.I2CIER.bit.XRDY = 0;			// Transmit-data-ready interrupt enable bit
//   I2caRegs.I2CIER.bit.RRDY = 0;			// Receive-data-ready interrupt enable bit
//   I2caRegs.I2CIER.bit.ARDY = 1;			// Register-access-ready interrupt enable bit
//   I2caRegs.I2CIER.bit.NACK = 0;			// No-acknowledgment interrupt enable bit
//   I2caRegs.I2CIER.bit.ARBL = 0;			// Arbitration-lost interrupt enable bit

   //I2caRegs.I2CFFTX.all = 0x6000;			// Enable FIFO mode and TXFIFO
   I2caRegs.I2CFFTX.bit.I2CFFEN = 1;		// Enable the I2C FIFO mode
   I2caRegs.I2CFFTX.bit.TXFFRST = 1;		// Enable the transmit FIFO operation
   I2caRegs.I2CFFTX.bit.TXFFINTCLR = 1;		// Clear the TXFFINT flag
   I2caRegs.I2CFFTX.bit.TXFFIENA = 0;		// TXFFINT flag generates an interrupt when set
   I2caRegs.I2CFFTX.bit.TXFFIL = 0;			// Transmit FIFO interrupt level

   //I2caRegs.I2CFFRX.all = 0x0000;			// No RXFIFO, no clear RXFFINT
   I2caRegs.I2CFFRX.bit.RXFFRST = 0;		// **Disable** the receive FIFO operation
   I2caRegs.I2CFFRX.bit.RXFFINTCLR = 0;		// Clear the RXFFINT flag
   I2caRegs.I2CFFRX.bit.RXFFIENA = 0;		// RXFFINT flag does generate an interrupt when set
   I2caRegs.I2CFFRX.bit.RXFFIL = 0;			// Receive FIFO interrupt level

   //I2caRegs.I2CMDR.all = 0x0020;			// Take I2C out of reset,Stop I2C when suspended
   I2caRegs.I2CMDR.bit.NACKMOD = 0;			// NACK mode bit
   I2caRegs.I2CMDR.bit.FREE = 0;			// Stop I2C when suspended
   I2caRegs.I2CMDR.bit.STT = 0;				// START condition bit
   I2caRegs.I2CMDR.bit.STP = 0;				// STOP condition bit
   I2caRegs.I2CMDR.bit.MST = 0;				// Slave mode
   I2caRegs.I2CMDR.bit.TRX = 0;				// Receiver mode
   I2caRegs.I2CMDR.bit.XA = 0;				// 7-bit addressing mode
   I2caRegs.I2CMDR.bit.RM = 0;				// Nonrepeat mode
   I2caRegs.I2CMDR.bit.DLB = 0;				// Digital loopback mode is disabled
   I2caRegs.I2CMDR.bit.IRS = 1;				// The I2C module is enabled
   I2caRegs.I2CMDR.bit.STB = 0;				// The I2C module is not in the START byte mode
   I2caRegs.I2CMDR.bit.FDF = 0;				// Free data format mode is disabled
   I2caRegs.I2CMDR.bit.BC = 0;				// 8 bits per data byte

   u8g2_Setup_ssd1306_i2c_128x64_noname_1(&u8g2, U8G2_R0, u8x8_byte_i2c_c2000, u8x8_gpio_and_delay_c2000);  // init u8g2 structure
   u8g2_InitDisplay(&u8g2); // send init sequence to the display, display is in sleep mode after this,
   u8g2_SetPowerSave(&u8g2, 0); // wake up display

   /*
	oledInit();
	oledClear();
	oledGotoYX( 0, 0 );
	oledPrint( "Ring" );
	oledGotoYX( 1, 0 );
	oledPrint( "Vdc" );
	oledGotoYX( 1, 10 );
	oledPrint( "V" );

	oledGotoYX( 2, 0 );
	oledPrint("Vref");

	oledGotoYX( 3, 0 );
	oledPrint("Thanks");
	*/
}
//-----------------------------------------------------------------------------
void i2c_Xfer(
	Uint8 address,
	Uint8* wdata, int wsize,
	Uint8* rdata, int rsize )
{
Uint16 i;
//	I2caRegs.I2CMDR.bit.IRS = 0;
//	usDelay(10);
//	I2caRegs.I2CMDR.bit.IRS = 1;
//	usDelay(10);
//	// Make sure I2C is not busy and has stopped
//	while (I2caRegs.I2CSTR.bit.BB == 1);		// busy loop
//	I2caRegs.I2CSTR.bit.SCD = 1;				// Clear the SCD bit (stop condition bit)
//	while(I2caRegs.I2CMDR.bit.STP == 1);		// stop bit loop

	I2caRegs.I2CSAR = address;           			// I2C slave address
//	while (I2caRegs.I2CSTR.bit.BB == 1);		// still busy?

    I2caRegs.I2CCNT = wsize;					// Setup number of bytes to send

//	I2caRegs.I2CMDR.all = 0x6E20; 				// Send the TX FIFO :   0110 1110 0010 0000
 // 	I2caRegs.I2CMDR.all = 0x6620; 				// STT=1 			:   0110 0110 0010 0000
  	I2caRegs.I2CMDR.all = 0x66A0; 				// STT=1 			:   0110 0110 1010 0000	RM, STT

//		I2caRegs.I2CMDR.bit.NACKMOD = 0;			// NACK mode bit
//		I2caRegs.I2CMDR.bit.FREE = 1;				// Run free I2C when suspended
//		I2caRegs.I2CMDR.bit.STT = 1;				// START condition bit
//		I2caRegs.I2CMDR.bit.STP = 1;				// STOP condition bit
//		I2caRegs.I2CMDR.bit.MST = 1;				// Master mode
//		I2caRegs.I2CMDR.bit.TRX = 1;				// Transmitter mode
//		I2caRegs.I2CMDR.bit.XA = 0;					// 7-bit addressing mode
//		I2caRegs.I2CMDR.bit.RM = 0;					// Nonrepeat mode
//		I2caRegs.I2CMDR.bit.DLB = 0;				// Digital loopback mode is disabled
//		I2caRegs.I2CMDR.bit.IRS = 1;				// The I2C module is enabled
//		I2caRegs.I2CMDR.bit.STB = 0;				// The I2C module is not in the START byte mode
//		I2caRegs.I2CMDR.bit.FDF = 0;				// Free data format mode is disabled
//		I2caRegs.I2CMDR.bit.BC = 0;					// 8 bits per data byte
//	int WaitTimeOut=0;

	//start transfer of 4 bytes max
	for (i=0; i<wsize; i++)
		{
		I2caRegs.I2CDXR = *(wdata+i);
		while(!I2caRegs.I2CSTR.bit.XRDY);
		}
	I2caRegs.I2CMDR.bit.STP = 1;	// manual set of Stop
	while(I2caRegs.I2CMDR.bit.STP == 1);		// stop bit loop
}
//-----------------------------------------------------------------------------
void i2c_Reset()
{
}
//-----------------------------------------------------------------------------
// SSD1308 OLED Routines
//

#define SSD1308_Address                 I2C_SLAVE_ADDR
#define SSD1308_Command_Mode		0x80
#define SSD1308_Data_Mode               0x40
#define SSD1308_Display_Off_Cmd         0xAE
#define SSD1308_Display_On_Cmd          0xAF
#define SSD1308_Normal_Display_Cmd      0xA6
#define SSD1308_Inverse_Display_Cmd	0xA7
#define SSD1308_Activate_Scroll_Cmd	0x2F
#define SSD1308_Dectivate_Scroll_Cmd	0x2E
#define SSD1308_Set_Brightness_Cmd      0x81

const Uint8 OledFont[][8] =
{
  {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
  {0x00,0x00,0x5F,0x00,0x00,0x00,0x00,0x00},
  {0x00,0x00,0x07,0x00,0x07,0x00,0x00,0x00},
  {0x00,0x14,0x7F,0x14,0x7F,0x14,0x00,0x00},
  {0x00,0x24,0x2A,0x7F,0x2A,0x12,0x00,0x00},
  {0x00,0x23,0x13,0x08,0x64,0x62,0x00,0x00},
  {0x00,0x36,0x49,0x55,0x22,0x50,0x00,0x00},
  {0x00,0x00,0x05,0x03,0x00,0x00,0x00,0x00},
  {0x00,0x1C,0x22,0x41,0x00,0x00,0x00,0x00},
  {0x00,0x41,0x22,0x1C,0x00,0x00,0x00,0x00},
  {0x00,0x08,0x2A,0x1C,0x2A,0x08,0x00,0x00},
  {0x00,0x08,0x08,0x3E,0x08,0x08,0x00,0x00},
  {0x00,0xA0,0x60,0x00,0x00,0x00,0x00,0x00},
  {0x00,0x08,0x08,0x08,0x08,0x08,0x00,0x00},
  {0x00,0x60,0x60,0x00,0x00,0x00,0x00,0x00},
  {0x00,0x20,0x10,0x08,0x04,0x02,0x00,0x00},
  {0x00,0x3E,0x51,0x49,0x45,0x3E,0x00,0x00},
  {0x00,0x00,0x42,0x7F,0x40,0x00,0x00,0x00},
  {0x00,0x62,0x51,0x49,0x49,0x46,0x00,0x00},
  {0x00,0x22,0x41,0x49,0x49,0x36,0x00,0x00},
  {0x00,0x18,0x14,0x12,0x7F,0x10,0x00,0x00},
  {0x00,0x27,0x45,0x45,0x45,0x39,0x00,0x00},
  {0x00,0x3C,0x4A,0x49,0x49,0x30,0x00,0x00},
  {0x00,0x01,0x71,0x09,0x05,0x03,0x00,0x00},
  {0x00,0x36,0x49,0x49,0x49,0x36,0x00,0x00},
  {0x00,0x06,0x49,0x49,0x29,0x1E,0x00,0x00},
  {0x00,0x00,0x36,0x36,0x00,0x00,0x00,0x00},
  {0x00,0x00,0xAC,0x6C,0x00,0x00,0x00,0x00},
  {0x00,0x08,0x14,0x22,0x41,0x00,0x00,0x00},
  {0x00,0x14,0x14,0x14,0x14,0x14,0x00,0x00},
  {0x00,0x41,0x22,0x14,0x08,0x00,0x00,0x00},
  {0x00,0x02,0x01,0x51,0x09,0x06,0x00,0x00},
  {0x00,0x32,0x49,0x79,0x41,0x3E,0x00,0x00},
  {0x00,0x7E,0x09,0x09,0x09,0x7E,0x00,0x00},
  {0x00,0x7F,0x49,0x49,0x49,0x36,0x00,0x00},
  {0x00,0x3E,0x41,0x41,0x41,0x22,0x00,0x00},
  {0x00,0x7F,0x41,0x41,0x22,0x1C,0x00,0x00},
  {0x00,0x7F,0x49,0x49,0x49,0x41,0x00,0x00},
  {0x00,0x7F,0x09,0x09,0x09,0x01,0x00,0x00},
  {0x00,0x3E,0x41,0x41,0x51,0x72,0x00,0x00},
  {0x00,0x7F,0x08,0x08,0x08,0x7F,0x00,0x00},
  {0x00,0x41,0x7F,0x41,0x00,0x00,0x00,0x00},
  {0x00,0x20,0x40,0x41,0x3F,0x01,0x00,0x00},
  {0x00,0x7F,0x08,0x14,0x22,0x41,0x00,0x00},
  {0x00,0x7F,0x40,0x40,0x40,0x40,0x00,0x00},
  {0x00,0x7F,0x02,0x0C,0x02,0x7F,0x00,0x00},
  {0x00,0x7F,0x04,0x08,0x10,0x7F,0x00,0x00},
  {0x00,0x3E,0x41,0x41,0x41,0x3E,0x00,0x00},
  {0x00,0x7F,0x09,0x09,0x09,0x06,0x00,0x00},
  {0x00,0x3E,0x41,0x51,0x21,0x5E,0x00,0x00},
  {0x00,0x7F,0x09,0x19,0x29,0x46,0x00,0x00},
  {0x00,0x26,0x49,0x49,0x49,0x32,0x00,0x00},
  {0x00,0x01,0x01,0x7F,0x01,0x01,0x00,0x00},
  {0x00,0x3F,0x40,0x40,0x40,0x3F,0x00,0x00},
  {0x00,0x1F,0x20,0x40,0x20,0x1F,0x00,0x00},
  {0x00,0x3F,0x40,0x38,0x40,0x3F,0x00,0x00},
  {0x00,0x63,0x14,0x08,0x14,0x63,0x00,0x00},
  {0x00,0x03,0x04,0x78,0x04,0x03,0x00,0x00},
  {0x00,0x61,0x51,0x49,0x45,0x43,0x00,0x00},
  {0x00,0x7F,0x41,0x41,0x00,0x00,0x00,0x00},
  {0x00,0x02,0x04,0x08,0x10,0x20,0x00,0x00},
  {0x00,0x41,0x41,0x7F,0x00,0x00,0x00,0x00},
  {0x00,0x04,0x02,0x01,0x02,0x04,0x00,0x00},
  {0x00,0x80,0x80,0x80,0x80,0x80,0x00,0x00},
  {0x00,0x01,0x02,0x04,0x00,0x00,0x00,0x00},
  {0x00,0x20,0x54,0x54,0x54,0x78,0x00,0x00},
  {0x00,0x7F,0x48,0x44,0x44,0x38,0x00,0x00},
  {0x00,0x38,0x44,0x44,0x28,0x00,0x00,0x00},
  {0x00,0x38,0x44,0x44,0x48,0x7F,0x00,0x00},
  {0x00,0x38,0x54,0x54,0x54,0x18,0x00,0x00},
  {0x00,0x08,0x7E,0x09,0x02,0x00,0x00,0x00},
  {0x00,0x18,0xA4,0xA4,0xA4,0x7C,0x00,0x00},
  {0x00,0x7F,0x08,0x04,0x04,0x78,0x00,0x00},
  {0x00,0x00,0x7D,0x00,0x00,0x00,0x00,0x00},
  {0x00,0x80,0x84,0x7D,0x00,0x00,0x00,0x00},
  {0x00,0x7F,0x10,0x28,0x44,0x00,0x00,0x00},
  {0x00,0x41,0x7F,0x40,0x00,0x00,0x00,0x00},
  {0x00,0x7C,0x04,0x18,0x04,0x78,0x00,0x00},
  {0x00,0x7C,0x08,0x04,0x7C,0x00,0x00,0x00},
  {0x00,0x38,0x44,0x44,0x38,0x00,0x00,0x00},
  {0x00,0xFC,0x24,0x24,0x18,0x00,0x00,0x00},
  {0x00,0x18,0x24,0x24,0xFC,0x00,0x00,0x00},
  {0x00,0x00,0x7C,0x08,0x04,0x00,0x00,0x00},
  {0x00,0x48,0x54,0x54,0x24,0x00,0x00,0x00},
  {0x00,0x04,0x7F,0x44,0x00,0x00,0x00,0x00},
  {0x00,0x3C,0x40,0x40,0x7C,0x00,0x00,0x00},
  {0x00,0x1C,0x20,0x40,0x20,0x1C,0x00,0x00},
  {0x00,0x3C,0x40,0x30,0x40,0x3C,0x00,0x00},
  {0x00,0x44,0x28,0x10,0x28,0x44,0x00,0x00},
  {0x00,0x1C,0xA0,0xA0,0x7C,0x00,0x00,0x00},
  {0x00,0x44,0x64,0x54,0x4C,0x44,0x00,0x00},
  {0x00,0x08,0x36,0x41,0x00,0x00,0x00,0x00},
  {0x00,0x00,0x7F,0x00,0x00,0x00,0x00,0x00},
  {0x00,0x41,0x36,0x08,0x00,0x00,0x00,0x00},
  {0x00,0x02,0x01,0x01,0x02,0x01,0x00,0x00},
  {0x00,0x02,0x05,0x05,0x02,0x00,0x00,0x00}
};

void oledCommand( Uint8 ch )
{
    Uint8 bytes[2];
    bytes[0] = SSD1308_Command_Mode;
    bytes[1] = ch;
    i2c_Xfer( SSD1308_Address, bytes, 2, NULL, 0 );
}

void oledDisplayOffset( Uint8 offset )
{
    Uint8 bytes[3];
    bytes[0] = SSD1308_Command_Mode;
    bytes[1] = 0xD3;
    bytes[2] = offset;
    i2c_Xfer( SSD1308_Address, bytes, 3, NULL, 0 );
}

void oledData( Uint8 data )
{
    Uint8 bytes[2];
    bytes[0] = SSD1308_Data_Mode;
    bytes[1] = data;
    i2c_Xfer( SSD1308_Address, bytes, 2, NULL, 0 );
}

void oledGotoYX(unsigned char Row, unsigned char Column)
{
    oledCommand( 0xB0 + Row);
    oledCommand( 0x00 + (8*Column & 0x0F) );
    oledCommand( 0x10 + ((8*Column>>4)&0x0F) );
}

void oledPutChar( char ch )
{
    if ( ( ch < 32 ) || ( ch > 127 ) )  ch = ' ';
    const Uint8 *base = &OledFont[ch - 32][0];
    Uint8 bytes[9];
    bytes[0] = SSD1308_Data_Mode;
    memmove( bytes + 1, base, 8 );
    i2c_Xfer( SSD1308_Address, bytes, 9, NULL, 0 );
}

void oledPutHexa( unsigned int Var )
{
int tmp;
			tmp=Var>>12 & 0x000F;
			if (tmp<=9)		tmp+=0x30;
					else			tmp+=0x37;
			oledPutChar(tmp);
  
			tmp=Var>>8 & 0x000F;
			if (tmp<=9)		tmp+=0x30;
					else			tmp+=0x37;
			oledPutChar(tmp);

			tmp=Var>>4 & 0x000F;
			if (tmp<=9)		tmp+=0x30;
					else			tmp+=0x37;
			oledPutChar(tmp);

			tmp=Var & 0x000F;
			if (tmp<=9)		tmp+=0x30;
					else			tmp+=0x37;
			oledPutChar(tmp);
}

void oledPutDec( unsigned int Var )
{
unsigned int k;
unsigned char c;
// Char
	k = Var;
	c = k/1000;
	if (c > 0)
		k = k - c*1000;
	oledPutChar(c + 0x30);
	c = k/100;
	if (c > 0)
		k = k - c*100;
	oledPutChar(c + 0x30);
	c = k/10;
	if (c > 0)
		k = k - c*10;
	oledPutChar(c + 0x30);
 // oledPutChar('.');
	oledPutChar(k + 0x30);
}

void oledPrint( char *s )
{
    while (*s) oledPutChar( *s++);
}

void oledClear()
{
Uint16 row, col;
    for ( row = 0; row < 8; row++ ) {
        for ( col = 0; col < 16; col++ ) {
            oledGotoYX( row, col );
            oledPutChar( ' ' );
        }
    }
}

void oledInit()
{
// olex way : 
#define OLED_SETCONTRAST 0x81
#define OLED_DISPLAYALLON_RESUME 0xA4
#define OLED_DISPLAYALLON 0xA5
#define OLED_NORMALDISPLAY 0xA6
#define OLED_INVERTDISPLAY 0xA7
#define OLED_DISPLAYOFF 0xAE
#define OLED_DISPLAYON 0xAF
#define OLED_SETDISPLAYOFFSET 0xD3
#define OLED_SETCOMPINS 0xDA
#define OLED_SETVCOMDETECT 0xDB
#define OLED_SETDISPLAYCLOCKDIV 0xD5
#define OLED_SETPRECHARGE 0xD9
#define OLED_SETMULTIPLEX 0xA8
#define OLED_SETLOWCOLUMN 0x00
#define OLED_SETHIGHCOLUMN 0x10
#define OLED_SETSTARTLINE 0x40
#define OLED_MEMORYMODE 0x20
#define OLED_COLUMNADDR 0x21
#define OLED_PAGEADDR   0x22
#define OLED_COMSCANINC 0xC0
#define OLED_COMSCANDEC 0xC8
#define OLED_SEGREMAP 0xA0
#define OLED_CHARGEPUMP 0x8D

	oledCommand( OLED_DISPLAYOFF );
	__delay_ms( 100 );
	oledCommand( OLED_SETDISPLAYCLOCKDIV );
	oledCommand( 0x80 );   // the suggested ratio 0x80
	__delay_ms( 100 );
	oledCommand( OLED_SETMULTIPLEX );
	oledCommand( 0x1F );
	__delay_ms( 100 );
	oledCommand( OLED_SETDISPLAYOFFSET );
	oledCommand( 0x0 );   // no offset
	__delay_ms( 100 );
	oledCommand(OLED_SETSTARTLINE | 0x0); // line #0
	__delay_ms( 100 );
	oledCommand( OLED_CHARGEPUMP );
	oledCommand( 0xAF );
	__delay_ms( 100 );
	oledCommand( OLED_MEMORYMODE );
	oledCommand( 0x0 );  // 0x0 act like ks0108
	__delay_ms( 100 );
				 // 0x0 act like ks0108
	oledCommand(OLED_SEGREMAP | 0x1);
	oledCommand(OLED_COMSCANDEC);
	oledCommand(OLED_SETCOMPINS);         // 0xDA
	oledCommand(0x02);
	oledCommand(OLED_SETCONTRAST);        // 0x81
	oledCommand(0x8F);
	oledCommand(OLED_SETPRECHARGE);       // 0xd9
	oledCommand(0xF1);
	oledCommand(OLED_SETVCOMDETECT);      // 0xDB
	oledCommand(0x40);
	oledCommand(OLED_DISPLAYALLON_RESUME);// 0xA4
	oledCommand(OLED_NORMALDISPLAY);      // 0xA6
	oledCommand(OLED_DISPLAYON);          //--turn on oled panel
}
