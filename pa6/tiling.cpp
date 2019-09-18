//
// Created by Baqiao Liu on 4/21/18.
//

#include "tiling.h"

float clampMode(float t) {
    return std::min(std::max(t, .0f), 0.99999f);
}

float repeatMode(float t) {
    return std::min(static_cast<float>(t - floor(t)), 0.99999f);
}

float mirrorMode(float t) {
    t = t * 0.5f;
    t = t - static_cast<float>(floor(t));
    if (t > 0.5f) {
        t = 1 - t;
    }
    return std::min(static_cast<float>(t * 2), 0.99999f);
}

tilemode tilemodes[3] = {
        clampMode,
        repeatMode,
        mirrorMode,
};
