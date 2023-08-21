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
#include "User_function.h"
#include "AD7606C.h"
#include "ad9959.h"
// #include  "stdio.h"
#include "fft_user.h"
#include "correlation.h"
// #include	"diagram1.h"
#include "W9825G6KH.h"
#include "W25Q256.h"
#include "stdlib.h"
#include "Appuser.h"
//#include "arm_math.h"
//#include "arm_const_structs.h"
uint8_t uartuse = 1;

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

#define FREQSTEP 500U
#define FREQTERM 20000U
#define AVENUM 10


#define Pos_Start_ADDRESS 1024 * 1024 + 2000

typedef enum
{
	DDS_STATE = 0U,
	NEG_MEASURE_STATE = 1U,
	POS_MEASURE_STATE = 2U,
	Calibration_STATE = 3U,
	POS_Storage_STATE = 4U,
	POS_MODE4_STATE = 5U
} System_State;

extern DMA_HandleTypeDef hdma_tim4_up;
struct W25Q256_Handler *qspi_handler;
uint32_t *pp = (uint32_t *)0xc0000000;

uint8_t buf1[100];
uint8_t SAMP_DIV = 1;
uint8_t re;
uint32_t start_address = 1000;
uint32_t find_address = 1000;
uint8_t AVE_NUM;

uint8_t sapa = 4;

uint8_t calibration_num = 0;
uint16_t Pos_Storage_Num = 0;

//uint8_t Knock_flag = 0;
uint8_t dds_stopflag = 0;
uint8_t pr_flag[4] = {0}; //  信号检测
uint16_t loc[4];
uint16_t chf;

uint8_t set_rand = 0; // rand

uint16_t AD7606C_ID = 0;

//uint16_t start;
//uint16_t stop;
int cnt = 0;
int cnt_ave = 0;
float percent[3] = {0, 0, 0};

float AD_Vpp[4][100];

float AD_Vpp_All[300];
float Neg_res[36];

float AD_Max[4][100];
float AD_Min[4][100];

// float	AD_AVE[4][39];

float AD_AVEM4[4][39];
float AD_AVEM4_Load[4][100];
float Mode4_Res[625];


float xcorr_out_test[4][AD7606C_SAMP_SIZE * 2 - 1];

uint16_t row[100];
uint16_t freq = 1000;

uint16_t row_neg;
float min_neg;

float norm_DE = 0;
float norm_EF = 0;
//float rand1[144] = {0.160976655755070, -0.520604730953458, 0.326319063310178, -0.225039635928461, -0.147861635649593, 0.163073632522137, -0.213615245853532, -0.219048379292065, 0.552925990685507, 0.333715505625190, 0.278853447213056, 0.194900141778310, 0.124574816131553, 0.464599683890306, -0.485950733521331, 0.167540211944786, -0.465314098930398, 0.181547696961576, 0.290705020285643, -0.0162185831787848, 0.543465098829006, 0.264412731967905, -0.143552738518692, -0.0241559591694313, -0.368113342788866, -0.430106993502625, 0.0958306033076811, -0.157137594211030, 0.327901257364306, 0.205934887282754, -0.513904964956917, 0.349464235172157, -0.432260462277813, 0.0692804400826619, 0.349011635410958, -0.264975656378834, -0.0431191722080995, -0.0294693404326436, -0.408491588688838, -0.00513990097012616, -0.308702819852713, 0.213996552729109, -0.164868631495447, -0.0724558361073091, -0.0280020882703580, -0.0200764181824692, -0.0693443823533409, -0.461140455397403, -0.222537181605306, -0.0666109373096358, 0.00626424077457748, 0.507145440732709, -0.251375942018045, 0.126993403232777, 0.511955642125458, 0.470276753077482, 0.0379525469907419, 0.321831141946531, 0.543791645927711, 0.815581962708073, -0.200403091320237, 0.157521822095492, -0.143045800514707, 0.180810246655662, -0.579105521184939, -0.634772385060023, -0.134789956152279, -0.300829379641542, -0.258652658437896, -0.139714586503819, 0.264642474894089, -0.341495167930982, 0.560953613005033, 0.117228535648999, -0.375382685838098, 0.377918444853715, 0.0427879422702603, -0.195823166558131, -0.178443318225154, -0.200931832027836, 0.116228841514265, 0.103664456046134, -0.0596395657820786, -0.137225374398490, 0.207885710047900, 0.395518352120055, -0.0814490941811690, -0.285285061634354, 0.107784833299498, -0.125505496504051, 0.157284767795404, 0.0853711168327881, -0.106694448980719, -0.446296817469148, -0.264865150724529, 0.324883484153220, -0.231009029987021, -0.0341322706498821, 0.192905441363682, -0.237881497298489, -0.494793018673956, 0.0300043622524817, -0.255169525766090, -0.553817754247909, -0.185400180144657, 0.215601194652850, -0.103599546440781, 0.0558744285248884, -0.00555759697040476, 0.436430120322175, 0.138684702375012, 0.166885961134659, -0.211692367541281, -0.0489035740024796, 0.212541797535819, -0.0671062646968827, -0.0496701693219067, -0.000581015968891084, -0.184723688831686, -0.296307456820374, 0.141849644047339, 0.0555646705028001, -0.438595459001381, 0.0411859745447155, 0.0667677728855881, -0.390225985416616, 0.279407706853213, 0.160456703230784, 0.0165960745859438, 0.188380772561121, -0.448917442357877, -0.283406314692052, -0.202186044439852, 0.357105109263449, -0.442670149868157, 0.278814976326815, -0.116600031793738, 0.365324146292208, 0.0711989848539124, -0.0465315286800240, 0.189597949999802, -0.388569234536321, 0.275235096895503, -0.0243178461809929};
//float rand2[144] = {0.232911466283750, 0.0706630450621788, 0.0793831745139245, 0.118641534225885, -0.362826807473442, 0.0275571452181513, -0.308770525857684, 0.0980327131531238, 0.309327347727636, 0.171616487094237, -0.0347927992852863, -0.236862704987593, -0.389617442095842, 0.0541691270180376, 0.504037151289563, 0.0658385124224895, 0.00896424915055951, 0.201578751013863, -0.0585781123593085, 0.176728974161242, -0.0484194460984841, 0.280772476768975, -0.0786429271368477, -0.176177872584427, 0.577770958100275, -0.214028489864964, -0.0109776857668243, 0.102132674456745, -0.0445092294174467, 0.0167028306775199, -0.0231196786492470, -0.694911870122705, -0.152836187904074, -0.0447472795094766, -0.221127424116869, 0.166513892401393, -0.463756748463660, -0.114999236406639, -0.181414274904493, 0.233288498065786, 0.204410106223197, 0.100019671662933, 0.0512739408589992, -0.277001059166711, 0.0880736700165148, 0.161213092881603, 0.269252694860418, -0.0709952633935867, 0.380269171598826, 0.383569103580099, 0.281043045836320, -0.226540470510289, 0.127949693042318, -0.367280375254305, 0.161898476103896, 0.0536786985338702, -0.0860291780566798, -0.195346906299408, -0.148662136599722, -0.517250320056107, -0.393901302222796, -0.359167032862313, -0.173285009290860, 0.107167422954503, -0.554862053398222, -0.0145683595490361, -0.452372932078574, 0.0108718585748639, 0.0477563445728062, 0.173986634143509, -0.0349430626460591, 0.0861710711394893, 0.264411683164560, 0.128126444259178, 0.177432559136418, -0.268065658406633, 0.0643373565705562, -0.00741142097348259, -0.375834681421643, 0.449598283655236, -0.305528151278894, -0.0319791387508182, 0.499447020926957, 0.121331073343040, -0.0843193062957126, 0.286837480345690, 0.453418518548493, -0.0799736695156579, 0.435453423517739, 0.272550992108683, 0.419183639388471, 0.341522209070719, 0.100826407235729, -0.363430405748687, 0.269101084510345, 0.179009715372816, -0.232877901234858, 0.148516635760547, -0.0360061349348813, 0.155949426886147, 0.0431711152270501, 0.409728177709171, -0.457687737068870, 0.236476941341919, 0.141452027995363, -0.0669904541332812, -0.330176176787054, 0.198776531198691, -0.430994325466751, 0.282309451491026, 0.169315076295826, -0.429090864372014, 0.0347856739386962, 0.194691905633071, -0.0348128129727053, -0.327553341046198, 0.291917990751492, 0.401492716912427, -0.0845991865184081, 0.146899505955996, 0.157111000086249, 0.628596855876707, 0.262394801623384, -0.0650526751435875, 0.00631182500618452, 0.0647636872964096, 0.506868466552006, 0.394761743661081, 0.320883043123287, 0.101898129727530, 0.316149007023209, -0.571018668519195, -0.269657132421425, 0.0859816854721235, 0.0121203732322607, 0.341338578229507, -0.0234332669523810, 0.639297751046884, -0.428547177043670, 0.128810983892074, 0.132309884828090, 0.00954336986967278, 0.130194094603632, 0.479538716872654};

const float BoardNullV[3][39] = {{0.0425724000000000, 0.243656200000000, 0.210330800000000, 0.333042700000000, 0.446967700000000, 0.385197800000000, 0.576546500000000, 0.682443100000000, 0.714731900000000, 0.883895100000000, 0.851819000000000, 1.02763380000000, 1.05357410000000, 1.18733560000000, 1.42375890000000, 1.18287840000000, 1.17268510000000, 0.933883300000000, 1.00376900000000, 0.840344200000000, 0.683755500000000, 0.581124100000000, 0.714426200000000, 0.402685400000000, 0.437720800000000, 0.310429200000000, 0.0874339000000000, 0.140658600000000, 0.190829800000000, 0.204746000000000, 0.112246400000000, 0.120852400000000, 0.277897400000000, 0.181888200000000, 0.139437800000000, 0.192203000000000, 0.161319200000000, 0.170749100000000, 0.060852600000000},
								 {0.0338139000000000, 0.161136000000000, 0.216220600000000, 0.149142500000000, 0.164157300000000, 0.246708000000000, 0.471900800000000, 0.312931700000000, 0.503059400000000, 0.949813600000000, 0.229312800000000, 0.28363450000000, 0.50553140000000, 0.19671990000000, 0.51346620000000, 0.35672470000000, 0.14126900000000, 0.229679000000000, 0.68052060000000, 0.553017200000000, 0.543190400000000, 0.505012600000000, 0.389928000000000, 0.384740000000000, 0.384892500000000, 0.604470100000000, 0.6716399000000000, 0.816296000000000, 0.594002700000000, 0.525062700000000, 0.521400900000000, 0.570870200000000, 0.609200300000000, 0.441138900000000, 0.362034800000000, 0.170413400000000, 0.116732700000000, 0.087128600000000, 0.184268400000000},
								 {0.0348820000000000, 0.226260900000000, 0.166141000000000, 0.320255900000000, 0.438788900000000, 0.327976900000000, 0.484779500000000, 0.640511800000000, 0.597328900000000, 0.749767400000000, 0.766979400000000, 0.90397590000000, 1.07753070000000, 1.09590230000000, 1.41356610000000, 1.11717310000000, 1.04771470000000, 0.826824900000000, 0.97840870000000, 0.946761600000000, 0.766277600000000, 0.597970000000000, 0.782421500000000, 0.446479600000000, 0.454749800000000, 0.323368700000000, 0.1260710000000000, 0.152713300000000, 0.190127900000000, 0.248295100000000, 0.143618800000000, 0.168033200000000, 0.279301000000000, 0.172061200000000, 0.128482000000000, 0.200595700000000, 0.181796600000000, 0.196811400000000, 0.063751900000000}};
const float DE67[39] = {0.863645, 1.114954, 1.259533, 1.148411, 0.960185, 1.148188, 1.088326, 1.087027, 1.068636, 0.812540, 0.999215, 1.048665, 1.035210, 0.969082, 0.971629, 1.017007, 0.959958, 1.075927, 1.082907, 0.970763, 0.974704, 0.958110, 0.877452, 0.716881, 1.196202, 1.169052, 0.610813, 0.799758, 1.084829, 1.013578, 1.342909, 0.698162, 0.868172, 1.019394, 1.110615, 1.121763, 0.977290, 0.972727, 1.081039};
const float EF56[39] = {0.874692, 0.875905, 1.187351, 1.167922, 0.834694, 0.951807, 1.101455, 1.131218, 1.215005, 0.893347, 1.237384, 1.130334, 1.007245, 1.231260, 0.993239, 0.972238, 0.943150, 0.895274, 0.930410, 0.835978, 0.902525, 0.877789, 1.004253, 1.014169, 1.090478, 0.950124, 0.446461, 0.705193, 0.877010, 1.196430, 1.530063, 0.850438, 0.882963, 0.937510, 0.963484, 1.082755, 0.950943, 0.994820, 0.846634};

System_State oystate;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
void	TestPosR4()
{
	float	temp[3];
	float min_M4,min1;
	uint16_t row_M4, row1;
	if (oystate == POS_MODE4_STATE)
	{
		find_address = Pos_Start_ADDRESS;
		for (int j = 0; j < 625; j++) //		need to change max j
		{
			for (int i = 0; i < 3; i++)
			{
				W25Q256_read(qspi_handler, find_address, (uint8_t *)AD_AVEM4_Load[i], POSOFFSET);
				find_address += POSOFFSET;
			}
			OY_Norm(AD_AVEM4_Load[0], AD_AVE[0], 39, &temp[0], (float)100);
			OY_Norm(AD_AVEM4_Load[1], AD_AVE[1], 39, &temp[1], (float)100);
			OY_Norm(AD_AVEM4_Load[2], AD_AVE[2], 39, &temp[2], (float)100);
			Mode4_Res[j] = temp[0] + temp[1] + temp[2];
		}

		OY_FindMin(Mode4_Res, 625, &min_M4, &row_M4);
		row1 = ListForRequest4.PositionFind(&min1);
		
		if(min_M4 > min1)
		{
			sprintf((char *)buf1, "t0.txt=\"%d pos\"",row1 + 626);
			HMISends(buf1);
			HMISend(0xff);
		}else
		{
			sprintf((char *)buf1, "t0.txt=\"%d pos\"",row_M4 + 1);
			HMISends(buf1);
			HMISend(0xff);
		}
		
//		uint16_t num;
//		uint8_t letter;
//		if ((row_M4 + 1) % 25 == 0)
//		{
//			num = 25;
//			letter = (row_M4 + 1) / 25;
//		}
//		else
//		{
//			num = (row_M4 + 1) % 25;
//			letter = (row_M4 + 1) / 25 + 1;
//		}
//		sprintf((char *)buf1, "t0.txt=\"(%.3fmm,%.3fmm)\"", (float)(letter - 13) * 12.5f, (float)(num - 13.f) * (-12.5f));
//		HMISends(buf1);
//		HMISend(0xff);
	}
}



// 初始化主动测量
// 修改采样频率至250k
void Pos_Init(void)
{
	SAMP_DIV = 1;
	HAL_TIM_Base_Stop(&htim4);
	HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_1);
	__HAL_TIM_SET_CLOCKDIVISION(&htim4, 1);
	AVE_NUM = 0;
	for (int i = 0; i < 117; i++)
		AD_Vpp_All[i] = 0;
}

//  初始化被动测量
//  修改采样频率至80k
void Neg_Init(void)
{
	SAMP_DIV = 1;
	HAL_TIM_Base_Stop(&htim4);
	HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_1);
	__HAL_TIM_SET_CLOCKDIVISION(&htim4, 3);
	start_address = 1000;
	find_address = 1000;
}

void Pos_Storage_Init(void)
{
	Pos_Init();
	int k = 0;
	start_address = 1024 * 1024 + 2000 + 39 * 4 * 3 * 25 * k;
	Pos_Storage_Num = 0;
}

void Pos_Storage(void)
{
	ListForRequest4.ListForPos_GetData(5);
	ListForRequest4.WavePrint(AD_AVE, 39);
	ListForRequest4.ListForPos_KeyDetect();
}

void Pos_Test(void)
{
	float	min1;
	ListForRequest4.ListForPos_GetData(5);
	ListForRequest4.WavePrint(AD_AVE, 39);
//	min1 = ListForRequest4.PositionFind();
	TestPosR4();
}

void	Neg_Storage()
{
	ListForRequest2.ListForPos_GetData();
	ListForRequest2.ListForPos_KeyDetect();
}

void	Neg_Test()
{
	ListForRequest2.ListForPos_GetData();
	ListForRequest2.PositionFind();
}




void PosM4_Test(void)
{
	float temp[3];
	float min_M4;
	uint16_t row_M4;
	float *vp[4];
	for (int k = 0; k < 3; k++)
	{
		vp[k] = volt[k];
		for (uint16_t i = 0; i < 39; i++)
		{
			AD_AVEM4[k][i] = 0;
		}
	}

	for (int k = 0; k < 5; k++)
	{
		freq = 4800;
		for (uint16_t i = 0; i < 39; i++)
		{
			ad9959_write_frequency(AD9959_CHANNEL_2, freq);
			ad9959_io_update();
			HAL_Delay(10);
			AD7606C_SAMP_Start(data, AD7606C_SAMP_SIZE);
			AD7606C_BitToFloat(data, vp, AD7606C_SAMP_SIZE);
			OY_FindMaxMin(volt[0], AD7606C_SAMP_SIZE, &AD_Max[0][i], &AD_Min[0][i], &AD_Vpp[0][i], row);
			OY_FindMaxMin(volt[1], AD7606C_SAMP_SIZE, &AD_Max[1][i], &AD_Min[1][i], &AD_Vpp[1][i], row);
			OY_FindMaxMin(volt[2], AD7606C_SAMP_SIZE, &AD_Max[2][i], &AD_Min[2][i], &AD_Vpp[2][i], row);
			// OY_FindMaxMin(volt[3],AD7606C_SAMP_SIZE,&AD_Max[3][i],&AD_Min[3][i],&AD_Vpp[3][i],row);
			if (freq == FREQTERM)
			{
				freq = 4800;
				dds_stopflag = 1;
			}
			else
			{
				freq = freq + 400;
			}
		}
		for (uint16_t i = 0; i < 3; i++)
		{
			for (int j = 0; j < 39; j++)
			{
				AD_AVEM4[i][j] += AD_Vpp[i][j] / 5.f;
			}
		}
	}

	uartuse = 1;
	//		for(int i = 0; i < 39;i++)
	//		printf("%f,%f,%f\n",AD_AVEM4[0][i],AD_AVEM4[1][i],AD_AVEM4[2][i]);
	if (oystate == POS_MODE4_STATE)
	{
		find_address = Pos_Start_ADDRESS;
		for (int j = 0; j < 625; j++) //		need to change max j
		{
			for (int i = 0; i < 3; i++)
			{
				W25Q256_read(qspi_handler, find_address, (uint8_t *)AD_AVEM4_Load[i], POSOFFSET);
				find_address += POSOFFSET;
			}
			OY_Norm(AD_AVEM4_Load[0], AD_AVEM4[0], 39, &temp[0], (float)100);
			OY_Norm(AD_AVEM4_Load[1], AD_AVEM4[1], 39, &temp[1], (float)100);
			OY_Norm(AD_AVEM4_Load[2], AD_AVEM4[2], 39, &temp[2], (float)100);
			Mode4_Res[j] = temp[0] + temp[1] + temp[2];
		}

		OY_FindMin(Mode4_Res, 625, &min_M4, &row_M4);
		//			sprintf((char*)buf1,"t0.txt=\"%d\"",row_M4 + 1);
		//			HMISends(buf1);
		//			HMISend(0xff);

		uint16_t num;
		uint8_t letter;
		if ((row_M4 + 1) % 25 == 0)
		{
			num = 25;
			letter = (row_M4 + 1) / 25;
		}
		else
		{
			num = (row_M4 + 1) % 25;
			letter = (row_M4 + 1) / 25 + 1;
		}
		// uint16_t randk = rand()%111+1;

		sprintf((char *)buf1, "t0.txt=\"(%.3fmm,%.3fmm)\"", (float)(letter - 13) * 12.5f, (float)(num - 13.f) * (-12.5f));

//		if (set_rand < 144)
//			set_rand++;
//		else if (set_rand > 144 && set_rand == 144)
//			set_rand = 0;
//		else
//			set_rand = 0;
		//			if
		//			if(row_M4 > 1)
		//				sprintf((char*)buf1,"t0.txt=\"(%.3fmm,%.3fmm)\"",(float)(letter-13)*12.5 + rand1[row_M4],(float)(num-13)*(-12.5) + rand1[row_M4 - 1]);
		//			else
		//				sprintf((char*)buf1,"t0.txt=\"(%.3fmm,%.3fmm)\"",(float)(letter-13)*12.5 + rand1[row_M4],(float)(num-13)*(-12.5) + rand1[row_M4]);
		//
		//			sprintf((char*)buf1,"t0.txt=\"(%fmm,%fmm)\"",rand1[row_M4],rand2[row_M4]);
		HMISends(buf1);
		HMISend(0xff);
	}
}

void Signal_Samp_Pos(void)
{
	uint16_t row_p;
	float min_p;
	freq = 1000;
	float *vp[4];
	for (int i = 0; i < 4; i++)
		vp[i] = volt[i];
	for (uint16_t i = 0; i < 39; i++)
	{
		ad9959_write_frequency(AD9959_CHANNEL_2, freq);
		ad9959_io_update();
		HAL_Delay(10);
		AD7606C_SAMP_Start(data, AD7606C_SAMP_SIZE);
		AD7606C_BitToFloat(data, vp, AD7606C_SAMP_SIZE);
		OY_FindMaxMin(volt[0], AD7606C_SAMP_SIZE, &AD_Max[0][i], &AD_Min[0][i], &AD_Vpp[0][i], row);
		OY_FindMaxMin(volt[1], AD7606C_SAMP_SIZE, &AD_Max[1][i], &AD_Min[1][i], &AD_Vpp[1][i], row);
		OY_FindMaxMin(volt[2], AD7606C_SAMP_SIZE, &AD_Max[2][i], &AD_Min[2][i], &AD_Vpp[2][i], row);
		OY_FindMaxMin(volt[3], AD7606C_SAMP_SIZE, &AD_Max[3][i], &AD_Min[3][i], &AD_Vpp[3][i], row);

		if (freq == FREQTERM)
		{
			freq = 1000;
			dds_stopflag = 1;
		}
		else
		{
			freq = freq + FREQSTEP;
		}
	}
	//			for(int i = 0;i < 39; i++)
	//			{
	//					printf("%f,%f,%f\n",AD_Vpp[0][i],AD_Vpp[1][i],AD_Vpp[2][i]);
	//			}

	for (int j = 0; j < 3; j++)
	{
		for (int i = 0; i < 39; i++)
		{
			AD_Vpp_All[i + 39 * j] = AD_Vpp[j][i];
		}
	}

	NormForPos1(AD_Vpp_All, 100000.f);
	OY_FindMin(NormPos1Data, 144, &min_p, &row_p);
	//			printf("%d\r\n",row_p + 1);
	//			sprintf((char*)buf1,"t1.txt=\"at %d cell\"",row_p + 1);
	//			HMISends(buf1);
	//			HMISend(0xff);
	// CoorTransMode3(row_p + 1);
	uint16_t num;
	uint8_t letter;
	if ((row_p + 1) % 12 == 0)
	{
		num = 12;
		letter = (row_p + 1) / 12;
	}
	else
	{
		num = (row_p + 1) % 12;
		letter = (row_p + 1) / 12 + 1;
	}
	if (num < 10)
		sprintf((char *)buf1, "t0.txt=\"(%c,0%d)\"", 'A' + (char)(letter - 1), num);
	else
		sprintf((char *)buf1, "t0.txt=\"(%c,%d)\"", 'A' + (char)(letter - 1), num);
	//				HMISends(buf1);
	//				HMISend(0xff);
}

void Signal_Samp_Neg(void)
{
	//	float	corr_resmax[6];
	//	uint16_t	corr_resrow[6];
	float *vp[4];
	for (int i = 0; i < 4; i++)
		vp[i] = VoltNeg[i];
	while (!Knock_flag)
	{
		AD7606C_SAMP_Start(DataNeg, 4 * AD7606C_SAMP_SIZE);
		VoltJudge();
	}

	if (Knock_flag)
	{
		AD7606C_BitToFloat(DataNeg, vp, 4 * AD7606C_SAMP_SIZE);
		arm_correlate_f32(&VoltNeg[0][start], 256, &VoltNeg[1][start], 256, xcorr_out[0]);
		arm_correlate_f32(&VoltNeg[0][start], 256, &VoltNeg[2][start], 256, xcorr_out[1]);
		arm_correlate_f32(&VoltNeg[1][start], 256, &VoltNeg[2][start], 256, xcorr_out[2]);
		arm_correlate_f32(&VoltNeg[2][start], 256, &VoltNeg[3][start], 256, xcorr_out[3]);
		OY_Normalization(xcorr_out[0], 511);
		OY_Normalization(xcorr_out[1], 511);
		OY_Normalization(xcorr_out[2], 511);
		OY_Normalization(xcorr_out[3], 511);

		//		OY_FindMax(xcorr_out[0],2*256-1,&corr_resmax[0],&corr_resrow[0]);
		//		OY_FindMax(xcorr_out[1],2*256-1,&corr_resmax[1],&corr_resrow[1]);
		//		OY_FindMax(xcorr_out[2],2*256-1,&corr_resmax[2],&corr_resrow[2]);
		//		OY_FindMax(xcorr_out[3],2*256-1,&corr_resmax[3],&corr_resrow[3]);

		for (uint16_t j = start - 50; j <= stop + 50; j++)
		{
			printf("%f,%f,%f,%f\n",VoltNeg[0][j],VoltNeg[1][j],VoltNeg[2][j],VoltNeg[3][j]);
		}
		uartuse = 1;
		//		printf("\r\n****************************\r\n");
		//		for (uint16_t j = 0; j < 511; j++)
		//		{
		//			printf("%f,%f,%f,%f\n",xcorr_out[0][j],xcorr_out[1][j],xcorr_out[2][j],xcorr_out[3][j]);
		//		}
		//		printf("\r\n****************************\r\n");
		//
		if (oystate == Calibration_STATE)
		{
			
			while (sapa == 4)
			{
				if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_6) == GPIO_PIN_RESET)
					sapa = 2;

				if (HAL_GPIO_ReadPin(GPIOH, GPIO_PIN_11) == GPIO_PIN_RESET)
					sapa = 3;
			};
			if (sapa == 2)
			{
				calibration_num++;
				if (calibration_num == 37)
				{
					sprintf((char *)buf1, "t0.txt=\"Calibration reload\"");
					HMISends(buf1);
					HMISend(0xff);
					calibration_num = 1;
					start_address = 1000;
				}
				find_address = start_address;
				for (int i = 0; i < 4; i++)
				{
					W25Q256_write(qspi_handler, start_address, (uint8_t *)xcorr_out[i], XCORRSIZE);
					start_address = start_address + XCORRSIZE;
				}
				sprintf((char *)buf1, "t0.txt=\"Save %d data Suceessfully\"", calibration_num);
				HMISends(buf1);
				HMISend(0xff);
				sprintf((char *)buf1, "t1.txt=\"Saved %d data\"", calibration_num);
				HMISends(buf1);
				HMISend(0xff);
				sapa = 4;

				for (int i = 0; i < 4; i++)
				{
					W25Q256_read(qspi_handler, find_address, (uint8_t *)xcorr_out_test[i], XCORRSIZE);
					find_address += XCORRSIZE;
				}

				for (uint16_t j = 0; j < 511; j++)
				{
					printf("%f,%f,%f,%f\n", xcorr_out_test[0][j], xcorr_out_test[1][j], xcorr_out_test[2][j], xcorr_out_test[3][j]);
				}
			}
			else if (sapa == 3)
			{
				sprintf((char *)buf1, "t0.txt=\"Skip Suceessfully\"");
				HMISends(buf1);
				HMISend(0xff);
				sapa = 4;
			}
		}
		else if (oystate == NEG_MEASURE_STATE)
		{
			find_address = 1000;
			for (int j = 0; j < 36; j++)
			{
				for (int i = 0; i < 4; i++)
				{
					W25Q256_read(qspi_handler, find_address, (uint8_t *)xcorr_out_test[i], XCORRSIZE);
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
				for (int k = 0; k < 511; k++)
				{
					Neg_res[j] += (xcorr_out_test[0][k] - xcorr_out[0][k]) * (xcorr_out_test[0][k] - xcorr_out[0][k]);
					Neg_res[j] += (xcorr_out_test[1][k] - xcorr_out[1][k]) * (xcorr_out_test[1][k] - xcorr_out[1][k]);
					Neg_res[j] += (xcorr_out_test[2][k] - xcorr_out[2][k]) * (xcorr_out_test[2][k] - xcorr_out[2][k]);
					Neg_res[j] += (xcorr_out_test[3][k] - xcorr_out[3][k]) * (xcorr_out_test[3][k] - xcorr_out[3][k]);
				}
			}

			OY_FindMin(Neg_res, 36, &min_neg, &row_neg);
			//			sprintf((char*)buf1,"t1.txt=\"at %dth cell\"",row_neg+1);
			//			HMISends(buf1);
			//			HMISend(0xff);

			uint16_t num;
			uint8_t letter;
			if ((row_neg + 1) % 6 == 0)
			{
				num = 6;
				letter = (row_neg + 1) / 6;
			}
			else
			{
				num = (row_neg + 1) % 6;
				letter = (row_neg + 1) / 6 + 1;
			}
			if (num < 5)
				sprintf((char *)buf1, "t0.txt=\"(%c%c,0%d0%d)\"", 'A' + (char)(letter - 1) * 2, 'A' + (char)(letter)*2 - 1, num * 2 - 1, num * 2);
			else if (num == 5)
				sprintf((char *)buf1, "t0.txt=\"(%c%c,0%d%d)\"", 'A' + (char)(letter - 1) * 2, 'A' + (char)(letter)*2 - 1, num * 2 - 1, num * 2);
			else
				sprintf((char *)buf1, "t0.txt=\"(%c%c,%d%d)\"", 'A' + (char)(letter - 1) * 2, 'A' + (char)(letter)*2 - 1, num * 2 - 1, num * 2);
			HMISends(buf1);
			HMISend(0xff);

			//			sprintf((char*)buf1,"t1.txt=\"%f\"",min_neg);
			//			HMISends(buf1);
			//			HMISend(0xff);
		}

		Knock_flag = 0;
	}
}

void Signal_Process(void)
{
}

void Volt_Freq_Print(void)
{
	//  取10次平均发送3个通道值
	if (cnt_ave == 0)
	{
		for (int k = 0; k < 3; k++)
		{
			for (uint16_t i = 0; i < 39; i++)
			{
				AD_AVE[k][i] = 0;
			}
		}
	}

	for (int k = 0; k < 3; k++)
	{
		for (uint16_t i = 0; i < 39; i++)
		{
			AD_AVE[k][i] += AD_Vpp[k][i];
		}
	}
	if (cnt_ave == AVENUM - 1)
	{
		for (int k = 0; k < 3; k++)
		{
			for (uint16_t i = 0; i < 39; i++)
			{
				AD_AVE[k][i] = AD_AVE[k][i] / AVENUM;
			}
			printf("CH%d=[", k + 1);
			for (uint16_t i = 0; i < 39; i++)
			{
				printf("%f, ", AD_AVE[k][i]);
			}
			printf("]\r\n");
			// printf("\r\n********\r\n");
		}
		cnt_ave = 0;
	}
	cnt_ave++;
}

void Signal_Print(void)
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
	ad9959_write_amplitude(AD9959_CHANNEL_2, 1023);
	ad9959_io_update();
	printf("Hello World!\n");
	AD7606C_ID = AD7606C_Init(p, SINGLE_10V, HighBand, OverSamp_off);
	freq = 1000;

	//	W9825G6KH_clear(W9825G6KH_initHardware(&hsdram1,(uint32_t *)0xC0000000));
	qspi_handler = W25Q256_initHardware(&hqspi, (uint32_t *)0);
	// HAL_UART_Receive_IT(&huart3,&re,1);

	oystate = POS_MEASURE_STATE;

	//	sprintf((char*)buf1,"t0.txt=\"lzfssb\"");
	//	HMISends(buf1);
	//	HMISend(0xff);

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{
		// HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_7);

		//		if(HAL_GPIO_ReadPin(GPIOH,GPIO_PIN_2) == GPIO_PIN_SET)
		//			HAL_GPIO_WritePin(GPIOH,GPIO_PIN_10,GPIO_PIN_SET);
		//		Signal_Samp_Pos();
		//    Signal_Print();

//		if ((GPIOC->IDR & GPIO_PIN_9) == GPIO_PIN_RESET)
//		{
//			oystate = DDS_STATE;
//			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
//			HAL_GPIO_WritePin(GPIOH, GPIO_PIN_14, GPIO_PIN_SET);
//			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
//			HAL_GPIO_WritePin(GPIOH, GPIO_PIN_9, GPIO_PIN_RESET);
//			HAL_GPIO_WritePin(GPIOH, GPIO_PIN_10, GPIO_PIN_RESET);
//			sprintf((char *)buf1, "t1.txt=\"Basic Requests\"");
//			HMISends(buf1);
//			HMISend(0xff);
//		}
//		if ((GPIOB->IDR & GPIO_PIN_5) == GPIO_PIN_RESET)
//		{
//			oystate = NEG_MEASURE_STATE;
//			Neg_Init();
//			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
//			HAL_GPIO_WritePin(GPIOH, GPIO_PIN_14, GPIO_PIN_RESET);
//			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
//			HAL_GPIO_WritePin(GPIOH, GPIO_PIN_9, GPIO_PIN_RESET);
//			HAL_GPIO_WritePin(GPIOH, GPIO_PIN_10, GPIO_PIN_RESET);
//			sprintf((char *)buf1, "t1.txt=\"Basic Requests\"");
//			HMISends(buf1);
//			HMISend(0xff);
//		}
//		if ((GPIOA->IDR & GPIO_PIN_12) == GPIO_PIN_RESET)
//		{
//			oystate = POS_MEASURE_STATE;
//			Pos_Init();
//			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
//			HAL_GPIO_WritePin(GPIOH, GPIO_PIN_14, GPIO_PIN_RESET);
//			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
//			HAL_GPIO_WritePin(GPIOH, GPIO_PIN_9, GPIO_PIN_SET);
//			HAL_GPIO_WritePin(GPIOH, GPIO_PIN_10, GPIO_PIN_RESET);
//			sprintf((char *)buf1, "t1.txt=\"Indicated Requests\"");
//			HMISends(buf1);
//			HMISend(0xff);
//		}
//		if ((GPIOA->IDR & GPIO_PIN_11) == GPIO_PIN_RESET)
//		{
//			oystate = POS_MODE4_STATE;
//			Pos_Init();
//			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
//			HAL_GPIO_WritePin(GPIOH, GPIO_PIN_14, GPIO_PIN_RESET);
//			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
//			HAL_GPIO_WritePin(GPIOH, GPIO_PIN_9, GPIO_PIN_RESET);
//			HAL_GPIO_WritePin(GPIOH, GPIO_PIN_10, GPIO_PIN_SET);
//			sprintf((char *)buf1, "t1.txt=\"Indicated Requests\"");
//			HMISends(buf1);
//			HMISend(0xff);
//		}

		if (oystate == DDS_STATE)
		{
			freq = 15000;
			for (; freq <= 20000; freq = freq + 1000)
			{
				ad9959_write_frequency(AD9959_CHANNEL_2, freq);
				ad9959_io_update();
				HAL_Delay(1000);
			}
		}
		
		if (oystate == NEG_MEASURE_STATE )
		{
				Neg_Test();
		}
		
		if (oystate == Calibration_STATE)
		{
			//	Signal_Samp_Neg();
			Neg_Storage();
		}

		if (oystate == POS_Storage_STATE)
		{
			Pos_Init();
			CTRLCH4(1);
//			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
			Pos_Storage();
		}

		if (oystate == POS_MEASURE_STATE)
		{
			Pos_Init();
//			CTRLCH4(1);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
			Signal_Samp_Pos();
			HMISends(buf1);
			HMISend(0xff);
		}

		if (oystate == POS_MODE4_STATE)
		{
			Pos_Init();
			CTRLCH4(1);
			Pos_Test();
		}


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
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
	{
		Error_Handler();
	}
	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART1 | RCC_PERIPHCLK_USART2 | RCC_PERIPHCLK_USART3;
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
	HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_1);
	AD7606C_flag = 1;
}

void TIM4_IRQHandler(void)
{

	HAL_TIM_IRQHandler(&htim4);
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

#ifdef USE_FULL_ASSERT
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
