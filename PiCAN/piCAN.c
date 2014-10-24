#include <stdio.h>
#include <string.h>
#include <piWirign.h>


#define BOOL char
#define BYTE unsigned char

/************ VARIABLES & CONSTANTS ****************************/

FILE*  wave_file;




int main()
{

	int i=0;
	while (i++<5)
	{
		printf("Recording...\n");
		sleep(1);
	}
	AUDIO_CloseRecorder();
	CloseAudioFile();	
	
}

