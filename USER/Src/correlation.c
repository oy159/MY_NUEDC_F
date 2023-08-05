//
// Created by 86187 on 2022/10/25.
//
#include "correlation.h"



//void cFFT_f32(float * DataBuffer,int NPT)
//{
//    arm_cfft_instance_f32 S;
//    arm_cfft_init_f32(&S,2*NPT);
//    arm_cfft_f32(&S,DataBuffer,0,1);
//}

void real_to_im_f32(float * DataBuffer,int NPT)
{
    int i;
    for(i=NPT-1;i>=0;i--)
    {
        DataBuffer[2*i]=DataBuffer[i];
        DataBuffer[2*i+1]=0;
    }
}

void cross_mag(float * DataBufferA,float * DataBufferB,int NPT)
{
    int i;
    float buf_realA,buf_imgA;
    for(i=0;i<NPT;i++)
    {
        buf_realA=DataBufferA[2*i];
        buf_imgA=DataBufferA[2*i+1];
        DataBufferA[2*i]=buf_realA*DataBufferB[2*i]+buf_imgA*DataBufferB[2*i+1];
        DataBufferA[2*i+1]=-buf_realA*DataBufferB[2*i+1]+buf_imgA*DataBufferB[2*i];
    }
}
void cross_mag_with_out(float * DataBufferA,float * DataBufferB,float * DataBufferC,int NPT)
{
    int i;
    for(i=0;i<NPT;i++)
    {
        DataBufferC[2*i]=DataBufferA[2*i]*DataBufferB[2*i]+DataBufferA[2*i+1]*DataBufferB[2*i+1];
        DataBufferC[2*i+1]=-DataBufferA[2*i]*DataBufferB[2*i+1]+DataBufferA[2*i+1]*DataBufferB[2*i];
    }
}
//void ciFFT_f32(float * DataBuffer,int NPT)
//{
//    arm_cfft_instance_f32 S;
//    arm_cfft_init_f32	(&S,2*NPT);
//    arm_cfft_f32(&S,DataBuffer,1,1);
//}
void arrayshift(float * DataBufferA,float * DataBufferB,int NPT)
{
    int i;
    for(i=0;i<4*NPT;i++)
    {
        DataBufferB[i]=DataBufferA[i];
    }
    for(i=0;i<NPT;i++)
    {
        DataBufferA[(i+NPT-1)]=DataBufferB[2*i];
    }
    for(i=NPT;i<2*NPT-1;i++)
    {
        DataBufferA[(i-NPT)]=DataBufferB[2*i+2];
    }
}
void xcorr(float * DataBufferA,float * DataBufferB,int NPT)
{
    int i;
    real_to_im_f32(DataBufferA,NPT);
    real_to_im_f32(DataBufferB,NPT);
    FFT(DataBufferA,NPT);
    FFT(DataBufferB,NPT);
    cross_mag(DataBufferA,DataBufferB,2*NPT);
    IFFT(DataBufferA,NPT);
    arrayshift(DataBufferA,DataBufferB,NPT);
}

float xcorr0(float * DataBufferA,float * DataBufferB,int NPT)
{
    int i;float buff = 0;
    for(i=0;i<NPT;i++)
    {
        buff+=DataBufferA[i]*DataBufferB[i]/NPT;
    }
    return buff;
}
float get_phase(float * DataBufferA,float * DataBufferB,int NPT)
{
    float buff;
    arm_sqrt_f32(xcorr0(DataBufferA,DataBufferA,NPT)*xcorr0(DataBufferB,DataBufferB,NPT),&buff);
    buff=acos(xcorr0(DataBufferA,DataBufferB,NPT)/buff);
    return buff;
}
void PHAT_applier(float * DataBufferA,int NPT)
{
    float mag,magbuf1,magbuf2;int i;
    for(i=0;i<2*NPT;i++)
    {
        arm_power_f32(DataBufferA+2*i,2,&mag);
        arm_sqrt_f32(mag,&mag);
        DataBufferA[2*i]=DataBufferA[2*i]/mag;
        DataBufferA[2*i+1]=DataBufferA[2*i+1]/mag;
    }
}
void PHAT_xcorr(float * DataBufferA,float * DataBufferB,int NPT)
{
    int i;
    real_to_im_f32(DataBufferA,NPT);
    real_to_im_f32(DataBufferB,NPT);
    FFT(DataBufferA,NPT);
    FFT(DataBufferB,NPT);
    cross_mag(DataBufferA,DataBufferB,2*NPT);
    PHAT_applier(DataBufferA,NPT);
    IFFT(DataBufferA,NPT);
    arrayshift(DataBufferA,DataBufferB,NPT);
}
