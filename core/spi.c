/*********************************************************************
Product of Beyond Kinetics, Inc
This core code handles SPI communications hardware

Notes on Initial Debug with PWM generator board:
		Several problems in the "core" spi.c file.
	A)	The ddr pins were originally with MOSI and not (1<<MOSI).
		This cause the SCLK (1<<SCLK) to not operate
	B)  Have to force SS pin to be an output to prevent it 
		from going into slave mode.  Believe this is why
		the SPIF flag was not getting set; module went into 
		slave mode and was expecting an external clock!
	C)  The alternate pins also were missing the 1<< (1<<SPIPS)

	After this, the motor ran! 

DATE 	:  9/23/2013
AUTHOR	:  Stephen Tenniswood
********************************************************************/
#include <stdlib.h>
#include <avr/sfr_defs.h>
#include <avr/common.h>
#include "inttypes.h"
#include "interrupt.h"
#include "bk_system_defs.h"
#include "pin_definitions.h"
#include "spi.h"
#include "leds.h"


#define SPI_PORT PORTB
#define SPI_DDR  DDRB
#define SS_DDR 	 DDRD
#define SS_PORT  PORTD

BOOL (*spi_isr_call_back)(void) = NULL;       // Call back function for system msg processing 

/*****************************************************************************
Name		: set_system_rx_callback()
INPUT 		: mCallback - a function ptr to receive incoming CAN msgs
OUTPUT		: none
RETURN		: none
DESCRIPTION:  CAN ISR will call the function sent here on a Receive interrupt
			  which has a system message ID
******************************************************************************/
void set_spi_isr_callback( BOOL (*mCallback)(void) )
{
	//printf("set_spi_isr_callback(%d)\n", mCallback);
	spi_isr_call_back = mCallback;
}

//#include "rpi_spi.h"
//#include "servos.h"

/******* SPI Transfer Complete ISR ***************/
ISR ( SPI_STC_vect )
{	
//	if (spi_isr_call_back)
//		spi_isr_call_back();

	SPSR &= 0x7F;
//test_rx_response( SPDR );		here for the PiCamScan board
}

char SPI_SlaveReceive(void) 
{
	/* Wait for reception complete */ 
	while(!(SPSR & (1<<SPIF)));
	/* Return data register */ 
	return SPDR;
}

// Set LSBFIRST or MSBFIRST
void spi_setBitOrder( uint8_t bitOrder )
{
  if(bitOrder == LSBFIRST)
      SPCR |= (1<<DORD);
  else
     SPCR &= ~((1<<DORD));
}

/************************************************************
mode	:	[SPI_MODE0..SPI_MODE3] or
			you can or in (SPI_CLK_POLARITY | SPI_CLK_PHASE)
			
Modes [0..4] have to do with the same polarity and phase.
*************************************************************/
void spi_setDataMode( uint8_t mode )
{
  SPCR = (SPCR & ~SPI_MODE_MASK) | mode;
}

// Sets BaudRate:
/* rate:  [0..7]  */
void spi_setClockDivider( uint8_t rate )
{
  SPCR = (SPCR & ~SPI_CLOCK_MASK)   | (rate & SPI_CLOCK_MASK);
  SPSR = (SPSR & ~SPI_2XCLOCK_MASK) | ((rate >> 2) & SPI_2XCLOCK_MASK);
}

// Set direction register for SCK and MOSI pin.
// MISO pin automatically overrides to INPUT.
// When the SS pin is set as OUTPUT, it can be used as
// a general purpose output port (it doesn't influence
// SPI operations)
void pin_ddr( BOOL mAlternatePins, BOOL mMaster )
{
  /************************ WARNING!! *****************************
   No need to modify MCUCR reigsters.  Default is '0' which means
   the SPI to use regular MISO/MOSI, not the alternate pins 
   NOTE: The Serial Progamming always uses the Alternate pins.
   ****************************************************************/
  if (mAlternatePins)
  {
  	MCUCR |= (1<<SPIPS); 				        // Clear for MOSI.  Set for MOSI_A
	if (mMaster)
	{
		  /* Set MISO output, all others input */
		  // OUTPUTS (MISO)
		  DDRD &= ~MISO_A;
		  // INPUTS  (MOSI, SCK, SS_)
		  DDRD |= MOSI_A|SCK_A;
		  DDRC &= ~SS_A;	
	}
	else {	
		  /* Set MISO output, all others input */
		  // OUTPUTS (MISO)
		  DDRD |= MISO_A;

		  // INPUTS  (MOSI, SCK, SS_)
		  DDRD &= ~(MOSI_A|SCK_A);
		  DDRC &= ~SS_A;
	}  
  }
  else
  {
  	  //MCUCR &= ~(1<<SPIPS); 				    // Clear for MOSI.  Set for MOSI_A
	  if (mMaster)
	  {
		  /* Set MISO,SS input, all others output */
		  // OUTPUTS (MISO)
		  DDRB &= ~MISO;
		  // INPUTS  (MOSI, SCK, SS_)
		  DDRB |= (MOSI|SCK);
		  DDRD |= SS;
	  } else {
		  /* Set MISO output, all others input */
		  // OUTPUTS (MISO)
		  DDRB |= MISO;
		  // INPUTS  (MOSI, SCK, SS_)
		  DDRB &= ~(MOSI|SCK);
		  DDRD &= ~SS;		  
	  }
  }
}
//***** SPI initialization *****************************************************

/****************************************************************************
INPUT:
	mAlternatePins : Use the pins on PortD instead
	mMaster			: TRUE=Master Mode;  FALSE=Slave Mode;
	
	mbit_order	:	[MSGFIRST,LSBFIRST]  
	mSPI_Mode	:	SPI mode [SPI_MODE0..SPI_MODE3]
****************************************************************************/
void spi_init( BOOL mAlternatePins, BOOL mMaster, uint8_t mbit_order, uint8_t mSPI_Mode )
{
	pin_ddr(mAlternatePins, mMaster);
	spi_setDataMode( mSPI_Mode  );		// FOR LIS3D Accelerometer, SPI_MODE3 is required.
										// CMR3000 wants SPI_MODE0, but sample is done on rising edge as in Mode3
	spi_setBitOrder( mbit_order );		// LIS3d & CMR3000 (accel & gyro) both require msb first.

	// Warning: if the SS pin ever becomes a LOW INPUT then SPI 
	// automatically switches to Slave, so the data direction of 
	// the SS pin MUST be kept as OUTPUT.
	if (mMaster)
	{
	  SPCR |= (1<<MSTR);		// Master SPI
	  spi_setClockDivider( SPI_CLOCK_DIV64 );
	} else {
	  SPCR &= ~(1<<MSTR);		// Slave SPI 
	  spi_attachInterrupt();
	}
	SPCR |= (1<<SPE);				// Enable SPI Module
}

byte spi_transfer(byte _data) 
{
  SPDR = _data;
  // 0xC0 = (1<<SPIF) | (1<<WCOL)
  while (! (SPSR&0xC0) ) {};
  return SPDR;
}

void spi_attachInterrupt() 
{
  SPCR |= (1<<SPIE);
}

void spi_detachInterrupt() 
{
  SPCR &= ~(1<<SPIE);
}


/*
  // PORTD bit 3 is the SS for normal pins.
  SS_DDR  |= (1<<SS);		// we have to make SS an output or it could go into slave mode!
  SS_PORT |= (1<<SS);		// 

  // DDR  CHIP SELECT PINS (OUTPUT)
  CS_DDR  |= CS_DDR_MASK;		
  CS_PORT |= CS_DDR_MASK;		// ALL HIGH for INIT

  // DDR  MISO,MOSI,SCLK pins
  SPI_DDR  |=  ((1<<MOSI) | (1<<SCLK));  	// output
  SPI_DDR  &=  ~(1<<MISO);					// input

  SPI_PORT &=  ~(1<<MOSI);					//
  SPI_PORT &=  ~(1<<SCLK);					//
*/