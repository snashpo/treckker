#include <stdio.h>

#include "stm32f10x.h"

#include "sim18.h"
#include "nmea.h"
#include "sirf.h"
#include "tools.h"



#ifdef DEBUG
#define DEBUGF(x, args...) printf(x, ##args)
#else
#define DEBUGF(x, args...)
#endif

#define NMEA_CRC_FILL 0


uint8_t * nmea_in_buf;
uint8_t * nmea_out_buf;

/*--------------------------------------------------
* int convert_str_to_point(char *data, struct coordonate_s * point){
* 	/ *--------------------------------------------------
* 	* //3113.3156
* 	* float pt =  strtof(data, NULL);
* 	* point->degree = (uint16_t)(pt / 100);
* 	* point->minute = (uint16_t)(pt % 100));
* 	* point->dec_minute = (uint32_t)((pt - (point->degree * 100 + point->minute)) * 10000);
* 	*--------------------------------------------------* /
* 	char * tmp = data;	
* 	uint8_t i = 0; 
* 	uint32_t val = 0;
* 
* 	while ( *(data + i) != '.'){
* 		val <<= 8;
* 		val |=  *(tmp + i);
* 	}	
* 	point->degree = (uint16_t)(val / 100);
* 	point->minute = (uint16_t)(val % 100);
* 
* 	tmp = strchr(data, ".");
* 	point->dec_minute = 0;
* 	i = 0;
* 	while ( *(tmp + i) != ','){
* 		point->dec_minute <<= 8;
* 		point->dec_minute |=  *(tmp + i);
* 	}	
* 
* 	return 0;
* }
*--------------------------------------------------*/


void nmea_coordonate_to_string(struct coordonate_s *point, char * string, uint32_t length){
	sprintf(string, "%c%d.%02d%05d"
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
//'$GPRMC,12019.000,A,4317.4396,N,00529.7541,E,0.57,171.53,070711,,,A'
static int nmea_parse_RMC(char *data){
	/*--------------------------------------------------
	* char * ptr_str;
	* uint32_t state = RMC_MESAGE_ID;
	* float tmp_float;
	* uint32_t tmp;
	*--------------------------------------------------*/

	/*--------------------------------------------------
	* for (ptr_str = strtok(data, NMEA_SEPARATOR); ptr_str; ptr_str = strtok(NULL, NMEA_SEPARATOR)){
	* 	switch(state){
	* 		case 	RMC_MESAGE_ID:
	* 			break;
	*--------------------------------------------------*/

	/*--------------------------------------------------
	* 		case 	RMC_UTC_TIME:
	* 			tmp = (uint32_t)strtof(ptr_str, NULL);
	* 			gps_mydata.date_time.hour = (uint8_t)(tmp / 10000);
	* 			gps_mydata.date_time.minute = (uint8_t)((tmp - (gps_mydata.date_time.hour * 10000))/100);
	* 			gps_mydata.date_time.seconde = (uint8_t)(tmp - ( gps_mydata.date_time.hour * 10000)
	* 					- ( gps_mydata.date_time.minute * 100));
	*--------------------------------------------------*/

	/*--------------------------------------------------
	* 			sprintf(gps_mydata.date_time.time_str, "%d:%d:%d"
	* 					, gps_mydata.date_time.hour
	* 					, gps_mydata.date_time.minute
	* 					, gps_mydata.date_time.seconde);
	* 			break;
	*--------------------------------------------------*/

	/*--------------------------------------------------
	* 		case 	RMC_STATUS:
	* 			break;
	*--------------------------------------------------*/

	/*--------------------------------------------------
	* 		case 	RMC_LATITUDE:
	* 			//convert_str_to_point(ptr_str, &gps_mydata.latitude);
	* 			break;
	*--------------------------------------------------*/

	/*--------------------------------------------------
	* 		case 	RMC_NS_INDICATOR:
	* 			//gps_mydata.latitude.cardinal = *ptr_str;
	* 			break;
	*--------------------------------------------------*/

	/*--------------------------------------------------
	* 		case 	RMC_LONGITUDE:
	* 			//convert_str_to_point(ptr_str, &gps_mydata.longitude);
	* 			break;
	*--------------------------------------------------*/

	/*--------------------------------------------------
	* 		case 	RMC_EO_INDICATOR:
	* 			//gps_mydata.longitude.cardinal = *ptr_str;
	* 			break;
	*--------------------------------------------------*/

	/*--------------------------------------------------
	* 		case 	RMC_SPEED:
	* 			tmp_float = strtof(ptr_str, NULL);
	* 			gps_mydata.speed_horizontal = (uint16_t) (tmp_float * KNOT_TO_MS);
	* 			break;
	*--------------------------------------------------*/

	/*--------------------------------------------------
	* 		case 	RMC_COURSE:
	* 			//gps_mydata.azimuth = (uint16_t) (strtol(ptr_str, NULL, 10));
	* 			break;
	*--------------------------------------------------*/

	/*--------------------------------------------------
	* 		case 	RMC_DATE:
	* 			tmp = (uint32_t)strtol(ptr_str, NULL, 10);
	* 			gps_mydata.date_time.day = (uint8_t)(tmp / 10000);
	* 			gps_mydata.date_time.month = (uint8_t)((tmp - (gps_mydata.date_time.day * 10000))/100);
	* 			gps_mydata.date_time.year = 2000 + (uint8_t)(tmp - ( gps_mydata.date_time.day * 10000)
	* 					- ( gps_mydata.date_time.month * 100));
	*--------------------------------------------------*/

	/*--------------------------------------------------
	* 			sprintf(gps_mydata.date_time.date_str, "%d/%d/%d"
	* 					, gps_mydata.date_time.day
	* 					, gps_mydata.date_time.month
	* 					, gps_mydata.date_time.year);
	*--------------------------------------------------*/

	/*--------------------------------------------------
	* 			break;
	*--------------------------------------------------*/

	/*--------------------------------------------------
	* 		case 	RMC_MAGNETIC_VARIATION:
	* 			break;
	*--------------------------------------------------*/

	/*--------------------------------------------------
	* 		case 	RMC_EASTWEST_INDICATOR:
	* 			break;
	*--------------------------------------------------*/

	/*--------------------------------------------------
	* 		case RMC_MODE:
	* 			//gps_mydata.gps_mode = *ptr_str;
	* 			break;
	* 	}
	*--------------------------------------------------*/

	/*--------------------------------------------------
	* state++;
	* }
	*--------------------------------------------------*/
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
   GGA_CS
};

int nmea_parse_GGA(char *data){

 	/*--------------------------------------------------
	* char * ptr_str;
	* uint32_t state = GGA_MESSAGE_ID;
	*--------------------------------------------------*/

	/*--------------------------------------------------
	* for (ptr_str = strtok(data, NMEA_SEPARATOR); ptr_str; ptr_str = strtok(NULL, NMEA_SEPARATOR)){
	* 	switch(state){
	* 		case GGA_MESSAGE_ID:
	* 			break;
	* 		case 	GGA_UTC_TIME:
	* 			break;
	* 		case GGA_LATITUDE:
	* 			convert_str_to_point(ptr_str, &gps_mydata.latitude);
	* 			break;
	* 		case GGA_NS_INDICATOR:
	* 			gps_mydata.latitude.cardinal = *ptr_str;
	* 			break;
	* 		case GGA_LONGITUDE:
	* 			convert_str_to_point(ptr_str, &gps_mydata.longitude);
	* 			break;
	* 		case GGA_EW_INDICATOR:
	* 			gps_mydata.longitude.cardinal = *ptr_str;
	* 			break;
	* 		case GGA_POSITION_FIX:
	* 			if( *ptr_str != '0' ){
	* 				gps_mydata.data_valide = 1;
	* 				if (*ptr_str == '1' ){
	* 					gps_mydata.gps_mode = 'G';
	* 				}else if (*ptr_str == '2' ){
	* 					gps_mydata.gps_mode = 'D';
	* 				}else if (*ptr_str == '6' ){
	* 					gps_mydata.gps_mode = 'R';
	* 				}
	* 			} else {
	* 				gps_mydata.data_valide = 0;
	* 			}
	* 			break;
	* 		case GGA_SATELITE_USED:
	* 			gps_mydata.sat_number = (uint8_t)strtol(ptr_str, NULL, 10);
	* 			break;
	* 		case GGA_HDOP:
	* 			break;
	* 		case GGA_MSL_ALTITUDE:
	* 			gps_mydata.altitude = (uint16_t)strtof(ptr_str, NULL);
	* 			break;
	* 		case GGA_UNITS_ALT:
	* 			break;
	* 		case GGA_GEOID_SEPARATION:
	* 			break;
	* 		case GGA_UNITS_SEP:
	* 			break;
	* 		case GGA_AGE_OF_DIFF:
	* 			break;
	* 		case GGA_DIFF_REF_STATION_ID:
	* 			break;
	* 		case GGA_CS:
	* 			break;
	* 	}
	*--------------------------------------------------*/

	/*--------------------------------------------------
	* state++;
	* }
	*--------------------------------------------------*/
	return 0;
}

/*--------------------------------------------------
* int nmea_parse_data(void){
* 
* 	char * tmp = strchr(nmea_in_buf, '*');
* 	
* 	if ( !tmp ){
* 		DEBUGF("NMEA PARSE DATA ERROR, '*' not found.\n");
* 		return -1;
* 	}
* 	if (!strncmp(nmea_in_buf, "$GPRMC", strlen("$GPRMC"))){
* 		*tmp = 0;
* 		return nmea_parse_RMC(nmea_in_buf);
* 	} else if (!strncmp(nmea_in_buf, "$GPGGA", strlen("$GPGGA"))){
* 		*tmp = 0;
* 		return nmea_parse_GGA(nmea_in_buf);
* 	} else {
* 		DEBUGF("NMEA PARSE '%s'\n", nmea_in_buf);
* 	}
* 	return 0;
* }
*--------------------------------------------------*/

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

	for (n = 1; n < length; n++){
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

int nmea_validate_sentence(uint16_t length){
	
	char crc[8];
	
	if(nmea_crc_calculate(crc, nmea_in_buf, length - 4)){
		DEBUGF("GSP_wrong NMEA format.");
		return -1;
	}
	/*--------------------------------------------------
	* DEBUGF("crc calculate : '%s'\n", crc);
	*--------------------------------------------------*/

	char * tmp = nmea_in_buf + length - 4;
	tmp++;
	/*--------------------------------------------------
	* DEBUGF("crc frame : '%s'\n", tmp);
	*--------------------------------------------------*/
	if( (*tmp != *crc) && (*(tmp + 1) != *(crc + 1))){
		DEBUGF("GSP_wrong NMEA crc.\n");
		return -1;
	}
	return 0;
}

uint32_t nmea_add_crc(char * data, uint32_t length){
	char crc[8];
	if (nmea_crc_calculate(crc, data, length)){
		DEBUGF("GSP_wrong NMEA format.\n");
		return 0;
	}
	
	*(data + length) = crc[0];
	*(data + length + 1) = crc[1];
	*(data + length + 2) = '\r';
	*(data + length + 3) = '\n';
	*(data + length + 4) = 0;

	return length + 4;	
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
int nmea_switch_to_sirf(enum sim18_BAUDRATE baudrate){

	char * buffer = nmea_out_buf;
	uint32_t length;

	while(*nmea_out_buf);

	/*	set prefered messages	*/
	length = sprintf(buffer, NMEA_INIT_PSRF100
			, sim18_SIRF
			, baudrate);
	length = nmea_add_crc(buffer, length);

	DEBUGF("NMEA switch to sirf message '%s'.\n", buffer);

	sim18_write_data(length);

	return 0;
}

#define NMEA_INIT_PSRF104	"$PSRF104,%s,%s,%d,0,%d,%d,%d,%d*"
void nmea_warn_restart(void){
	char *buffer = nmea_out_buf;
	char latitude[64];
	char longitude[64];
	uint32_t length;

	while(*nmea_out_buf);

	/*	set prefered messages	*/
	nmea_coordonate_to_string(&gps_mydata.latitude, latitude, sizeof(latitude));
	nmea_coordonate_to_string(&gps_mydata.longitude, longitude, sizeof(longitude));

	length = sprintf(buffer, NMEA_INIT_PSRF104
			, latitude
			, longitude
			, gps_mydata.altitude
			, gps_mydata.time_of_week
			, gps_mydata.week_no
			, gps_mydata.channel_count
			, gps_mydata.reset_cfg);

	length = nmea_add_crc(buffer, length);
	DEBUGF("NMEA init 104 message '%s'.\n", buffer);
	sim18_write_data(length);
}

#define NMEA_INIT_PSRF117			"$PSRF117,16*"
#define NMEA_INIT_PSRF117_CRC		"0B"
void nmea_stop(void){
	
	char * buffer = nmea_out_buf;
	uint32_t length;

	while(*nmea_out_buf);

	length = sprintf(buffer, "%s%s", NMEA_INIT_PSRF117, NMEA_INIT_PSRF117_CRC);
	DEBUGF("NMEA init 117 message '%s'.\n", buffer);
	sim18_write_data(length);
}

enum {
	WAIT_START,
	FILL_FRAME,
	CRC_1,
	CRC_2
};

int nmea_get_frame(char read_value){

	static uint8_t *data_ptr;
	static uint32_t state = WAIT_START;
	
	uint32_t frame_completed = 0;

	switch (state){
		case WAIT_START:
			data_ptr = nmea_in_buf;

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
			data_ptr = nmea_in_buf;
			frame_completed = 0;
			state = WAIT_START;
			break;
	}

	if (frame_completed){
		if (nmea_validate_sentence((uint16_t)(data_ptr - nmea_in_buf))){
			DEBUGF("VALIDATE ERROR.\n");
		}else{
			DEBUGF("FRAME VALID.\n");
			/*--------------------------------------------------
			* nmea_parse_data();
			*--------------------------------------------------*/
		}
		return 0;
	}
	
	return 1;
}

void nmea_init(void){
	
	nmea_in_buf = sim18_in_buf;
	nmea_out_buf = sim18_out_buf;
	
}
