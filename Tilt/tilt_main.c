/*********************************************************************
Product of Beyond Kinetics, Inc

This code reads the keypad array of buttons
and sends a CAN message with the info

AFTER COMPILE, PROGRAM WITH:
avrdude -p atmega16m1 -b 19200 -c avrisp -P /dev/tty.usbmodemfd131 -Uflash:w:./tilt_board.hex
avrdude -p atmega16m1 -b 19200 -c avrisp -P /dev/tty.usbmodemfd131 -Ulfuse:w:0xE4:m

    LIS3D - Requirements are :
	SCLK must IDLE High  (CPOL=1)
	MOSI/MISO driven on Falling edge of clk and sampled on Rising edge.
	Mode 3

	CMR3000 wants SPI_MODE0, but sample is done on rising edge as in Mode3
	LIS3d & CMR3000 (accel & gyro) both require msb first
	FOR LIS3D Accelerometer, SPI_MODE3 is required

  NOTE: Tilt board Rev 1 uses the regular SPI pins (MISO,MOSI,SCK) on PB0,PB1,PB7.  CONFIRMED
  
  NOTE: ANALOG board uses the ALTERNATE SPI pins (MISO,MOSI,SCK) on PB0,PB1,PB7
  	    This is different than the probramming SPI which defaults to the alternate pins.

Problem:  MISO line is not going low.  So we'll do a test wiggle.
			Yes, it wiggles as an output.  So maybe 2 chip selects going at a time no.
			
			Chip Select to Accel is correct 		( 3.3v to 1.0v however)
			CS to Magnet is HIGH always 			(~3.3 v )
			CS to Gyro (CMR)						( 1.8V!?) IN CONTENTION WITH CMR3000 INT output

	*		SCK does not go to Gnd however - Goes 3.3V to 2.0V!
	*		SDO follows SCK (3.3V to 2.8V)	Therefore the all 0xFF's

ROOT CAUSE:
	CMR3000 GYRO pins were mirror imaged -top to bottom.
============================================================
Atmel Micro:
	Input Voltage Low Vil = 0.2*5 = 1 volt
	Input Voltage Hi  Vil = 0.6*5 = 3 volt

Possible reasons :
			GND is not connected to the Accel chip. (bad solder)
				GND looks same level as the CS.

			Another chip is not connected properly.
				
3 PARTS CONFLICTING ?
	Even if I2C mode is activated on one of the chips, the clock lines are all passive.
		They are all slave devices.  Now is there an internal pull-up.  Or one of the grounds
		offset and diode protected not to go below it.		
	
How hard would it be to isolate chips these again?
	TODO:   Cut the SCK jumper on back of board.  SCK is now connected to CMR & LIS3D only.  
				IF SCK still bad, [YES, STILL BAD]
					Remove SCK jump near CMR, check micro pin:
							Voltage goes lower, but extremely bad signal.  harmonics with lots of ringing.
							Avg voltages:  High 3.4V.  Low 1.0V
							Clk is at 4Mhz.  2nd board does bit better.  Much less ringing.
							Slowed clock to 245Khz.  Good square wave signal.

					Low Voltage goes high on touching it to either of CMR & LIS3D
					Doesn't do it on touching to the HMC.

			***	NOTE: ATMEL IS RUNNING AT 3.3V									***
			***		AT THIS SPEED NEED TO STEP THE CORE SPEED DOWN TO _8MHZ_	***	
					Attach to the HMC corner.  Check SCK - SCK IS GOOD.

				IF improved, keep probing those 2 chips.

SPI MODES:	
	   			SCK Idle	Sample Edge		CS Active
	LIS3D		High						Low			
	CMR3000		Low			Rising			Low			
	HMC 		High 						Low 		

DATE 	:  9/28/2013
AUTHOR	:  Stephen Tenniswood
********************************************************************/
#include "inttypes.h"
#include "interrupt.h"
#include "bk_system_defs.h"
#include "pin_definitions.h"
#include "leds.h"
#include "spi.h"
#include "can.h"
#include "can_instance.h"
#include "can_msg.h"
#include "accelerometer.h"
#include "gyro.h"
#include "magnetometer.h"
#include "OS_timers.h"
#include "configuration.h"
#include "calibrations.h"
#include "adc.h"
#include "can_board_msg.h"


/***** Initialization ***********************************/

/* Callback for configuration change. */
void config_change(byte mByteChanged)
{
	if (mByteChanged==3)
	{
		byte Sensitivity = getAccelSensitivity();
		accel_set_sensitivity( Sensitivity );
	}
}

void configure_buttons()
{
   //   byte value = (SPIPS & ~0x10);
   MCUCR &= ~PUD;  				        // clear the PUD bit
   BUTTON_DDR  &= ~BUTTON_DDR_MASK;     // 0b0001 1000  0=>INPUT
   BUTTON_PORT |=  BUTTON_DDR_MASK;     // 
}

void read_buttons()
{
  byte b1_down = 1;
  if ((PINB & 0x10)==0)        // digitalRead( BUTTON1 );
        b1_down = 0;

  byte b2_down = 1;
  if ((PINB & 0x08)==0)        // digitalRead( BUTTON2 );
        b2_down = 0;
}

// Highest Level Init:
void init()
{
 	cli();
    chip_init();
	init_leds ();
	delay(100000);					// ~ 2 sec
	read_cal();						// Read everything 

    // CAN INIT:
    can_init(CAN_250K_BAUD);

	// CONFIGURATION INIT:
	set_configure_callback( config_change );
    config_init();
	can_instance_init( );
	set_rx_callback  ( can_file_message );	// empty 
	sei();

	// OTHER CHIPS INIT:
	// parameter change:  spi_init(BOOL mAlternatePins, BOOL mMaster, uint8_t mbit_order, uint8_t mSPI_Mode )
	// This change means that the spi.c has been updated since last flashed onto the part.
	// so if there are problems look there!	
    spi_init( FALSE, TRUE, MSBFIRST, SPI_MODE3 );

	accel_init();
	gyro_init ();
	mag_init  ();
	OS_InitTask();
}

void send_accel()
{
	accel_read_xyz		( &accel );
	can_prep_accel_msg	( &msg1, &accel );
	can_send_msg		( 0, &msg1 );
}

void send_gyro()
{
	gyro_read_xyz	 ( &gyro 		);
	can_prep_gyro_msg( &msg2, &gyro );
	can_send_msg	 ( 0, &msg2 	);
}

void send_magnet()
{
	mag_read_xyz	 	( &magnet 		 );
	can_prep_magnet_msg	( &msg1, &magnet );
	can_send_msg	 	( 0, &msg1 		 );
}

void can_init_test_msg()
{
    msg1.id 	 = create_CAN_eid( 0x00, 0x0092, MyInstance );
    msg1.data[0] = 0x00;
    msg1.data[1] = 0x11;
    msg1.data[2] = 0x22;
    msg1.data[3] = 0x33;
    msg1.data[4] = 0x44;
    msg1.data[5] = 0x55;
    msg1.data[6] = 0x66;
    msg1.data[7] = 0x77;
    msg1.header.DLC = 8;
    msg1.header.rtr = 0;
	can_send_msg	 	( 0, &msg1 	);    
}

/***************************************************************
TEST PLAN:
	I) Read Accel WHO_AM_I.	Send on CAN to observe.  
	II) Read gyro WHO_AM_I.	Send on CAN to observe.  
	III) Read magnetometer WHO_AM_I.	Send on CAN to observe.

STRATEGY:  (to use interrupts)
	A) Accelerometer has 2 interrupt lines.
	Suppose we set it up to read at a certain frequency.
	Then on each interrupt, read data, package as CAN and send.
	
	B) Repeat for Gyro Interrupt.  	
	
	C) Repeat for Magnetometer DRDY interrupt.

***************************************************************/
int main(void)
{
	init();
    while (1)		// BACKGROUND TASKS HERE:
    {
		//can_init_test_msg();
		//strobe_leds(1);
		delay(one_second/5);
		//strobe_leds(0);
		delay(one_second/5);
    }
    return(0);
}
