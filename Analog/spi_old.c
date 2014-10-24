#include <avr/sfr_defs.h>
#include "iom16m1.h"
#include "pin_definitions.h"
#include "spi.h"


#define MSBFIRST 		 	0x00
#define LSBFIRST 		 	0x01

#define SPI_CLOCK_DIV4   	0x00
#define SPI_CLOCK_DIV16  	0x01
#define SPI_CLOCK_DIV64  	0x02
#define SPI_CLOCK_DIV128 	0x03
#define SPI_CLOCK_DIV2   	0x04
#define SPI_CLOCK_DIV8   	0x05
#define SPI_CLOCK_DIV32  	0x06
//#define SPI_CLOCK_DIV64 	0x07

#define SPI_MODE0 			0x00
#define SPI_MODE1 			0x04
#define SPI_MODE2 			0x08
#define SPI_MODE3 			0x0C

#define SPI_MODE_MASK 	 	0x0C  // CPOL = bit 3, CPHA = bit 2 on SPCR
#define SPI_CLOCK_MASK   	0x03  // SPR1 = bit 1, SPR0 = bit 0 on SPCR
#define SPI_2XCLOCK_MASK 	0x01  // SPI2X = bit 0 on SPSR


void spi_setBitOrder( uint8_t bitOrder )
{
  if(bitOrder == LSBFIRST)
      SPCR |= _BV(DORD);
  else
     SPCR &= ~(_BV(DORD));
}

void spi_setDataMode( uint8_t mode )
{
  SPCR = (SPCR & ~SPI_MODE_MASK) | mode;
}

void spi_setClockDivider( uint8_t rate )
{
  SPCR = (SPCR & ~SPI_CLOCK_MASK) | (rate & SPI_CLOCK_MASK);
  SPSR = (SPSR & ~SPI_2XCLOCK_MASK) | ((rate >> 2) & SPI_2XCLOCK_MASK);
}


//***** SPI ialization *****************************************************
void spi_init(void)
{
  // Set direction register for SCK and MOSI pin.
  // MISO pin automatically overrides to INPUT.
  // When the SS pin is set as OUTPUT, it can be used as
  // a general purpose output port (it doesn't influence
  // SPI operations).

  // NOTE: Tilt board uses the regular SPI pins (MISO,MOSI,SCK) on PB0,PB1,PB7
  // NOTE: ANALOG board uses the ALTERNATE SPI pins (MISO,MOSI,SCK) on PB0,PB1,PB7
  // This is different than the probramming SPI which defaults to the alternate pins.	

  // DDR  CHIP SELECT PINS (OUTPUT)
  CS_DDR  |= CS_DDR_MASK;
  CS_PORT |= CS_DDR_MASK;		// ALL HIGH for INIT

  
  // DDR  MISO,MOSI,SCLK pins
  DDRB  |=  (MOSI | SCLK);  	// output
  DDRB  &=  ~MISO;				// input

  PORTB &=  ~MOSI;
  PORTB &=  ~SCLK;

  /************************ WARNING!! *****************************
   No need to modify MCUCR reigsters.  Default is '0' which means
   the SPI to use regular MISO/MOSI, not the alternate pins 
   NOTE: The Serial Progamming always uses the Alternate pins.
   ****************************************************************/
  MCUCR |= SPIPS;        // Clear for MOSI.  Set for MOSI_A

  spi_setBitOrder( MSBFIRST  );		// LIS3d & CMR3000 (accel & gyro) both require msb first.
  spi_setDataMode( SPI_MODE3 );		// FOR LIS3D Accelerometer, SPI_MODE3 is required.
									// CMR3000 wants SPI_MODE0, but sample is done on rising edge as in Mode3
  spi_setClockDivider( SPI_CLOCK_DIV128 );
  
  // Warning: if the SS pin ever becomes a LOW INPUT then SPI 
  // automatically switches to Slave, so the data direction of 
  // the SS pin MUST be kept as OUTPUT.
  SPCR |= _BV(MSTR);		// Master SPI
  SPCR |= _BV(SPE);			// Enable SPI Module
}

byte spi_transfer(byte _data) 
{
  SPDR = _data;
  while (!(SPSR & _BV(SPIF)));
  return SPDR;
}

void spi_attachInterrupt()
{
  SPCR |= _BV(SPIE);
}

void spi_detachInterrupt() 
{
  SPCR &= ~_BV(SPIE);
}


