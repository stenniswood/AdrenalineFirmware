/********************************************************
NAME		:	DTC - Diagnostic Trouble Code
Description	:	Header file for DTC class.  A DTC object
				describes 1 trouble code for the module.

				Use "DTC_Logger" to compile a list and maintain
				occurence information.

Product of Beyond Kinetics, Inc.
*********************************************************/
#ifndef DTC_h
#define DTC_h

#include "Arduino.h"

#define MAX_AGE 40

#define PID_type unsigned short

class DTC
{
public:
    DTC     (PID_type mpid, char* mdescription);    // Constructor
	void  	Clear (short mpasscode);			// Clear status
	void  	LogFault( );						// New occurence
	void  	Age   	( );						// advance power cycle count
	short 	Report	( );
	char* 	GetSummary( );	

	DTC*	GetNextDTC  ( );						// Linked List
	void	AddNextDTC  ( DTC* next );
	bool	isMatch		( PID_type mpid );
	byte	GetPID		( );
	byte	GetAge		( );
	bool	isActive	( ) { return active; };
	void	SetAge		( byte age, short mpasscode);
	char*	GetDescription();

  private:  	
  	  /* Note once logged, the DTC will automatically go away after
  	  	 MAX_AGE power cycles			*/
  	  byte	power_cycle_count;		// Since the last logged occurrence.
	  bool	active;

	  DTC*	NextNode;				// Linked List
      PID_type	pid;
      char* description;
};




#endif



