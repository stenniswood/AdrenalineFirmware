


BOOL parse_set_base_frequency_command( char* mIncoming )
{
	BOOL is_position_command = strcmp(mIncoming, "set base frequency:");

	return is_position_command;
}

void parse_Gxx( int Code, char* mText )
{
	switch( Code )
	{
	case 0  :	/* Move Motors Rapid Positioning */
	
		break;
	case 1  :	/* Move Linear Interpolation 	 */
	
		break;
	case 20 : 	/* Use inches					 */
	
		break;
	case 21 : 	/* Use Millimeters 				 */

		break;
	case 90 : 	/* Use Absolute Positioning 	 */

		break;
	case 92 : 	/* Use Relative Positioning 	 */

		break;
	default: break;
	}
}




