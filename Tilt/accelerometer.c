/*********************************************************************
Accelerometer SPI Interface		LIS3D chip
SOLUTIONS

DATA ORDER:
	X (lo, hi)
	Y (lo, hi)
	Z (lo, hi)
	
DATE 	:  9/28/2013
AUTHOR	:  Stephen Tenniswood
Product of Beyond Kinetics, Inc
********************************************************************/
#include <stdlib.h>
#include <avr/sfr_defs.h>
#include <avr/common.h>
#include "inttypes.h"
#include "interrupt.h"
#include "bk_system_defs.h"
#include "pin_definitions.h"
#include "spi.h"
#include "can.h"
#include "can_msg.h"
#include "leds.h"
#include "accelerometer.h"
#include "gyro.h"
#include "configuration.h"
#include "calibrations.h"
#include "can_id_list.h"


union uXYZ accel;
union uXYZ XYZ;
BOOL ReadyToSend = FALSE;

/*****************************************************

*****************************************************/
void accel_init()
{
	// MOSI,MISO,SCK Pins are already setup in spi.c init()
	// See CS_DDR_MASK definition in "pin_definitions.h"
	CS_DDR  |= CS_ACCEL;
	INT_DDR &= ~(ACCEL_INT1|ACCEL_INT2);	// Make Interrupt line an input

	spi_setDataMode( SPI_MODE3    );
	accel_data_rate( SAMPLE_400HZ );
	accel_bypass_mode();

	byte reg = getAccelSensitivity();		// Get from EEPROM Config - 0x03 mask
	accel_set_sensitivity(reg);				// Set in the LIS3D

//	DDRC   |= 0xC0;			// inputs
//	PCMSK1 |= 0xC0;			// Enable Pin Change interrupt pins ACCEL_INT1, ACCEL_INT2 on PC6 & PC7
//	PCICR  |= (1<<PCIE1);	// Enable PinChangeInterrupt 1
}

/*****************************************************
ISR - READ DATA WHEN COMPLETE
*****************************************************/
ISR( PCINT1_vect )
{
	//This is also vector for Gyro Int.
	if ((INT_PORT_IN & ACCEL_INT1) && (INT_PORT_IN & ACCEL_INT2))
	{
		accel_read_xyz		( &accel );
		swap_bytes			( &accel );	
		ReadyToSend = TRUE;
	}
	else if (INT_PORT_IN & GYRO_INT1)
		gyro_isr();
}

/*****************************************************
FUNCTIONS READ/WRITE TO THE LIS3D ACCELEROMETER CHIP 
*****************************************************/
byte last_read = 0x00;
byte accel_read( byte address )
{
  SET_CS_ACCEL_LOW
  byte Address = (address | 0x80);	 // set read bit
  Address     &= ~(0x40);        	 // Clear the MS_ bit for single reg read (address remains same between sequences)
  spi_transfer(Address);			 // 
  last_read = spi_transfer(0x00);
  SET_CS_ACCEL_HIGH
  return last_read;
}

void accel_multi_read( byte address, byte* mData, byte mLength )
{
/* The LIS3D accelerometer increases the pointer after each byte */
/* The pattern then is 
	OUT_X_L, OUT_X_H, 
	OUT_Y_L, OUT_Y_H
	OUT_Z_L, OUT_Z_H	*/

  SET_CS_ACCEL_LOW
  byte Address = (address);
  Address |= 0x80;            // set read bit
  Address |= 0x40;            // Set the Multiple bit.  address increment by 1
  spi_transfer(Address);
  for (int i=0; i<mLength; i++)
  {
      mData[i] = spi_transfer(0xFF);
  }
  SET_CS_ACCEL_HIGH
}

void accel_write( byte address, byte value )
{
  // For LIS3D, SPI is setup for MSBFIRST.  Meaning RW is msb 
  // and MS is 2nd highest bit.
  SET_CS_ACCEL_LOW  
  // byte Address = (address)|0x40;	 // Write + Auto Increment
  spi_transfer(address);
  spi_transfer(value);
  SET_CS_ACCEL_HIGH
}

/************** CONFIGURATION ******************************/
// Use one of the defines (SAMPLE_10HZ) in the header.
void accel_data_rate(byte mDataRate )
{
	accel_write( CTRL_REG1, mDataRate|0x07 );
}
void accel_temperature_enable( )
{
	// Turns on aux ADC (external ADC123 pins)
	accel_write( TEMP_CFG_REG, 0xC0 );	
}
void accel_temperature_disable( )
{
	// Turns on aux ADC (external ADC123 pins)
	accel_write( TEMP_CFG_REG, 0x80 );
}

/************** MODES OF OPERATION *************************/
void accel_bypass_mode()
{ // measurements skip over the FIFO
	accel_write( FIFO_CTRL_REG, 0x00);		// bypass
}
void accel_FIFO_mode()
{ // 32 slots
	accel_write( FIFO_CTRL_REG, 0x40);		
}
void accel_stream_mode()
{ 
	accel_write( FIFO_CTRL_REG, 0x80);		
}
void accel_trigger_mode()	// stream_to_FIFO
{ 
	accel_write( FIFO_CTRL_REG, 0xC0);		
}

// 2g, 4g, 8g, 16g
void accel_set_sensitivity( byte Sensitivity )	
{
	accel_write( CTRL_REG4, Sensitivity<<4 );
}

/*void accel_retrieve_mode()
{ 
	accel_write( FIFO_CTRL_REG, 0x00);		
}*/

/******** XYZ DATA ********************************************/
word accel_read_X()
{
  short result = 0x00;
  result   = accel_read( OUT_X_L );
  byte tmp = accel_read( OUT_X_H );
  result   += (tmp<<8);
  return result;
}
word accel_read_Y()
{
  short result = 0x00;
  result   = accel_read( OUT_Y_L );
  byte tmp = accel_read( OUT_Y_H );
  result += (tmp<<8);
  return result;
}
word accel_read_Z()
{
  short result = 0x00;
  result   = accel_read( OUT_Z_L );
  byte tmp = accel_read( OUT_Z_H );
  result += (tmp<<8);
  return result;
}
void accel_read_xyz( uXYZ* mXYZ)
{
   accel_multi_read( OUT_X_L, mXYZ->array, 6 );
}

void swap_bytes(  uXYZ* mXYZ )
{
	short tmp = mXYZ->coords.x;
	mXYZ->coords.x = hi(tmp) + (lo(tmp)<<8);
	tmp = mXYZ->coords.y;
	mXYZ->coords.y = hi(tmp) + (lo(tmp)<<8);
	tmp = mXYZ->coords.z;
	mXYZ->coords.z = hi(tmp) + (lo(tmp)<<8);
}

byte accel_new_data_avail()
{
  byte result = accel_read( STATUS_REG_AUX );
/*  if ((result & 0x03) > 0)
    result = 1;
  else 
    result = 0; */
  return result;
}

void can_blank_accel_msg(byte m1, byte m2)
{
    msg2.id = create_CAN_eid( ID_ACCEL_XYZ, MyInstance );
    msg2.data[0] = m1;
    msg2.data[1] = m2;
    msg2.data[2] = 0x33;
    msg2.data[3] = 0x44;
    msg2.header.DLC = 4;
    msg2.header.rtr = 0;
}

/****** HIGH LEVEL FUNCTIONS **************************************/
// Guestimated to take less than 0.5ms !
void accel_timeslice()
{
   spi_setDataMode( SPI_MODE3 );
   byte avail = accel_new_data_avail();
   byte send  = isConfigured2(MODE_SEND_ACCEL);
   if ((avail&0x07) && (send))
   {
	  SET_LED_1(); 
      accel_read_xyz	  ( &accel 		  );
      can_prep_accel_msg  ( &msg2, &accel );
      can_send_msg_no_wait( 0, &msg2 	  );
      //can_send_msg( 0, &msg2 	  );
   }
   else 
   {
      can_blank_accel_msg ( avail, send	  );
      can_send_msg_no_wait( 0,     &msg2  );
   }

/*  When interrupts are being used: 
	if (ReadyToSend)
   {
	  can_prep_accel_msg( &msg2, &accel );
      can_send_msg( 0, &msg2 );
   } */
}
