#include <stdio.h>
#include <math.h>
#include <string.h>
#include "stm32f10x.h"

#include "sht1x.h"
#include "timer.h"
#include "hw_config.h"

#ifdef DEBUG
#define DEBUGF(x, args...) printf(x, ##args)
#else
#define DEBUGF(x, args...)
#endif

struct sht1x_s mydata;


static uint8_t ReadByte(void)
{
	uint8_t l_uiLoop = 8;
	uint8_t myByte = 0;
	
	SHT1x_SET_DATA_AS_INPUT();

	ndelay(SHT1x_TSCLK_HALF);

	while(l_uiLoop--)
	{
		ndelay(SHT1x_TSCLK_HALF);
		SHT1x_CLK_RISING_EDGE();	
		ndelay(SHT1x_TSCLK_HALF);

		if ( SHT1x_READ_DATA() ){
			myByte |= (1 << l_uiLoop);
		}

		ndelay(SHT1x_TSCLK_HALF);
		SHT1x_CLK_FALLING_EDGE();	
		ndelay(SHT1x_TSCLK_HALF);

	}
	return myByte;	
}

static void WriteByte(uint8_t p_ucValue)
{
	uint8_t l_uiLoop = 8;
	
	SHT1x_SET_DATA_AS_OUTPUT();
	ndelay(SHT1x_TSCLK_HALF);

	while(l_uiLoop--)
	{
		ndelay(SHT1x_TSCLK_HALF);
		
		if ( p_ucValue & ( 0x01 << l_uiLoop ) ){
			SHT1x_SET_DATA_HIGH();
		} else {
			SHT1x_SET_DATA_LOW();
		}

		SHT1x_CLK_RISING_EDGE();
		ndelay(SHT1x_TSCLK);
		SHT1x_CLK_FALLING_EDGE();	
		ndelay(SHT1x_TSCLK_HALF);

	}
	
	read_ACK();
}

void WriteStatus(uint8_t mode)
{
	send_start();

	WriteByte(SHT1x_CMD_WRITE_STATUS);

	WriteByte(mode);

	sht1x_release();
}

/*--------------------------------------------------
* void CalculateRealValues(uint8_t * p_pucBuffer)
* {
* 	double RH, l_dTempRH;
* 	double T, l_dTempT;
* 	
* #ifdef DEW_POINT	
* 	double A, B;
* 	double DW;
* 	double Tn = Tn_sup;
* 	double m = m_sup;
* #endif
* 		
* 	l_dTempT = (((uint16_t)*(p_pucBuffer)) << 8) |  *(p_pucBuffer + 1);
* 	l_dTempRH = (((uint16_t)*(p_pucBuffer + 2)) << 8) |  *(p_pucBuffer + 3);
* 	
* 	RH = ((C3 * l_dTempRH) + C2 ) * l_dTempRH + C1;
* 	T = D1 + (D2 * l_dTempT);
* 	
* 	mydata.temperature = (int16_t)(T * 100);
* 	mydata.humidity = (uint16_t)(RH * 100);
* 
* #ifdef DEW_POINT	
* 	if (T < 0){
* 		Tn = Tn_inf;
* 		m = m_inf;
* 	}
* 	A = log(RH);
* 	B = (m * T)/ (Tn + T);
* 	DW = Tn * (A + B) / (m - A - B);
* 	
* 	mydata.dewpoint = (int16_t)(DW * 100);
* #endif
* }
*--------------------------------------------------*/

#define TIMEOUT			500
int Execute(uint8_t p_ucCommand, uint8_t * p_pucBuffer)
{
	tick_t now = tick_1khz();
	send_start();
	
	WriteByte(p_ucCommand);
	
	while( SHT1x_READ_DATA() && !expire_timer( now, TIMEOUT));
	if (expire_timer( now, TIMEOUT)){
		return -1;
	}
	
	*p_pucBuffer = ReadByte();

	send_ACK();	
	 
	*(p_pucBuffer + 1) = ReadByte();

	send_NACK();
	
	sht1x_release();

	return 0;
}


void SHT1x_Init(void)
{
	memset(&mydata, 0, sizeof(struct sht1x_s));
	mydata.last_cmd = SHT1x_CMD_MEASURE_HUMIDITY;
	WriteStatus(SHT1x_MODE);
}

void SHT1x_sleep(void)
{
	WriteByte(SHT1x_CMD_SOFT_RESET);
	mdelay(SHT1x_START_TIME_MS);
	SHT1x_SWITCH_OFF();

}

void SHT1x_acquire_data(void)
{
#ifdef DEW_POINT
	uint8_t l_pucBuffer[6];
#else
	uint8_t l_pucBuffer[4];
#endif

	send_start();

	WriteStatus(SHT1x_MODE);

	Execute(SHT1x_CMD_MEASURE_TEMPERATURE, l_pucBuffer);
	
	Execute(SHT1x_CMD_MEASURE_HUMIDITY, l_pucBuffer + 2);
	
	DEBUGF("Read values are temp 0x%02x%02x, rh 0x%02x%02x.\n"
			, *l_pucBuffer
			, *(l_pucBuffer + 1)
			, *(l_pucBuffer + 2)
			, *(l_pucBuffer + 3));

	mydata.lock = TRUE;	
	//CalculateRealValues(l_pucBuffer);	
	mydata.lock = FALSE;
}

void SHT1x_Config(void){

	/* Set VCC pin HIGH	*/
	GPIO_SetBits(SHT1x_PORT, SHT1x_VCC_PIN);

	/* Set GND pin LOW */
	SHT1x_SWITCH_ON();

	sht1x_release();

	mdelay(SHT1x_START_TIME_MS);
}

int16_t SHT1x_get_data(enum sht1x_data_n data){
	int16_t * tmp;
	while (mydata.lock);
	tmp = (int16_t*)&(mydata.temperature);
	return (int16_t)*(tmp + data);
}


/*--------------------------------------------------
* void SHT1x_StartMeasure(void)
* {
* 	BUZZER_PIN_TOGGLE;
* 	SendStart();
* 	WriteByte(s_ucLastCmd);
* 	
* 	// Configure interrupt here!!!
* 	
* 	SHT1x_INTERRUPT_FALLING_EDGE;
* 	P2IFG &= ~SHT1x_INT_PIN;
* 	SHT1x_INTERRUPT_ENABLE;
* }
* 
* void GetSHT1xResult()
* {
* 	SHT1x_INTERRUPT_DISABLE;
* 		
* 	*s_pucPtrBuff = ReadByte();
* 	
* 	// Ack the first byte
* 	SHT1x_DATA_AS_OUTPUT;		//	P4DIR |= SHT1x_DATA;
* 	SHT1x_DATA_LOW_LEVEL;		//	P4OUT &= ~SHT1x_DATA;
* 	SHT1x_CLK_RISING_EDGE;		//	P4OUT |= SHT1x_CLK;
* 	SHT1x_CLK_FALLING_EDGE;		//	P4OUT &= ~SHT1x_CLK;
* 	SHT1x_DATA_HIGH_LEVEL;		//P4OUT |= SHT1x_DATA;
* 	SHT1x_DATA_AS_INPUT;		//	P4DIR &= ~SHT1x_DATA;
* 	 
* 	*(s_pucPtrBuff + 1) = ReadByte();
* 	
* 	// NACK the second byte to avoid CRC transmission
* 	SHT1x_DATA_AS_OUTPUT;		//P4DIR |= SHT1x_DATA;
* 	SHT1x_DATA_HIGH_LEVEL;		//P4OUT |= SHT1x_DATA;
* 	SHT1x_CLK_RISING_EDGE;		//P4OUT |= SHT1x_CLK;
* 	SHT1x_CLK_FALLING_EDGE;		//P4OUT &= ~SHT1x_CLK;
* 	
* 	P2IFG &= ~SHT1x_INT_PIN;
* 	
* 	if(s_ucLastCmd == SENSIRION_CMD_MEASURE_TEMPERATURE)
* 	{
* 		s_ucLastCmd = SENSIRION_CMD_MEASURE_HUMIDITY;
* 		s_pucPtrBuff += 2;
* 		SHT1x_StartMeasure();
* 	}
* 	else
* 	{
* 		s_ucLastCmd = SENSIRION_CMD_MEASURE_TEMPERATURE;
* 		s_pucPtrBuff = s_pucBuffer;
* 		CalculateRealValues(s_pucBuffer);
* 	}
* }
*--------------------------------------------------*/
/*--------------------------------------------------
* void SHT1x_AcquireValues(uint8_t* p_pucBuffer)
* {
* 	CopyIntToBigEndianBuffer(s_iLastTemp, sizeof(int), p_pucBuffer);
* 	CopyIntToBigEndianBuffer(s_uiLastRH, sizeof(int), p_pucBuffer + 2);
* 	
* #ifdef DEW_POINT
* 	CopyIntToBigEndianBuffer(s_uiLastPR, sizeof(int), p_pucBuffer + 4);
* #endif
* }
*--------------------------------------------------*/



