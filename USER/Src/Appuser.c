#include "Appuser.h"
#include "tim.h"
#include "diagram1.h"
#include "AD7606C.h"
#include "ad9959.h"
#include "User_function.h"
#include "usart.h"
#include "W25Q256.h"


float NormPos1Data[144];

float AD_AVE[4][39];


OY_APP_Configure	oyconfig = 
{
		1000000,
		100,
		&htim4
};


OY_List_Process ListCH1ForPos = 
{
		2*1024*1024 + 1000,
		4*39,
		2*1024*1024 + 1000,
		0,
		ListForPos1_GetData,
		ListKeyDetect_POS1,
		WavePrintFORPOS1
};


OY_List_Process ListCH2ForPos = 
{
		3*1024*1024 + 1000,
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



/*	REQUEST 3or4 GETDATA	*/
void	ListForPos1_GetData(uint16_t	AVENUM)
{
	/*	Initialize	*/
	float *vp[4];
	float AD_Max[4][39];
	float AD_Min[4][39];
	float AD_Vpp[4][39];
	uint16_t row[1];
	uint16_t freq;
	
	for(int k = 0;k<4;k++)
	{
		vp[k] = volt[k];
		for (uint16_t i = 0; i < 39; i++)
		{
			AD_AVE[k][i] = 0;
		}
	}
	
	/*	ADC SAMPLE	*/
	for(int k = 0;k < AVENUM; k++)
	{
		freq = 4800;
		for (uint16_t i = 0; i < 39; i++)
		{
			ad9959_write_frequency(AD9959_CHANNEL_2,freq);
			ad9959_io_update();
			HAL_Delay(10);
			AD7606C_SAMP_Start(data,AD7606C_SAMP_SIZE);
			AD7606C_BitToFloat(data,vp,AD7606C_SAMP_SIZE);
			OY_FindMaxMin(volt[0],AD7606C_SAMP_SIZE,&AD_Max[0][i],&AD_Min[0][i],&AD_Vpp[0][i],row);
			OY_FindMaxMin(volt[1],AD7606C_SAMP_SIZE,&AD_Max[1][i],&AD_Min[1][i],&AD_Vpp[1][i],row);
			OY_FindMaxMin(volt[2],AD7606C_SAMP_SIZE,&AD_Max[2][i],&AD_Min[2][i],&AD_Vpp[2][i],row);
			OY_FindMaxMin(volt[3],AD7606C_SAMP_SIZE,&AD_Max[3][i],&AD_Min[3][i],&AD_Vpp[3][i],row);
			if(freq == 20000){
				freq = 4800;
			}else{
				freq = freq + 400;
			}
		}
		for (uint16_t i = 0; i < 4; i++){
			for(int j = 0;j < 39;j++)
			{
				AD_AVE[i][j] += AD_Vpp[i][j]/5.f;
			}
		}
	}
}

void	WavePrintFORPOS1(float (*wave)[39], int len)
{
		for(int i = 0; i < len; i++)
			printf("%f,%f,%f,%f",wave[0][i],wave[1][i],wave[2][i],wave[3][i]);
}


void	ListKeyDetect_POS1(void)
{
	ListKeyState	keystate = WAIT_STATE;
	uint32_t	StartAddress;
	uint8_t buf1[256];
	
	while(keystate == WAIT_STATE)
	{
		if(KEY1 == GPIO_PIN_RESET)
		{
			keystate = STORAGE_STATE;
		}
		else if(KEY2 == GPIO_PIN_RESET)
		{
			keystate = SKIP_STATE;
		}
		else if(KEY3 == GPIO_PIN_RESET)
		{
			keystate = RESET_STATE;
		}
	}
	
	switch(keystate)
	{
		case	STORAGE_STATE:
			StartAddress = ListCH1ForPos.ListCurrentAddress;
		
			/* Choose	CH1 to CH3 Data!
				 It may need to change if you want to 
				 change the activation channel. */
			for(int i = 0;i < 3;i++)
			{
					W25Q256_write(qspi_handler,StartAddress,(uint8_t *)AD_AVE[i],ListCH1ForPos.ListOffsetAddress);
					ListCH1ForPos.ListCurrentAddress += ListCH1ForPos.ListOffsetAddress;// 1024*1024+2000+39*4*3*25
			}
			
			sprintf((char*)buf1,"t0.txt=\"save succesfully\"");
			HMISends(buf1);
			HMISend(0xff);
			ListCH1ForPos.DotsNum++;
			sprintf((char*)buf1,"t1.txt=\"saved %d\"",ListCH1ForPos.DotsNum);
			HMISends(buf1);
			HMISend(0xff);
			keystate = WAIT_STATE;
			break;
		case	SKIP_STATE:
			sprintf((char*)buf1,"t0.txt=\"Skip Suceessfully\"");
			HMISends(buf1);
			HMISend(0xff);
			keystate = WAIT_STATE;
			break;
		case	RESET_STATE:
			ListCH1ForPos.ListCurrentAddress -= 3*ListCH1ForPos.ListOffsetAddress;
			sprintf((char*)buf1,"t0.txt=\"reset succesfully\"");
			HMISends(buf1);
			HMISend(0xff);
			ListCH1ForPos.DotsNum--;
			sprintf((char*)buf1,"t1.txt=\"saved %d\"",ListCH1ForPos.DotsNum);
			HMISends(buf1);
			HMISend(0xff);
			keystate = WAIT_STATE;	
			break;
		default:
			keystate = WAIT_STATE;	
			break;
	}
	
}















void	NormForPos1(float *input1, float multnum)
{
	for(int i = 0;i	< 144; i++)
		NormPos1Data[i] = 0;
	
	for(int i = 0; i < 117; i++){
		NormPos1Data[0] += ((a01_CH1[i]*multnum - input1[i]*multnum))*((a01_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[1] += ((a02_CH1[i]*multnum - input1[i]*multnum))*((a02_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[2] += ((a03_CH1[i]*multnum - input1[i]*multnum))*((a03_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[3] += ((a04_CH1[i]*multnum - input1[i]*multnum))*((a04_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[4] += ((a05_CH1[i]*multnum - input1[i]*multnum))*((a05_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[5] += ((a06_CH1[i]*multnum - input1[i]*multnum))*((a06_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[6] += ((a07_CH1[i]*multnum - input1[i]*multnum))*((a07_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[7] += ((a08_CH1[i]*multnum - input1[i]*multnum))*((a08_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[8] += ((a09_CH1[i]*multnum - input1[i]*multnum))*((a09_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[9] += ((a10_CH1[i]*multnum - input1[i]*multnum))*((a10_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[10] += ((a11_CH1[i]*multnum - input1[i]*multnum))*((a11_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[11] += ((a12_CH1[i]*multnum - input1[i]*multnum))*((a12_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[12] += ((b01_CH1[i]*multnum - input1[i]*multnum))*((b01_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[13] += ((b02_CH1[i]*multnum - input1[i]*multnum))*((b02_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[14] += ((b03_CH1[i]*multnum - input1[i]*multnum))*((b03_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[15] += ((b04_CH1[i]*multnum - input1[i]*multnum))*((b04_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[16] += ((b05_CH1[i]*multnum - input1[i]*multnum))*((b05_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[17] += ((b06_CH1[i]*multnum - input1[i]*multnum))*((b06_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[18] += ((b07_CH1[i]*multnum - input1[i]*multnum))*((b07_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[19] += ((b08_CH1[i]*multnum - input1[i]*multnum))*((b08_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[20] += ((b09_CH1[i]*multnum - input1[i]*multnum))*((b09_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[21] += ((b10_CH1[i]*multnum - input1[i]*multnum))*((b10_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[22] += ((b11_CH1[i]*multnum - input1[i]*multnum))*((b11_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[23] += ((b12_CH1[i]*multnum - input1[i]*multnum))*((b12_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[24] += ((c01_CH1[i]*multnum - input1[i]*multnum))*((c01_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[25] += ((c02_CH1[i]*multnum - input1[i]*multnum))*((c02_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[26] += ((c03_CH1[i]*multnum - input1[i]*multnum))*((c03_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[27] += ((c04_CH1[i]*multnum - input1[i]*multnum))*((c04_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[28] += ((c05_CH1[i]*multnum - input1[i]*multnum))*((c05_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[29] += ((c06_CH1[i]*multnum - input1[i]*multnum))*((c06_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[30] += ((c07_CH1[i]*multnum - input1[i]*multnum))*((c07_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[31] += ((c08_CH1[i]*multnum - input1[i]*multnum))*((c08_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[32] += ((c09_CH1[i]*multnum - input1[i]*multnum))*((c09_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[33] += ((c10_CH1[i]*multnum - input1[i]*multnum))*((c10_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[34] += ((c11_CH1[i]*multnum - input1[i]*multnum))*((c11_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[35] += ((c12_CH1[i]*multnum - input1[i]*multnum))*((c12_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[36] += ((d01_CH1[i]*multnum - input1[i]*multnum))*((d01_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[37] += ((d02_CH1[i]*multnum - input1[i]*multnum))*((d02_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[38] += ((d03_CH1[i]*multnum - input1[i]*multnum))*((d03_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[39] += ((d04_CH1[i]*multnum - input1[i]*multnum))*((d04_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[40] += ((d05_CH1[i]*multnum - input1[i]*multnum))*((d05_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[41] += ((d06_CH1[i]*multnum - input1[i]*multnum))*((d06_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[42] += ((d07_CH1[i]*multnum - input1[i]*multnum))*((d07_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[43] += ((d08_CH1[i]*multnum - input1[i]*multnum))*((d08_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[44] += ((d09_CH1[i]*multnum - input1[i]*multnum))*((d09_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[45] += ((d10_CH1[i]*multnum - input1[i]*multnum))*((d10_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[46] += ((d11_CH1[i]*multnum - input1[i]*multnum))*((d11_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[47] += ((d12_CH1[i]*multnum - input1[i]*multnum))*((d12_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[48] += ((e01_CH1[i]*multnum - input1[i]*multnum))*((e01_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[49] += ((e02_CH1[i]*multnum - input1[i]*multnum))*((e02_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[50] += ((e03_CH1[i]*multnum - input1[i]*multnum))*((e03_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[51] += ((e04_CH1[i]*multnum - input1[i]*multnum))*((e04_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[52] += ((e05_CH1[i]*multnum - input1[i]*multnum))*((e05_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[53] += ((e06_CH1[i]*multnum - input1[i]*multnum))*((e06_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[54] += ((e07_CH1[i]*multnum - input1[i]*multnum))*((e07_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[55] += ((e08_CH1[i]*multnum - input1[i]*multnum))*((e08_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[56] += ((e09_CH1[i]*multnum - input1[i]*multnum))*((e09_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[57] += ((e10_CH1[i]*multnum - input1[i]*multnum))*((e10_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[58] += ((e11_CH1[i]*multnum - input1[i]*multnum))*((e11_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[59] += ((e12_CH1[i]*multnum - input1[i]*multnum))*((e12_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[60] += ((f01_CH1[i]*multnum - input1[i]*multnum))*((f01_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[61] += ((f02_CH1[i]*multnum - input1[i]*multnum))*((f02_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[62] += ((f03_CH1[i]*multnum - input1[i]*multnum))*((f03_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[63] += ((f04_CH1[i]*multnum - input1[i]*multnum))*((f04_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[64] += ((f05_CH1[i]*multnum - input1[i]*multnum))*((f05_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[65] += ((f06_CH1[i]*multnum - input1[i]*multnum))*((f06_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[66] += ((f07_CH1[i]*multnum - input1[i]*multnum))*((f07_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[67] += ((f08_CH1[i]*multnum - input1[i]*multnum))*((f08_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[68] += ((f09_CH1[i]*multnum - input1[i]*multnum))*((f09_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[69] += ((f10_CH1[i]*multnum - input1[i]*multnum))*((f10_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[70] += ((f11_CH1[i]*multnum - input1[i]*multnum))*((f11_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[71] += ((f12_CH1[i]*multnum - input1[i]*multnum))*((f12_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[72] += ((g01_CH1[i]*multnum - input1[i]*multnum))*((g01_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[73] += ((g02_CH1[i]*multnum - input1[i]*multnum))*((g02_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[74] += ((g03_CH1[i]*multnum - input1[i]*multnum))*((g03_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[75] += ((g04_CH1[i]*multnum - input1[i]*multnum))*((g04_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[76] += ((g05_CH1[i]*multnum - input1[i]*multnum))*((g05_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[77] += ((g06_CH1[i]*multnum - input1[i]*multnum))*((g06_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[78] += ((g07_CH1[i]*multnum - input1[i]*multnum))*((g07_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[79] += ((g08_CH1[i]*multnum - input1[i]*multnum))*((g08_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[80] += ((g09_CH1[i]*multnum - input1[i]*multnum))*((g09_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[81] += ((g10_CH1[i]*multnum - input1[i]*multnum))*((g10_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[82] += ((g11_CH1[i]*multnum - input1[i]*multnum))*((g11_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[83] += ((g12_CH1[i]*multnum - input1[i]*multnum))*((g12_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[84] += ((h01_CH1[i]*multnum - input1[i]*multnum))*((h01_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[85] += ((h02_CH1[i]*multnum - input1[i]*multnum))*((h02_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[86] += ((h03_CH1[i]*multnum - input1[i]*multnum))*((h03_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[87] += ((h04_CH1[i]*multnum - input1[i]*multnum))*((h04_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[88] += ((h05_CH1[i]*multnum - input1[i]*multnum))*((h05_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[89] += ((h06_CH1[i]*multnum - input1[i]*multnum))*((h06_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[90] += ((h07_CH1[i]*multnum - input1[i]*multnum))*((h07_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[91] += ((h08_CH1[i]*multnum - input1[i]*multnum))*((h08_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[92] += ((h09_CH1[i]*multnum - input1[i]*multnum))*((h09_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[93] += ((h10_CH1[i]*multnum - input1[i]*multnum))*((h10_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[94] += ((h11_CH1[i]*multnum - input1[i]*multnum))*((h11_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[95] += ((h12_CH1[i]*multnum - input1[i]*multnum))*((h12_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[96] += ((i01_CH1[i]*multnum - input1[i]*multnum))*((i01_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[97] += ((i02_CH1[i]*multnum - input1[i]*multnum))*((i02_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[98] += ((i03_CH1[i]*multnum - input1[i]*multnum))*((i03_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[99] += ((i04_CH1[i]*multnum - input1[i]*multnum))*((i04_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[100] += ((i05_CH1[i]*multnum - input1[i]*multnum))*((i05_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[101] += ((i06_CH1[i]*multnum - input1[i]*multnum))*((i06_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[102] += ((i07_CH1[i]*multnum - input1[i]*multnum))*((i07_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[103] += ((i08_CH1[i]*multnum - input1[i]*multnum))*((i08_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[104] += ((i09_CH1[i]*multnum - input1[i]*multnum))*((i09_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[105] += ((i10_CH1[i]*multnum - input1[i]*multnum))*((i10_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[106] += ((i11_CH1[i]*multnum - input1[i]*multnum))*((i11_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[107] += ((i12_CH1[i]*multnum - input1[i]*multnum))*((i12_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[108] += ((j01_CH1[i]*multnum - input1[i]*multnum))*((j01_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[109] += ((j02_CH1[i]*multnum - input1[i]*multnum))*((j02_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[110] += ((j03_CH1[i]*multnum - input1[i]*multnum))*((j03_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[111] += ((j04_CH1[i]*multnum - input1[i]*multnum))*((j04_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[112] += ((j05_CH1[i]*multnum - input1[i]*multnum))*((j05_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[113] += ((j06_CH1[i]*multnum - input1[i]*multnum))*((j06_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[114] += ((j07_CH1[i]*multnum - input1[i]*multnum))*((j07_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[115] += ((j08_CH1[i]*multnum - input1[i]*multnum))*((j08_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[116] += ((j09_CH1[i]*multnum - input1[i]*multnum))*((j09_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[117] += ((j10_CH1[i]*multnum - input1[i]*multnum))*((j10_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[118] += ((j11_CH1[i]*multnum - input1[i]*multnum))*((j11_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[119] += ((j12_CH1[i]*multnum - input1[i]*multnum))*((j12_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[120] += ((k01_CH1[i]*multnum - input1[i]*multnum))*((k01_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[121] += ((k02_CH1[i]*multnum - input1[i]*multnum))*((k02_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[122] += ((k03_CH1[i]*multnum - input1[i]*multnum))*((k03_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[123] += ((k04_CH1[i]*multnum - input1[i]*multnum))*((k04_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[124] += ((k05_CH1[i]*multnum - input1[i]*multnum))*((k05_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[125] += ((k06_CH1[i]*multnum - input1[i]*multnum))*((k06_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[126] += ((k07_CH1[i]*multnum - input1[i]*multnum))*((k07_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[127] += ((k08_CH1[i]*multnum - input1[i]*multnum))*((k08_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[128] += ((k09_CH1[i]*multnum - input1[i]*multnum))*((k09_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[129] += ((k10_CH1[i]*multnum - input1[i]*multnum))*((k10_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[130] += ((k11_CH1[i]*multnum - input1[i]*multnum))*((k11_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[131] += ((k12_CH1[i]*multnum - input1[i]*multnum))*((k12_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[132] += ((l01_CH1[i]*multnum - input1[i]*multnum))*((l01_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[133] += ((l02_CH1[i]*multnum - input1[i]*multnum))*((l02_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[134] += ((l03_CH1[i]*multnum - input1[i]*multnum))*((l03_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[135] += ((l04_CH1[i]*multnum - input1[i]*multnum))*((l04_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[136] += ((l05_CH1[i]*multnum - input1[i]*multnum))*((l05_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[137] += ((l06_CH1[i]*multnum - input1[i]*multnum))*((l06_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[138] += ((l07_CH1[i]*multnum - input1[i]*multnum))*((l07_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[139] += ((l08_CH1[i]*multnum - input1[i]*multnum))*((l08_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[140] += ((l09_CH1[i]*multnum - input1[i]*multnum))*((l09_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[141] += ((l10_CH1[i]*multnum - input1[i]*multnum))*((l10_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[142] += ((l11_CH1[i]*multnum - input1[i]*multnum))*((l11_CH1[i]*multnum - input1[i]*multnum)) ;
		NormPos1Data[143] += ((l12_CH1[i]*multnum - input1[i]*multnum))*((l12_CH1[i]*multnum - input1[i]*multnum)) ;
	}
}