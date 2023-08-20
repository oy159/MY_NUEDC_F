#ifndef __AD7606C_H
#define __AD7606C_H

#include "main.h"

//  以下定义寄存器地址
#define STATUS_ADDRESS		0x0100
#define RANGE1_ADDRESS		0x0300
#define RANGE2_ADDRESS		0x0400
#define RANGE3_ADDRESS		0x0500
#define RANGE4_ADDRESS		0x0600
#define BANDWIDTH_ADDRESS	0x0700
#define OVERSAMP_ADDRESS	0x0800
#define NAME_ID_ADDRESS		0x2F00

//  以下定义命令
#define SINGLE_2V5			0X0000
#define SINGLE_5V			0X0011
#define SINGLE_6V25			0X0022
#define SINGLE_10V			0X0033
#define SINGLE_12V5			0X0044
#define DIFF_5V				0X0088
#define DIFF_10V			0X0099
#define DIFF_12V5			0X00AA
#define DIFF_20V			0X00BB

#define HighBand			0x00FF

//  过采样设置
#define OverSamp_off		0x0000
#define OverSamp2			0x0001
#define OverSamp4			0x0002
#define OverSamp8			0x0003
#define OverSamp16			0x0004
#define OverSamp32			0x0005
#define OverSamp64			0x0006
#define OverSamp128			0x0007
#define OverSamp256			0x0008


uint16_t AD7606C_Init(uint16_t* p, uint16_t Range, uint16_t Bandwidth, uint16_t OverSamp);
void	AD7606C_BitToFloat(uint16_t *p1, float **p2, uint16_t len);
void	AD7606C_SAMP_Start(uint16_t *data, uint16_t len);



//  以下为用户自行配置

#define AD7606C_SAMP_SIZE 1024
#define AD7606C_ADDR ((uint32_t)0x6C000000)


extern	uint8_t	sampflag;
extern	uint16_t *p;
extern 	uint8_t AD7606C_flag;     //  AD采集完成标志

/*		ADC DATA		*/
/* ************************************ */

extern 	uint16_t	data[8*AD7606C_SAMP_SIZE];
extern 	float	volt[4][AD7606C_SAMP_SIZE];

extern 	uint16_t	DataNeg[32*AD7606C_SAMP_SIZE];
extern 	float VoltNeg[4][4*AD7606C_SAMP_SIZE];

/* ************************************ */
/* ************************************ */



#endif /* __AD7606C_H */
