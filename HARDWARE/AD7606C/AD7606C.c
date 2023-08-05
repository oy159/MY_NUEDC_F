#include "AD7606C.h"

uint16_t AD7606C_Init(uint16_t* P, uint16_t Range, uint16_t Bandwidth, uint16_t OverSamp)
{
	uint16_t ID = 0;
	//���ò�����Χ
	*P = RANGE1_ADDRESS | Range;
	HAL_Delay(1);
	*P = RANGE2_ADDRESS | Range;
		HAL_Delay(1);
	*P = RANGE3_ADDRESS | Range;
		HAL_Delay(1);
	*P = RANGE4_ADDRESS | Range;
		HAL_Delay(1);
	//���ô���
	*P = BANDWIDTH_ADDRESS | Bandwidth;
		HAL_Delay(1);
	//���ù�����
	*P = OVERSAMP_ADDRESS | OverSamp;
		HAL_Delay(1);
	//��ȡID
	*P = 0x8000 | NAME_ID_ADDRESS;
	HAL_Delay(1);
	ID = *P;
		HAL_Delay(1);
	//�ָ�ADCģʽ
	*P = 0x0000;
	
	return ID;
}
