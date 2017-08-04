#ifndef _G_CODE_H_
#define _G_CODE_H_

#include "global.h"


BOOL parse_set_base_frequency_command( char* mIncoming 		 );
void parse_Gxx						 ( int Code, char* mText );



#endif
