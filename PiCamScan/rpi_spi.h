#ifdef  __cplusplus
extern "C" {
#endif


// COMMANDS:
#define UPDATE_PAN_TILT 		0x01
#define UPDATE_LOWSIDE_DRIVER 	0x02
#define UPDATE_LOWSIDE_DRIVERS 	0x03
#define READ_ANALOG				0x04
#define READ_ANALOGS			0x05

void test_rx_response( byte mData );

byte spi_callback( );



void parse_pan_tilt_angles	( byte* mData, float *pan, float *tilt	);
void parse_low_side_driver	( byte* mData, byte* mBit, byte* mValue	);
void parse_low_side_drivers	( byte* mData, byte* mValues	);
void parse_read_analog		( byte* mData, byte* mChannel	);
void parse_read_all_analogs	( byte* mData, float* mValue	);
void pack_read_all_analogs	( byte* mData, float* mValue	);

byte num_expected_bytes		( byte mToken 					);
void distribute_parsing		( byte* mData 					);


#ifdef  __cplusplus
}
#endif

