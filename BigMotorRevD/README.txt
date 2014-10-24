/* This folder:  BigMotorEnPatch is intended for the Rev C (red) boards 
	which have their PWM and DIR lines swapped.  This takes us back to the 
	PWMing the hi/low side of the H bridge (signed magnitude method).  This
	allows the current in the motoro to recirculate during the off
	phase of the PWM preventing huge surge into the power supply/other circuitry.
	
	The Rev C boards initially PWM'ed the Inhibit line and this completely cuts 
	off the high & low side MOSFETs.  Leaving no where for the current to go, and SNAP!
	
	
	
	



*/