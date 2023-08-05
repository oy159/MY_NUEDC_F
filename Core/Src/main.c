/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "quadspi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "fmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include  "User_function.h"
#include	"AD7606C.h"
#include	"ad9959.h"
#include  "stdio.h"
#include 	"fft_user.h"
#include	"correlation.h"
#include	"diagram1.h"
#include "W9825G6KH.h"
#include "W25Q256.h"



//#include	"arm_struct.h"
//#include	"arm_math.h"



uint8_t		uartuse = 1;


int fputc(int ch, FILE *f)
{
	if(uartuse)
		HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xffff);
	else
		HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 0xffff);
  return ch;
}
int fgetc(FILE *f)
{
  uint8_t ch = 0;
	if(uartuse)
		HAL_UART_Receive(&huart1, &ch, 1, 0xffff);
	else
		HAL_UART_Receive(&huart2, &ch, 1, 0xffff);
  return ch;
}

void HMISends(uint8_t *buf1)		  
{
	uint8_t i=0;
	while(1)
	{
	 if(buf1[i]!=0)
	 	{
			HAL_UART_Transmit(&huart2,&buf1[i],1,1000); 
			while((__HAL_UART_GET_FLAG(&huart2,UART_FLAG_TXE)==RESET)){};
		 	i++;
		}
	 else 
	 return ;

		}
	}
void HMISend(uint8_t k)           
{   
    uint8_t i;
    for(i=0;i<3;i++)
    {
        if(k!=0)
        {  
       HAL_UART_Transmit(&huart2,&k,1,1000);
       while((__HAL_UART_GET_FLAG(&huart2,UART_FLAG_TXE)==RESET)){};                
        }
		else
	  return ;				
    }
}


/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define	THREVOLT	0x200
#define	FREQSTEP	500U
#define FREQTERM  20000U
#define AVENUM  10
#define	XCORRSIZE	4*511
#define	POSOFFSET	4*39


#define	Pos_Start_ADDRESS	1024*1024 + 2000

typedef	enum
{
		DDS_STATE = 0U,
		NEG_MEASURE_STATE = 1U,
		POS_MEASURE_STATE = 2U,
		Calibration_STATE = 3U,
		POS_Storage_STATE = 4U,
		POS_MODE4_STATE = 5U
}System_State;



extern	DMA_HandleTypeDef hdma_tim4_up;;


uint16_t *p=(uint16_t *)AD7606C_ADDR;

struct W25Q256_Handler *qspi_handler;

uint32_t*	pp= (uint32_t *)0xc0000000;
uint8_t buf1[100];
uint8_t SAMP_DIV = 1;
uint8_t re;
uint32_t	start_address = 1000;
uint32_t	find_address = 1000;
uint8_t	AVE_NUM;

uint8_t	sapa = 4;

uint8_t calibration_num = 0;
uint16_t	Pos_Storage_Num = 0;

uint8_t Knock_flag = 0;
uint8_t dds_stopflag = 0;
uint8_t AD7606C_flag = 0;     //  AD采集完成标志
uint8_t sampflag = 0;         //  单次采集过程标志
uint8_t pr_flag[4] = {0};     //  信号检测
uint16_t  loc[4];
uint16_t	chf;

uint16_t AD7606C_ID=0;
uint16_t	data[8*AD7606C_SAMP_SIZE];
uint16_t	data_mode2[32*AD7606C_SAMP_SIZE];
float volt_mode2[4][4*AD7606C_SAMP_SIZE];

uint16_t	start;
uint16_t	stop;
int cnt = 0;
int	cnt_ave = 0;
float percent[3] = {0,0,0};
float volt[4][AD7606C_SAMP_SIZE];
float AD_Vpp[4][100];

float	AD_Vpp_All[300];
float	Neg_res[36];


float AD_Max[4][100];
float AD_Min[4][100];

float	AD_AVE[4][39];

float	AD_AVEM4[4][39] ;
float AD_AVEM4_Load[4][100];
float	Mode4_Res[625];

float Norm_Mode2[144];

float xcorr_out[4][AD7606C_SAMP_SIZE*2 - 1];
float xcorr_out_test[4][AD7606C_SAMP_SIZE*2 - 1];

uint16_t  row[100];
uint16_t  freq = 1000;

uint16_t row_neg;float min_neg;

float	norm_DE = 0;
float	norm_EF = 0;


const	float	BoardNullV[3][39] ={{0.0425724000000000,0.243656200000000,0.210330800000000,0.333042700000000,0.446967700000000,0.385197800000000,0.576546500000000,0.682443100000000,0.714731900000000,0.883895100000000,0.851819000000000,1.02763380000000,1.05357410000000,1.18733560000000,1.42375890000000,1.18287840000000,1.17268510000000,0.933883300000000,1.00376900000000,0.840344200000000,0.683755500000000,0.581124100000000,0.714426200000000,0.402685400000000,0.437720800000000,0.310429200000000,0.0874339000000000,0.140658600000000,0.190829800000000,0.204746000000000,0.112246400000000,0.120852400000000,0.277897400000000,0.181888200000000,0.139437800000000,0.192203000000000,0.161319200000000,0.170749100000000,0.060852600000000},\
																{0.0338139000000000,0.161136000000000,0.216220600000000,0.149142500000000,0.164157300000000,0.246708000000000,0.471900800000000,0.312931700000000,0.503059400000000,0.949813600000000,0.229312800000000,0.28363450000000,0.50553140000000,0.19671990000000,0.51346620000000,0.35672470000000,0.14126900000000,0.229679000000000,0.68052060000000,0.553017200000000,0.543190400000000,0.505012600000000,0.389928000000000,0.384740000000000,0.384892500000000,0.604470100000000,0.6716399000000000,0.816296000000000,0.594002700000000,0.525062700000000,0.521400900000000,0.570870200000000,0.609200300000000,0.441138900000000,0.362034800000000,0.170413400000000,0.116732700000000,0.087128600000000,0.184268400000000},\
																{0.0348820000000000,0.226260900000000,0.166141000000000,0.320255900000000,0.438788900000000,0.327976900000000,0.484779500000000,0.640511800000000,0.597328900000000,0.749767400000000,0.766979400000000,0.90397590000000,1.07753070000000,1.09590230000000,1.41356610000000,1.11717310000000,1.04771470000000,0.826824900000000,0.97840870000000,0.946761600000000,0.766277600000000,0.597970000000000,0.782421500000000,0.446479600000000,0.454749800000000,0.323368700000000,0.1260710000000000,0.152713300000000,0.190127900000000,0.248295100000000,0.143618800000000,0.168033200000000,0.279301000000000,0.172061200000000,0.128482000000000,0.200595700000000,0.181796600000000,0.196811400000000,0.063751900000000}};
const	float	DE67[39] = {0.863645, 1.114954, 1.259533, 1.148411, 0.960185, 1.148188, 1.088326, 1.087027, 1.068636, 0.812540, 0.999215, 1.048665, 1.035210, 0.969082, 0.971629, 1.017007, 0.959958, 1.075927, 1.082907, 0.970763, 0.974704, 0.958110, 0.877452, 0.716881, 1.196202, 1.169052, 0.610813, 0.799758, 1.084829, 1.013578, 1.342909, 0.698162, 0.868172, 1.019394, 1.110615, 1.121763, 0.977290, 0.972727, 1.081039};
const	float	EF56[39] = {0.874692, 0.875905, 1.187351, 1.167922, 0.834694, 0.951807, 1.101455, 1.131218, 1.215005, 0.893347, 1.237384, 1.130334, 1.007245, 1.231260, 0.993239, 0.972238, 0.943150, 0.895274, 0.930410, 0.835978, 0.902525, 0.877789, 1.004253, 1.014169, 1.090478, 0.950124, 0.446461, 0.705193, 0.877010, 1.196430, 1.530063, 0.850438, 0.882963, 0.937510, 0.963484, 1.082755, 0.950943, 0.994820, 0.846634};
	
	
System_State oystate;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

	
	
	
	
	
	
	
	
void	AD7606C_MEASURE(void)
{
  sampflag = 0;
  HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_1);
  HAL_TIM_Base_Start(&htim4);
  while(1)
  {
    while(sampflag != 1){};
    if(sampflag == 1)
    {
      if(cnt != AD7606C_SAMP_SIZE/SAMP_DIV)
      {
        data[8*cnt] = *p;
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

void	AD7606C_MEASURE_Mode2(void)
{
  sampflag = 0;
  HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_1);
  HAL_TIM_Base_Start(&htim4);
  while(1)
  {
    while(sampflag != 1){};
    if(sampflag == 1)
    {
      if(cnt != AD7606C_SAMP_SIZE/SAMP_DIV * 4)
      {
        data_mode2[8*cnt] = *p;
        data_mode2[1+8*cnt] = *p;
        data_mode2[2+8*cnt] = *p;
        data_mode2[3+8*cnt] = *p;
        data_mode2[4+8*cnt] = *p;
        data_mode2[5+8*cnt] = *p;
        data_mode2[6+8*cnt] = *p;
        data_mode2[7+8*cnt] = *p;
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


void	ADC_BitToFloat(void)
{
	for(int i = 0;i<4;i++)
	{
		for(int j = 0 ;j<AD7606C_SAMP_SIZE/SAMP_DIV; j++)
		{
			if(data[j*8+i]>=32768)
				volt[i][j] = (float)data[j*8+i]/65535*20-20;
			else
				volt[i][j] = (float)data[j*8+i]/65535*20;	
		}
	}
}

void	ADC_BitToFloat_Mode2(void)
{
	for(int i = 0;i<4;i++)
	{
		for(int j = 0 ;j<4 * AD7606C_SAMP_SIZE/SAMP_DIV; j++)
		{
			if(data_mode2[j*8+i]>=32768)
				volt_mode2[i][j] = (float)data_mode2[j*8+i]/65535*20-20;
			else
				volt_mode2[i][j] = (float)data_mode2[j*8+i]/65535*20;	
		}
	}
}


void  VoltJudge(void)
{

		for(uint16_t i = 0;i < 4; i++){
			loc[i] = 0;pr_flag[i] = 0;
			for (uint16_t j = 0; j < AD7606C_SAMP_SIZE/SAMP_DIV * 4; j++)
			{
					if(data_mode2[8*j + i] > THREVOLT && data_mode2[8*j + i] < 0x8000)
					{
						pr_flag[i] = 1;
						loc[i] = j;
						break;
					}
			}
		}
		if(loc[0]<=loc[1]&&loc[0]<=loc[2]&&loc[0]<=loc[3])
		{
				start = loc[0];
				chf = 1;
		}else if(loc[1]<=loc[0]&&loc[1]<=loc[2]&&loc[1]<=loc[3])
		{
				start = loc[1];
				chf = 2;
		}else if(loc[2]<=loc[0]&&loc[2]<=loc[1]&&loc[2]<=loc[3])
		{
				start = loc[2];
				chf = 3;
		}else if(loc[3]<=loc[0]&&loc[3]<=loc[1]&&loc[3]<=loc[2])
		{
				start = loc[3];
				chf = 4;
		}
		
		if(start > AD7606C_SAMP_SIZE/SAMP_DIV*4 - 256)
		{
				start = AD7606C_SAMP_SIZE/SAMP_DIV*4 - 256;
				stop = AD7606C_SAMP_SIZE/SAMP_DIV*4 - 1;
		}else
		{
				start = start	;
				stop = start + 255 ;
		}
		
    if(start > 100&&pr_flag[0] == 1)
    {
        Knock_flag = 1;
    }
}


void	NormMode2(float	*input1)
{
	for(int i = 0;i	< 144; i++)
		Norm_Mode2[i] = 0;
	
	for(int i = 0; i < 117; i++){
Norm_Mode2[0] += ((a01_CH1[i]*1000000000 - input1[i]*1000000000))*((a01_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[1] += ((a02_CH1[i]*1000000000 - input1[i]*1000000000))*((a02_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[2] += ((a03_CH1[i]*1000000000 - input1[i]*1000000000))*((a03_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[3] += ((a04_CH1[i]*1000000000 - input1[i]*1000000000))*((a04_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[4] += ((a05_CH1[i]*1000000000 - input1[i]*1000000000))*((a05_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[5] += ((a06_CH1[i]*1000000000 - input1[i]*1000000000))*((a06_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[6] += ((a07_CH1[i]*1000000000 - input1[i]*1000000000))*((a07_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[7] += ((a08_CH1[i]*1000000000 - input1[i]*1000000000))*((a08_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[8] += ((a09_CH1[i]*1000000000 - input1[i]*1000000000))*((a09_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[9] += ((a10_CH1[i]*1000000000 - input1[i]*1000000000))*((a10_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[10] += ((a11_CH1[i]*1000000000 - input1[i]*1000000000))*((a11_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[11] += ((a12_CH1[i]*1000000000 - input1[i]*1000000000))*((a12_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[12] += ((b01_CH1[i]*1000000000 - input1[i]*1000000000))*((b01_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[13] += ((b02_CH1[i]*1000000000 - input1[i]*1000000000))*((b02_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[14] += ((b03_CH1[i]*1000000000 - input1[i]*1000000000))*((b03_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[15] += ((b04_CH1[i]*1000000000 - input1[i]*1000000000))*((b04_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[16] += ((b05_CH1[i]*1000000000 - input1[i]*1000000000))*((b05_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[17] += ((b06_CH1[i]*1000000000 - input1[i]*1000000000))*((b06_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[18] += ((b07_CH1[i]*1000000000 - input1[i]*1000000000))*((b07_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[19] += ((b08_CH1[i]*1000000000 - input1[i]*1000000000))*((b08_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[20] += ((b09_CH1[i]*1000000000 - input1[i]*1000000000))*((b09_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[21] += ((b10_CH1[i]*1000000000 - input1[i]*1000000000))*((b10_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[22] += ((b11_CH1[i]*1000000000 - input1[i]*1000000000))*((b11_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[23] += ((b12_CH1[i]*1000000000 - input1[i]*1000000000))*((b12_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[24] += ((c01_CH1[i]*1000000000 - input1[i]*1000000000))*((c01_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[25] += ((c02_CH1[i]*1000000000 - input1[i]*1000000000))*((c02_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[26] += ((c03_CH1[i]*1000000000 - input1[i]*1000000000))*((c03_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[27] += ((c04_CH1[i]*1000000000 - input1[i]*1000000000))*((c04_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[28] += ((c05_CH1[i]*1000000000 - input1[i]*1000000000))*((c05_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[29] += ((c06_CH1[i]*1000000000 - input1[i]*1000000000))*((c06_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[30] += ((c07_CH1[i]*1000000000 - input1[i]*1000000000))*((c07_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[31] += ((c08_CH1[i]*1000000000 - input1[i]*1000000000))*((c08_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[32] += ((c09_CH1[i]*1000000000 - input1[i]*1000000000))*((c09_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[33] += ((c10_CH1[i]*1000000000 - input1[i]*1000000000))*((c10_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[34] += ((c11_CH1[i]*1000000000 - input1[i]*1000000000))*((c11_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[35] += ((c12_CH1[i]*1000000000 - input1[i]*1000000000))*((c12_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[36] += ((d01_CH1[i]*1000000000 - input1[i]*1000000000))*((d01_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[37] += ((d02_CH1[i]*1000000000 - input1[i]*1000000000))*((d02_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[38] += ((d03_CH1[i]*1000000000 - input1[i]*1000000000))*((d03_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[39] += ((d04_CH1[i]*1000000000 - input1[i]*1000000000))*((d04_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[40] += ((d05_CH1[i]*1000000000 - input1[i]*1000000000))*((d05_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[41] += ((d06_CH1[i]*1000000000 - input1[i]*1000000000))*((d06_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[42] += ((d07_CH1[i]*1000000000 - input1[i]*1000000000))*((d07_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[43] += ((d08_CH1[i]*1000000000 - input1[i]*1000000000))*((d08_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[44] += ((d09_CH1[i]*1000000000 - input1[i]*1000000000))*((d09_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[45] += ((d10_CH1[i]*1000000000 - input1[i]*1000000000))*((d10_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[46] += ((d11_CH1[i]*1000000000 - input1[i]*1000000000))*((d11_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[47] += ((d12_CH1[i]*1000000000 - input1[i]*1000000000))*((d12_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[48] += ((e01_CH1[i]*1000000000 - input1[i]*1000000000))*((e01_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[49] += ((e02_CH1[i]*1000000000 - input1[i]*1000000000))*((e02_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[50] += ((e03_CH1[i]*1000000000 - input1[i]*1000000000))*((e03_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[51] += ((e04_CH1[i]*1000000000 - input1[i]*1000000000))*((e04_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[52] += ((e05_CH1[i]*1000000000 - input1[i]*1000000000))*((e05_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[53] += ((e06_CH1[i]*1000000000 - input1[i]*1000000000))*((e06_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[54] += ((e07_CH1[i]*1000000000 - input1[i]*1000000000))*((e07_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[55] += ((e08_CH1[i]*1000000000 - input1[i]*1000000000))*((e08_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[56] += ((e09_CH1[i]*1000000000 - input1[i]*1000000000))*((e09_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[57] += ((e10_CH1[i]*1000000000 - input1[i]*1000000000))*((e10_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[58] += ((e11_CH1[i]*1000000000 - input1[i]*1000000000))*((e11_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[59] += ((e12_CH1[i]*1000000000 - input1[i]*1000000000))*((e12_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[60] += ((f01_CH1[i]*1000000000 - input1[i]*1000000000))*((f01_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[61] += ((f02_CH1[i]*1000000000 - input1[i]*1000000000))*((f02_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[62] += ((f03_CH1[i]*1000000000 - input1[i]*1000000000))*((f03_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[63] += ((f04_CH1[i]*1000000000 - input1[i]*1000000000))*((f04_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[64] += ((f05_CH1[i]*1000000000 - input1[i]*1000000000))*((f05_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[65] += ((f06_CH1[i]*1000000000 - input1[i]*1000000000))*((f06_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[66] += ((f07_CH1[i]*1000000000 - input1[i]*1000000000))*((f07_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[67] += ((f08_CH1[i]*1000000000 - input1[i]*1000000000))*((f08_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[68] += ((f09_CH1[i]*1000000000 - input1[i]*1000000000))*((f09_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[69] += ((f10_CH1[i]*1000000000 - input1[i]*1000000000))*((f10_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[70] += ((f11_CH1[i]*1000000000 - input1[i]*1000000000))*((f11_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[71] += ((f12_CH1[i]*1000000000 - input1[i]*1000000000))*((f12_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[72] += ((g01_CH1[i]*1000000000 - input1[i]*1000000000))*((g01_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[73] += ((g02_CH1[i]*1000000000 - input1[i]*1000000000))*((g02_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[74] += ((g03_CH1[i]*1000000000 - input1[i]*1000000000))*((g03_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[75] += ((g04_CH1[i]*1000000000 - input1[i]*1000000000))*((g04_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[76] += ((g05_CH1[i]*1000000000 - input1[i]*1000000000))*((g05_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[77] += ((g06_CH1[i]*1000000000 - input1[i]*1000000000))*((g06_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[78] += ((g07_CH1[i]*1000000000 - input1[i]*1000000000))*((g07_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[79] += ((g08_CH1[i]*1000000000 - input1[i]*1000000000))*((g08_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[80] += ((g09_CH1[i]*1000000000 - input1[i]*1000000000))*((g09_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[81] += ((g10_CH1[i]*1000000000 - input1[i]*1000000000))*((g10_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[82] += ((g11_CH1[i]*1000000000 - input1[i]*1000000000))*((g11_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[83] += ((g12_CH1[i]*1000000000 - input1[i]*1000000000))*((g12_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[84] += ((h01_CH1[i]*1000000000 - input1[i]*1000000000))*((h01_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[85] += ((h02_CH1[i]*1000000000 - input1[i]*1000000000))*((h02_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[86] += ((h03_CH1[i]*1000000000 - input1[i]*1000000000))*((h03_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[87] += ((h04_CH1[i]*1000000000 - input1[i]*1000000000))*((h04_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[88] += ((h05_CH1[i]*1000000000 - input1[i]*1000000000))*((h05_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[89] += ((h06_CH1[i]*1000000000 - input1[i]*1000000000))*((h06_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[90] += ((h07_CH1[i]*1000000000 - input1[i]*1000000000))*((h07_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[91] += ((h08_CH1[i]*1000000000 - input1[i]*1000000000))*((h08_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[92] += ((h09_CH1[i]*1000000000 - input1[i]*1000000000))*((h09_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[93] += ((h10_CH1[i]*1000000000 - input1[i]*1000000000))*((h10_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[94] += ((h11_CH1[i]*1000000000 - input1[i]*1000000000))*((h11_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[95] += ((h12_CH1[i]*1000000000 - input1[i]*1000000000))*((h12_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[96] += ((i01_CH1[i]*1000000000 - input1[i]*1000000000))*((i01_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[97] += ((i02_CH1[i]*1000000000 - input1[i]*1000000000))*((i02_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[98] += ((i03_CH1[i]*1000000000 - input1[i]*1000000000))*((i03_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[99] += ((i04_CH1[i]*1000000000 - input1[i]*1000000000))*((i04_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[100] += ((i05_CH1[i]*1000000000 - input1[i]*1000000000))*((i05_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[101] += ((i06_CH1[i]*1000000000 - input1[i]*1000000000))*((i06_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[102] += ((i07_CH1[i]*1000000000 - input1[i]*1000000000))*((i07_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[103] += ((i08_CH1[i]*1000000000 - input1[i]*1000000000))*((i08_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[104] += ((i09_CH1[i]*1000000000 - input1[i]*1000000000))*((i09_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[105] += ((i10_CH1[i]*1000000000 - input1[i]*1000000000))*((i10_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[106] += ((i11_CH1[i]*1000000000 - input1[i]*1000000000))*((i11_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[107] += ((i12_CH1[i]*1000000000 - input1[i]*1000000000))*((i12_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[108] += ((j01_CH1[i]*1000000000 - input1[i]*1000000000))*((j01_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[109] += ((j02_CH1[i]*1000000000 - input1[i]*1000000000))*((j02_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[110] += ((j03_CH1[i]*1000000000 - input1[i]*1000000000))*((j03_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[111] += ((j04_CH1[i]*1000000000 - input1[i]*1000000000))*((j04_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[112] += ((j05_CH1[i]*1000000000 - input1[i]*1000000000))*((j05_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[113] += ((j06_CH1[i]*1000000000 - input1[i]*1000000000))*((j06_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[114] += ((j07_CH1[i]*1000000000 - input1[i]*1000000000))*((j07_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[115] += ((j08_CH1[i]*1000000000 - input1[i]*1000000000))*((j08_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[116] += ((j09_CH1[i]*1000000000 - input1[i]*1000000000))*((j09_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[117] += ((j10_CH1[i]*1000000000 - input1[i]*1000000000))*((j10_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[118] += ((j11_CH1[i]*1000000000 - input1[i]*1000000000))*((j11_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[119] += ((j12_CH1[i]*1000000000 - input1[i]*1000000000))*((j12_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[120] += ((k01_CH1[i]*1000000000 - input1[i]*1000000000))*((k01_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[121] += ((k02_CH1[i]*1000000000 - input1[i]*1000000000))*((k02_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[122] += ((k03_CH1[i]*1000000000 - input1[i]*1000000000))*((k03_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[123] += ((k04_CH1[i]*1000000000 - input1[i]*1000000000))*((k04_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[124] += ((k05_CH1[i]*1000000000 - input1[i]*1000000000))*((k05_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[125] += ((k06_CH1[i]*1000000000 - input1[i]*1000000000))*((k06_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[126] += ((k07_CH1[i]*1000000000 - input1[i]*1000000000))*((k07_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[127] += ((k08_CH1[i]*1000000000 - input1[i]*1000000000))*((k08_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[128] += ((k09_CH1[i]*1000000000 - input1[i]*1000000000))*((k09_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[129] += ((k10_CH1[i]*1000000000 - input1[i]*1000000000))*((k10_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[130] += ((k11_CH1[i]*1000000000 - input1[i]*1000000000))*((k11_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[131] += ((k12_CH1[i]*1000000000 - input1[i]*1000000000))*((k12_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[132] += ((l01_CH1[i]*1000000000 - input1[i]*1000000000))*((l01_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[133] += ((l02_CH1[i]*1000000000 - input1[i]*1000000000))*((l02_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[134] += ((l03_CH1[i]*1000000000 - input1[i]*1000000000))*((l03_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[135] += ((l04_CH1[i]*1000000000 - input1[i]*1000000000))*((l04_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[136] += ((l05_CH1[i]*1000000000 - input1[i]*1000000000))*((l05_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[137] += ((l06_CH1[i]*1000000000 - input1[i]*1000000000))*((l06_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[138] += ((l07_CH1[i]*1000000000 - input1[i]*1000000000))*((l07_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[139] += ((l08_CH1[i]*1000000000 - input1[i]*1000000000))*((l08_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[140] += ((l09_CH1[i]*1000000000 - input1[i]*1000000000))*((l09_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[141] += ((l10_CH1[i]*1000000000 - input1[i]*1000000000))*((l10_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[142] += ((l11_CH1[i]*1000000000 - input1[i]*1000000000))*((l11_CH1[i]*1000000000 - input1[i]*1000000000)) ;
Norm_Mode2[143] += ((l12_CH1[i]*1000000000 - input1[i]*1000000000))*((l12_CH1[i]*1000000000 - input1[i]*1000000000)) ;
		}
}

OY_STATE OY_FindMaxMin(float *p1, uint16_t length, float *max, float *min, float *vpp, uint16_t *row)
{
    *min = *p1;
    *max = 0;
    *row = 0;
    uint16_t i;
    for (i = 1; i < length; i++)
    {
      if (*(p1 + i)<*min)
      {
          *min = *(p1 + i);
          *row = i;
      }
      if (*(p1 + i)>*max)
      {
          *max = *(p1 + i);
          *row = i;
      }
    }
    *vpp = *max - *min;
}

// 初始化主动测量
// 修改采样频率至250k
void	Pos_Init(void)
{
  SAMP_DIV = 1;
  HAL_TIM_Base_Stop(&htim4);
  HAL_TIM_PWM_Stop(&htim4,TIM_CHANNEL_1);
	__HAL_TIM_SET_CLOCKDIVISION(&htim4,1);		
	AVE_NUM = 0;
	for(int i = 0;i < 117;i++)
		AD_Vpp_All[i] = 0;
}

//  初始化被动测量
//  修改采样频率至80k
void	Neg_Init(void)
{
  SAMP_DIV = 1;
  HAL_TIM_Base_Stop(&htim4);
  HAL_TIM_PWM_Stop(&htim4,TIM_CHANNEL_1);
	__HAL_TIM_SET_CLOCKDIVISION(&htim4,3);
	start_address = 1000;
	find_address = 1000;
}

void Pos_Storage_Init(void)
{
		Pos_Init();
		int k = 0;
		start_address = 1024*1024+2000+39*4*3*25*k;
		Pos_Storage_Num = 0;
}



void Pos_Storage(void)
{
		for(int k = 0;k<3;k++){
			for (uint16_t i = 0; i < 39; i++)
			{
				AD_AVEM4[k][i] = 0;
			}
		}
		
		for(int k = 0;k < 5; k++){
			freq = 4800;
			for (uint16_t i = 0; i < 39; i++)
			{
				ad9959_write_frequency(AD9959_CHANNEL_2,freq);
				ad9959_io_update();
				HAL_Delay(10);
				AD7606C_MEASURE();
				ADC_BitToFloat();
				OY_FindMaxMin(volt[0],AD7606C_SAMP_SIZE,&AD_Max[0][i],&AD_Min[0][i],&AD_Vpp[0][i],row);
				OY_FindMaxMin(volt[1],AD7606C_SAMP_SIZE,&AD_Max[1][i],&AD_Min[1][i],&AD_Vpp[1][i],row);
				OY_FindMaxMin(volt[2],AD7606C_SAMP_SIZE,&AD_Max[2][i],&AD_Min[2][i],&AD_Vpp[2][i],row);
				//OY_FindMaxMin(volt[3],AD7606C_SAMP_SIZE,&AD_Max[3][i],&AD_Min[3][i],&AD_Vpp[3][i],row);
				if(freq == FREQTERM){
						freq = 4800;
						dds_stopflag = 1;
				}else{
						freq = freq + 400;
				}
			}
			for (uint16_t i = 0; i < 3; i++){
				for(int j = 0;j < 39;j++)
				{
						AD_AVEM4[i][j] += AD_Vpp[i][j]/5.;
				}
			}
		}
		
		uartuse = 1;
		for(int i = 0; i < 39;i++)
			printf("%f,%f,%f\n",AD_AVEM4[0][i],AD_AVEM4[1][i],AD_AVEM4[2][i]);
		if(oystate == POS_Storage_STATE)
		{
				while(sapa == 4){
						if(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_6) == GPIO_PIN_RESET)
							sapa = 2;

						if(HAL_GPIO_ReadPin(GPIOH,GPIO_PIN_11) == GPIO_PIN_RESET)
							sapa = 3;
				};
				if(sapa == 2){
					find_address = start_address;
					for(int i = 0;i < 3;i++)
					{
							W25Q256_write(qspi_handler,start_address,(uint8_t *)AD_AVEM4[i],POSOFFSET);
							start_address = start_address + POSOFFSET;// 1024*1024+2000+39*4*3*25
					}
					sprintf((char*)buf1,"t0.txt=\"save succesfully\"");
					HMISends(buf1);
					HMISend(0xff);
					Pos_Storage_Num++;
					sprintf((char*)buf1,"t1.txt=\"has saved %d dots Address: %d\"",Pos_Storage_Num,start_address);
					HMISends(buf1);
					HMISend(0xff);
					
					for(int i = 0;i < 3;i++)
					{
							W25Q256_read(qspi_handler,find_address,(uint8_t *)AD_AVEM4_Load[i],POSOFFSET);
							find_address += POSOFFSET;
					}
					for (uint16_t j = 0; j < 39; j++)
					{
						printf("%f,%f,%f\n",AD_AVEM4_Load[0][j],AD_AVEM4_Load[1][j],AD_AVEM4_Load[2][j]);		
					}
					sapa = 4;
				}
				else if(sapa == 3){
						sprintf((char*)buf1,"t0.txt=\"Skip Suceessfully\"");
						HMISends(buf1);
						HMISend(0xff);
						sapa = 4;
			}
		}
}





void	PosM4_Test(void)
{
		float temp[3];
		float min_M4;
		uint16_t row_M4;
		for(int k = 0;k<3;k++){
			for (uint16_t i = 0; i < 39; i++)
			{
				AD_AVEM4[k][i] = 0;
			}
		}
		
		for(int k = 0;k < 5; k++){
			freq = 4800;
			for (uint16_t i = 0; i < 39; i++)
			{
				ad9959_write_frequency(AD9959_CHANNEL_2,freq);
				ad9959_io_update();
				HAL_Delay(10);
				AD7606C_MEASURE();
				ADC_BitToFloat();
				OY_FindMaxMin(volt[0],AD7606C_SAMP_SIZE,&AD_Max[0][i],&AD_Min[0][i],&AD_Vpp[0][i],row);
				OY_FindMaxMin(volt[1],AD7606C_SAMP_SIZE,&AD_Max[1][i],&AD_Min[1][i],&AD_Vpp[1][i],row);
				OY_FindMaxMin(volt[2],AD7606C_SAMP_SIZE,&AD_Max[2][i],&AD_Min[2][i],&AD_Vpp[2][i],row);
				//OY_FindMaxMin(volt[3],AD7606C_SAMP_SIZE,&AD_Max[3][i],&AD_Min[3][i],&AD_Vpp[3][i],row);
				if(freq == FREQTERM){
						freq = 4800;
						dds_stopflag = 1;
				}else{
						freq = freq + 400;
				}
			}
			for (uint16_t i = 0; i < 3; i++){
				for(int j = 0;j < 39;j++)
				{
						AD_AVEM4[i][j] += AD_Vpp[i][j]/5.;
				}
			}
		}
		
		uartuse = 1;
//		for(int i = 0; i < 39;i++)
//		printf("%f,%f,%f\n",AD_AVEM4[0][i],AD_AVEM4[1][i],AD_AVEM4[2][i]);
		if(oystate == POS_MODE4_STATE)
		{
			find_address = Pos_Start_ADDRESS;
			for(int j = 0;j < 625;j++)				//		need to change max j
			{
					for(int i = 0;i < 3;i++)
					{
							W25Q256_read(qspi_handler,find_address,(uint8_t *)AD_AVEM4_Load[i],POSOFFSET);
							find_address += POSOFFSET;
					}
					OY_Norm(AD_AVEM4_Load[0], AD_AVEM4[0],39,&temp[0],(float)100);
					OY_Norm(AD_AVEM4_Load[1], AD_AVEM4[1],39,&temp[1],(float)100);
					OY_Norm(AD_AVEM4_Load[2], AD_AVEM4[2],39,&temp[2],(float)100);
					Mode4_Res[j] = temp[0] + temp[1] + temp[2];
			}
			
			OY_FindMin(Mode4_Res,625,&min_M4,&row_M4);
			sprintf((char*)buf1,"t0.txt=\"%d\"",row_M4 + 1);
			HMISends(buf1);
			HMISend(0xff);
			
			uint16_t num;
			uint8_t letter;
			if((row_M4 + 1)%25 == 0)
			{
					num = 25;
					letter = (row_M4 + 1)/25;
			}else
			{
					num = (row_M4 + 1)%25;
					letter = (row_M4 + 1)/25 + 1;
			}
			sprintf((char*)buf1,"t0.txt=\"(%d,%d)\"",letter,num);
			HMISends(buf1);
			HMISend(0xff);
			
			
			
		}

}




void  Signal_Samp_Pos(void)
{
	uint16_t row_p;
	float min_p;
	freq = 1000;
  for (uint16_t i = 0; i < 39; i++)
  {
    ad9959_write_frequency(AD9959_CHANNEL_2,freq);
    ad9959_io_update();
    HAL_Delay(10);
    AD7606C_MEASURE();
    ADC_BitToFloat();
    OY_FindMaxMin(volt[0],AD7606C_SAMP_SIZE,&AD_Max[0][i],&AD_Min[0][i],&AD_Vpp[0][i],row);
    OY_FindMaxMin(volt[1],AD7606C_SAMP_SIZE,&AD_Max[1][i],&AD_Min[1][i],&AD_Vpp[1][i],row);
    OY_FindMaxMin(volt[2],AD7606C_SAMP_SIZE,&AD_Max[2][i],&AD_Min[2][i],&AD_Vpp[2][i],row);
    OY_FindMaxMin(volt[3],AD7606C_SAMP_SIZE,&AD_Max[3][i],&AD_Min[3][i],&AD_Vpp[3][i],row);
		
		
    if(freq == FREQTERM){
        freq = 1000;
        dds_stopflag = 1;
    }else{
        freq = freq + FREQSTEP;
    }
  }
		

		for(int j = 0;j < 3; j++)
		{
			for(int i = 0;i < 39; i++)
			{
							AD_Vpp_All[i+39*j] = AD_Vpp[j][i];
			}
		}
				
			NormMode2(AD_Vpp_All);
			OY_FindMin(Norm_Mode2,144,&min_p,&row_p);
			printf("%d\r\n",row_p + 1);
			sprintf((char*)buf1,"t1.txt=\"at %d cell\"",row_p + 1);
			HMISends(buf1);
			HMISend(0xff);
			//CoorTransMode3(row_p + 1);
				uint16_t num;
			uint8_t letter;
			if((row_p + 1)%12 == 0)
			{
					num = 12;
					letter = (row_p + 1)/12;
			}else
			{
					num = (row_p + 1)%12;
					letter = (row_p + 1)/12 + 1;
			}
				if(num < 10)
					sprintf((char*)buf1,"t0.txt=\"(%c,0%d)\"",'A'+(char)(letter - 1),num);
				else
					sprintf((char*)buf1,"t0.txt=\"(%c,%d)\"",'A'+(char)(letter - 1),num);
				HMISends(buf1);
				HMISend(0xff);
}


void  Signal_Samp_Neg(void)
{
//	float	corr_resmax[6];
//	uint16_t	corr_resrow[6];
  while(!Knock_flag)
  {
    AD7606C_MEASURE_Mode2();
    VoltJudge();
  }
  
	if(Knock_flag){
		ADC_BitToFloat_Mode2();
		arm_correlate_f32(&volt_mode2[0][start],256,&volt_mode2[1][start],256,xcorr_out[0]);
		arm_correlate_f32(&volt_mode2[0][start],256,&volt_mode2[2][start],256,xcorr_out[1]);
		arm_correlate_f32(&volt_mode2[1][start],256,&volt_mode2[2][start],256,xcorr_out[2]);
		arm_correlate_f32(&volt_mode2[2][start],256,&volt_mode2[3][start],256,xcorr_out[3]);
		OY_Normalization(xcorr_out[0],511);
		OY_Normalization(xcorr_out[1],511);
		OY_Normalization(xcorr_out[2],511);
		OY_Normalization(xcorr_out[3],511);
		
//		OY_FindMax(xcorr_out[0],2*256-1,&corr_resmax[0],&corr_resrow[0]);
//		OY_FindMax(xcorr_out[1],2*256-1,&corr_resmax[1],&corr_resrow[1]);
//		OY_FindMax(xcorr_out[2],2*256-1,&corr_resmax[2],&corr_resrow[2]);
//		OY_FindMax(xcorr_out[3],2*256-1,&corr_resmax[3],&corr_resrow[3]);
		
//		for (uint16_t j = start - 50; j <= stop + 50; j++)
//		{
//			printf("%f,%f,%f,%f\n",volt_mode2[0][j],volt_mode2[1][j],volt_mode2[2][j],volt_mode2[3][j]);
//		}
		uartuse = 1;
//		printf("\r\n****************************\r\n");
//		for (uint16_t j = 0; j < 511; j++)
//		{
//			printf("%f,%f,%f,%f\n",xcorr_out[0][j],xcorr_out[1][j],xcorr_out[2][j],xcorr_out[3][j]);		
//		}
//		printf("\r\n****************************\r\n");
//		
		if(oystate == Calibration_STATE)
		{
				sprintf((char*)buf1,"t0.txt=\"Wait for Press\"");
				HMISends(buf1);
				HMISend(0xff);
				while(sapa == 4){
						if(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_6) == GPIO_PIN_RESET)
							sapa = 2;

						if(HAL_GPIO_ReadPin(GPIOH,GPIO_PIN_11) == GPIO_PIN_RESET)
							sapa = 3;
				};
				if(sapa == 2){
						calibration_num++;
						if(calibration_num == 37)
						{
								sprintf((char*)buf1,"t0.txt=\"Calibration reload\"");
								HMISends(buf1);
								HMISend(0xff);
								calibration_num = 1;
								start_address = 1000;
						}
						find_address = start_address;
						for(int i = 0;i < 4;i++)
						{
								W25Q256_write(qspi_handler,start_address,(uint8_t *)xcorr_out[i],XCORRSIZE);
								start_address = start_address + XCORRSIZE;
						}
						sprintf((char*)buf1,"t0.txt=\"Save %d data Suceessfully\"",calibration_num);
						HMISends(buf1);
						HMISend(0xff);
						sprintf((char*)buf1,"t1.txt=\"Saved %d data\"",calibration_num);
						HMISends(buf1);
						HMISend(0xff);
						sapa = 4;
						

						for(int i = 0;i < 4;i++)
						{
								W25Q256_read(qspi_handler,find_address,(uint8_t *)xcorr_out_test[i],XCORRSIZE);
								find_address += XCORRSIZE;
						}
						
						for (uint16_t j = 0; j < 511; j++)
						{
							printf("%f,%f,%f,%f\n",xcorr_out_test[0][j],xcorr_out_test[1][j],xcorr_out_test[2][j],xcorr_out_test[3][j]);		
						}
						
				}else if(sapa == 3)
				{
						sprintf((char*)buf1,"t0.txt=\"Skip Suceessfully\"");
						HMISends(buf1);
						HMISend(0xff);
						sapa = 4;
				}
				
		}else if(oystate == NEG_MEASURE_STATE)
		{
			find_address = 1000;
			for(int j = 0;j < 36;j++){
				for(int i = 0;i < 4;i++)
				{
						W25Q256_read(qspi_handler,find_address,(uint8_t *)xcorr_out_test[i],XCORRSIZE);
						find_address += XCORRSIZE;
				}
//				Neg_res[j] = xcorr0(xcorr_out_test[0],xcorr_out[0],511) + xcorr0(xcorr_out_test[1],xcorr_out[1],511) + xcorr0(xcorr_out_test[2],xcorr_out[2],511)+xcorr0(xcorr_out_test[3],xcorr_out[3],511);
//				for (uint16_t j = 0; j < 511; j++)
//						{
//							printf("%f,%f,%f,%f\n",xcorr_out_test[0][j]-xcorr_out[0][j],xcorr_out_test[1][j]-xcorr_out[1][j],xcorr_out_test[2][j]-xcorr_out[2][j],xcorr_out_test[3][j]-xcorr_out[3][j]);		
//						}
//				
//				for (uint16_t j = 0; j < 511; j++)
//						{
//							printf("%f,%f,%f,%f\n",xcorr_out[0][j],xcorr_out[1][j],xcorr_out[2][j],xcorr_out[3][j]);		
//						}		
//						
				Neg_res[j] = 0;
				for(int k = 0;k < 511; k++)
				{
					Neg_res[j] += (xcorr_out_test[0][k] - xcorr_out[0][k])*(xcorr_out_test[0][k] - xcorr_out[0][k]);
					Neg_res[j] += (xcorr_out_test[1][k] - xcorr_out[1][k])*(xcorr_out_test[1][k] - xcorr_out[1][k]);
					Neg_res[j] += (xcorr_out_test[2][k] - xcorr_out[2][k])*(xcorr_out_test[2][k] - xcorr_out[2][k]);
					Neg_res[j] += (xcorr_out_test[3][k] - xcorr_out[3][k])*(xcorr_out_test[3][k] - xcorr_out[3][k]);
				}
				
			}
			
			OY_FindMin(Neg_res,36,&min_neg,&row_neg);
//			sprintf((char*)buf1,"t1.txt=\"at %dth cell\"",row_neg+1);
//			HMISends(buf1);
//			HMISend(0xff);
			
			uint16_t num;
			uint8_t letter;
			if((row_neg + 1)%6 == 0)
			{
					num = 6;
					letter = (row_neg + 1)/6;
			}else
			{
					num = (row_neg + 1)%6;
					letter = (row_neg + 1)/6 + 1;
			}
			if(num < 5)
				sprintf((char*)buf1,"t0.txt=\"(%c%c,0%d0%d)\"",'A'+(char)(letter - 1)*2,'A'+(char)(letter)*2 - 1,num*2-1,num*2);
			else if(num == 5)
				sprintf((char*)buf1,"t0.txt=\"(%c%c,0%d%d)\"",'A'+(char)(letter - 1)*2,'A'+(char)(letter)*2 - 1,num*2-1,num*2);
			else
				sprintf((char*)buf1,"t0.txt=\"(%c%c,%d%d)\"",'A'+(char)(letter - 1)*2,'A'+(char)(letter)*2 - 1,num*2-1,num*2);
			HMISends(buf1);
			HMISend(0xff);
			
			
//			sprintf((char*)buf1,"t1.txt=\"%f\"",min_neg);
//			HMISends(buf1);
//			HMISend(0xff);
			
		}
		
		Knock_flag = 0;
	}
  
}



void  Signal_Process(void)
{
  
}


void  Volt_Freq_Print(void)
{
  //  取10次平均发送3个通道值
	if(cnt_ave == 0){
	for(int k = 0;k<3;k++){
			for (uint16_t i = 0; i < 39; i++)
			{
				AD_AVE[k][i] = 0;
			}
		}
	}
	
	for(int k = 0;k<3;k++){
			for (uint16_t i = 0; i < 39; i++)
			{
				AD_AVE[k][i] += AD_Vpp[k][i];
			}
		}
	if(cnt_ave == AVENUM - 1){
		for(int k = 0;k<3;k++){
			for(uint16_t i = 0; i < 39; i++)
			{
				AD_AVE[k][i]  = AD_AVE[k][i]/AVENUM;
			}
			printf("CH%d=[",k+1);
			for (uint16_t i = 0; i < 39; i++)
			{
				printf("%f, ",AD_AVE[k][i]);
			}
			printf("]\r\n");
			//printf("\r\n********\r\n");

		}
		cnt_ave = 0;
	}
	cnt_ave++;
}

void  Signal_Print(void)
{

	Volt_Freq_Print();
	
}

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MPU_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MPU Configuration--------------------------------------------------------*/
  MPU_Config();

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_FMC_Init();
  MX_TIM4_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_TIM5_Init();
  MX_QUADSPI_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */
	ad9959_init();
	ad9959_write_amplitude(AD9959_CHANNEL_2,1023);
	ad9959_io_update();
	printf("Hello World!\n");
	AD7606C_ID = AD7606C_Init(p,SINGLE_10V,HighBand,OverSamp_off);
  freq = 1000;
	
	W9825G6KH_clear(W9825G6KH_initHardware(&hsdram1,(uint32_t *)0xC0000000));
	qspi_handler = W25Q256_initHardware(&hqspi,(uint32_t *)0);
	//HAL_UART_Receive_IT(&huart3,&re,1);
	
	oystate = 100;
	
	
	sprintf((char*)buf1,"t0.txt=\"lzfssb\"");
	HMISends(buf1);
	HMISend(0xff);
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		//HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_7);

		
//		if(HAL_GPIO_ReadPin(GPIOH,GPIO_PIN_2) == GPIO_PIN_SET)
//			HAL_GPIO_WritePin(GPIOH,GPIO_PIN_10,GPIO_PIN_SET);
//		Signal_Samp_Pos();
//    Signal_Print();
		
		
				if((GPIOC->IDR & GPIO_PIN_9) == GPIO_PIN_RESET)
		{
				oystate = DDS_STATE;
				HAL_GPIO_WritePin(GPIOC,GPIO_PIN_7,GPIO_PIN_SET);
				HAL_GPIO_WritePin(GPIOH,GPIO_PIN_14,GPIO_PIN_SET);
				HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOH,GPIO_PIN_9,GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOH,GPIO_PIN_10,GPIO_PIN_RESET);
		}
		if((GPIOB->IDR & GPIO_PIN_5) == GPIO_PIN_RESET)
		{
				oystate = NEG_MEASURE_STATE;
				Neg_Init();
				HAL_GPIO_WritePin(GPIOC,GPIO_PIN_7,GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOH,GPIO_PIN_14,GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,GPIO_PIN_SET);
				HAL_GPIO_WritePin(GPIOH,GPIO_PIN_9,GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOH,GPIO_PIN_10,GPIO_PIN_RESET);
		}
		if((GPIOA->IDR & GPIO_PIN_12) == GPIO_PIN_RESET)
		{
				oystate = POS_MEASURE_STATE;
				Pos_Init();
				HAL_GPIO_WritePin(GPIOC,GPIO_PIN_7,GPIO_PIN_SET);
				HAL_GPIO_WritePin(GPIOH,GPIO_PIN_14,GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOH,GPIO_PIN_9,GPIO_PIN_SET);
				HAL_GPIO_WritePin(GPIOH,GPIO_PIN_10,GPIO_PIN_RESET);
		}
		if((GPIOA->IDR & GPIO_PIN_11) == GPIO_PIN_RESET)
		{
				oystate = POS_MODE4_STATE;
				Pos_Init();
				HAL_GPIO_WritePin(GPIOC,GPIO_PIN_7,GPIO_PIN_SET);
				HAL_GPIO_WritePin(GPIOH,GPIO_PIN_14,GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOH,GPIO_PIN_9,GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOH,GPIO_PIN_10,GPIO_PIN_SET);
		}
		
//		if(oystate == NEG_MEASURE_STATE || oystate == Calibration_STATE)
//		{
//				Neg_Init();
//		}
//		
//		if(oystate == POS_MEASURE_STATE )
//		{
//				Pos_Init();
//		}
//		
//		if(oystate == POS_Storage_STATE )
//		{
//				Pos_Storage_Init();
//		}
//		if(oystate == POS_MODE4_STATE )
//		{
//				Pos_Init();
//		}
//		
		
		if(oystate == DDS_STATE)
		{
			freq = 15000;
			for(; freq <=20000;freq = freq + 1000){
					ad9959_write_frequency(AD9959_CHANNEL_2,freq);
					ad9959_io_update();
					HAL_Delay(1000);
			}
		}
		
		
		if(oystate == NEG_MEASURE_STATE || oystate == Calibration_STATE)
		{
				Signal_Samp_Neg();
		}
		
		if(oystate == POS_Storage_STATE )
		{
			Pos_Storage();
		}
		
		
		if(oystate == POS_MEASURE_STATE )
		{
				Signal_Samp_Pos();
		}
		
		if(oystate == POS_MODE4_STATE )
		{
				PosM4_Test();
		}
		

//		if((GPIOH->IDR & GPIO_PIN_11) == GPIO_PIN_RESET)
//		{
//				oystate = Calibration_STATE;
//				HAL_GPIO_WritePin(GPIOG,GPIO_PIN_9,GPIO_PIN_SET);
//				HAL_GPIO_WritePin(GPIOH,GPIO_PIN_14,GPIO_PIN_SET);
//				HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,GPIO_PIN_SET);
//				HAL_GPIO_WritePin(GPIOH,GPIO_PIN_9,GPIO_PIN_SET);
//				HAL_GPIO_WritePin(GPIOH,GPIO_PIN_10,GPIO_PIN_SET);
//		}
		
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 432;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_USART2
                              |RCC_PERIPHCLK_USART3;
  PeriphClkInitStruct.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInitStruct.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  PeriphClkInitStruct.Usart3ClockSelection = RCC_USART3CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void DMA1_Stream6_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_tim4_up);
	HAL_TIM_Base_Stop(&htim4);
	HAL_TIM_PWM_Stop(&htim4,TIM_CHANNEL_1);
	AD7606C_flag = 1;

}

void TIM4_IRQHandler(void)
{

  HAL_TIM_IRQHandler(&htim4);
}

void EXTI9_5_IRQHandler(void)
{
	sampflag = 1;
  __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_6);;
}



/* USER CODE END 4 */

/* MPU Configuration */

void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};

  /* Disables the MPU */
  HAL_MPU_Disable();
  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress = 0x60000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_256MB;
  MPU_InitStruct.SubRegionDisable = 0x0;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER1;
  MPU_InitStruct.BaseAddress = 0xC0000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_32MB;
  MPU_InitStruct.SubRegionDisable = 0x0;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
