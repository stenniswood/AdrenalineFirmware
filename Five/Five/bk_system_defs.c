#include <avr/sfr_defs.h>
#include "inttypes.h"
#include "interrupt.h"
#include "bk_system_defs.h"


// Use volatile so that the compiler wont optimize the delay loop out
//volatile long int time_delay = 0;
void delay( long int mdelay )
{
	long int time_delay = 0;
 	 for (time_delay=0; time_delay<mdelay; time_delay++) {  };
}

//***** chip Initialization **********************************
void chip_init(void)
{
// If using crystal oscillator of 16 Mhz   
//   CLKPR = ( 1 << CLKPCE );       // Set Clock Prescaler change enable
//   CLKPR = 0x01;            		// Clock Division Factor of 2
// Note - CLKPR must be written within 4 clock cycles of setting CLKPCE
   DDRB = 0xFF;    // PORT B to be outputs for LED monotoring
   DDRC = 0x00;    // Inputs, not used
   DDRE = 0x00;    // Inputs, not used
   PORTB = 0xFE;   // ALL LEDs OFF
   PORTC = 0x00;   // Inputs, not used
   PORTD = 0x00;   // Inputs, not used
   PORTE = 0x00;   // Inputs, not used
   //PRR   = 0x00;   // Individual peripheral clocks enabled
}
