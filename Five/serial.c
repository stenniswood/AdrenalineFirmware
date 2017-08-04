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
//#define FOSC 8000000  // Clock Speed
//#define BAUD 38400
//#define MYUBRR (FOSC/16/BAUD-1)

struct stTxBuffer TxBuffer;
struct stRxBuffer RxBuffer;


#define BACKSPACE_KEY 0x08
#define DELETE_KEY 	  0x7F


/*
FTxxx RTS# pin is an output.
If RTS# is logic 0 it is indicating the FTxxx device can accept more data on the RXD pin.
If RTS# is logic 1 it is indicating the FTxxx device cannot accept more data. 

FTxxx CTS# pin is an input. 
If CTS# is logic 0 it is indicating the external device can accept more data, and the FTxxx will transmit on the TXD pin.
If CTS# is logic 1 it is indicating the external device cannot accept more data. the FTxxx will stop transmitting within 0~3 characters, depending on what is in the buffer. 
*/

void set_baud( const unsigned int ubrr )
{
//	const unsigned int ubrr = 0x3367;
//	const unsigned int ubrr = 0x0033;		

	const unsigned char ubrrh = (ubrr&0xFF00)>>8;
	const unsigned char ubrrl = (ubrr&0x00FF);	

	/* Set baud rate */
	UBRR0H = (unsigned char)(ubrrh);		// 51  = 0x33
	UBRR0L = (unsigned char)(ubrrl);			// 103 = 0x67  for 9600 at 8Mhz
}

void init_serial( )
{
	SERIAL_PINS_DDR 	&= ~(RX_PIN);
	SERIAL_PINS_DDR		|=   TX_PIN;	
	SERIAL_PINS_DDR		&=  ~(RTS_PIN);		// output of FT230X
	SERIAL_PINS_DDR		|=   CTS_PIN;

	SERIAL_PINS_PORT &= ~(CTS_PIN);			// set CTS to zero always
	
	set_baud( 0x000C );		// if clock at 1mhz	- Go at 38400...
	//set_baud( 0x0033 );		// if clock at 1mhz

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

BOOL is_deliminator(char mCharacter)
{
	static BOOL have_cr = FALSE;
	static BOOL have_lf = FALSE;	
	
	have_cr |= (mCharacter == '\r');		// If we already have it keep it!
	have_lf |= (mCharacter == '\n');		// 
	
	/* Be careful if using a 2 byte deliminator (such as \r\n )
		if we key off of the '\r' then it leaves a '\n' in the buffer starting the next
		telegram. And are you sure the terminal sends them in that order, '\r\n' different
		both in opposite order!	[order doesn't matter with this method]
		terminals may only send 1 - ooops!! user is SOL!
		
	*/
	if (have_cr && have_lf) {
//	if (new_byte == PROTOCOL_DELIMINATOR) {
		have_cr = FALSE;		// start over.
		have_lf = FALSE;
		return TRUE;			// but let caller know;
	}
	return FALSE;
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
	
		// ECHO BACK TO TERMINAL:
			// BACKSPACE Special:
			if ((new_byte==BACKSPACE_KEY) || (new_byte==DELETE_KEY))
				new_byte = '#';			// Send something so that display is in sync with telegram internally.				

			/* Wait for empty transmit buffer */
			while ( !( UCSR0A & (1<<UDRE0)) );
			/* Put data into buffer, sends the data */
			UDR0 = new_byte;

//	if (RxBuffer.msg_complete == TRUE) 
//		return;

////	if (new_byte == PROTOCOL_DELIMINATOR)	{	
//	if (is_deliminator(new_byte)) {
//		RxBuffer.msg_complete = TRUE;	
		// Mark it for top_level processing (not to be done in this interrupt)
//	} 
//	else {
		RxBuffer.buffer[RxBuffer.index] = new_byte;
		RxBuffer.index++;
//	}
}


/* Blocks until full message is received 
	Sentence ends with a DElim
*/
void receive_message( char* mMessage, int mMaxLength )
{
	*mMessage = 0;
	char* ptr = mMessage;
		
	BOOL delim = is_deliminator( *ptr );		// *ptr != PROTOCOL_DELIMINATOR
	while (!delim)
	{
		/* Wait for receive buffer */
		while ( !(UCSR0A & (1<<RXC0)) );
		/* Get and return received data from buffer */
		*ptr = UDR0;
		ptr++;
		delim = is_deliminator( *ptr );
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
	send_message( (char*)">" );
}

void serial_timeslice()
{
	// From Serial.c :
	if (RxBuffer.msg_complete == TRUE)
	{
		led_on( SERIAL_LED );
		led_off( WARNING_LED );
		send_message( (char*)"\r\n" );	
		 
		top_level( RxBuffer.buffer );
		clear_RxBuffer();		
	}
	
	if (ResponseReady) 
	{
		send_message( Response );	
		led_off( SERIAL_LED );
		ResponseReady = FALSE;
		send_prompt( );			
	}
}

/*
0x48 = 0b 0100 1000 
 
*/
