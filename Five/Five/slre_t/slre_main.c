#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "slre.h"
#include "protocol.h"


void test()
{
	printf   ("Testing Protocol...");
	
	top_level( "set 500 counts_per_unit v w x y z"  );
	top_level( "set 500 counts_per_unit x "  );	

	top_level( "zero encoder"		);
	top_level( "home v w x"			);
	top_level( "measure travel" 	);

	top_level( "pwm v101 w501 x201 y301 z401" 	);
	top_level( "pwm v100 x200 y300 z400" 		);
	top_level( "speed x200 y300 z400" 			);
	top_level( "speed v100 w500 x200 y300 z400" );	
	top_level( "position x0 y0 z0" 				);
	top_level( "position v100 w500 x200 y300 z400" );	

	top_level( "enable limits"  );
	top_level( "disable limits" );		

	// Test Differential Drive Commands:	
	top_level( "set counts_per_rev 500"  		);
	top_level( "set wheel diameter 6"      		);	// must be given in system set units
	top_level( "set wheel separation 18"	    );
		
	top_level( "spin 180 degrees" 		);
	top_level( "spin 360 degrees" 		);
	top_level( "forward 50 "  	);
	top_level( "backward 5 " 	);
	
	top_level( "use encoder" 		);
	top_level( "use potentiometer"	);
	top_level( "set unit counts"	);	
	top_level( "set unit inch"		);	
	top_level( "set unit cm"		);
	top_level( "set unit feet"		);	top_level( "read unit" );

	top_level( "read position"		);
	top_level( "read speed" 		);
	top_level( "read base frequency");		
	top_level( "read serial number" );
	top_level( "read version"		);
}

int main()
{
	test();	
	return 0;
}