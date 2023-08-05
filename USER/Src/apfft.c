//
// Created by 86187 on 2022/10/25.
//
#include "apfft.h"

void CreatApFFTSeries(const float32_t * DataBuffer,float32_t *OutputBuffer,const double *win, int NPT)
{
    int i;
    for(i=0;i<NPT-1;i++)
    {
        OutputBuffer[2*i]=DataBuffer[i]*win[i]+DataBuffer[NPT+i]*win[NPT+i];
        OutputBuffer[2*i+1]=0;
    }
    OutputBuffer[2*NPT-2]=DataBuffer[NPT-1]*win[NPT-1];
    OutputBuffer[2*NPT-1]=0;
}

void conv(const double DataBufferA[], const double DataBufferB[], double OutPutBuffer[], int m, int n) {
    int k, i;
    for (k = 0; k < m + n - 1; k++) {
        for (i = max(0, k + 1 - n); i <= min(k, m - 1); i++)
            OutPutBuffer[k] += DataBufferA[i] * DataBufferB[k - i];
    }
}

void Normalization(double *p, int NPT) {
    double sum = 0;
    for (int i = 0; i < 2 * NPT - 1; i++) {
        sum += p[i];
    }
    for (int i = 0; i < 2 * NPT - 1; i++) {
        p[i] /= sum;
    }
}

void apFFT(float32_t *DataA,float32_t *OutDataA, int NPT) {
    double Hanwin[NPT];
    double HanwinConv[2*NPT-1];

    hannWin(NPT, Hanwin);

    conv(Hanwin, Hanwin, HanwinConv, NPT, NPT);

    Normalization(HanwinConv, NPT);

    CreatApFFTSeries(DataA,OutDataA,HanwinConv,NPT);

    FFT(OutDataA,NPT);
}


float32_t Phase_Get(float32_t *DataA, float32_t *DataB, int NPT)
{
    uint16_t row1,row2;
    float32_t ph1,ph2,ph;
    float32_t OutDataA[2*NPT];
    float32_t CmplexA[NPT];
    float32_t OutDataB[2*NPT];
    float32_t CmplexB[NPT];

    apFFT(DataA,OutDataA,NPT);
    apFFT(DataB,OutDataB,NPT);

    row1=CmplexMag(OutDataA,CmplexA,NPT);
    row2=CmplexMag(OutDataB,CmplexB,NPT);

    ph1=atan(OutDataA[2*row1+1]/OutDataA[2*row1]);
    ph2=atan(OutDataB[2*row1+1]/OutDataB[2*row1]);

    ph=ph1-ph2;
    if(ph<-PI/2){
        ph+=PI;
    }
    if(ph>PI/2){
        ph-=PI;
    }

    return ph;
}

