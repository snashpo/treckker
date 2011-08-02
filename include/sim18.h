#ifndef __GPS_H__
#define __GPS_H__


#define SIM18_Port_AUX		GPIOA
#define SIM18_NRESET			GPIO_Pin_4

#define SIM18_Port			GPIOC
#define SIM18_V_ANT			GPIO_Pin_2
#define SIM18_ON_OFF			GPIO_Pin_4
#define SIM18_WAKEUP			GPIO_Pin_3


extern uint8_t * nmea_in_buf;
extern uint8_t * nmea_out_buf;
extern uint8_t * sirf_in_buf;
extern uint8_t * sirf_out_buf;

/********** GPS_ALMANAC	************/
enum GPS_ALMANAC_RESET_MODE{
	GPS_ALMANAC_RESET_MODE_HOTSTART = 0,
	GPS_ALMANAC_RESET_MODE_WARMSTART_NOINIT,
	GPS_ALMANAC_RESET_MODE_WARMSTART_INIT,
	GPS_ALMANAC_RESET_MODE_COLDSTART,
	GPS_ALMANAC_RESET_MODE_FACTORYSTART = 8,
};

enum GPS_DATA{
	LOCK,
	LATITUDE,
	LAT_CARD,
	LONGITUDE,
	LONG_CARD,
	ALTITUDE,
	SPEED,
	DATE,
	TIME,
	SAT_NUMBER,
	GPS_MODE,
	DATA_VALIDE
};

struct coordonate_s{
	char cardinal;
	uint16_t degree;
	uint16_t minute;
	uint32_t dec_minute;
};

struct date_time_s{
	uint8_t day;
	uint8_t month;
	uint16_t year;
	uint8_t hour;
	uint8_t minute;
	uint8_t seconde;
	char date_str[20];
	char time_str[20];
};

struct sim18_data_s{
	uint32_t lock;
	struct coordonate_s latitude;
	struct coordonate_s longitude;
	uint16_t altitude;
	uint16_t azimuth;
	uint16_t speed_horizontal;
	uint16_t speed_vertical;
	uint16_t error_horizontal;
	uint16_t error_vertical;
	struct date_time_s date_time;
	uint32_t sat_number;
	char gps_mode;
	uint8_t data_valide;
	uint32_t clk_drift;
	uint32_t time_of_week;
	uint32_t week_no;
	uint32_t channel_count;
	enum GPS_ALMANAC_RESET_MODE reset_cfg;
};

/********** GPS SERIAL PORT SETTINGS	************/

enum sim18_PROTOCOL{
	 sim18_SIRF = 0,
	 sim18_NMEA,
};
enum sim18_BAUDRATE{
	sim18_115200 = 115200,
	sim18_4800 = 4800,
};
struct sim18_serial_settings_s{
	char device[64];
	enum sim18_PROTOCOL protocol;
	enum sim18_BAUDRATE baudrate;
};


/********** Low level functions	************/
int sim18_init(void);
void sim18_Configuration(void);
void sim18_read_data(uint8_t read_value);
void sim18_write_data(uint32_t length);
#endif

