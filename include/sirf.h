#ifndef __SIRF_H__
#define __SIRF_H__


#define SIRF_CHAR_START_1				((uint8_t)0xA0)
#define SIRF_CHAR_START_2				((uint8_t)0xA2)
#define SIRF_CHAR_END_1					((uint8_t)0xB0)
#define SIRF_CHAR_END_2					((uint8_t)0xB3)



#define SIRF_MSG_41_ID_INDEX									4
#define SIRF_MSG_41_NAV_VALID_INDEX							5
#define SIRF_MSG_41_NAV_TYPE_INDEX							7
#define SIRF_MSG_41_EXT_WEEK_NUM_INDEX						9
#define SIRF_MSG_41_TOW_INDEX									11
#define SIRF_MSG_41_YEAR_INDEX								15
#define SIRF_MSG_41_MONTH_INDEX								17
#define SIRF_MSG_41_DAY_INDEX									18
#define SIRF_MSG_41_HOUR_INDEX								19
#define SIRF_MSG_41_MINUTE_INDEX								20
#define SIRF_MSG_41_SECOND_INDEX								21
#define SIRF_MSG_41_SAT_LST_INDEX							23
#define SIRF_MSG_41_LAT_INDEX									27
#define SIRF_MSG_41_LON_INDEX									31
#define SIRF_MSG_41_ALT_ELIPS_INDEX							35
#define SIRF_MSG_41_ALT_MSL_INDEX							39
#define SIRF_MSG_41_MAP_DATUM_INDEX							40
#define SIRF_MSG_41_SPEED_OVER_GOURND_INDEX				41
#define SIRF_MSG_41_COURSE_OVER_GROUND_INDEX				43
#define SIRF_MSG_41_MAGNETIC_VARIATION_INDEX				45
#define SIRF_MSG_41_CLIMB_RATE_INDEX						47
#define SIRF_MSG_41_HEADING_RATE_INDEX						49
#define SIRF_MSG_41_EST_HORIZONTAL_ERROR_INDEX			51
#define SIRF_MSG_41_EST_VERTICAL_ERROR_INDEX				55
#define SIRF_MSG_41_EST_TIME_ERROR_INDEX					59
#define SIRF_MSG_41_EST_VELOCITY_ERROR_INDEX				63
#define SIRF_MSG_41_CLOCK_BIAS_INDEX						65
#define SIRF_MSG_41_CLOCK_BIAS_ERROR_INDEX				69
#define SIRF_MSG_41_CLOCK_DRIFT_INDEX						73
#define SIRF_MSG_41_CLOCK_DRIFT_ERROR_INDEX				77
#define SIRF_MSG_41_DISTANCE_INDEX							81
#define SIRF_MSG_41_DISTANCE_ERROR_INDEX					85
#define SIRF_MSG_41_HEADING_ERROR_INDEX					87
#define SIRF_MSG_41_NB_SV_IN_FIX_INDEX						89
#define SIRF_MSG_41_HODP_INDEX								90
#define SIRF_MSG_41_ADD_MODE_INFO_INDEX					91

extern struct sim18_serial_settings_s sim18_port_config;
extern struct sim18_data_s gps_mydata;
extern uint8_t sim18_in_buf[];
extern uint8_t sim18_out_buf[];


int sirf_add_crc(uint8_t * data, uint32_t length);
int sirf_validate_sentence(void);
void sirf_init( void );

void sirf_get_frame(uint8_t data);
int sirf_parse_data(void);




#endif
