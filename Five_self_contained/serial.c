#include <avr/sfr_defs.h>
#include <avr/common.h>
#include <avr/io.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "bk_system_defs.h"
#include "inttypes.h"
#include "interrupt.h"
#include "pin_definitions.h"
#include "pwm.h"
#include "serial.h"
#include "protocol.h"
#include "leds2.h"


#define PROTOCOL_DELIMINATOR ';'
#define FOSC 1843200  // Clock Speed
#define BAUD 9600
#define MYUBRR (FOSC/16/BAUD-1)

struct stTxBuffer TxBuffer;
struct stRxBuffer RxBuffer;


/*
FTxxx RTS# pin is an output.
If RTS# is logic 0 it is indicating the FTxxx device can accept more data on the RXD pin.
If RTS# is logic 1 it is indicating the FTxxx device cannot accept more data. 

FTxxx CTS# pin is an input. 
If CTS# is logic 0 it is indicating the external device can accept more data, and the FTxxx will transmit on the TXD pin.
If CTS# is logic 1 it is indicating the external device cannot accept more data. the FTxxx will stop transmitting within 0~3 characters, depending on what is in the buffer. 
*/

void init_serial( )
{
	SERIAL_PINS_DDR 	&= ~(RX_PIN);
	SERIAL_PINS_DDR		|=   TX_PIN;	
	SERIAL_PINS_DDR		&=  ~(RTS_PIN);		// output of FT230X
	SERIAL_PINS_DDR		|=   CTS_PIN;

	SERIAL_PINS_PORT &= ~(CTS_PIN);			// set CTS to zero always
	
//	const unsigned int ubrr = 0x3367;
	const unsigned int ubrr = 0x0033;		// if clock at 1mhz
	const unsigned char ubrrh = (ubrr&0xFF00)>>8;
	const unsigned char ubrrl = (ubrr&0x00FF);	

	/* Set baud rate */
	UBRR0H = (unsigned char)(ubrrh);		// 51  = 0x33
	UBRR0L = (unsigned char)(ubrrl);			// 103 = 0x67  for 9600 at 8Mhz

	/* Enable receiver and transmitter */
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	UCSR0B |= 0x80;		// RX Interrupt Enable
		
// clear global int flag....

	/* Set frame format: 8data, N, 1stop bit - default */
	//UCSR0C = (3<<UCSZ00);	// (1<<USBS0)|

	//PRUSART0 &= ~USART0;
	PRR0 &= ~PRUSART0;	
}

void clear_RxBuffer()
{
	RxBuffer.msg_complete = FALSE;		// clear for next msg already!
	RxBuffer.index = 0;
	for (int i=0; i<RXBUFFER_LENGTH; i++)
	{
		RxBuffer.buffer[i] = 0;
	}
}

void USART_Transmit( unsigned char data ) 
{
	/* Wait for empty transmit buffer */
	while ( !( UCSR0A & (1<<UDRE0)) );
	
	/* Put data into buffer, sends the data */
	UDR0 = data;
}

ISR( USART0_TX_vect )
{

}

ISR( USART0_RX_vect )
{
	// Check for Errors:
	unsigned char flags = UCSR0A;
	BOOL FrameError  = (flags & (0x10));		// Scrap this message
	BOOL DataOverrun = (flags & (0x08));		// 
	BOOL ParityError = (flags & (0x04));		// not used
	if ((FrameError) || (DataOverrun) || ParityError)
	{
		led_on( WARNING_LED );
		RxBuffer.index = 0;		// scrap this message!
	}

	char new_byte = UDR0;	
	if (RxBuffer.msg_complete == TRUE) 
		return;
	
	if (new_byte == PROTOCOL_DELIMINATOR)	{	// \new_byte
		RxBuffer.msg_complete = TRUE;	
		// Mark it for top_level processing (not to be done in this interrupt)
	} 
	else {
		RxBuffer.buffer[RxBuffer.index] = new_byte;
		RxBuffer.index++;
	}

}


/* Blocks until full message is received 
	Sentence ends with a DElim
*/
void receive_message( char* mMessage, int mMaxLength )
{
	*mMessage = 0;
	char* ptr = mMessage;	
	while (*ptr != PROTOCOL_DELIMINATOR)
	{
		/* Wait for receive buffer */
		while ( !(UCSR0A & (1<<RXC0)) );
		/* Get and return received data from buffer */
		*ptr = UDR0;
		ptr++;
	}
}

/*
FTxxx RTS# pin is an output.
If RTS# is logic 0 it is indicating the FTxxx device can accept more data on the RXD pin.
If RTS# is logic 1 it is indicating the FTxxx device cannot accept more data. 

FTxxx CTS# pin is an input. 
If CTS# is logic 0 it is indicating the external device can accept more data, and the FTxxx will transmit on the TXD pin.
If CTS# is logic 1 it is indicating the external device cannot accept more data. the FTxxx will stop transmitting within 0~3 characters, depending on what is in the buffer. 
*/

/* Blocks until full message is sent */
BOOL send_message( char* mMessage )
{
	int length = strlen(mMessage);
	for (int i=0; i<length; i++)
	{
		/* We also want to check the RTS signal on the USB-serial chip. */
		int rts = (SERIAL_PINS_PIN & RTS_PIN);
		if (rts==0) {
			/* Wait for empty transmit buffer */
			while ( !( UCSR0A & (1<<UDRE0)) );
			/* Put data into buffer, sends the data */
			UDR0 = mMessage[i];
		}
	}
	return TRUE;
}
void send_prompt( )
{
	send_message( ">" );
}

void serial_timeslice()
{
	// From Serial.c :
	if (RxBuffer.msg_complete == TRUE)
	{
		led_on( SERIAL_LED );
		led_off( WARNING_LED );
		strcat(RxBuffer.buffer, "\r\n");
		BOOL ok = send_message( RxBuffer.buffer );	
		
		top_level( RxBuffer.buffer );
		clear_RxBuffer();		
	}

	
	if (ResponseReady) 
	{
		BOOL ok = send_message( Response );	
		led_off( SERIAL_LED );
		ResponseReady = FALSE;
		send_prompt( );			
	}
}

/*
0x48 = 0b 0100 1000 
 


*/