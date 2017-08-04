#ifndef _SERIAL_H_
#define _SERIAL_H_


#define RX_PIN_DDR 	DDRE 
#define RX_PIN_PORT PORTE

#define TX_PIN_DDR  DDRE 
#define TX_PIN_PORT PORTE


struct stTxBuffer {
	char	buffer[255];
	int		transmitted_index;	// last one sent.
	int     length;
	int     busy;		// transmission in progress, don't load anymore into buffer.	
};

struct stRxBuffer {
	char	buffer[255];
	int     bytes_received;
	int     busy;		// reading until terminating ';'
};


void init_serial();
void USART_Transmit( unsigned char data );

BOOL send_message  ( char* mMessage );


#endif
