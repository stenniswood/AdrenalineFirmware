If the make command does not work:
	This is because a new XCode changed the path.  You need to put the CrossPack compiler path before the /usr/bin/

do a "echo $PATH"

Copy the contents into the clipboard buffer.
Paste:  /usr/local/CrossPack-AVR-20120217/bin: somewhere before /usr/bin

then 
PATH="/sw/bin:/sw/sbin:/usr/local/CrossPack-AVR-20120217/bin:/opt/local/bin:/opt/local/sbin:/opt/local/bin:/opt/local/sbin:/usr/bin:/bin:/usr/sbin:/sbin:/Users/stephentenniswood/Downloads/android-sdk-mac_x86/platform-tools/:/usr/local/bin:/opt/X11/bin"


As of 04/27/2016, BigMotorEn is the folder to use on the red BigMotorboards. 

	BigMotor - does not compile and quite a few changes between RevD
	BigMotorEn - compiles and works
	BigMotorRevD - compiles but motors act dead no motion when run.

