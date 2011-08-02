#include <stdio.h>
#include <string.h>

#include "stm32f10x.h"


#include "sim18.h"
#include "sirf.h"
#include "tools.h"

#ifdef DEBUG
#define DEBUGF(x, args...) printf(x, ##args)
#else
#define DEBUGF(x, args...)
#endif

#define HI(n)		((n) >> 8)
#define LO(n)		((n) & 0x00ff)

uint8_t * sirf_in_buf;
uint8_t * sirf_out_buf;


#define sirf_CRC_FILL		0
static uint8_t sirf_crc_calculate(uint16_t *crc, uint8_t *data){
	uint16_t crc_temp = sirf_CRC_FILL;
	uint16_t n;
	if(!crc || !data ){
		DEBUGF("GPS_ERROR sirf: Wrong attributes\n");
		return -1;
	}
	uint16_t len = ((uint16_t)(*(data + 2)) << 8) | * (data + 3);
	/*--------------------------------------------------
	* DEBUGF("GPS_ERROR sirf: len = %d.\n", len);
	*--------------------------------------------------*/

	if ((*data != 0xA0) && (* (data + 1) != 0xA2)){
		DEBUGF("GPS_ERROR sirf: GPS data have  wrong start '.\n");
		return -1;
	}

	for (n = 0; n < len; n++){
		crc_temp += (uint16_t)(*(data + 4 + n));
	}
	*crc = crc_temp & 0x7FFF;

	/*--------------------------------------------------
	* DEBUGF("GPS calculate CRC sirf: %04X.\n", *crc);
	*--------------------------------------------------*/
	return 0;

}

int sirf_validate_sentence(void){
	uint16_t crc_calc, crc_frame;
	if(sirf_crc_calculate(&crc_calc, sirf_in_buf)){
		DEBUGF("GSP_wrong sirf format.");
		return -1;
	}

	uint16_t len = ((uint16_t)(*(sirf_in_buf + 2)) << 8) | *(sirf_in_buf + 3);
	crc_frame = (((uint16_t)*(sirf_in_buf + len + 4)) << 8)
					| (uint16_t)*(sirf_in_buf + len + 5);
	DEBUGF("GPS sirf  frame CRC: 0x%04x, calculate CRC: 0x%04x.\n", crc_frame, crc_calc);
	if( crc_calc != crc_frame ){
		DEBUGF("GSP_wrong sirf crc.\n");
		return -2;
	}
	return 0;
}

int sirf_add_crc(uint8_t * data, uint32_t length){
	uint16_t crc;
	if (sirf_crc_calculate(&crc, data)){
		DEBUGF("GSP_wrong sirf format.\n");
		return -1;
	}
	
	uint16_t len = ((uint16_t)(*(data + 2)) << 8) | * (data + 3);

	if (len > length + 8 ){
		return -1;
	}
	
	*(data + len + 4) = (uint8_t)((crc & 0xFF00) >> 8);
	*(data + len + 5) = (uint8_t)(crc & 0x00FF);
	*(data + len + 6) = 0xB0;
	*(data + len + 7) = 0xB3;
	
	return 0;	
}


int sirf_to_nmea(enum sim18_BAUDRATE baudrate){

	static uint8_t msg[] = {
		0xa0, 0xa2, 0x00, 0x18,
		0x81, 
		0x02,
		0x01, 0x01, /* GGA */
		0x00, 0x00, /* suppress GLL */
		0x01, 0x00, /* suppress GSA */
		0x05, 0x00, /* suppress GSV */
		0x01, 0x01, /* RMC */
		0x00, 0x00, /* suppress VTG */
		0x00, 0x01, 0x00, 0x01,
		0x00, 0x01, 0x00, 0x01,
		0x12, 0xc0, /* 4800 bps */
		0x00, 0x00, 0xb0, 0xb3};

	uint16_t length = ((((uint16_t)msg[2]) << 8) | msg[3]) + 8;

	msg[26] = (uint8_t)HI(baudrate);
	msg[27] = (uint8_t)LO(baudrate);

	sirf_add_crc(msg, sizeof(msg));

	memcpy(msg, sirf_out_buf, length);

	sim18_write_data(length);
		
	return 0;
}

int sirf_set_ptf_mode(void){
	static uint8_t msg[] = {
		0xa0, 0xa2, 
		0x00, 0x0F,
		0xA7,
	  	0x00, 0x00, 0x3A, 0x78,	/* Max time for sleep ms (15s)*/
	  	0x00, 0x00, 0x3A, 0x78, /* Max sat search	ms (30s)*/
		0x00, 0x00, 0x01, 0x2C, /* PTF period sec (5min)*/
		0x00, 0x00,					/* Enable adaptative Trickle mode */
		0x00, 0x00, 0xb0, 0xb3
	};
	uint16_t length = ((((uint16_t)msg[2]) << 8) | msg[3]) + 8;

	sirf_add_crc(msg, sizeof(msg));
	print_buf(msg, sizeof(msg));	
	memcpy(msg, sirf_out_buf, length);

	sim18_write_data(length);

	return 0;
}

int sirf_set_trickle_mode(void){
 	static uint8_t msg[] = {
 		0xa0, 0xa2, 
		0x00, 0x09,
 		0x97,							
 		0x00, 0x01,					/* PTF ON=1 OFF=0 */
 		0x00, 0xC8,					/* %Time ON (duty cycle) 500 -> 50%*/
 		0x00, 0x00, 0x00, 0xC8, /* Time on (200ms) */
 		0x00, 0x00, 0xb0, 0xb3
 	};
	uint16_t length = ((((uint16_t)msg[2]) << 8) | msg[3]) + 8;
 	sirf_add_crc(msg, sizeof(msg));
 
 	print_buf(msg, sizeof(msg));	
	memcpy(msg, sirf_out_buf, length);

	sim18_write_data(length);
 	return 0;
}


int sirf_set_msg_41_2s(void){
	static uint8_t msg[] = {

		0xA0, 0xA2, 
		0x00, 0x08,					/* payload length	*/
		0xA6,							/* message id	*/
		0x00,							 
		0x29,							/* Message 41 : Geodetic info	*/
		0x01,							/* Every 1 sec	*/
		0x00, 0x00, 0x00, 0x00, /* Not used, set to zero */
		0x00, 0xAD, 
		0xB0, 0xB3};

	uint16_t length = ((((uint16_t)msg[2]) << 8) | msg[3]) + 8;
	sirf_add_crc(msg, sizeof(msg));

	print_buf(msg, sizeof(msg));	
	memcpy(msg, sirf_out_buf, length);

	sim18_write_data(length);

	return 0;
}

int sirf_stop(void){
	static uint8_t msg[] = {
		0xA0, 0xA2, 0x00, 0x00,
		0xCD, 
		0x10,							 
		0x00, 0xAD, 
		0xB0, 0xB3};

	uint16_t length = ((((uint16_t)msg[2]) << 8) | msg[3]) + 8;
	sirf_add_crc(msg, sizeof(msg));

	print_buf(msg, sizeof(msg));	
	memcpy(msg, sirf_out_buf, length);

	sim18_write_data(length);

	return 0;
}




static int translate_sirf_coordonnate(uint8_t * data, uint8_t *index
		, struct coordonate_s * point){
 	int degree;
 
 	pop_int32(data, &index, &degree);
 
 	point->degree = degree / 10000000;
 	point->minute = (degree - (point->degree *  10000000)) / 60;
 	point->dec_minute = (degree - (point->degree *  10000000)) % 60;
 
/*	if(degree >= 0){
 		point->cardinal = '+';
 	}else{
 		point->cardinal = '-';
 	}	
*/
 	return 0;
}

/*--------------------------------------------------
 * Message ID 41 sample:
*  A0 A2 
*  00 5B
*  29 00 00 02 04 04 E8 1D 97 A7 62 07 D4 02 06 11 36 61 DA 1A 80 01 58 16 47
*  03 DF B7 55 48 8F FF FF FA C8 00 00 04 C6 15 00 00 00 00 00 00 00 00 00 00
*  00 00 00 BB 00 00 01 38 00 00 00 00 00 00 6B 0A F8 61 00 00 00 00 00 1C 13 
*  14 00 00 00 00 00 00 00 00 00 00 00 00 08 05 00 
*  11 03 
*  B0 B3
*--------------------------------------------------*/
int sirf_parse_message_id_41(uint8_t *data){
 	
	int index;
 	gps_mydata.lock = 1;
 
	index = SIRF_MSG_41_NAV_VALID_INDEX;
 	pop_int16(data, &index, &gps_mydata.data_valide);
	pop_int16(data, &index, &gps_mydata.gps_mode);

	index = SIRF_MSG_41_EXT_WEEK_NUM_INDEX;
	pop_int16(data, &index, &gps_mydata.week_no);
 	pop_int16(data, &index, &gps_mydata.time_of_week);

	index = SIRF_MSG_41_LAT_INDEX;
 	translate_sirf_coordonnate(data, &index, &gps_mydata.latitude);
 	translate_sirf_coordonnate(data, &index, &gps_mydata.longitude);

	index = SIRF_MSG_41_ALT_MSL_INDEX;
 	pop_int32(data, &index, &gps_mydata.altitude);

 	index = SIRF_MSG_41_SPEED_OVER_GOURND_INDEX;
 	pop_int16(data, &index, &gps_mydata.speed_horizontal);

	index = SIRF_MSG_41_CLIMB_RATE_INDEX;
 	pop_int16(data, &index, &gps_mydata.speed_vertical);

	index = SIRF_MSG_41_MONTH_INDEX;
 	pop_int16(data, &index, &gps_mydata.date_time.year);
 	gps_mydata.date_time.month	= *(data + index++);
 	gps_mydata.date_time.day	= *(data + index++);
 	gps_mydata.date_time.hour	= *(data + index++);
 	gps_mydata.date_time.minute = *(data + index++);
 	pop_int16(data, &index, &gps_mydata.date_time.seconde);

	index = SIRF_MSG_41_CLOCK_DRIFT_INDEX;
	gps_mydata.clk_drift	= *(data + index++);

	index = SIRF_MSG_41_NB_SV_IN_FIX_INDEX;
 	gps_mydata.sat_number	= *(data + index++);
	
// 	gps_mydata.GPS_ALMANAC_RESET_MODE	= ;
 	gps_mydata.lock = 0;
 	return 0;
}

int sirf_parse_data(void){

	uint8_t * data = sirf_in_buf;
	switch (*(data + 3))
	{
		/*--------------------------------------------------
		* case 0x02:		/ * Measure Navigation Data Out * /
		* 	return sirf_msg_navsol(session, buf, len);
		*--------------------------------------------------*/

		/*--------------------------------------------------
		* case 0x04:		/ * Measured tracker data out * /
		* 	return sirf_msg_svinfo(session, buf, len);
		*--------------------------------------------------*/

		/*--------------------------------------------------
		* case 0x05:		/ * Raw Tracker Data Out * /
		* 	return 0;
		*--------------------------------------------------*/

		/*--------------------------------------------------
		* case 0x06:		/ * Software Version String * /
		* 	return sirf_msg_swversion(session, buf, len);
		*--------------------------------------------------*/

		/*--------------------------------------------------
		* case 0x07:		/ * Clock Status Data * /
		* 	gpsd_report(LOG_PROG, "CLK 0x07\n");
		* 	return 0;
		*--------------------------------------------------*/

		/*--------------------------------------------------
		* case 0x08:		/ * subframe data -- extract leap-second from this * /
		* 	return sirf_msg_navdata(session, buf, len);
		*--------------------------------------------------*/

		/*--------------------------------------------------
		* case 0x09:		/ * CPU Throughput * /
		* 	gpsd_report(LOG_PROG, 
		* 			"THR 0x09: SegStatMax=%.3f, SegStatLat=%3.f, AveTrkTime=%.3f, Last MS=%3.f\n", 
		* 			(float)getuw(buf, 1)/186, (float)getuw(buf, 3)/186, 
		* 			(float)getuw(buf, 5)/186, getuw(buf, 7));
		* 	return 0;
		*--------------------------------------------------*/

		/*--------------------------------------------------
		* case 0x0a:		/ * Error ID Data * /
		* 	return sirf_msg_errors(buf, len);
		*--------------------------------------------------*/

		/*--------------------------------------------------
		* case 0x0b:		/ * Command Acknowledgement * /
		* 	gpsd_report(LOG_PROG, "ACK 0x0b: %02x\n",getub(buf, 1));
		* 	return 0;
		*--------------------------------------------------*/

		/*--------------------------------------------------
		* case 0x0c:		/ * Command NAcknowledgement * /
		* 	gpsd_report(LOG_PROG, "NAK 0x0c: %02x\n",getub(buf, 1));
		* 	return 0;
		*--------------------------------------------------*/

		/*--------------------------------------------------
		* case 0x0d:		/ * Visible List * /
		* 	DEBUGF("PARSE message ID 41.\n");
		* 	/ *--------------------------------------------------
		* 	* sirf_parse_message_id_41(data);
		* 	*--------------------------------------------------* /
		* 	return 0;
		*--------------------------------------------------*/

		/*--------------------------------------------------
		* case 0x0e:		/ * Almanac Data * /
		* 	gpsd_report(LOG_PROG, "ALM  0x0e: %s\n", gpsd_hexdump(buf, len));
		* 	return 0;
		*--------------------------------------------------*/

		/*--------------------------------------------------
		* case 0x0f:		/ * Ephemeris Data * /
		* 	gpsd_report(LOG_PROG, "EPH  0x0f: %s\n", gpsd_hexdump(buf, len));
		* 	return 0;
		*--------------------------------------------------*/

		/*--------------------------------------------------
		* case 0x11:		/ * Differential Corrections * /
		* 	gpsd_report(LOG_PROG, "DIFF 0x11: %s\n", gpsd_hexdump(buf, len));
		* 	return 0;
		*--------------------------------------------------*/

		/*--------------------------------------------------
		* case 0x12:		/ * OK To Send * /
		* 	gpsd_report(LOG_PROG, "OTS 0x12: send indicator = %d\n",getub(buf, 1));
		* 	return 0;
		*--------------------------------------------------*/

		/*--------------------------------------------------
		* case 0x1b:		/ * DGPS status (undocumented) * /
		* 	return 0;
		*--------------------------------------------------*/

		/*--------------------------------------------------
		* case 0x1c:		/ * Navigation Library Measurement Data * /
		* 	gpsd_report(LOG_PROG, "NLMD 0x1c: %s\n", gpsd_hexdump(buf, len));
		* 	return 0;
		*--------------------------------------------------*/

		/*--------------------------------------------------
		* case 0x1d:		/ * Navigation Library DGPS Data * /
		* 	gpsd_report(LOG_PROG, "NLDG 0x1d: %s\n", gpsd_hexdump(buf, len));
		* 	return 0;
		*--------------------------------------------------*/

		/*--------------------------------------------------
		* case 0x1e:		/ * Navigation Library SV State Data * /
		* 	gpsd_report(LOG_PROG, "NLSV 0x1e: %s\n", gpsd_hexdump(buf, len));
		* 	return 0;
		*--------------------------------------------------*/

		/*--------------------------------------------------
		* case 0x1f:		/ * Navigation Library Initialization Data * /
		* 	gpsd_report(LOG_PROG, "NLID 0x1f: %s\n", gpsd_hexdump(buf, len));
		* 	return 0;
		*--------------------------------------------------*/

		/*--------------------------------------------------
		* case 0x29:		/ * Geodetic Navigation Information * /
		* 	return sirf_msg_geodetic(session, buf, len);
		*--------------------------------------------------*/

		/*--------------------------------------------------
		* case 0x32:		/ * SBAS corrections * /
		* 	return 0;
		*--------------------------------------------------*/

		/*--------------------------------------------------
		* case 0x34:		/ * PPS Time * /
		* 	return sirf_msg_ppstime(session, buf, len);
		*--------------------------------------------------*/

		/*--------------------------------------------------
		* case 0x62:		/ * uBlox Extended Measured Navigation Data * /
		* 	return sirf_msg_ublox(session, buf, len);
		*--------------------------------------------------*/

		/*--------------------------------------------------
		* case 0x80:		/ * Initialize Data Source * /
		* 	gpsd_report(LOG_PROG, "INIT 0x80: %s\n", gpsd_hexdump(buf, len));
		* 	return 0;
		*--------------------------------------------------*/

		/*--------------------------------------------------
		* case 0xe1:		/ * Development statistics messages * /
		* 	/ * FALLTHROUGH * /
		* case 0xff:		/ * Debug messages * /
		* 	(void)sirf_msg_debug(buf, len);
		* 	return 0;
		*--------------------------------------------------*/

		default:
			/*--------------------------------------------------
			* gpsd_report(LOG_WARN, "Unknown SiRF packet id %d length %d: %s\n", 
			* 		buf[0], len, gpsd_hexdump(buf, len));
			*--------------------------------------------------*/
			return 0;
	}

	return *data;
}


enum {
	SIRF_WAIT_START1,
	SIRF_WAIT_START2,
	SIRF_WAIT_LENGTH1,
	SIRF_WAIT_LENGTH2,
	SIRF_FILL_FRAME,
	SIRF_WAIT_CRC_1,
	SIRF_WAIT_CRC_2,
	SIRF_WAIT_END1,
	SIRF_WAIT_END2,

};

void sirf_get_frame(uint8_t read_value){

	static uint8_t *data_ptr;
	static uint16_t frame_length = 0;
	static uint16_t frame_crc = 0;
	static uint16_t frame_byte_number = 0;
	static uint32_t state = SIRF_WAIT_START1;

	uint32_t frame_completed = 0;

	switch (state){
		case SIRF_WAIT_START1:
			data_ptr = sirf_in_buf;
			if(read_value == (unsigned char)SIRF_CHAR_START_1){
				state++;
				*data_ptr = (uint8_t)read_value;
				data_ptr++;
			}
			break;
		case SIRF_WAIT_START2:
			if(read_value == (unsigned char)SIRF_CHAR_START_2){
				state++;
				*data_ptr = (uint8_t)read_value;
				data_ptr++;
			}else{
				state = SIRF_WAIT_START1;	
			}
			break;
		case SIRF_WAIT_LENGTH1:
			state++;
			*data_ptr = read_value;
			data_ptr++;
			frame_length = ((uint16_t)read_value) << 8;
			break;
		case SIRF_WAIT_LENGTH2:
			state++;
			*data_ptr = (uint8_t)read_value;
			data_ptr++;
			frame_length |= read_value;
			break;
		case SIRF_FILL_FRAME:
			*data_ptr = (uint8_t)read_value;
			data_ptr++;
			frame_byte_number++;
			if (frame_byte_number == 1 
					&& (uint8_t)read_value != 0x29){
				state = SIRF_WAIT_START1;
			}
			if(frame_byte_number == frame_length){
				state++;
			}
			break;
		case SIRF_WAIT_CRC_1:
			state++;
			*data_ptr = (uint8_t)read_value;
			data_ptr++;
			frame_crc = ((uint16_t)read_value) << 8;
			break;
		case SIRF_WAIT_CRC_2:
			*data_ptr = (uint8_t)read_value;
			frame_crc |= read_value;
			data_ptr++;
			state++;
			break;
		case SIRF_WAIT_END1:
			if(read_value == (unsigned char)SIRF_CHAR_END_1){
				state++;
				*data_ptr = (uint8_t)read_value;
				data_ptr++;
			}else{
				state = SIRF_WAIT_START1;	
			}
			break;
		case SIRF_WAIT_END2:
			if(read_value == (unsigned char)SIRF_CHAR_END_2){
				*data_ptr = (uint8_t)read_value;
				data_ptr++;
				frame_completed = 1;
			}
			state = SIRF_WAIT_START1;	
			break;

		default :
			state = SIRF_WAIT_START1;
			break;
	}
	if (frame_completed){
		/*--------------------------------------------------
		* print_buf(data, frame_length + 8);
		*--------------------------------------------------*/
		if (sirf_validate_sentence()){
			DEBUGF("VALIDATE ERROR '%s'\n", sirf_in_buf);
		}else{
			sirf_parse_data();
		}
	}
}

void sirf_init(void){
	sirf_in_buf  = sim18_in_buf;
	sirf_out_buf = sim18_out_buf;

	/*--------------------------------------------------
	* sirf_set_trickle_mode();
	* sirf_set_ptf_mode();
	*--------------------------------------------------*/
	sirf_set_msg_41_2s();
	// sirf_to_nmea(4800);

}


