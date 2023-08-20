#ifndef __USER_FUNCTION_H
#define __USER_FUNCTION_H

#include "main.h"

typedef enum
{
    OY_OK = 1,
    OY_Err = 0
    /* data */
}OY_STATE;


OY_STATE OY_FindMax(float *p1, uint16_t length, float *max, uint16_t *row);
OY_STATE OY_FindMin(float *p1, uint16_t length, float *min, uint16_t *row);
OY_STATE OY_FindMaxMin(float *p1, uint16_t length, float *max, float *min, float *vpp, uint16_t *row);
OY_STATE OY_Normalization(float *p1, uint16_t length);
OY_STATE OY_Norm(float *p1, float *p2, uint16_t length, float *out, float bp);

#endif
