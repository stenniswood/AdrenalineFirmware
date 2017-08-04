This folder:  Five_G2 is intended for the Rev G2 (yellow) boards 
	


				   Five protocol

The following commands can specifify which AXIS to apply to : 
set|read %d counts_per_unit [vwxyz]
read position
read speed
read base frequency
zero encoder [vwxyz]
measure travel [vwxyz]
home [vwxyz]
pwm  v0.5 w0.75 x0.50 y0.50 z0.95

position v500 w650 x350 y400 z450
speed v20.0 w15.0 x10.0 y10.0 z10.0

enable limits [vwxyz]
disable limits [vwxyz]
read serial number
read version
set base frequency 500
spin 180 degrees
forward 100
backward 100
set wheel diameter 10
set wheel separation 50

//avrdude -P comport -b 19200 -c avrisp -p m328p -v -e 
//-U efuse:w:0x05:m -U hfuse:w:0xD6:m -U lfuse:w:0xFF:m

