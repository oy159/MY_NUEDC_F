#ifndef __APPUSER_H
#define __APPUSER_H

#include "main.h"

typedef enum
{
	APP_REQUEST1 = 0, //	DDS
	APP_REQUEST2 = 1, //	Negtive
	APP_REQUEST3 = 2, //	Postive1
	APP_REQUEST4 = 3  //	Postive2
} APP_STATUS;

typedef struct
{
	uint32_t ListStartAddress;
	uint32_t ListOffsetAddress;

	TIM_HandleTypeDef *_htim; // ADC SAMPLING_RATE

} OY_APP_Configure;

typedef enum
{
	WAIT_STATE = 0,
	STORAGE_STATE = 1,
	SKIP_STATE = 2,
	RESET_STATE = 3
} ListKeyState;

typedef struct
{
	uint32_t ListStartAddress;
	uint32_t ListOffsetAddress;
	uint32_t ListCurrentAddress;
	uint16_t DotsNum;

	void (*ListForPos_GetData)(void);
	void (*ListForPos_KeyDetect)(void);
//	void (*WavePrint)(float (*wave)[39], int len);
	void (*PositionFind)(void);
} OY_REQUEST2_Handle;

typedef struct
{
	uint32_t ListStartAddress;
	uint32_t ListOffsetAddress;
	uint32_t ListCurrentAddress;
	uint16_t DotsNum;

	void (*ListForPos_GetData)(void);
	void (*ListForPos_KeyDetect)(void);
//	void (*PositionFind)(void);
} OY_REQUEST3_Handle;


typedef struct
{
	uint32_t ListStartAddress;
	uint32_t ListOffsetAddress;
	uint32_t ListCurrentAddress;
	uint16_t DotsNum;

	void (*ListForPos_GetData)(uint16_t AVENUM);
	void (*ListForPos_KeyDetect)(void);
	void (*WavePrint)(float (*wave)[39], int len);
	uint16_t (*PositionFind)(float *min);
} OY_REQUEST4_Handle;






typedef struct
{
	APP_STATUS _status;
	// OY_APP_Configure	_config;

	void (*OYAPP_Init)(APP_STATUS _status, OY_APP_Configure *_config);

} OY_APP_HandleTypeDef;

extern float NormPos1Data[144];
extern float AD_AVE[4][39];
extern float xcorr_out[4][511];
extern uint8_t	Knock_flag;
extern uint16_t	start,stop;

extern OY_APP_Configure oyconfig;
extern OY_REQUEST4_Handle ListForRequest4;
extern OY_REQUEST2_Handle ListForRequest2;

// extern OY_APP_HandleTypeDef	oyapp;
void VoltJudge(void);
void Request2DataGet(void);
void KeyDetectForRequest2(void);
void Request2PositionFind(void);


void OYAPP_Init(APP_STATUS _status, OY_APP_Configure *_config);
void ListForPos1_GetData(uint16_t AVENUM);
void WavePrintFORPOS1(float (*wave)[39], int len);
void ListKeyDetect_POS1(void);
uint16_t PositionFind(float *min);

void NormForPos1(float *input1, float multnum);

#endif
