
#ifndef _CAN_INSTANCE_H_
#define _CAN_INSTANCE_H_


#define INSTANCE_EEPROM_ADDRESS 0
extern byte MyInstance;


void channel_init();
void save_instance_number();
void read_instance_number();


word pick_random_number();

#endif


