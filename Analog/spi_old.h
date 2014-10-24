#ifndef _ADREN_SPI_H_
#define _ADREN_SPI_H_


void spi_init			( void				);
void spi_setBitOrder	( uint8_t bitOrder	);
void spi_setDataMode	( uint8_t mode		);
void spi_setClockDivider( uint8_t rate		);
byte spi_transfer		( byte    _data		);

void spi_attachInterrupt(					);
void spi_detachInterrupt(					);


#endif

