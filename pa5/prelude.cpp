//
// Created by Baqiao Liu on 4/7/18.
//

#include "prelude.h"

inline int rnd(int x) {
    return static_cast<int>(floor(x + 0.5));
}

inline unsigned int flt255 (float x) {
    auto r = rnd(x * 255);
    GASSERT(r >= 0); GASSERT(r <= 255);
    return r;
}

GPixel color2px(const GColor& color_) {
    float a, r, g, b;
    auto color = color_.pinToUnit();
    a = color.fA;
    r = color.fR * a;
    g = color.fG * a;
    b = color.fB * a;

    r = std::min(a, r);
    g = std::min(a, g);
    b = std::min(a, b);

    return GPixel_PackARGB(flt255(a), flt255(r), flt255(g), flt255(b));
}

void printColor(const GColor& color) {
    printf("%f %f %f %f\n", color.fA, color.fR, color.fG, color.fB);
}