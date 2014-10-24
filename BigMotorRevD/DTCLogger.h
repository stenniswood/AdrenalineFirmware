/********************************************************
NAME		:	DTC Logger - Diagnostic Trouble Code
Description	:	Header file for DTC Logger class.  This 
				keeps a linked list of trouble codes.  
				Saves & Retrieves them to/from EEPROM.

Author		:  Stephen Tenniswood
Product of Beyond Kinetics, Inc.  2012
*********************************************************/
#ifndef DTC_LOGBOOK_h
#define DTC_LOGBOOK_h

#include "Arduino.h"
#include "DTC.h"

#define MAX_EEPROM 512
#define EEPROM_DTC_BASEADDRESS 0	// pass to SaveCodes()

class DTC_Logbook
{
  public:
	DTC_Logbook  ( );
    DTC_Logbook  ( char* mDeviceDescription );  		// Constructor
	DTC* GetTail ( );

	void   AddCode        ( unsigned short mpid, char* mdescription);	// New pid for this device
	byte   GetNumberActive( );			// Get total number of active faults.
	DTC*   GetFirst       ( );
	DTC*   GetFirstActive ( );			// Handle to the first active fault.
	DTC*   GetNextActive  ( );				// Handle to the next active fault.
	void   QueryAll       ( );					// Retrieve List of PIDs available.
	void   QueryActive    ( );					// Retrieve List of active PIDs.
	void   LogFault       ( unsigned short mPID );		// Report the occurence of a fault.
	void   PowerCycle     ( );					// Adjust counts of all active codes.
	void   ClearAllCodes  ( );					// Clear all active codes.
	//void   Clear_Code( byte mPID );		// Clear all active codes.
	short  SaveCodes      ( short eeprom_start_address = EEPROM_DTC_BASEADDRESS );
	short  LoadCodes      ( short eeprom_start_address = EEPROM_DTC_BASEADDRESS );
	//void   DumpEEPROM(short eeprom_start_address = EEPROM_DTC_BASEADDRESS);
	DTC*   GetDTC         ( unsigned short mpid );
	void   EraseEEPROM    ( );

  private:
  	  DTC*	FirstDTC;					// In the list
	  DTC*  LastRetrieved;
  	  float	LogFrequency;				// How often per 100 power cycles are faults occuring
      char*	DeviceDescription;  
};


#endif
