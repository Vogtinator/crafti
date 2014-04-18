#ifndef FASTMATH_H
#define FASTMATH_H

#include "gl.h"

constexpr int PRECISION = GLFix::precision;
typedef GLFix FFix;

void init_fastmath();
void uninit_fastmath();
FFix fast_sin(FFix deg);
FFix fast_cos(FFix deg);

#endif
