#ifndef _ADREN_SPI_H_
#define _ADREN_SPI_H_

#ifdef  __cplusplus
extern "C" {
#endif

#define MSBFIRST 		0x00
#define LSBFIRST 		0x01

#define SPI_CLOCK_DIV4   0x00
#define SPI_CLOCK_DIV16  0x01
#define SPI_CLOCK_DIV64  0x02
#define SPI_CLOCK_DIV128 0x03
#define SPI_CLOCK_DIV2   0x04
#define SPI_CLOCK_DIV8   0x05
#define SPI_CLOCK_DIV32  0x06
//#define SPI_CLOCK_DIV64 0x07

#define SPI_CLK_POS_POLARITY 0x80	// 0=> CLK Idles low;  1=> CLK Idles high; 
#define SPI_CLK_2ND_PHASE	 0x40	// 0=> Sample on first edge;  1=> Sample on 2nd edge;
#define SPI_MODE0 		0x00
#define SPI_MODE1 		0x04
#define SPI_MODE2 		0x08
#define SPI_MODE3 		0x0C

#define SPI_MODE_MASK 	 0x0C  // CPOL = bit 3, CPHA = bit 2 on SPCR
#define SPI_CLOCK_MASK 	 0x03  // SPR1 = bit 1, SPR0 = bit 0 on SPCR
#define SPI_2XCLOCK_MASK 0x01  // SPI2X = bit 0 on SPSR

void spi_setBitOrder	( uint8_t bitOrder );
void spi_setDataMode	( uint8_t mode );
void spi_setClockDivider( uint8_t rate );
byte spi_transfer		( byte _data   );

// Pin definitions:
#define MISO (0x01)
#define MOSI (0x02)
#define SCK  (0x80)
#define SS   (0x08)

// Alternate Pin definitions:
#define MISO_A (0x04)		// PortD 
#define MOSI_A (0x08)		// PortD 
#define SCK_A  (0x10)		// PortD 
#define SS_A   (0x01)		// PortC bit 1

/*  NOTE: 
	Tilt board uses the regular SPI pins (MISO,MOSI,SCK) on PB0,PB1,PB7
   	ANALOG board uses the ALTERNATE SPI pins (MISO,MOSI,SCK) on PB0,PB1,PB7
   	The programming SPI pins always defaults to the alternate pins.	
*/
void spi_init( BOOL mAlternatePins, BOOL mMaster, uint8_t mbit_order, uint8_t mSPI_Mode );

byte spi_transfer		(byte _data	);
void spi_attachInterrupt(			);
void spi_detachInterrupt(			);

void set_spi_isr_callback( BOOL (*mCallback)(void) );


#ifdef  __cplusplus
}
#endif

#endif
