#include "fft_user.h"


uint32_t FFT(float32_t *p1, uint16_t n) {
    switch (n) {
        case 16:
            arm_cfft_f32(&arm_cfft_sR_f32_len16, p1, 0, 1);
            break;
        case 32:
            arm_cfft_f32(&arm_cfft_sR_f32_len32, p1, 0, 1);
            break;
        case 64:
            arm_cfft_f32(&arm_cfft_sR_f32_len64, p1, 0, 1);
            break;
        case 128:
            arm_cfft_f32(&arm_cfft_sR_f32_len128, p1, 0, 1);
            break;
        case 256:
            arm_cfft_f32(&arm_cfft_sR_f32_len256, p1, 0, 1);
            break;
        case 512:
            arm_cfft_f32(&arm_cfft_sR_f32_len512, p1, 0, 1);
            break;
        case 1024:
            arm_cfft_f32(&arm_cfft_sR_f32_len1024, p1, 0, 1);
            break;
        case 2048:
            arm_cfft_f32(&arm_cfft_sR_f32_len2048, p1, 0, 1);
            break;
        case 4096:
            arm_cfft_f32(&arm_cfft_sR_f32_len4096, p1, 0, 1);
            break;
        default :
            return -1;
    }
    return 1;
}

uint32_t IFFT(float32_t *p1, uint16_t n) {
    switch (n) {
        case 16:
            arm_cfft_f32(&arm_cfft_sR_f32_len16, p1, 1, 1);
            break;
        case 32:
            arm_cfft_f32(&arm_cfft_sR_f32_len32, p1, 1, 1);
            break;
        case 64:
            arm_cfft_f32(&arm_cfft_sR_f32_len64, p1, 1, 1);
            break;
        case 128:
            arm_cfft_f32(&arm_cfft_sR_f32_len128, p1, 1, 1);
            break;
        case 256:
            arm_cfft_f32(&arm_cfft_sR_f32_len256, p1, 1, 1);
            break;
        case 512:
            arm_cfft_f32(&arm_cfft_sR_f32_len512, p1, 1, 1);
            break;
        case 1024:
            arm_cfft_f32(&arm_cfft_sR_f32_len1024, p1, 1, 1);
            break;
        case 2048:
            arm_cfft_f32(&arm_cfft_sR_f32_len2048, p1, 1, 1);
            break;
        case 4096:
            arm_cfft_f32(&arm_cfft_sR_f32_len4096, p1, 1, 1);
            break;
        default :
            return -1;
    }
    return 1;
}


uint16_t CmplexMag(float32_t *p1, float32_t *fft_outputbuf, uint16_t n) {
    float32_t max = 0;
    uint16_t row;
    arm_cmplx_mag_f32(p1, fft_outputbuf, n);
    for (int i = 1; i < n / 2; i++) {
        if (fft_outputbuf[i] > max) {
            max = fft_outputbuf[i];
            row = i;
        }
    }
    return row;
}

uint16_t OY_CmplexMag(float32_t *p1, float32_t *fft_outputbuf, uint16_t n, uint16_t *row, float32_t *max) {
    *max = 0;*(max+1) = 0;
    *row = 0;*(row+1) = 0;
    arm_cmplx_mag_f32(p1, fft_outputbuf, n);
    for (int i = 5; i < n / 2; i++) {
        if (fft_outputbuf[i] > max[0]) {
            max[0] = fft_outputbuf[i];
            row[0] = i;
        }
    }
		 for (int i = 5; i < n / 2; i++) {
        if (fft_outputbuf[i] > max[1]&&(i<(row[0]-4)||i>(row[0]+4))) {
            max[1] = fft_outputbuf[i];
            row[1] = i;
        }
    }
    return 1;
}
