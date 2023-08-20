#ifndef	__APPUSER_H
#define __APPUSER_H

#include	"main.h"

typedef enum
{
		APP_REQUEST1,					//	DDS
		APP_REQUEST2,					//	Negtive
		APP_REQUEST3,					//	Postive1
		APP_REQUEST4					//	Postive2
}APP_STATUS;


typedef	struct
{
		uint32_t	ListStartAddress;
		uint32_t	ListOffsetAddress;
		
		TIM_HandleTypeDef	*_htim;			// ADC SAMPLING_RATE
		
}OY_APP_Configure;


typedef struct
{
		uint32_t	ListStartAddress;
		uint32_t	ListOffsetAddress;
}OY_List_Process;



typedef struct
{
		APP_STATUS	_status;
		//OY_APP_Configure	_config;
	
		void	(* OYAPP_Init)(APP_STATUS	_status, OY_APP_Configure	*_config);
		
}OY_APP_HandleTypeDef;

extern OY_APP_Configure	oyconfig;
extern OY_List_Process ListCH1ForPos;
extern OY_List_Process ListCH2ForPos;

//extern OY_APP_HandleTypeDef	oyapp;


void	OYAPP_Init(APP_STATUS	_status, OY_APP_Configure	*_config);


#endif
