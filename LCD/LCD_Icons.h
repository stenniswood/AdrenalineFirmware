#ifndef ICONS_H
#define ICONS_H


struct stIcon
{
	const char *ptr;
	byte  sizey;			// Y is [0..127]
	byte  sizex;			// X is [0..7]
};

extern const char* Tach_Settings[10];
extern struct stIcon Battery;
extern struct stIcon* Tachs[11];
extern struct stIcon WaterIcon;
extern struct stIcon AndroidIcon;
extern struct stIcon AndroidNim_1Icon;
extern struct stIcon AndroidNim_2Icon;
extern struct stIcon AndroidNim_3Icon;
extern struct stIcon AndroidNim_4Icon;
extern struct stIcon AndroidNim_5Icon;

extern struct stIcon TriangleIcon ;
extern struct stIcon SquareIcon ;
extern struct stIcon CircleIcon ;


extern const char ZigZag[];
extern const char FullOn[];
extern const char HalfLine[];

// String Table of Sensor Speeds  (ISS_index=16 minimum; 4 chars each)
extern const char SensorSpeeds[455];
extern const char SensorSpeeds2[455];
extern const char SensorSpeeds3[65];


#endif