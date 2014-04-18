#include <cmath>

#include "fix.h"
#include "fastmath.h"

#define pi 3.14159265

#define LUT_SIZE ((FFix(360+90).value)+1)

static FFix *table_sin;

void init_fastmath()
{	
    table_sin = new FFix[LUT_SIZE];

    float incr = (1.0 / (1<<FFix::precision)) * (pi/180);

    FFix deg = 0;
    float rad = 0;
    while(deg.value < LUT_SIZE)
	{
        table_sin[deg.value].fromFloat(sinf(rad));
        deg.value++;
        rad += incr;
	}
}

void uninit_fastmath()
{
    delete[] table_sin;
}

FFix fast_sin(FFix deg)
{	
    return table_sin[deg.value];
}

FFix fast_cos(FFix deg)
{	
    return table_sin[deg.value + FFix(90).value];
}
