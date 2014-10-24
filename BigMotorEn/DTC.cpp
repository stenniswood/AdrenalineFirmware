/*******************************************************************
NAME		:	DTC - Diagnostic Trouble Code
Description	:	Header file for DTC class.  A DTC object
				describes 1 trouble code for the module.

				Use "DTCLogger" to compile a list and maintain
				occurence information.

Product of Beyond Kinetics, Inc.
********************************************************************/
#include "inc/DTC.h"

/**********************************************************************//**
*  Name		  :  DTC()
*  Purpose	  :  Constructor.  This holds 1 diagnostic trouble code,
*				 it's description, age, ID, and link to the next DTC in the list.
*  @param   mpid - integer id of the trouble code
*  @param   mdescription - Text description of the trouble code
*  Return	  :  none
*************************************************************************/
DTC::DTC(PID_type mpid, char* mdescription): 
power_cycle_count(0), 
active(0),
NextNode(NULL),
pid(mpid), 
description(mdescription)
{
}

/**********************************************************************//**
*  Name		  :  GetSummary()
*  Purpose	  :  Construct a string description of the DTC Status.
*  Return	  :  string
NEEDS TESTING!
*************************************************************************/
#define EXTRA_INFO_LENGTH 10+12+1
char* 	DTC::GetSummary( )
{
	char* retval;
	int description_length = strlen(description);
	retval = new char[EXTRA_INFO_LENGTH];
	
	// Description:
	strcpy( retval, description );

	// PID & Age
	char tmp_str[EXTRA_INFO_LENGTH];
	sprintf(tmp_str, " PID=%x Age=%d ", pid, power_cycle_count );
	strcpy( retval+description_length, tmp_str );
	
	return retval;	
}

	
/**********************************************************************//**
*  Name		  :  LogFault()
*  Purpose	  :  Flag this potential fault condition as active.  Reset it's age.
*  Return	  :  none
*************************************************************************/
void DTC::LogFault()
{
	active = true;
	power_cycle_count = 0;
}

/**********************************************************************//**
*  Name		  :  Age()
*  Purpose	  :  Celebrate it's 1st power cycle birthday.  If it hasn't occured
*				 for the predefined life time, consider it obsolete.
*				 If robot went thru 80 runs without any report, this is probably 
*				 not worth keeping track of anymore.
*  Return	  :  none
*************************************************************************/
void DTC::Age()
{
	if (active)
		power_cycle_count++;
	if (power_cycle_count >  MAX_AGE)
	{
		active = false;
		power_cycle_count = 0;
	}
}

/**********************************************************************//**
*  Name		  :  AddNextDTC()
*  Purpose	  :  Connect another DTC to this node.
*  @param  next  - Pointer to the next node
*  Return	  :  none
*************************************************************************/
void DTC::AddNextDTC(DTC* next)
{
	NextNode = next;
}

/**********************************************************************//**
*  Name		  :  GetNextDTC()
*  Purpose	  :  Retrieve the pointer to next DTC in the list (not this one)
*  @return	  :  Pointer to the next DTC
*************************************************************************/
DTC* DTC::GetNextDTC()
{
	return NextNode;
}
	
/**********************************************************************//**
*  Name		  :  isMatch()
*  Purpose	  :  Test if this DTC's ID matches the requested. Use this for scanning
*				 the linked list looking for an ID.
*  @param  mpid - an integer id of the param
*  @return  true if this DTC pid matches the parameter.  false otherwise.
*************************************************************************/
bool	DTC::isMatch( PID_type mpid )
{
	return (mpid==pid);
}
/**********************************************************************//**
*  Name		  :  GetPID()
*  Purpose	  :  Retrieve the ID for this DTC.
*  @return	  :  the integer id (pid) of this DTC
*************************************************************************/
byte	DTC::GetPID( )
{
	return pid;
}
/**********************************************************************//**
*  Name		  :  GetAge()
*  Purpose	  :  How long has it been (in power cycles) since this fault occured.
*  @return	    the age of the DTC
*************************************************************************/
byte	DTC::GetAge( )
{
	return power_cycle_count;
}
/**********************************************************************//**
*  Name		  :  GetDescription()
*  Purpose	  :  Get the text description telling what this fault is all about.
*				 Used especially after a fault occured.  The human user wont know
*				 what a number id fault is all about, but a description should help.
*  @return	  :  text description of this DTC
*************************************************************************/
char*	DTC::GetDescription()
{
	return description;
}
/**********************************************************************//**
*  Name		  :  SetAge()
*  Purpose	  :  Set the age to a certain value.  This is not recommended
*				 except when reading from EEPROM and rare occasions.  Thats
				why a passcode is required (so that you don't accidentally change it).
*  @param   mage		age to adjust to
*  @param   mpasscode  0x1234 so age is not accidentally changed.
*  Return	  :  none
*************************************************************************/
void DTC::SetAge(byte mage, short mpasscode)
{
	if (mpasscode==0x1234)
		power_cycle_count = mage;
}

/**********************************************************************//**
*  Name		  :  Report()
*  Purpose	  :  Return the id if it's active.  0 otherwise.  Used for 
				 scanning the list.
*  Return	  :  none
*************************************************************************/
short DTC::Report()
{
	if (active)
		return pid;
	else return 0;
}

/**********************************************************************//**
*  Name		  :  Clear()
*  Purpose	  :  Reset the DTC fault condition.  An automotive/robot tech
				 does this after a repair has been made.
*  @param   mpasscode - 0x1234 to prevent accidental clearing.
*  Return	  :  none
*************************************************************************/
void DTC::Clear(short mpasscode)
{
	if (mpasscode == 0x1234)
	{
		active = false;
		power_cycle_count = 0;		
	}
}

