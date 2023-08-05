#include    "User_function.h"


OY_STATE OY_FindMax(float *p1, uint16_t length, float *max, uint16_t *row)
{
    *max = 0;
    *row = 0;
    uint16_t i;
    for (i = 0; i < length; i++)
    {
        if (*(p1 + i)>*max)
        {
            *max = *(p1 + i);
            *row = i;
        }
    }
    
}

OY_STATE OY_FindMin(float *p1, uint16_t length, float *min, uint16_t *row)
{
    *min = *p1;
    *row = 0;
    uint16_t i;
    for (i = 1; i < length; i++)
    {
        if (*(p1 + i)<*min)
        {
            *min = *(p1 + i);
            *row = i;
        }
    }
    
}

OY_STATE OY_Normalization(float *p1, uint16_t length)
{
		float max,min;
		uint16_t row;
		OY_FindMax(p1,length,&max,&row);
		OY_FindMax(p1,length,&min,&row);
		if(-min > max)
				max = -min;
		for(uint16_t i = 0;i < length; i++)
			p1[i] = p1[i]/max;
	
}

OY_STATE OY_Norm(float *p1, float *p2, uint16_t length, float *out, float bp)
{
		*out = 0;
		for(uint16_t i = 0;i < length; i++)
			*out += (p1[i]*bp - p2[i]*bp)*(p1[i]*bp - p2[i]*bp);
}







