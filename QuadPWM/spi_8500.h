#ifndef _SPI_8500_H_
#define _SPI_8500_H_

// 48 x 12 = 576 => 72 bytes
#define NUM_BYTES_48PWMS (48*12/8)

#define LOWER_LDIBLANK() { CONTROL_PORT &= ~(1<<LDI); }
#define RAISE_LDIBLANK() { CONTROL_PORT |= (1<<LDI);  }

#define LOWER_OPEN_LED() { CONTROL_PORT &= ~(1<<OPEN_LED); }
#define RAISE_OPEN_LED() { CONTROL_PORT |=  (1<<OPEN_LED); }

// 8500 COMMANDS:
#define SYNC_UPDATE_FRAME			0x00	// preferred to prevent "runt" pulses
#define ASYNC_UPDATE_FRAME  		0x10
#define CORRECTION_FRAME			0x20
#define OUTPUT_ENABLE_FRAME			0x30
#define OUTPUT_DISABLE_FRAME		0x40
#define SELFTEST_FRAME				0x50
#define PHASE_SHIFT_TOGGLE_FRAME	0x60
#define CORRECTION_TOGGLE_FRAME 	0x70

#define NUMBER_PWMS 48
#define tPWM word //int:12;		// PWM 		  (12 bits)
#define tCOR word //int:12		// Correction (6 bits)

#define M1 16 
#define M2 17
#define M3 18
#define M4 19

#define M5 20
#define M6 21
#define M7 22
#define M8 23


// Data placed in here goes into the send_update_frame();
extern tPWM  PWM_array     [NUMBER_PWMS];
extern tCOR  CorrectionBits[NUMBER_PWMS];  // Only upper 6 bits are valid

void init_pwm				( );
void init_Correction		( );
void init_8500				( );
void pack_array 			( tPWM* mWordArray );
void send_frame_spi			( uint8_t* min_array, uint8_t* mout_array, uint8_t mcommand );	// generic frame
void send_correction_frame	( );
void send_update_frame		( BOOL mSync);		// Update all 48 PWMs (12 bits each)
void send_output_enable		( BOOL mEnable );

void send_phase_toggle_frame( );
void send_correction_toggle_frame( );



#endif

