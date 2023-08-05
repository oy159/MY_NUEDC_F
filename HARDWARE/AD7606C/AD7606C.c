#include "AD7606C.h"

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
