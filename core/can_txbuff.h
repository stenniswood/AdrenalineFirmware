#ifndef _CAN_TXBUFF_H_
#define _CAN_TXBUFF_H_

#ifdef  __cplusplus
extern "C" {
#endif

#define MAX_CAN_TXMSG_MEMORY_SIZE 8
#define NO_HISTORY_OP_MODE 		0
#define NORMAL_HISTORY_OP_MODE 	1
#define MAILBOX_HISTORY_OP_MODE 2

void 		 copy_can_msg 	( struct sCAN* mDest, struct sCAN* mSrc );
BOOL 		 AddToSendList	( struct sCAN* mMsg  );
BOOL 		 SendNext	  	( 				  	 );
struct sCAN* GetTxMessagePtr( byte mIndex 		 );
extern byte  TransmissionInProgress;

void 		 can_tx_timeslice();

#ifdef  __cplusplus
}
#endif

#endif

