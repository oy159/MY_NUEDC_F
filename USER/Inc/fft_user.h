//
// Created by 86187 on 2022/10/15.
//

#ifndef FFT_USER_H
#define FFT_USER_H

/*
*file		fft_user.h
*author		Oycr
*e-mail
*version	0.1
*data		2022/10/15
*brief		fft相关运算
*/

#include "arm_math.h"
#include "arm_const_structs.h"




/**
 *
 * @param p1 :输入
 * @param n ：长度
 * @return ：
 */
uint32_t FFT(float32_t *p1, uint16_t n);

/**
 *
 * @param p1 :输入
 * @param n ：长度
 * @return
 */
uint32_t IFFT(float32_t *p1, uint16_t n);

/**
 *
 * @param p1 :输入
 * @param fft_outputbuf ：输出
 * @param n ：长度
 * @return ：最大值下标
 */
uint16_t CmplexMag(float32_t *p1, float32_t *fft_outputbuf, uint16_t n);


uint16_t OY_CmplexMag(float32_t *p1, float32_t *fft_outputbuf, uint16_t n, uint16_t *row, float32_t *max);
#endif //FFT_USER_H
