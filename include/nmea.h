#ifndef __NMEA_H__
#define __NMEA_H__
//--------------------------------------------------
// 
// enum{
// 	TRUE = 0,
// 	FALSE
// };
//-------------------------------------------------- 


/********** NMEA_PSRF103	************/
enum NMEA_PSRF103_MESSAGE_CTRL{
	NMEA_PSRF103_GGA = 0,
	NMEA_PSRF103_GLL,
	NMEA_PSRF103_GSA,
	NMEA_PSRF103_GSV,
	NMEA_PSRF103_RMC,
	NMEA_PSRF103_VTG,
	NMEA_PSRF103_ZDA = 8
};
enum NMEA_PSRF103_MODE{
	NMEA_PSRF103_MODE_SET_RATE = 0,
	NMEA_PSRF103_MODE_ONE_TIME,
	NMEA_PSRF103_MODE_ABP_ON,
	NMEA_PSRF103_MODE_ABP_OFF
};
enum NMEA_PSRF103_RATE{
	NMEA_PSRF103_RATE_OFF = 0,
	NMEA_PSRF103_RATE_ONCE_PER_CYCLE
};
struct NMEA_PSRF103{
	enum NMEA_PSRF103_MESSAGE_CTRL message_ctrl;
	enum NMEA_PSRF103_MODE mode;
	enum NMEA_PSRF103_RATE rate;
};

/********** high level functions	************/
void nmea_init(void);
void nmea_warn_restart(void);
void nmea_stop(void);
int nmea_validate_sentence(uint16_t length);
uint32_t nmea_add_crc(char * data, uint32_t length);
int nmea_parse_data(void);
int nmea_get_frame(char data);
int nmea_switch_to_sirf(enum sim18_BAUDRATE);
#endif
