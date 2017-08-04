/********************************************************
NAME		:	DTC Logger - Diagnostic Trouble Code
Description	:	Header file for DTC Logger class.  This 
				keeps a linked list of trouble codes.  
Author		:  Stephen Tenniswood
Product of Beyond Kinetics, Inc.  2012
*********************************************************/
#include "inc/DTCLogger.h"
#include <avr/eeprom.h>

/**********************************************************************//**
*  Purpose	  :  Construct a DTC logbook.  This will manage and hold all
				potential fault conditions for the robot. 
*  Parameters :  none
*  Return	  :  none
*************************************************************************/
DTC_Logbook::DTC_Logbook()
: FirstDTC(NULL), LogFrequency(0), DeviceDescription(NULL)
{
}

/**********************************************************************//**
*  Purpose	 :  Construct a DTC logbook.  This will manage and hold all
*				potential fault conditions for the robot. 
*  @param[in] :  mDeviceDescription - description of the logbook
*  Return	  :  none
*************************************************************************/
DTC_Logbook::DTC_Logbook(char* mDeviceDescription )
: FirstDTC(NULL), LogFrequency(0), DeviceDescription(mDeviceDescription)
{
}

/**********************************************************************//**
*  Purpose	  :  Get first defined DTC (in the linked list of available DTCs)
*  Parameters :  none
*  @return	  pointer to the first DTC registered
*************************************************************************/
DTC* DTC_Logbook::GetFirst( )
{
	DTC* tmp = FirstDTC;
	return tmp;
}

/**********************************************************************//**
*  Purpose	  :  Get the last DTC registered in this logbook.
*  Parameters :  none
*  @return	  Pointer to the last DTC registered
*************************************************************************/
DTC* DTC_Logbook::GetTail()
{
	DTC* prev = FirstDTC;	
	DTC* tmp = NULL;
	if (prev)
		tmp = prev->GetNextDTC();
	
	while (tmp != NULL)
	{
		prev = tmp;
		tmp = tmp->GetNextDTC();		
	}
	return prev;
}

/**********************************************************************//**
*  Purpose	  :  Get a handle to the DTC with the corresponding pid.
*  @param[in]  mpid - integer id of the DTC to find
*  @return	   the first DTC which has a matching pid
*************************************************************************/
DTC* DTC_Logbook::GetDTC(unsigned short mpid)
{
	DTC* tmp = FirstDTC;
	while (tmp != NULL)
	{
		if (tmp->isMatch(mpid))
			return tmp;	
		tmp = tmp->GetNextDTC();
	}
	return NULL;
}

/**********************************************************************//**
*  Purpose	  :  Create and Add a DTC to this logbook.  Available for fault logging later.
*  @param[in]  mpid - integer id of the new DTC 
*  @param[in]  mdescription - Text description of the new DTC
*  Return	  :  none
*************************************************************************/
void DTC_Logbook::AddCode(unsigned short mpid, char* mdescription)
{
	DTC* tmp = new DTC( mpid, mdescription );
	
	DTC* tail = GetTail();
	if (tail==NULL)
		FirstDTC = tmp;
	else
		tail->AddNextDTC( tmp );
}

/**********************************************************************//**
*  Purpose	  :  Search all codes.  Not used.
*  Parameters :  none
*  Return	  :  none
*************************************************************************/
void DTC_Logbook::QueryAll()
{
//	DTC* tmp = FirstDTC;
//	while (tmp != NULL)
	{
	   // Send DTC pid & description over CAN.
	}
}

/**********************************************************************//**
*  Purpose	  :  Get number of faults currently logged.
*  Parameters :  none
*  @return	  number of active faults.
*************************************************************************/
byte DTC_Logbook::GetNumberActive()
{
	byte count = 0;
	DTC* tmp = FirstDTC;
	while (tmp != NULL)
	{	   
	   if (tmp->isActive())
	     count++;
	   tmp = tmp->GetNextDTC(); 
	}
	return count;
}

/**********************************************************************//**
*  Purpose	  :  Get the first logged fault.  
*  Parameters :  none
*  @return	  Pointer to the first active fault.
*************************************************************************/
DTC* DTC_Logbook::GetFirstActive()
{
	DTC* tmp = FirstDTC;
	while (tmp != NULL)
	{
	   if (tmp->isActive())	
	   {
	   	  LastRetrieved = tmp;
		  return tmp;
	   }
	   tmp = tmp->GetNextDTC();
	}
	return NULL;
}

/**********************************************************************//**
*  Purpose	  :  Get next logged fault.
*  Parameters :  none
*  @return	  Pointer to the next active fault.
*************************************************************************/
DTC* DTC_Logbook::GetNextActive()
{
	DTC* tmp = LastRetrieved->GetNextDTC();
	while (tmp != NULL)
	{
	   if ( tmp->isActive() )
	   {
	   	  LastRetrieved = tmp;
	      return tmp;
	   }
	   tmp = tmp->GetNextDTC();
	}
	return NULL;
}


/**********************************************************************//**
*  Purpose	  :  Send list of active DTC's over CAN.  Not complete.
*  @param    none
*  Return	  :  none
*************************************************************************/
void DTC_Logbook::QueryActive()
{
	byte dummy = 0;
	DTC* tmp = FirstDTC;
	while (tmp != NULL)
	{
	   if (tmp->Report() != 0)	 
	     dummy=dummy;
	   	// Send DTC pid & description over CAN.
	}
}


/**********************************************************************//**
*  Purpose	  :  Call this when a fault condition happens.  In future reporting,
*				this fault will be shown as active.  Will start aging on next power cycle.
*  @param[in] integer ID of the DTC to log
*  Return	  :  none
*************************************************************************/
void DTC_Logbook::LogFault(unsigned short mPID)
{
	DTC* dtc = GetDTC(mPID);
	dtc->LogFault();
}

/**********************************************************************//**
*  Purpose	  :  Call this upon reset of the micro.  Each power cycle ages 
*				 all faults.  After 80 cycles of no further logging, the fault
*				 record is erased.
*  Parameters :  none
*  Return	  :  none
*************************************************************************/
void DTC_Logbook::PowerCycle()
{
	DTC* tmp = FirstDTC;
	while (tmp != NULL)
	{
	   tmp->Age();
	   tmp = tmp->GetNextDTC();
	}
}

/**********************************************************************//**
*  Purpose	  :  Clear all logged faults.
*  Parameters :  none
*  Return	  :  none
*************************************************************************/
void DTC_Logbook::ClearAllCodes()
{
	DTC* tmp = FirstDTC;
	while (tmp != NULL)
	{
	   tmp->Clear(0x1234);
	}
}


/**********************************************************************//**
*  Purpose	  :  Save the RAM fault linked list to EEPROM.  Faults will be 
*				 remembered on next power up.
*  @param  eeprom_start_address - starting address to save the log book at.
*  @return  number of bytes written
*************************************************************************/
short DTC_Logbook::SaveCodes(short eeprom_start_address)
{
	short addr = eeprom_start_address;
//	byte code=0;
	DTC* tmp = GetFirstActive();
	
	// Store number of fault codes:
	byte numFaults = GetNumberActive();	
	eeprom_write_byte((unsigned char*)addr++, numFaults );

	while (tmp != NULL)
	{
		if (addr > MAX_EEPROM)
			return -1;
		//code = tmp->Report();
		if (tmp->isActive())
		{
			eeprom_write_byte((unsigned char*)addr++, tmp->GetPID() );
			eeprom_write_byte((unsigned char*)addr++, tmp->GetAge() );
		}
		tmp = GetNextActive();
	}
	// Return bytes written:
	return (addr-eeprom_start_address);
}


/**********************************************************************//**
*  Purpose	  :  Retrieve faults from EEPROM.
*  @param[in]   eeprom_start_address - Starting address of the logbook
*  @return	    number of bytes read.
*************************************************************************/
short DTC_Logbook::LoadCodes( short eeprom_start_address )
{
	short addr = eeprom_start_address;
	byte numCodes=0;
	byte code=0;
	byte age =0;
	DTC* tmp = NULL;

	numCodes = eeprom_read_byte( (unsigned char*)addr++ );
	for (int i=0; i<numCodes; i++)
	{
		code = eeprom_read_byte( (unsigned char*)addr   );
		age  = eeprom_read_byte( (unsigned char*)addr+1 );
	
		tmp = GetDTC(code);
		tmp->LogFault();
		tmp->SetAge(age,0x1234);				
	}
	// Return bytes written:
	return (addr-eeprom_start_address);
}

/**********************************************************************//**
*  Purpose	  :  Erase the EEPROM memory (512 bytes)
*  Parameters :  none
*  Return	  :  none
*************************************************************************/
void DTC_Logbook::EraseEEPROM()
{
	for (int i = 0; i < 512; i++)
    	eeprom_write_byte((unsigned char*)i, 0);
}

