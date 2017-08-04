
/***************** TESTING DEFINES ***************/
#define HOMING_SPEED 0.5
//#include "../global.h"
#define BOOL char
#define bool BOOL
#define byte char 
#define BYTE char 
#define uint32_t long int
#define uint16_t  short
#define NUM_MOTORS 5
#define TRUE 1
#define FALSE 0
/***************** END TESTING DEFINES ***************/

struct stEncoder
{
	uint32_t Count;
	uint32_t Speed;	
	uint32_t Acceleration;	
	uint32_t Direction;		// last 

	uint32_t  CountPrev;		// used for Speed
	uint32_t  SpeedPrev;		// used for acceleration
};
extern uint16_t PotSample[5];
extern struct stEncoder Encoders[NUM_MOTORS];

/****************CONFIGURATION EXTERN FOR TESTING PROTOCOL **********************************/
enum eMeasuringUnit
{
	counts,
	inch,
	cm,
	feet
};

/* Global Config Data */
struct stConfigData
{
	BOOL	use_encoder;
	BOOL	v_use_limits;
	BOOL	w_use_limits;
	BOOL	x_use_limits;
	BOOL	y_use_limits;
	BOOL	z_use_limits;
	enum eMeasuringUnit  units;
	long int			 v_counts_per_unit;
	long int			 w_counts_per_unit;
	long int			 x_counts_per_unit;
	long int			 y_counts_per_unit;
	long int			 z_counts_per_unit;
	
	float				 wheel_separation;
	float				 wheel_diameter;	
	float				 wheel_circumference;		// calculated quantity
		
	unsigned long		 serialNumber;
	BYTE				 FirmwareRevA;
	BYTE				 FirmwareRevB;
	BYTE				 FirmwareRevC;		
};
extern struct stConfigData FiveMotorConfigData;
/**************************************************/

void  set_motor_speed_letter( char Letter, float mFraction );
void  set_motor_position ( int mIndex, uint32_t Position );
BOOL  send_message   	 ( char* mMessage );
void  set_motor_duty_letter( char Letter, float mFraction );
float get_base_frequency_herz( );

void  init_configuration();
void  enable_limit( char Letter, BOOL mEnable);
void  set_motor_position_letter( char Letter, uint32_t Position );






