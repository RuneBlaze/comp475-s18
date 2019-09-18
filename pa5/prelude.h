#include <GColor.h>
#include <functional>

#include "GBitmap.h"
#include "GPixel.h"
#include "GBlendMode.h"
#include "GFilter.h"




//
// Created by Baqiao Liu on 4/7/18.
//

#ifndef PA5_PRELUDE_H
#define PA5_PRELUDE_H


int rnd(int x);

unsigned int flt255 (float x);

GPixel color2px(const GColor& color_);

void printColor(const GColor& color);

#endif //PA5_PRELUDE_H
