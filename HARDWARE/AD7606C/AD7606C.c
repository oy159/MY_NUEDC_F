#include "AD7606C.h"
#include "tim.h"

uint8_t	sampflag;
uint16_t *p=(uint16_t *)AD7606C_ADDR;
uint8_t AD7606C_flag = 0;     //  AD采集完成标志


uint16_t AD7606C_Init(uint16_t* P, uint16_t Range, uint16_t Bandwidth, uint16_t OverSamp)
{
	uint16_t ID = 0;
	//设置采样范围
	*P = RANGE1_ADDRESS | Range;
	HAL_Delay(1);
	*P = RANGE2_ADDRESS | Range;
		HAL_Delay(1);
	*P = RANGE3_ADDRESS | Range;
		HAL_Delay(1);
	*P = RANGE4_ADDRESS | Range;
		HAL_Delay(1);
	//设置带宽
	*P = BANDWIDTH_ADDRESS | Bandwidth;
		HAL_Delay(1);
	//设置过采样
	*P = OVERSAMP_ADDRESS | OverSamp;
		HAL_Delay(1);
	//读取ID
	*P = 0x8000 | NAME_ID_ADDRESS;
	HAL_Delay(1);
	ID = *P;
		HAL_Delay(1);
	//恢复ADC模式
	*P = 0x0000;
	
	return ID;
}



void	AD7606C_BitToFloat(uint16_t *p1, float **p2, uint16_t len)
{
		for(int i = 0; i<4; i++)
		{
				for(int j = 0; j < len; j++)
						p2[i][j] = (float)((int)(p1[i])) / 32767.f * 10.f;
		}
}



void	AD7606C_SAMP_Start(uint16_t *data, uint16_t len)
{
	uint16_t cnt;
	sampflag = 0;
  HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_1);
  HAL_TIM_Base_Start(&htim4);
  while(1)
  {
    while(sampflag != 1){};
    if(sampflag == 1)
    {
      if(cnt != len)
      {
        data[8*cnt] 	= *p;
        data[1+8*cnt] = *p;
        data[2+8*cnt] = *p;
        data[3+8*cnt] = *p;
        data[4+8*cnt] = *p;
        data[5+8*cnt] = *p;
        data[6+8*cnt] = *p;
        data[7+8*cnt] = *p;
        cnt++;
      }else{
        cnt = 0;
        AD7606C_flag = 1;
        HAL_TIM_Base_Stop(&htim4);
        HAL_TIM_PWM_Stop(&htim4,TIM_CHANNEL_1);
        break;
      }
      sampflag = 0;
    }
  }

}

void EXTI9_5_IRQHandler(void)
{
	sampflag = 1;
  __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_6);;
}

