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



#define FOSC 1843200  // Clock Speed
#define BAUD 9600
#define MYUBRR (FOSC/16/BAUD-1)

struct stTxBuffer TxBuffer;
struct stRxBuffer RxBuffer;


void USART_Init( unsigned int ubrr)
{
	/* Set baud rate */
	UBRR0H = (unsigned char)(ubrr>>8);		// 51  = 0x33
	UBRR0L = (unsigned char)ubrr;			// 103 = 0x67  for 9600 at 8Mhz

	/* Enable receiver and transmitter */
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);

	/* Set frame format: 8data, 2stop bit */
	UCSR0C = (1<<USBS0)|(3<<UCSZ00);
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

}


void set_baud_rate()
{

}

void enable_transmit()
{

}
void enable_receiver()
{

}

void init_serial()
{
	RX_PIN_DDR 	&= ~(0x01);
	TX_PIN_DDR  |=   0x02;
	
	USART_Init( 0x3367 );

	//PRUSART0 &= ~USART0;
	PRR0 &= ~PRUSART0;	
}


BOOL send_message( char* mMessage )
{
/*	if (TxBuffer.busy) return FALSE;
	TxBuffer.transmitted_index = 0;
	TxBuffer.length = strlen(mMessage);
	strcpy (TxBuffer.buffer, mMessage);
	TxBuffer.busy = TRUE;		// ISR has to clear this flag
*/

	int length = strlen(mMessage);
	for (int i=0; i<length; i++)
	{
		/* Wait for empty transmit buffer */
		while ( !( UCSR0A & (1<<UDRE0)) );
		/* Put data into buffer, sends the data */
		UDR0 = mMessage[i];
	}
	return TRUE;
}


