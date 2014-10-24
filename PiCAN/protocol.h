
/**************** TOKENS - TELEGRAM TYPE *********SERVER CODE********
OVERALL STRUCTURE:
		1000	GENERAL PURPOSE ("BK")		General Purpose System level (ie. keyboard, mouse, print, etc)
		2000	COMMANDS ("CMD")			Drive some change in the system (including incoming audio, video, etc)
		3000	REQUESTS ("REQ")			Request a stream, or other info (including incoming audio, video, etc)
		4000	COMPUTE REQUEST ("CPU")		Requests which utilize idle processing power. (ie. does this photo match any in your database?)
		F000	SYSTEM   ("SYS")
*********************************************************************/
#define BYTE unsigned char
#define WORD unsigned short
#define TRUE 1
#define FALSE 0
#define BOOL char
//#define BK_MEDIA_PORT 8080
#define BK_MEDIA_PORT 5000


// CLIENT COMMANDS: (in java char is unsigned 16 bit)
#define BK_PING				0x1000		//	Ping bounce to show alertness.
#define BK_TEXT_MESSAGE 	0x1001		//	Incoming Text Message
#define BK_KEY_PRESSED	 	0x1002		//	Keyboard key press
#define BK_POINTING_DEV	 	0x1003		//	Pointing device (mouse, touchpad, etc)
#define BK_EMAIL_NA		 	0x1004		//	Receive an email (not implemented yet)


#define CMD_GPIO			0x2001		// Set digital Output to a value
#define CMD_DDR				0x2002		// Set Direction of a digital output
#define CMD_CAN_SEND		0x2100		// Send CAN message
#define CMD_CAN_SET_MASK	0x2101		// Only Listen to certain CAN messages
#define CMD_CAN_LISTEN		0x2102		// Turn On/Off the listening to CAN messages.

#define CMD_AUDIO_START		0x2400		// Data contains the Header info 
#define CMD_AUDIO_DATA		0x2401		// Data length and the audio data.
#define CMD_AUDIO_END		0x2402		// End Audio Transmission

#define CMD_VIDEO_START		0x2500		// Video Header info
#define CMD_VIDEO_DATA		0x2501		// Video frame Data	
#define CMD_VIDEO_END		0x2502		// Video frame Data	

#define CMD_SHOW_HTML_FILE	0x2601		// HTML Data
#define CMD_FILE_TRANSFER	0x2602		// HTML Data

// CLIENT REQUESTS (implies data will be received):
#define REQ_DEVICE_TYPE		0x3400		//	Ping bounce to show alertness.
#define REQ_GPIO			0x3500		// IN MEANS DATA IS INCOMING FROM SOCKET CLIENT.
#define REQ_CAN_LISTEN  	0x3600		// Data byte determines if listening ON/OFF
#define	REQ_IMAGE			0x3700

#define REQ_AUDIO_CAPABILITIES	0x3800		// returns number of cameras, type of each (rgb, depth, far infrared), resolution, etc.
#define	REQ_AUDIO_LIVE		0x3801			// 
#define	REQ_AUDIO_STOP		0x3802			// Done Listening
#define	REQ_AUDIO_TIMESTAMP	0x3803
#define	REQ_AUDIO_FILE		0x3804

#define REQ_CAMERA_CAPABILITIES		0x3900		// returns number of cameras, type of each (rgb, depth, far infrared), resolution, etc.
#define REQ_IMAGE_LIVE		0x3901
#define REQ_IMAGE_TIMESTAMP	0x3902
#define REQ_IMAGE_FILE		0x3903

#define REQ_VIDEO_LIVE		0x3A01
#define REQ_VIDEO_TIMESTAMP	0x3A02
#define REQ_VIDEO_FILE		0x3A03

#define	REQ_MESSAGE			0x3900		// any msgs in queue?
#define	REQ_STATUS			0x3500
#define	REQ_HISTORY			0x3600
#define	REQ_SHUT_DOWN		0x3700		// THIS COULD BE SENT PERHAPS BY MASTER PHONE SOFTWARE. WITH PASSWORD!!

// SYSTEM EXCLUSIVE MESSAGES:
#define SYS_OVERRIDE		0xF000		// IN MEANS DATA IS INCOMING FROM SOCKET CLIENT.
#define SYS_HALT			0xF100		// IN MEANS DATA IS INCOMING FROM SOCKET CLIENT.
#define SYS_REBOOT			0xF200		// IN MEANS DATA IS INCOMING FROM SOCKET CLIENT.
#define SYS_REPROGRAM		0xF300		// IN MEANS DATA IS INCOMING FROM SOCKET CLIENT.


void Init_GPIO();
void Print_GPIO_Acknowledgement(WORD mToken);
BOOL Process_GPIO_Telegrams( WORD  mToken, BYTE* mheader, int DataLength, int mconnfd );

void Init_AUDIO();
void Print_AUDIO_Acknowledgement(WORD mToken);
BOOL Process_AUDIO_Telegrams( WORD  mToken, BYTE* mheader, int DataLength, int mconnfd );

void Init_CAN();
void Print_CAN_Acknowledgement(WORD mToken);
BOOL Process_CAN_Telegrams( WORD  mToken, BYTE* mheader, int DataLength, int mconnfd );

void Init_IMAGE();
void Print_IMAGE_Acknowledgement(WORD mToken);
BOOL Process_IMAGE_Telegrams( WORD  mToken, BYTE* mheader, int DataLength, int mconnfd );

void Init_VIDEO();
void Print_VIDEO_Acknowledgement(WORD mToken);
BOOL Process_VIDEO_Telegrams( WORD  mToken, BYTE* mheader, int DataLength, int mconnfd );






