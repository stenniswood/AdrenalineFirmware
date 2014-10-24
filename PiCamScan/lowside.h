#ifndef _LOWSIDE_H_
#define _LOWSIDE_H_

#ifdef  __cplusplus
extern "C" {
#endif


// MP1..7 pins are spread over 3 ports: PortB, PortC, PortD
#define LOWSIDE_DDR_1 DDRB
#define LOWSIDE_DDR_2 DDRC
#define LOWSIDE_DDR_3 DDRD

#define LOWSIDE_PORT_1 PORTB
#define LOWSIDE_PORT_2 PORTC
#define LOWSIDE_PORT_3 PORTD

#define LOWSIDE_1 (1<<3)		// Port B
#define LOWSIDE_2 (1<<4)		// Port B

#define LOWSIDE_3 (1<<0)		// Port 	C
#define LOWSIDE_4 (1<<0)		// Port 		D
#define LOWSIDE_5 (1<<1)		// Port 		D
#define LOWSIDE_6 (1<<6)		// Port 	C
#define LOWSIDE_7 (1<<7)		// Port 	C

void lowside_init    ( );
void set_lowside_byte( byte mValue );

void turn_output_on ( byte bit_number );
void turn_output_off( byte bit_number );


#ifdef  __cplusplus
}
#endif

#endif

