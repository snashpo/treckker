#ifndef SHT1x_H_
#define SHT1x_H_

///////////////////////////////////////////////////////////////////////////////
// SHT1x.h : header file
// 
//
///////////////////////////////////////////////////////////////////////////////
// All rights reserved - not to be sold.
///////////////////////////////////////////////////////////////////////////////

//#define DEW_POINT								0																

/* Port definition	*/
#define SHT1x_PORT								GPIOC

/*	Pin definition	*/
#define SHT1x_GND_PIN								GPIO_Pin_0
#define SHT1x_DATA_PIN								GPIO_Pin_1
#define SHT1x_CLOCK_PIN								GPIO_Pin_2
#define SHT1x_VCC_PIN								GPIO_Pin_3


/* Macros definition	*/

#define SHT1x_CLK_FALLING_EDGE()						GPIO_ResetBits(SHT1x_PORT, SHT1x_CLOCK_PIN)
#define SHT1x_CLK_RISING_EDGE()						GPIO_SetBits(SHT1x_PORT, SHT1x_CLOCK_PIN)

#define SHT1x_SET_DATA_LOW()							GPIO_ResetBits(SHT1x_PORT, SHT1x_DATA_PIN)
#define SHT1x_SET_DATA_HIGH()							GPIO_SetBits(SHT1x_PORT, SHT1x_DATA_PIN)

#define SHT1x_SWITCH_OFF()								GPIO_SetBits(SHT1x_PORT, SHT1x_GND_PIN)
#define SHT1x_SWITCH_ON()								GPIO_ResetBits(SHT1x_PORT, SHT1x_GND_PIN)

#define SHT1x_READ_DATA()								GPIO_ReadInputDataBit(SHT1x_PORT, SHT1x_DATA_PIN)	

#define SHT1x_SET_DATA_AS_INPUT()					{ \
																	GPIO_InitTypeDef GPIO_InitStructure;\
																	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;\
																	GPIO_InitStructure.GPIO_Pin = SHT1x_DATA_PIN;\
																	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;\
																	GPIO_Init(SHT1x_PORT, &GPIO_InitStructure);\
																}while(0);

#define SHT1x_SET_DATA_AS_OUTPUT()					{ \
																	GPIO_InitTypeDef GPIO_InitStructure;\
																	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;\
																	GPIO_InitStructure.GPIO_Pin = SHT1x_DATA_PIN;\
																	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;\
																	GPIO_Init(SHT1x_PORT, &GPIO_InitStructure);\
																}while(0);



/*	Component definitions	*/
#define SHT1x_STATUS_HEATER							0x04
#define SHT1x_STATUS_NO_RELOAD						0x02
#define SHT1x_STATUS_MODE_ECONOMY					0x01
#define SHT1x_STATUS_MODE_NORMAL						0x00
#define SHT1x_MODE										SHT1x_STATUS_MODE_ECONOMY

#define HUMIDITY_MODE_NORMAL_MASK					0x0F
#define TEMPERATURE_MODE_NORMAL_MASK				0x3F
#define TEMPERATURE_MODE_ECO_MASK					0x0F 

#define SHT1x_CMD_MEASURE_TEMPERATURE				0x03
#define SHT1x_CMD_MEASURE_HUMIDITY					0x05
#define SHT1x_CMD_READ_STATUS							0x07
#define SHT1x_CMD_WRITE_STATUS						0x06
#define SHT1x_CMD_SOFT_RESET							0x1D

#define SHT1x_TSCLK										110
#define SHT1x_TSCLK_HALF								55
#define SHT1x_START_TIME_MS							15

struct sht1x_s{
	bool lock;
	uint8_t valid;
	uint8_t last_cmd;
	int16_t temperature;
	uint16_t humidity;
#ifdef DEW_POINT
	int16_t dewpoint;
#endif
};

enum sht1x_data_n {
	TEMPERATURE = 0,
	HUMIDITY = 1
#ifdef DEW_POINT
	, DEWPOINT = 2
#endif
};

#if (SHT1x_MODE == SHT1x_STATUS_MODE_ECONOMY)
	static const double C1 = -2.0468;
	static const double C2 = 0.5872;
	static const double C3 = -4.0845E-4;

	static const double D1 = -39.72;
	static const double D2 = 0.04;
#else

	static const double C1 = -2.0468;
	static const double C2 = 0.0367;
	static const double C3 = -1.595E-6;

	static const double D1 = -39.72;
	static const double D2 = 0.01;
#endif

#ifdef DEW_POINT
	static const double Tn_sup = 243.12;
	static const double m_sup = 17.62;
	static const double Tn_inf = 272.62;
	static const double m_inf = 22.46;
#endif

static inline void sht1x_release(void){
	SHT1x_SET_DATA_AS_INPUT();
	SHT1x_CLK_FALLING_EDGE();
}
#include "hw_config.h"
//----------------------------------------------------------------------------------
//		static inline void send_start(void)
//----------------------------------------------------------------------------------
// generates a transmission start 
//       _____         ________
// DATA:      |_______|
//           ___     ___
// SCK : ___|   |___|   |______
static inline void send_start(void){
	SHT1x_SET_DATA_AS_OUTPUT();

	SHT1x_SET_DATA_HIGH();
	ndelay(SHT1x_TSCLK);
	SHT1x_CLK_RISING_EDGE();
	ndelay(SHT1x_TSCLK_HALF);
	SHT1x_SET_DATA_LOW();
	ndelay(SHT1x_TSCLK_HALF);
	SHT1x_CLK_FALLING_EDGE();
	ndelay(SHT1x_TSCLK);
	SHT1x_CLK_RISING_EDGE();
	ndelay(SHT1x_TSCLK_HALF);
	SHT1x_SET_DATA_HIGH();
	ndelay(SHT1x_TSCLK_HALF);
	SHT1x_CLK_FALLING_EDGE();
	ndelay(SHT1x_TSCLK);
}

static inline void send_NACK(void){
	// NACK the second measure byte to avoid CRC transmission
	SHT1x_SET_DATA_AS_OUTPUT();

	SHT1x_SET_DATA_HIGH();
	ndelay(SHT1x_TSCLK_HALF);
	SHT1x_CLK_RISING_EDGE();
	ndelay(SHT1x_TSCLK);
	SHT1x_CLK_FALLING_EDGE();
	ndelay(SHT1x_TSCLK);	
}

static inline void send_ACK(void){
	// Ack the first measure byte
	SHT1x_SET_DATA_AS_OUTPUT();

	SHT1x_SET_DATA_LOW();
	ndelay(SHT1x_TSCLK_HALF);	
	SHT1x_CLK_RISING_EDGE();
	ndelay(SHT1x_TSCLK);
	SHT1x_CLK_FALLING_EDGE();	
	ndelay(SHT1x_TSCLK);	
}

inline static int read_ACK(void){
	uint8_t res;
// Read ACK from sht1x
	SHT1x_SET_DATA_AS_INPUT();

	ndelay(SHT1x_TSCLK_HALF);
	SHT1x_CLK_RISING_EDGE();	
	ndelay(SHT1x_TSCLK_HALF);	
	res = SHT1x_READ_DATA();	
	ndelay(SHT1x_TSCLK_HALF);
	SHT1x_CLK_FALLING_EDGE();		
	ndelay(SHT1x_TSCLK);
	return res;
}
//----------------------------------------------------------------------------------
//		static inline void send_connection_reset(void)
//----------------------------------------------------------------------------------
// communication reset: DATA-line=1 and at least 9 SCK cycles followed by transstart
//       _____________________________________________________         ________
// DATA:                                                      |_______|
//          _    _    _    _    _    _    _    _    _        ___     ___
// SCK : __| |__| |__| |__| |__| |__| |__| |__| |__| |______|   |___|   |______
static inline void send_connection_reset(void){
	uint8_t i;
	SHT1x_SET_DATA_AS_OUTPUT();

	SHT1x_SET_DATA_HIGH();
	SHT1x_CLK_FALLING_EDGE();
	ndelay(SHT1x_TSCLK);

	for(i = 0; i< 9; i++){
		SHT1x_CLK_RISING_EDGE();
		ndelay(SHT1x_TSCLK);
		SHT1x_CLK_FALLING_EDGE();
		ndelay(SHT1x_TSCLK);
	}

	send_start();
}


/*	Function declartions	*/
/**	Public	**/
void SHT1x_Config(void);
void SHT1x_Init(void);
void SHT1x_acquire_data(void);
void SHT1x_GetData(enum sht1x_data_n);

//--------------------------------------------------
// extern void SHT1x_StartMeasure();
// extern void SHT1xGet_Result(void);
//-------------------------------------------------- 

#endif /*SHT1x_H_*/
