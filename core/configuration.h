#ifndef _CONFIGURATION_H_
#define _CONFIGURATION_H_

/************* CONFIGURATION *****************/
extern byte config_byte_1 ;
extern byte config_byte_2 ;
extern byte config_byte_3 ;
extern byte config_byte_4 ;
extern byte sys_config_byte;

inline byte isConfigured (byte Test)	{  return ((config_byte_1 & Test)>0); };
inline byte isConfigured2(byte Test)	{  return ((config_byte_2 & Test)>0); };
inline byte isConfigured3(byte Test)	{  return ((config_byte_3 & Test)>0); };
inline byte isConfigured4(byte Test)	{  return ((config_byte_4 & Test)>0); };
inline byte isSysConfigured(byte Test)	{  return ((sys_config_byte & Test)>0); };


/**** CONFIG BYTE 4 - BIT DEFS ****/
// There are 16 configurable baudrates for CAN:
#define CAN_BAUD_100K 0x10
#define CAN_BAUD_125K 0x20
#define CAN_BAUD_200K 0x30
#define CAN_BAUD_250K 0x40
#define CAN_BAUD_500K 0x50
#define CAN_BAUD_1M   0x60
inline byte getConfiguredCANBaudRate()			{  return ((config_byte_4 & 0xF0)>0); };
inline void ConfigureCANBaudRate(byte BaudRate)	{  config_byte_4 = (BaudRate & 0xF0); };

// The defaults are set on first boot.
#define CAN_NEW_BOARD 			0x01		// This is 1 after a reflash.
#define NORMAL_HISTORY_OP_MODE	0x02		// defined in can_buff.c
//#define MAILBOX_HISTORY_OP_MODE	0x02	
/**** END OF CONFIG BYTE 4 DEFS ****/

void default_configuration();
void config_init();
byte* save_instance_number (byte* mEEPROMAddress);
byte* read_instance_number (byte* mEEPROMAddress);
byte* save_confirmed_status(byte* mEEPROMAddress);
byte* read_confirmed_status(byte* mEEPROMAddress);

byte  make_system_new_board( word passcode );

void  can_parse_configure_request( sCAN* mMsg );
byte* save_configuration_eeprom  ( byte* addr );
byte* read_configuration_eeprom  ( byte* addr );

// The following 2 functions are abstract.  Each Module must define them.
// The should call all sub EEPROM saving functions.  The start
#define MAX_EEPROM 			512
//#define EEPROM_BASEADDRESS	(byte*)1			// myinstance is at 0 and goes on its own.
#define INSTANCE_EEPROM_ADDRESS (byte*)1

byte* save_configuration();
byte* read_configuration();

#endif
