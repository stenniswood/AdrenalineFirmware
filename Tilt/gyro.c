#include <avr/sfr_defs.h>
#include "bk_system_defs.h"
#include "iom16m1.h"
#include "pin_definitions.h"
#include "spi.h"
#include "leds.h"
#include "can.h"
#include "can_msg.h"
#include "gyro.h"
#include "accelerometer.h"
#include "configuration.h"
#include "calibrations.h"


union uXYZ gyro;

/*************************************************************
*  CMR3000 Gyro - every transfer is 16 bits.
*  (msb first - big endian)  
* 6 bit address
* 1 bit Read/Write='1'
* 1 bit '0'
* Data bits 7..0 for write operation.  Don't care X for reads 
* INPUT:  address  - 6 bit in lsbs.  [0..63]
*       value - 8 bit data to be written
USES SPI MODE 0 Clk Polarity=0 (idles low)
	            Clk Phase : Sample data on Rising edge of Clk.
**************************************************************/
void gyro_init()
{
	spi_setDataMode( SPI_MODE0 );
	CS_DDR |= CS_GYRO;
	INT_DDR &= ~(GYRO_INT1);		// Make Interrupt line an input.

	gyro_reset();
	delay(one_second/2);
	gyro_disable_i2c();
	delay(one_second/2);	
	gyro_fast_measure_mode();
	delay(one_second/2);	
}



// See ISR( PCINT1_vect ) in "accelerometer.c" for gyro vector
//			it calls this function:
void gyro_isr()
{
}

/**************************************** 
*  CMR3000 gyroerometer - every transfer is 16 bits.
*  (msb first - big endian)  
* 6 bit address
* 1 bit Read/Write='1'
* 1 bit '0'
* Data bits 7..0 for write operation.  Don't care X for reads 
* INPUT:  address  - 6 bit in lsbs.  [0..63]
*       value - 8 bit data to be written
************************************************/
byte gyro_read( byte address )
{
  SET_CS_GYRO_LOW 
  byte Address = (address << 2);
  spi_transfer(Address);
  byte retval = spi_transfer(0xFF);
  SET_CS_GYRO_HIGH
  return retval;
}

void gyro_multi_read( byte address, byte* mData, byte mLength )
{
  SET_CS_GYRO_LOW  
  byte Address = (address << 2);
  spi_transfer(Address);
  for (int i=0; i<mLength; i++)
    mData[i] = spi_transfer(0xFF);
  SET_CS_GYRO_HIGH
}

/************************************************
*  CMR3000 Gyro - every transfer is 16 bits.
*  (msb first - big endian)
* 6 bit address
* 1 bit Read/Write='1'
* 1 bit '0'
* Data bits 7..0 for write operation.  Don't care X for reads 
* INPUT:  address  - 6 bit in lsbs.  [0..63]
*       value - 8 bit data to be written
************************************************/
void gyro_write( byte address, byte value )
{
  SET_CS_GYRO_LOW  
  byte Address = (address << 2) + 0x02;
  spi_transfer(Address);
  spi_transfer(value);
  SET_CS_GYRO_HIGH
}

/***************************************************/
void gyro_disable_i2c()
{
   // READ/MODIFY/WRITE:
   byte prevValue = gyro_read( CTRL );
   prevValue |= I2C_DIS;
   prevValue |= INT_DIS;
   gyro_write( CTRL, prevValue );
}
void gyro_reset()
{
  gyro_write( CTRL, RESET ); 
  delay(2);
}

/********** POWER MODING *********************/
void gyro_set_mode( byte Mode )
{
   byte prevValue = gyro_read( CTRL );
   prevValue &= 0xF9;
   prevValue |= (Mode << 1);
   gyro_write(CTRL, prevValue );
}

// Longer time required to wakeup than standby.
void gyro_powerdown()
{
  gyro_set_mode( 0 );
}
// quick recovery to measurement mode.
void gyro_standby()
{
  gyro_set_mode( 1 );
}
void gyro_measurement_mode()
{
  gyro_set_mode( 2 );  // BandWidth = 20Hz
}
void gyro_fast_measure_mode()
{
  gyro_set_mode( 3 );  // BandWidth = 80Hz
}

void gyro_int_enable(  )
{
   // READ/MODIFY/WRITE:
   byte prevValue = gyro_read( CTRL );
   prevValue |= (INT_DIS | INT_LEVEL);
   gyro_write( CTRL, prevValue );
}

word gyro_read_x(  )
{
	byte msb = gyro_read( X_MSB ) ;//& 0x1F;
	word X   = gyro_read( X_LSB );
	X |= (msb<<8);
	return X;
}
word gyro_read_y(  )
{
	byte msb = gyro_read( Y_MSB );// & 0x1F;
	word Y   = gyro_read( Y_LSB );
	Y |= (msb<<8);
	return Y;
}
word gyro_read_z(  )
{
	byte msb = gyro_read( Z_MSB );  // & 0x1F;
	word Z   = gyro_read( Z_LSB );
	Z |= (msb<<8);
	return Z;
}

void gyro_read_xyz( uXYZ* mData )
{
    // multiple reads decrement, so start highest.
	gyro_multi_read( X_LSB, mData->array, 6 );		// Z_MSB
}

void gyro_timeslice( )
{
	spi_setDataMode( SPI_MODE0 );
	byte send = isConfigured2(MODE_SEND_GYRO);
	if (send)
	{
		SET_LED_2();
		gyro.coords.x = gyro_read_x();
		gyro.coords.y = gyro_read_y();
		gyro.coords.z = gyro_read_z();
		//gyro_read_xyz( &gyro 	 );
		can_prep_gyro_msg( &msg1, &gyro );
		can_send_msg_no_wait( 2, &msg1  );
	}
	spi_setDataMode  ( SPI_MODE3 );
}


#include "can_eid.h"

void can_init_test_msg(byte m1, byte m2, byte m3, byte m4)
{
	static word count = 0;
	count++;
    msg1.id 	 = create_CAN_eid( 0x02, 0x0004, 0x01);
    msg1.data[0] = m1;
    msg1.data[1] = m2;    
    msg1.data[2] = m3;
    msg1.data[3] = m4;
    
    msg1.data[4] = hi(count);
    msg1.data[5] = lo(count);
    msg1.data[6] = 0xEE;
    msg1.data[7] = 0xFF;
    msg1.header.DLC = 8;
    msg1.header.rtr = 0;
}

void gyro_tests()
{
	spi_setDataMode  ( SPI_MODE0 );
  // VERIFY OPERATION BY READING REG & SETTING LED IF MATCH:
  byte result1 = gyro_read( GYRO_WHO_AM_I );		//0x43
  byte result2 = gyro_read( REVID );				//0x21
  byte result3 = gyro_read( CTRL );  
  byte result4 = gyro_read( STATUS );    
  SET_LED_2()
  can_init_test_msg( result1, result2, result3, result4 );
  
//  result = gyro_read( REVID );
//  if (result == 0x33)  digitalWrite( LED_2, HIGH );
	//can_prep_gyro_msg( &msg2, &gyro );
	can_send_msg_no_wait( 0, &msg1  );
	delay(one_second);
}
