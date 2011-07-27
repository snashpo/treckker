#include <math.h>
#include "sht1x.h"



static int s_iLastTemp;
static unsigned int s_uiLastRH;
static unsigned char s_ucSHT11Lock = FALSE;
static unsigned char s_ucLastCmd = SENSIRION_CMD_MEASURE_TEMPERATURE;

static 	unsigned char s_pucBuffer[4];
static unsigned char * s_pucPtrBuff = s_pucBuffer;


static const double C1 = -4;
static const double C2 = 0.0405;
static const double C3 = -2.8E-6;

static const double D1 = -39.66;
static const double D2 = 0.01;

#ifdef DEW_POINT
	static const double B1 = 0.4343;
	static const double B2 = 17.62;
	static const double B3 = 243.12;
	static int s_iLastPR;
#endif

void SendStart(void)
{
	SHT11_DATA_AS_OUTPUT;	
	SHT11_DATA_HIGH_LEVEL; 		//	P4OUT |= SHT11_DATA;
	SHT11_CLK_RISING_EDGE;		//	P4OUT |= SHT11_CLK;
	SHT11_DATA_LOW_LEVEL;		//	P4OUT &= ~SHT11_DATA;
	SHT11_CLK_FALLING_EDGE;		//	P4OUT &= ~SHT11_CLK;
	SHT11_CLK_RISING_EDGE;		//	P4OUT |= SHT11_CLK;
	SHT11_DATA_HIGH_LEVEL;		//	P4OUT |= SHT11_DATA;
	SHT11_CLK_FALLING_EDGE;		//	P4OUT &= ~SHT11_CLK;
	
}

void Execute(unsigned char p_ucCommand, unsigned char p_uiMode, unsigned char * p_pucBuffer)
{
	SendStart();
	
	WriteByte(p_ucCommand);
	
	WDTCTL = WDT_ADLY_250;
	g_Wdt_Max_Loop = TIMEOUT_DELAY;
	IE1 |= WDTIE;
	
	while( (SHT11_DATA_UP) && g_Wdt_Max_Loop);
	
	*p_pucBuffer = ReadByte();
	
	// Ack the first byte
	SHT11_DATA_AS_OUTPUT;		//	P4DIR |= SHT11_DATA;
	SHT11_DATA_LOW_LEVEL;		//	P4OUT &= ~SHT11_DATA;
	SHT11_CLK_RISING_EDGE;		//	P4OUT |= SHT11_CLK;
	SHT11_CLK_FALLING_EDGE;		//	P4OUT &= ~SHT11_CLK;
	SHT11_DATA_HIGH_LEVEL;		//P4OUT |= SHT11_DATA;
	SHT11_DATA_AS_INTPUT;		//	P4DIR &= ~SHT11_DATA;
	 
	*(p_pucBuffer + 1) = ReadByte();
	
	// NACK the second byte to avoid CRC transmission
	SHT11_DATA_AS_OUTPUT;		//P4DIR |= SHT11_DATA;
	SHT11_DATA_HIGH_LEVEL;		//P4OUT |= SHT11_DATA;
	SHT11_CLK_RISING_EDGE;		//P4OUT |= SHT11_CLK;
	SHT11_CLK_FALLING_EDGE;		//P4OUT &= ~SHT11_CLK;
	
	IE1 &= ~WDTIE;
}


void SHT11_StartMeasure(void)
{
	BUZZER_PIN_TOGGLE;
	SendStart();
	WriteByte(s_ucLastCmd);
	
	// Configure interrupt here!!!
	
	SHT11_INTERRUPT_FALLING_EDGE;
	P2IFG &= ~SHT11_INT_PIN;
	SHT11_INTERRUPT_ENABLE;
}

void GetSHT11Result()
{
	SHT11_INTERRUPT_DISABLE;
		
	*s_pucPtrBuff = ReadByte();
	
	// Ack the first byte
	SHT11_DATA_AS_OUTPUT;		//	P4DIR |= SHT11_DATA;
	SHT11_DATA_LOW_LEVEL;		//	P4OUT &= ~SHT11_DATA;
	SHT11_CLK_RISING_EDGE;		//	P4OUT |= SHT11_CLK;
	SHT11_CLK_FALLING_EDGE;		//	P4OUT &= ~SHT11_CLK;
	SHT11_DATA_HIGH_LEVEL;		//P4OUT |= SHT11_DATA;
	SHT11_DATA_AS_INTPUT;		//	P4DIR &= ~SHT11_DATA;
	 
	*(s_pucPtrBuff + 1) = ReadByte();
	
	// NACK the second byte to avoid CRC transmission
	SHT11_DATA_AS_OUTPUT;		//P4DIR |= SHT11_DATA;
	SHT11_DATA_HIGH_LEVEL;		//P4OUT |= SHT11_DATA;
	SHT11_CLK_RISING_EDGE;		//P4OUT |= SHT11_CLK;
	SHT11_CLK_FALLING_EDGE;		//P4OUT &= ~SHT11_CLK;
	
	P2IFG &= ~SHT11_INT_PIN;
	
	if(s_ucLastCmd == SENSIRION_CMD_MEASURE_TEMPERATURE)
	{
		s_ucLastCmd = SENSIRION_CMD_MEASURE_HUMIDITY;
		s_pucPtrBuff += 2;
		SHT11_StartMeasure();
	}
	else
	{
		s_ucLastCmd = SENSIRION_CMD_MEASURE_TEMPERATURE;
		s_pucPtrBuff = s_pucBuffer;
		CalculateRealValues(s_pucBuffer);
	}
}


unsigned char ReadByte(void)
{
	volatile unsigned int l_uiLoop = 8;
	unsigned char myByte = 0;
	
	SHT11_DATA_AS_INTPUT;		//	P4DIR &= ~SHT11_DATA;
	
	while(l_uiLoop)
	{
		l_uiLoop--;	
		SHT11_CLK_RISING_EDGE;		//P4OUT |= SHT11_CLK;
		if (SHT11_DATA_UP)
			 myByte |= (1 << l_uiLoop);
		SHT11_CLK_FALLING_EDGE;		//P4OUT &= ~SHT11_CLK;
	}
	return myByte;	
}

void WriteByte(unsigned char p_ucValue)
{
	volatile unsigned int l_uiLoop = 8;
	
	SHT11_DATA_AS_OUTPUT;
	
	while(l_uiLoop)
	{
		l_uiLoop--;	
		if ( p_ucValue & ( BIT0 << l_uiLoop ) )
			SHT11_DATA_HIGH_LEVEL;	//P4OUT |= SHT11_DATA;
		else 
			SHT11_DATA_LOW_LEVEL;	//P4OUT &= ~SHT11_DATA;
			
		SHT11_CLK_RISING_EDGE;		//P4OUT |= SHT11_CLK;

		SHT11_CLK_FALLING_EDGE;		//P4OUT &= ~SHT11_CLK;
	}
	SHT11_DATA_AS_INTPUT;
	SHT11_CLK_RISING_EDGE;		//P4OUT |= SHT11_CLK;		
	SHT11_CLK_FALLING_EDGE;		//P4OUT &= ~SHT11_CLK;	
}

void WriteStatus(unsigned char p_ucParams)
{
	SendStart();
	WriteByte(SENSIRION_CMD_MEASURE_WRITE_STATUS);
	SHT11_DATA_AS_OUTPUT;		//P4DIR |= SHT11_DATA;
	WriteByte(p_ucParams);
}


void SHT11_GetData(unsigned char p_ucMode)
{
#ifdef DEW_POINT
	unsigned char l_pucBuffer[6];

#else
	unsigned char l_pucBuffer[4];
#endif
	
	Execute(SENSIRION_CMD_MEASURE_TEMPERATURE, p_ucMode, l_pucBuffer);
	
	Execute(SENSIRION_CMD_MEASURE_HUMIDITY, p_ucMode, l_pucBuffer + 2);
	
	s_ucSHT11Lock = TRUE;	
	CalculateRealValues(l_pucBuffer);	
	
	s_ucSHT11Lock = FALSE;
}

void SHT11_AcquireValues(unsigned char* p_pucBuffer)
{
	CopyIntToBigEndianBuffer(s_iLastTemp, sizeof(int), p_pucBuffer);
	CopyIntToBigEndianBuffer(s_uiLastRH, sizeof(int), p_pucBuffer + 2);
	
#ifdef DEW_POINT
	CopyIntToBigEndianBuffer(s_uiLastPR, sizeof(int), p_pucBuffer + 4);
#endif
}



void InitSHT11(void)
{
	SHT11_PINS_OUT; 
	SHT11_CLK_FALLING_EDGE;
	SHT11_CLK_RISING_EDGE;
	SHT11_CLK_FALLING_EDGE;
	s_uiLastRH = 0;
	s_iLastTemp = 0;
	
#ifdef DEW_POINT
	s_uiLastPR = 0;
#endif
	
	WriteStatus(SHT11_NORMAL_MODE);
	
	
	P2DIR &= ~SHT11_INT_PIN;
}



void CalculateRealValues(unsigned char * p_pucBuffer)
{
	double RH, l_dTempRH;
	double T, l_dTempT;
	
#ifdef DEW_POINT	
	double H;
	double PR;
#endif
		
	l_dTempT = GetIntFromBigEndianBuffer(p_pucBuffer, sizeof(int));
	l_dTempRH = GetIntFromBigEndianBuffer(p_pucBuffer + 2, sizeof(int));
	
	RH = ((C3 * l_dTempRH) + C2 ) * l_dTempRH + C1;
	T = D1 + (D2 * l_dTempT);
	
#ifdef DEW_POINT	
	H = ((log10(RH) - 2) / B1) + ((B2 * T) / ( B3 + T));
	PR = (B3 * H) / (B2 - H);
	
	s_iLastPR = (int)(PR * 100);
#endif

	s_iLastTemp = (int)(T * 100);
	s_uiLastRH = (unsigned int)(RH * 100);
	
}
