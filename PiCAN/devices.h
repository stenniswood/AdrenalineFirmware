
struct DEVICE_CAPS {
	int Recordable_Channels ;
	int Playable_Channels   ;
	int Max_SampleRate 	    ;
};

struct DEVICE_CAPS* DEVICE_ComposeAudioCapabilitiesStructure( );


struct WAVE_HEADER
{
	short 	format;				// PCM=1
	short 	channels_rxd;	
	int 	sample_rate;	
	int		byte_rate;	
	short 	block_align;		
	short 	bits_per_sample;		
};

// include protocol.h before this file to pick up BOOL definition.

extern struct WAVE_HEADER audio_hdr;
extern BOOL audio_hardware_enabled;

void AUDIO_SetupPlayer( BYTE mChannels, short mSampleRate );
BOOL AUDIO_QueueBuffer( char* mBuffer, int mSize );
struct DEVICE_CAPS* DEVICE_ComposeAudioCapabilitiesStructure( );
