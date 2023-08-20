#include "Appuser.h"
#include "tim.h"

OY_APP_Configure	oyconfig = 
{
		1000000,
		100,
		&htim4
};


OY_List_Process ListCH1ForPos = 
{
		2*1024*1024 + 1000,
		4*39
};


OY_List_Process ListCH2ForPos = 
{
		2*1024*1024 + 1000,
		4*39
};

//OY_APP_HandleTypeDef	oyapp = 
//{
//		APP_REQUEST1,
//		OYAPP_Init
//};



void	OYAPP_Init(APP_STATUS	_status, OY_APP_Configure	*_config)
{
		TIM_HandleTypeDef	*TIM;
		TIM = _config->_htim;
		
		switch(_status)
		{
			case APP_REQUEST1:
				break;
			case APP_REQUEST2:
				HAL_TIM_Base_Stop(TIM);
				HAL_TIM_PWM_Stop(TIM,TIM_CHANNEL_1);
				TIM->Instance->PSC = 3;
				break;
			case APP_REQUEST3:
				HAL_TIM_Base_Stop(TIM);
				HAL_TIM_PWM_Stop(TIM,TIM_CHANNEL_1);
				TIM->Instance->PSC = 1;
				break;
			case APP_REQUEST4:
				HAL_TIM_Base_Stop(TIM);
				HAL_TIM_PWM_Stop(TIM,TIM_CHANNEL_1);
				TIM->Instance->PSC = 1;
				break;
		}
}

