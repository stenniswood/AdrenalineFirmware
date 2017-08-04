#include <avr/sfr_defs.h>
#include <avr/common.h>
#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
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

struct stTxBuffer TxBuffer;
struct stRxBuffer RxBuffer;

#define BACKSPACE_KEY 0x08
#define DELETE_KEY 	  0x7F

BOOL dont_add  =FALSE;
BOOL dont_echo = FALSE;
BOOL have_cr = FALSE;
BOOL have_lf = FALSE;	
char new_byte;


void set_baud( const unsigned int ubrr )
{
	const unsigned char ubrrh = (ubrr&0xFF00)>>8;
	const unsigned char ubrrl = (ubrr&0x00FF);	

	/* Set baud rate */
	UBRR0H = (unsigned char)(ubrrh);		// 51  = 0x33
	UBRR0L = (unsigned char)(ubrrl);			// 103 = 0x67  for 9600 at 8Mhz
}

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

	clear_RxBuffer();

	set_baud( 0x000C );			// if clock at 1mhz	- Go at 38400...
	//set_baud( 0x0033 );		// if clock at 1mhz

	/* Enable receiver and transmitter */
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	UCSR0B |= 0x80;		// RX Interrupt Enable

	/* Set frame format: 8data, N, 1stop bit - default */
	//UCSR0C = (3<<UCSZ00);	// (1<<USBS0)|

	//PRUSART0 &= ~USART0;
	PRR0 &= ~PRUSART0;	
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

enum arrow 
{
	none, up,down,left,right
};


BOOL have_escape	= FALSE;		// If we already have it keep it!
BOOL have_bracket	= FALSE;		// 
BOOL have_up		= FALSE;		// up
BOOL have_down		= FALSE;		// down	
BOOL have_left		= FALSE;		// left	
BOOL have_right		= FALSE;		// right	

enum arrow is_arrow(char mCharacter)
{
	have_escape  |= (mCharacter == 27);		// If we already have it keep it!
	have_bracket |= (mCharacter == '[');		// 
	have_up      |= (mCharacter == 'A');		// up
	have_down    |= (mCharacter == 'B');		// down	
	have_left    |= (mCharacter == 'D');		// left	
	have_right   |= (mCharacter == 'C');		// right	

	if ((have_escape && have_bracket) && have_left)	{
		have_escape = have_bracket = have_up = have_down = have_left = have_right = 0;
		return left;		
	}
	if ((have_escape && have_bracket) && have_right) {	
		have_escape = have_bracket = have_up = have_down = have_left = have_right = 0;
		return right;		
	}
	if ((have_escape && have_bracket) && have_up)	{
		have_escape = have_bracket = have_up = have_down = have_left = have_right = 0;
		return up;		
	} 
	if ((have_escape && have_bracket) && have_down)	{
		have_escape = have_bracket = have_up = have_down = have_left = have_right = 0;
		return down;		
	}
	return none;
}

BOOL is_deliminator(char mCharacter)
{
	have_cr |= (mCharacter == '\r');		// If we already have it keep it!
	have_lf |= (mCharacter == '\n');		// 
	
	/* Be careful if using a 2 byte deliminator (such as \r\n )
		if we key off of the '\r' then it leaves a '\n' in the buffer starting the next
		telegram. And are you sure the terminal sends them in that order, '\r\n' different
		both in opposite order!	[order doesn't matter with this method]
		terminals may only send 1 - ooops!! user is SOL!
		
	*/
	if (have_cr) {
//	if (have_cr && have_lf) {
//	if (new_byte == PROTOCOL_DELIMINATOR) {
		have_cr = FALSE;		// start over.
		have_lf = FALSE;
		return TRUE;			// but let caller know;
	}
	return FALSE;
}

void rx_insert_char( char mNewChar, int mIndex )
{
	int length = strlen( &(RxBuffer.buffer[mIndex]) );
	memmove( &(RxBuffer.buffer[mIndex+1]), &(RxBuffer.buffer[mIndex]), length);
	RxBuffer.buffer[mIndex] = mNewChar;
}

void rx_delete_char( int mIndex )
{
	int length = strlen( &(RxBuffer.buffer[mIndex]) );
	memmove( &(RxBuffer.buffer[mIndex]), &(RxBuffer.buffer[mIndex+1]), length);
}

ISR( USART0_RX_vect )
{
 	cli();
	char temp[15];
	// HANDLE RECEIVER ERRORS:
	unsigned char flags = UCSR0A;
	BOOL FrameError  = (flags & (0x10));		// Scrap this message
	BOOL DataOverrun = (flags & (0x08));		// 
	BOOL ParityError = (flags & (0x04));		// not used
	if ((FrameError) || (DataOverrun) || ParityError)
	{
		led_on( WARNING_LED );
		RxBuffer.index = 0;		// scrap this message!
	}

	dont_add  = FALSE;			// DON'T ADD TO RXBUFFER (ie. ignore the character)
	dont_echo = FALSE;			// DON'T SEND BACK TO THE TERMINAL (ie. don't want it displayed)	
	new_byte = UDR0;			// GET THE RECEIVED CHARACTER
		
		int RxLength = strlen( RxBuffer.buffer );
		
		enum arrow cursor_arrow = is_arrow(new_byte);		
		
		// If in an escape sequence, do not echo/add:
		if ((have_escape) && (cursor_arrow == none))
		{	sei(); return;		}
		
		// No action for up and down!
		if (cursor_arrow == up)  {
			send_message((char*)"\r>");
			RxBuffer.index = 0;
			dont_echo = TRUE;
			dont_add  = TRUE;			
		} 
		else if ((cursor_arrow == down)) {		// No action for up and down!
			dont_echo = TRUE;
			dont_add  = TRUE;
		}
		else if (cursor_arrow == right)  {
			if (RxBuffer.index < RxLength) 
			{
				RxBuffer.index++;			
				temp[0] = 0x1B;
				temp[1] = '[';
				temp[2] = 'C';
				temp[3] = 0;
				send_message( temp );			
			}
			dont_add = TRUE;
			dont_echo = TRUE;			
		} 
		else if (cursor_arrow == left) 
		{
			dont_add = TRUE;
			if (RxBuffer.index>0) {
				new_byte = BACKSPACE_KEY;
				RxBuffer.index--;		// moves current index back, but doesn't shorten the line!			
			} else 
				dont_echo = TRUE;		// b/c we are at beginning of line.
		} 
		else if ((new_byte==DELETE_KEY) || (new_byte==BACKSPACE_KEY))	// using screen, this is the Mac Delete key
		{
			dont_add = TRUE;
			if (RxBuffer.index>0) {
				new_byte = BACKSPACE_KEY;
// RxL=4;  index=4;  col=6;		Rxl==index b/c though index is zero based we point to the next byte of storage.
				RxBuffer.index--;		// moves current index back, but doesn't shorten the line!			
// RxL=4;  index=3;  col=6;

				// Now Delete the character (shifting remaining up)
				int len = (RxLength - (RxBuffer.index));
//=1
				memmove( &(RxBuffer.buffer[RxBuffer.index]),
						 &(RxBuffer.buffer[RxBuffer.index+1]), len);

				// Now Reprint line:
				temp[0] = new_byte;
// col=5;
				temp[1] = 0x1B;			// Erase to end of line.
				temp[2] = '[';
				temp[3] = 'K';
				temp[4] = 0;
				send_message( temp );
				send_message (&(RxBuffer.buffer[RxBuffer.index]) );
// how long is this?  want to move back that exact num of bytes.
// real length was reduce by 1; RxLength==>3;  index=3; 
				// now need to reposition cursor!
				int len3 = strlen( &(RxBuffer.buffer[RxBuffer.index]) );

				if (len3>0) {
					// how many characters to move back?				
					char tmp2[5];
					int move_back = len3;	// -2 b/c we already issue the backspace above.  and we want to go 1 back.
					if (move_back<0) move_back = 0;
					itoa(move_back, tmp2, 10);	 		
				
					temp[0] = 0x1B;			// backspace cursor to end of line.
					temp[1] = '[';
					temp[2] = 0;
					// insert count
					strcat(temp,tmp2);								
					strcat(temp, "D");
					send_message( temp );
				}
				dont_echo = TRUE;
			} else 
				dont_echo = TRUE;		// b/c we are at beginning of line.				
		}

		//sprintf(temp, "%2x ", new_byte);
		//send_message (temp);

		// Dont send the backspace when we are at beginning of the telegram:
		// Otherwise echo the character:!
		if (dont_echo==FALSE)		// ECHO BACK TO TERMINAL:	
		{
			/* Wait for empty transmit buffer */
			while ( !( UCSR0A & (1<<UDRE0)) );
			/* Put data into buffer, sends the data */
			UDR0 = new_byte;
		}
		
	if (RxBuffer.msg_complete == TRUE) 
	{	sei();	return;	}

	if (is_deliminator(new_byte)) {
		// Mark it for top_level processing (not to be done in this interrupt)
		RxBuffer.msg_complete = TRUE;	
	}
	else if (dont_add==FALSE)
	{
		if (RxBuffer.index >= RXBUFFER_LENGTH)	// safety check
			RxBuffer.index = 0;
		RxBuffer.buffer[RxBuffer.index] = new_byte;
		RxBuffer.index++;
		RxBuffer.buffer[RxBuffer.index] = 0;	// for future str functions!
	}
	sei();
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

void clear_RxBuffer()
{
	RxBuffer.msg_complete = FALSE;		// clear for next msg already!
	RxBuffer.index = 0;
	for (int i=0; i<RXBUFFER_LENGTH; i++)
	{
		RxBuffer.buffer[i] = 0;
	}
}

void serial_timeslice()
{
	if (RxBuffer.msg_complete == TRUE)
	{
		led_on ( SERIAL_LED  );
		led_off( WARNING_LED );
/*		send_message( (char*)"\r\nMessage:");
		send_message( (char*)RxBuffer.buffer );	
		send_message( (char*)"\r\n");
*/		
		top_level_caller( RxBuffer.buffer );
		clear_RxBuffer();		
		//send_prompt( );
	}	
}

/*0x48 = 0b 0100 1000 */
/* Blocks until full message is received 
	Sentence ends with a DElim*/
/*void receive_message( char* mMessage, int mMaxLength )
{
	*mMessage = 0;
	char* ptr = mMessage;
		
	BOOL delim = is_deliminator( *ptr );		// *ptr != PROTOCOL_DELIMINATOR
	while (!delim)
	{
		* Wait for receive buffer *
		while ( !(UCSR0A & (1<<RXC0)) );
		* Get and return received data from buffer *
		*ptr = UDR0;
		ptr++;
		delim = is_deliminator( *ptr );
	}
}*/
