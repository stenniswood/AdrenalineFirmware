#ifndef _SERIAL_H_
#define _SERIAL_H_


#define SERIAL_PINS_DDR 	DDRE 
#define SERIAL_PINS_PORT	PORTE
#define SERIAL_PINS_PIN		PINE


#define TX_PIN  (1<<0)
#define RX_PIN  (1<<1)
#define RTS_PIN (1<<2)
#define CTS_PIN (1<<3)

#define RXBUFFER_LENGTH 512
#define TXBUFFER_LENGTH 512

struct stTxBuffer {
	char	buffer[TXBUFFER_LENGTH];
	int		transmitted_index;	// last one sent.
	int     length;
	int     busy;		// transmission in progress, don't load anymore into buffer.	
};

struct stRxBuffer {
	char	buffer[RXBUFFER_LENGTH];
	int     index;
	int     msg_complete;		// reading until terminating ';'
};


void init_serial   ( );
void clear_RxBuffer();
void send_prompt( );

void USART_Transmit( unsigned char data );


void receive_message( char* mMessage, int mMaxLength );
BOOL send_message   ( char* mMessage );
void serial_timeslice();


#endif
