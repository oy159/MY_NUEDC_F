//
// Created by 86187 on 2022/10/25.
//

#ifndef APFFT_H
#define APFFT_H

#include "fft_user.h"
#include "windows_user.h"

#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))
#define abs(x) x>=0 ? x : -x

/****************************************************************
 * apfft注意事项，需要过采样，即做NPT个点的运算需要2*NPT-1个采样点。
 *
 */


void apFFT(float32_t *DataA,float32_t *OutDataA, int NPT);

#endif //APFFT_H
