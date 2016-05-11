#ifndef _CAN_BUFF_H_
#define _CAN_BUFF_H_

#define MAX_CAN_MSG_MEMORY_SIZE 6

/*#define NO_HISTORY_OP_MODE 		0
#define NORMAL_HISTORY_OP_MODE 	1
#define MAILBOX_HISTORY_OP_MODE 2
defined in "configuration.h" file now
*/
//extern byte OperationalMode;

byte 		 FindMailBox  	( tID mID 			  );
void 		 copy_can_msg 	( struct sCAN* mDest, struct sCAN* mSrc );
void 		 QueueMessage 	( struct sCAN* mMsg   );
struct sCAN* GetMessagePtr  ( byte mIndex 		  );
struct sCAN* RetrieveMessage( 	);
void 		 CAN_Q_Timeslice(	);


// "PROTECTED/HELPER" Functions:
void addNormalHistory ( struct sCAN* mMsg );
void addMailboxHistory( struct sCAN* mMsg );


#endif

