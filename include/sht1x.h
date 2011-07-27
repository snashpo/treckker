#ifndef SHT1x_H_
#define SHT1x_H_

///////////////////////////////////////////////////////////////////////////////
// sht11.h : header file
// 
//
// CyrLink declarations for interfacing Sensirion SHT1x device
///////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2006  CyrLink S.A.S. http://www.cyrlink.com
// All rights reserved - not to be sold.
///////////////////////////////////////////////////////////////////////////////

//#define DEW_POINT								1																

/* Port definition	*/
#define SHT11_PORT_DIR								P4DIR
#define SHT11_PORT_OUT								P4OUT
#define SHT11_PORT_IN								P4IN
#define SHT11_PORT_IE								P2IE
#define SHT11_PORT_IES								P2IES

/*	Pin definition	*/
#define SHT11_DATA_PIN								BIT7
#define SHT11_CLK_PIN								BIT6
#define SHT11_INT_PIN								BIT2


/* Macros definition	*/

#define SHT11_PINS_OUT								SHT11_PORT_DIR |= (SHT11_DATA_PIN | SHT11_CLK_PIN)

#define SHT11_CLK_FALLING_EDGE						SHT11_PORT_OUT &= ~SHT11_CLK_PIN
#define SHT11_CLK_RISING_EDGE						SHT11_PORT_OUT |= SHT11_CLK_PIN

#define SHT11_DATA_AS_INTPUT						SHT11_PORT_DIR &= ~SHT11_DATA_PIN
#define SHT11_DATA_AS_OUTPUT						SHT11_PORT_DIR |= SHT11_DATA_PIN

#define SHT11_DATA_LOW_LEVEL						SHT11_PORT_OUT &= ~SHT11_DATA_PIN 
#define SHT11_DATA_HIGH_LEVEL						SHT11_PORT_OUT |= SHT11_DATA_PIN

#define SHT11_DATA_UP								SHT11_PORT_IN & SHT11_DATA_PIN	
#define SHT11_DATA_DOWN								SHT11_PORT_IN & ~SHT11_DATA_PIN

#define	SHT11_DATA_WAIT_FALLING						while(SHT11_DATA_UP)

#define SHT11_INTERRUPT_ENABLE						SHT11_PORT_IE |= SHT11_INT_PIN
#define SHT11_INTERRUPT_DISABLE						SHT11_PORT_IE &= ~SHT11_INT_PIN
#define SHT11_INTERRUPT_FALLING_EDGE				SHT11_PORT_IES |= SHT11_INT_PIN


/*	Component definitions	*/
#define SHT11_NORMAL_MODE							0x00
#define SHT11_ECONOMY_MODE							0x01

#define HUMIDITY_NORMAL_MODE_MASK					0x0F
#define TEMPERATURE_NORMAL_MODE_MASK				0x3F
#define TEMPERATURE_ECO_MODE_MASK					0x0F 

#define SENSIRION_CMD_MEASURE_TEMPERATURE			0x03
#define SENSIRION_CMD_MEASURE_HUMIDITY				0x05
#define SENSIRION_CMD_MEASURE_READ_STATUS			0x07
#define SENSIRION_CMD_MEASURE_WRITE_STATUS			0x06
#define SENSIRION_CMD_MEASURE_SOFT_RESET			0x1D


/*	Function declartions	*/
/**	Public	**/
extern void SHT11_GetData(unsigned char p_bMode);
extern void SHT11_AcquireValues (unsigned char* p_pucBuffer);
extern void InitSHT11(void);
extern void SHT11_StartMeasure();
extern void GetSHT11Result(void);

/**	Private	**/
void SendStart(void);
void Execute(unsigned char p_ucCommand, unsigned char p_ucMode, unsigned char * p_pucBuffer);
void WriteByte(unsigned char p_ucValue);
void WriteStatus(unsigned char p_ucParams);
unsigned char ReadByte(void);
void CalculateRealValues(unsigned char * p_pucBuffer);




#endif /*SHT11_H_*/
