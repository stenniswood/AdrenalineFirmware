#ifndef _EEPROM_H_
#define _EEPROM_H_



void 			EEPROM_write(unsigned int uiAddress, unsigned char ucData);
unsigned char 	EEPROM_read	(unsigned int uiAddress);


#endif
