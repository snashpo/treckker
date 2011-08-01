#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>

#include "sim18.h"
#include "nmea.h"
#include "sirf.h"



#ifdef DEBUG
#define DEBUGF(x, args...) printf(x, ##args)
#else
#define DEBUGF(x, args...)
#endif

#define NMEA_CRC_FILL 0
int convert_str_to_point(char *data, struct coordonate_s * point){
	//3113.3156
	float pt =  strtof(data, NULL);
	point->degree = (uint16_t)(pt / 100);
	point->minute = (uint16_t)(pt - (point->degree * 100));
	point->dec_minute = (uint32_t)((pt - (point->degree * 100 + point->minute)) * 10000);

	return 0;
}


void nmea_coordonate_to_string(struct coordonate_s *point, char * string, uint32_t length){
	snprintf(string, length, "%c%d.%02d%05d"
			, (point->cardinal == 'E'||point->cardinal == 'N'?'+':'-')
			, point->degree
			, point->minute
			, point->dec_minute);
}


enum{
	RMC_MESAGE_ID,
	RMC_UTC_TIME,
	RMC_STATUS,
	RMC_LATITUDE,
	RMC_NS_INDICATOR,
	RMC_LONGITUDE,
	RMC_EO_INDICATOR,
	RMC_SPEED,
	RMC_COURSE,
	RMC_DATE,
	RMC_MAGNETIC_VARIATION,
	RMC_EASTWEST_INDICATOR,
	RMC_MODE,
	RMC_CS
};

#define KNOT_TO_MS				0.5144
#define NMEA_SEPARATOR			","
//'$GPRMC,120159.000,A,4317.4396,N,00529.7541,E,0.57,171.53,070711,,,A'
static int nmea_parse_RMC(char *data){
	char * ptr_str;
	uint32_t state = RMC_MESAGE_ID;
	float tmp_float;
	uint32_t tmp;

	for (ptr_str = strtok(data, NMEA_SEPARATOR); ptr_str; ptr_str = strtok(NULL, NMEA_SEPARATOR)){
		switch(state){
			case 	RMC_MESAGE_ID:
				break;

			case 	RMC_UTC_TIME:
				tmp = (uint32_t)strtof(ptr_str, NULL);
				mydata.date_time.hour = (uint8_t)(tmp / 10000);
				mydata.date_time.minute = (uint8_t)((tmp - (mydata.date_time.hour * 10000))/100);
				mydata.date_time.seconde = (uint8_t)(tmp - ( mydata.date_time.hour * 10000)
						- ( mydata.date_time.minute * 100));

				snprintf(mydata.date_time.time_str, sizeof(mydata.date_time.time_str)
						, "%d:%d:%d"
						, mydata.date_time.hour
						, mydata.date_time.minute
						, mydata.date_time.seconde);
				break;

			case 	RMC_STATUS:
				break;

			case 	RMC_LATITUDE:
				//convert_str_to_point(ptr_str, &mydata.latitude);
				break;

			case 	RMC_NS_INDICATOR:
				//mydata.latitude.cardinal = *ptr_str;
				break;

			case 	RMC_LONGITUDE:
				//convert_str_to_point(ptr_str, &mydata.longitude);
				break;

			case 	RMC_EO_INDICATOR:
				//mydata.longitude.cardinal = *ptr_str;
				break;

			case 	RMC_SPEED:
				tmp_float = strtof(ptr_str, NULL);
				mydata.speed_horizontal = (uint16_t) (tmp_float * KNOT_TO_MS);
				break;

			case 	RMC_COURSE:
				//mydata.azimuth = (uint16_t) (strtol(ptr_str, NULL, 10));
				break;

			case 	RMC_DATE:
				tmp = (uint32_t)strtol(ptr_str, NULL, 10);
				mydata.date_time.day = (uint8_t)(tmp / 10000);
				mydata.date_time.month = (uint8_t)((tmp - (mydata.date_time.day * 10000))/100);
				mydata.date_time.year = 2000 + (uint8_t)(tmp - ( mydata.date_time.day * 10000)
						- ( mydata.date_time.month * 100));

				snprintf(mydata.date_time.date_str, sizeof(mydata.date_time.date_str)
						, "%d/%d/%d"
						, mydata.date_time.day
						, mydata.date_time.month
						, mydata.date_time.year);

				break;

			case 	RMC_MAGNETIC_VARIATION:
				break;

			case 	RMC_EASTWEST_INDICATOR:
				break;

			case RMC_MODE:
				//mydata.gps_mode = *ptr_str;
				break;
		}

	state++;
	}
	return 0;
}

enum{
 	GGA_MESSAGE_ID,
	GGA_UTC_TIME,
	GGA_LATITUDE,
	GGA_NS_INDICATOR,
	GGA_LONGITUDE,
	GGA_EW_INDICATOR,
	GGA_POSITION_FIX,
	GGA_SATELITE_USED,
	GGA_HDOP,
	GGA_MSL_ALTITUDE,
	GGA_UNITS_ALT,
	GGA_GEOID_SEPARATION,
	GGA_UNITS_SEP,
	GGA_AGE_OF_DIFF,
	GGA_DIFF_REF_STATION_ID,
   GGA_CS,
};

int nmea_parse_GGA(char *data){
 	char * ptr_str;
	uint32_t state = GGA_MESSAGE_ID;

	for (ptr_str = strtok(data, NMEA_SEPARATOR); ptr_str; ptr_str = strtok(NULL, NMEA_SEPARATOR)){
		switch(state){
			case GGA_MESSAGE_ID:
				break;
			case 	GGA_UTC_TIME:
				break;
			case GGA_LATITUDE:
				convert_str_to_point(ptr_str, &mydata.latitude);
				break;
			case GGA_NS_INDICATOR:
				mydata.latitude.cardinal = *ptr_str;
				break;
			case GGA_LONGITUDE:
				convert_str_to_point(ptr_str, &mydata.longitude);
				break;
			case GGA_EW_INDICATOR:
				mydata.longitude.cardinal = *ptr_str;
				break;
			case GGA_POSITION_FIX:
				if( *ptr_str != '0' ){
					mydata.data_valide = 1;
					if (*ptr_str == '1' ){
						mydata.gps_mode = 'G';
					}else if (*ptr_str == '2' ){
						mydata.gps_mode = 'D';
					}else if (*ptr_str == '6' ){
						mydata.gps_mode = 'R';
					}
				} else {
					mydata.data_valide = 0;
				}
				break;
			case GGA_SATELITE_USED:
				mydata.sat_number = (uint8_t)strtol(ptr_str, NULL, 10);
				break;
			case GGA_HDOP:
				break;
			case GGA_MSL_ALTITUDE:
				mydata.altitude = (uint16_t)strtof(ptr_str, NULL);
				break;
			case GGA_UNITS_ALT:
				break;
			case GGA_GEOID_SEPARATION:
				break;
			case GGA_UNITS_SEP:
				break;
			case GGA_AGE_OF_DIFF:
				break;
			case GGA_DIFF_REF_STATION_ID:
				break;
			case GGA_CS:
				break;
		}

	state++;
	}
	return 0;
}

int nmea_parse_data(char *data){
	char * tmp = strchr(data, '*');
	
	if ( !tmp ){
		DEBUGF("NMEA PARSE DATA ERROR, * not found.\n");
		return -1;
	}
	if (!strncmp(data, "$GPRMC", strlen("$GPRMC"))){
		*tmp = 0;
		return nmea_parse_RMC(data);
	} else if (!strncmp(data, "$GPGGA", strlen("$GPGGA"))){
		*tmp = 0;
		return nmea_parse_GGA(data);
	} else {
		DEBUGF("NMEA PARSE '%s'\n", data);
	}
	/*--------------------------------------------------
	* else if (strcmp(data, "$GPGSV")){
	* 	return nmea_parse_GSV(data);
	* } 
	*--------------------------------------------------*/
	return 0;
}

static int nmea_crc_calculate(char *crc, char *data, uint32_t length){
	char crc_temp = NMEA_CRC_FILL;
	uint16_t n;
	if(!crc || !data || (length < 4)){
		/*--------------------------------------------------
		* DEBUGF("GPS_ERROR : Wrong attributes\n");
		*--------------------------------------------------*/
		return -1;
	}
	if (*data != '$'){
		/*--------------------------------------------------
		* DEBUGF("GPS_ERROR : GPS data have to start with '$'\n");
		*--------------------------------------------------*/
		return -1;
	}

	for (n=1; n < length; n++){
		if (data[n] == '*'){
			/*--------------------------------------------------
			* DEBUGF("End of NMEA sequence found.\n");
			*--------------------------------------------------*/
			sprintf(crc, "%02X", crc_temp);
			return 0;
		}
		crc_temp ^= data[n];
	}
	return -1;
}

int nmea_validate_sentence(char * data){
	char crc[8];
	uint32_t length = strlen(data);
	if(nmea_crc_calculate(crc, data, length)){
		DEBUGF("GSP_wrong NMEA format.");
		return -1;
	}
	/*--------------------------------------------------
	* DEBUGF("crc calculate : '%s'\n", crc);
	*--------------------------------------------------*/

	char * tmp = strchr(data, '*');
	tmp++;
	/*--------------------------------------------------
	* DEBUGF("crc frame : '%s'\n", tmp);
	*--------------------------------------------------*/
	if( strcmp(tmp, crc) ){
		DEBUGF("GSP_wrong NMEA crc.\n");
		return -1;
	}
	return 0;
}

char * nmea_add_crc(char * data, uint32_t length){
	char crc[8];
	if (nmea_crc_calculate(crc, data, length)){
		DEBUGF("GSP_wrong NMEA format.\n");
		return NULL;
	}
	
	if (strlen(data) + 4 >= length){
		DEBUGF("GSP_wrong Not enought place to insert crc.\n");
		return NULL;
	}
	
	strcat(data, crc);
	strcat(data, "\r\n");
	return data;	
}	

/*--------------------------------------------------
* #define NMEA_INIT_PSRF103		"$PSRF103,%d,%d,%d,01*"
* struct NMEA_PSRF103 PSRF103_tab[] = {
* 	{NMEA_PSRF103_GGA, NMEA_PSRF103_MODE_SET_RATE, NMEA_PSRF103_RATE_ONCE_PER_CYCLE},
* 	{NMEA_PSRF103_GLL, NMEA_PSRF103_MODE_SET_RATE, NMEA_PSRF103_RATE_OFF},
* 	{NMEA_PSRF103_GSA, NMEA_PSRF103_MODE_SET_RATE, NMEA_PSRF103_RATE_OFF},
* 	{NMEA_PSRF103_GSV, NMEA_PSRF103_MODE_SET_RATE, NMEA_PSRF103_RATE_OFF},
* 	{NMEA_PSRF103_RMC, NMEA_PSRF103_MODE_SET_RATE, NMEA_PSRF103_RATE_ONCE_PER_CYCLE},
* 	{NMEA_PSRF103_VTG, NMEA_PSRF103_MODE_SET_RATE, NMEA_PSRF103_RATE_OFF},
* 	{NMEA_PSRF103_ZDA, NMEA_PSRF103_MODE_SET_RATE, NMEA_PSRF103_RATE_OFF}
* };
* #define PSRF103_TAB_NUMBER_ELEMENT	(sizeof(PSRF103_tab)/sizeof(struct NMEA_PSRF103))
* 
* static uint32_t gps_init_PSRF103(uint32_t param_n){
* 	char buffer[64];
* 
* 	snprintf(buffer, sizeof(buffer), NMEA_INIT_PSRF103
* 			, PSRF103_tab[param_n].message_ctrl
* 			, PSRF103_tab[param_n].mode
* 			, PSRF103_tab[param_n].rate);
* 	nmea_add_crc(buffer, sizeof(buffer));
* 
* 	DEBUGF("NMEA init 103 message '%s'.\n", buffer);
* 	int res = gps_write(buffer, strlen(buffer));
* 	if (res < 0 ){ // strlen(buffer)){
* 	 	DEBUGF("GPS WRITE ERROR\n");
* 	}
* 	return 0;
* }
*--------------------------------------------------*/

#define NMEA_INIT_PSRF100		"$PSRF100,%d,%d,8,1,0*"
int nmea_switch_to_sirf(void){

	char buffer[64];

	/*	set prefered messages	*/
	snprintf(buffer, sizeof(buffer), NMEA_INIT_PSRF100
			, sim18_SIRF
			, sim18_4800);
	nmea_add_crc(buffer, sizeof(buffer));

	DEBUGF("NMEA switch to sirf message '%s'.\n", buffer);
	int res = gps_write((unsigned char *)buffer, strlen(buffer));
	if (res < 0){
		DEBUGF("GPS WRITE ERROR\n");
		return -1;
	}
	sim18_port_config.baudrate = sim18_4800;
	sim18_port_config.protocol = sim18_SIRF;
	/*--------------------------------------------------
	* gps_restart();
	*--------------------------------------------------*/

	return 0;
}

#define NMEA_INIT_PSRF104	"$PSRF104,%s,%s,%d,0,%d,%d,%d,%d*"
uint32_t nmea_warn_restart(void){
	char buffer[128];
	char latitude[64];
	char longitude[64];
	
	/*	set prefered messages	*/
	nmea_coordonate_to_string(&mydata.latitude, latitude, sizeof(latitude));
	nmea_coordonate_to_string(&mydata.longitude, longitude, sizeof(longitude));

	snprintf(buffer, sizeof(buffer), NMEA_INIT_PSRF104
			, latitude
			, longitude
			, mydata.altitude
			, mydata.time_of_week
			, mydata.week_no
			, mydata.channel_count
			, mydata.reset_cfg);

	nmea_add_crc(buffer, sizeof(buffer));
	DEBUGF("NMEA init 104 message '%s'.\n", buffer);
	/*--------------------------------------------------
	 * int res = gps_write(buffer, strlen(buffer));
	 * if (res != strlen(buffer)){
	 * 	DEBUGF("GPS WRITE ERROR\n");
	 * 	continue;
	 * }
	 * gps_port_config.baudrate = PSRF100_value.baudrate;
	 * if (gps_port_config.protocol == PSRF100_tab[param_n].protocol){
	 *		gps_restart();
	 *	}
	 *--------------------------------------------------*/

	return 0;
}

#define NMEA_INIT_PSRF117		"$PSRF117,16*0B"
int nmea_stop(void){
	char buffer[64];
	snprintf(buffer, sizeof(buffer), NMEA_INIT_PSRF117);
	DEBUGF("NMEA init 117 message '%s'.\n", buffer);
	/*--------------------------------------------------
	* int res = gps_write(buffer, strlen(buffer));
	* if (res != strlen(buffer)){
	* 	DEBUGF("GPS STOP ERROR\n");
	*  }
	*--------------------------------------------------*/
	return 0;
}

enum {
	WAIT_START,
	FILL_FRAME,
	CRC_1,
	CRC_2,
};

int nmea_get_frame(char data){

	static char *data_ptr;
	char read_value;
	uint32_t frame_completed = 0;

	static uint32_t state = WAIT_START;

	switch (state){
		case WAIT_START:
			data_ptr = data;

			if(read_value == '$'){
				state++;
				*data_ptr = read_value;
				data_ptr++;
			}
			break;
		case FILL_FRAME:
			*data_ptr = read_value;
			data_ptr++;
			if(read_value == '*'){
				state++;
			}
			break;
		case CRC_1:
				state++;
				*data_ptr = read_value;
				data_ptr++;
				break;
		case CRC_2:
				*data_ptr = read_value;
				data_ptr++;
				*data_ptr = 0;
				frame_completed = 1;
				state = WAIT_START;
			break;
		default :
			data_ptr = data;
			frame_completed = 0;
			state = WAIT_START;
			break;
	}

	if (frame_completed){
		if (nmea_validate_sentence(data)){
			DEBUGF("VALIDATE ERROR '%s'\n", data);
		}
		return 0;
	}
	
	return 1;
}

