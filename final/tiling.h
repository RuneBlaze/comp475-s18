//
// Created by Baqiao Liu on 4/21/18.
//

#ifndef PA6_TILING_H
#define PA6_TILING_H

#include "GShader.h"

typedef float (*tilemode) (float);

float clampMode(float t);

float repeatMode(float t);

float mirrorMode(float t);

extern tilemode tilemodes[3];

#endif //PA6_TILING_H
