#ifndef	__APPUSER_H
#define __APPUSER_H

#include	"main.h"

typedef enum
{
		APP_REQUEST1 = 0,					//	DDS
		APP_REQUEST2 = 1,					//	Negtive
		APP_REQUEST3 = 2,					//	Postive1
		APP_REQUEST4 = 3					//	Postive2
}APP_STATUS;


typedef	struct
{
		uint32_t	ListStartAddress;
		uint32_t	ListOffsetAddress;
		
		TIM_HandleTypeDef	*_htim;			// ADC SAMPLING_RATE
		
}OY_APP_Configure;


typedef	enum {
	WAIT_STATE = 0,
	STORAGE_STATE = 1,
	SKIP_STATE = 2,
	RESET_STATE = 3
}ListKeyState;


typedef struct
{
		uint32_t	ListStartAddress;
		uint32_t	ListOffsetAddress;
		uint32_t	ListCurrentAddress;
		uint16_t	DotsNum;
	
		void	(* ListForPos_GetData)(uint16_t	AVENUM);
		void	(* ListForPos_KeyDetect)(void);
		void	(* WavePrint)(float (*wave)[39], int len);
}OY_List_Process;



typedef struct
{
		APP_STATUS	_status;
		//OY_APP_Configure	_config;
	
		void	(* OYAPP_Init)(APP_STATUS	_status, OY_APP_Configure	*_config);
	
}OY_APP_HandleTypeDef;


extern float NormPos1Data[144];
extern float AD_AVE[4][39];

extern OY_APP_Configure	oyconfig;
extern OY_List_Process ListCH1ForPos;
extern OY_List_Process ListCH2ForPos;

//extern OY_APP_HandleTypeDef	oyapp;


void	OYAPP_Init(APP_STATUS	_status, OY_APP_Configure	*_config);
void	ListForPos1_GetData(uint16_t	AVENUM);
void	WavePrintFORPOS1(float (*wave)[39], int len);
void	ListKeyDetect_POS1(void);

void	NormForPos1(float *input1, float multnum);

#endif
