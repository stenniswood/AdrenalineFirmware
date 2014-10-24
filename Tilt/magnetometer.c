#include <avr/sfr_defs.h>
#include "bk_system_defs.h"
#include "iom16m1.h"
#include "pin_definitions.h"
#include "spi.h"
#include "leds.h"
#include "can.h"
#include "can_msg.h"
#include "magnetometer.h"
#include "configuration.h"
#include "calibrations.h"


union uXYZ magnet;


void mag_init()
{  
	spi_setDataMode( SPI_MODE3 );
	CS_DDR |= CS_MAGNET;
	
	/* Note:  SAMPLE_30_HZ results in all FF's for the data.
			  Not sure why this sample rate does not work. 
			  SAMPLE_15_HZ results in all not all FF's but unchanging data. (even when sensor direction is changed).
			  SAMPLE_75_HZ produces data which appears correct.
			  */
	mag_set_data_rate  ( SAMPLE_75_HZ );	
	mag_average_samples( AVG_2_SAMPLE );
	mag_write( MODE_REGISTER, 0X00 );		// Continuous Measure Mode
}

/*********************** READ & WRITE ROUTINES ***********************/
byte mag_read( byte address )
{
  SET_CS_MAGNET_LOW  
  byte Address = (address | 0x80);
  spi_transfer(Address);
  byte retval = spi_transfer(0xFF);
  SET_CS_MAGNET_HIGH
  return retval;
}

void mag_multi_read( byte address, byte* mData, byte mLength )
{
  SET_CS_MAGNET_LOW  
  byte Address = (address | 0x80 | 0x40 );
  spi_transfer(Address);
  for (int i=0; i<mLength; i++) {
      mData[i] = spi_transfer(0xFF);
      //mData[i] = i;
      }
  SET_CS_MAGNET_HIGH
}

void mag_write( byte address, byte value )
{
  SET_CS_MAGNET_LOW  
  byte Address = (address << 2) + 0x02;
  spi_transfer(Address);
  spi_transfer(value);
  SET_CS_MAGNET_HIGH
}

void mag_read_xyz( uXYZ* mXYZ )
{
  mag_multi_read(DATA_OUTPUT_X_HI, mXYZ->array, 6 );
}

/******************** MISC ROUTINES ******************************/
void mag_enable_temperature( byte mDataRate)
{
	byte cfg_a = mag_read( CONFIG_REG_A );
	byte value = (cfg_a | TS);
	mag_write( CONFIG_REG_A, value );
}
void mag_disable_temperature( byte mDataRate)
{
	byte cfg_a = mag_read( CONFIG_REG_A );
	byte value = (cfg_a & (~TS));
	mag_write( CONFIG_REG_A, value );
}

void mag_average_samples( byte mNConst )
{
	byte cfg_a = mag_read( CONFIG_REG_A );
	byte value = (cfg_a & 0x9F);
	value |= (mNConst<<5);
	mag_write( CONFIG_REG_A, value );
}

void mag_set_data_rate( byte mSampleRate)
{
	byte cfg_a = mag_read( CONFIG_REG_A );
	byte value = (cfg_a & 0x63);
	value |= (mSampleRate<<2);
	mag_write( CONFIG_REG_A, value );
}

void mag_set_gain( byte mGain )
{
	if (mGain > MAX_GAIN) return;
	mag_write( CONFIG_REG_B, (mGain<<5) );
}

void mag_timeslice( )
{
	byte send = isConfigured2(MODE_SEND_MAGNET);
	if (send)
	{	
		SET_LED_3();
		spi_setDataMode	    ( SPI_MODE3 	 );
		mag_read_xyz        ( &magnet 		 );
		can_prep_magnet_msg ( &msg2, &magnet );
		can_send_msg_no_wait( 4, &msg2 		 );
	}
}

byte mag_id_tests( )
{
  // VERIFY OPERATION BY READING REG & SETTING LED IF MATCH:
  byte result = mag_read( ID_REG_A );
  if (result == 'H') {
	  result = mag_read( ID_REG_B );
	  if (result == '4') {
	  		result = mag_read( ID_REG_C );
	  		if (result == '3')
				return TRUE;
	  }
  }
  return FALSE;
}

