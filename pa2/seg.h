//
// Created by Baqiao Liu on 2/10/18.
//

#include <cmath>

struct Seg {
    float y1;
    float y2;
    float curX;
    float k;
};

inline unsigned int rnd (float x) {
//    GASSERT(x >= 0.0);
    auto res = floor(x + 0.5);
//    GASSERT(x >= 0);
    return res;
}

float gx2(const Seg &seg) {
    return seg.k * (seg.y2 - seg.y1) + seg.curX;
}

void printSeg(const Seg &seg) {
    printf("Seg{%f, %f, %f, %f} ((%f, %f) -> (%f, %f))\n",
           seg.y1, seg.y2, seg.curX, seg.k, seg.curX, seg.y1, gx2(seg), seg.y2);
}

Seg mkSeg(int x1, int y1, int x2, int y2) {
    if (y1 > y2) {
        return mkSeg(x2, y2, x1, y1);
    }
    return {static_cast<float>(y1), static_cast<float>(y2), static_cast<float>(x1), static_cast<float>(x2 - x1) / (y2 - y1)};
}

Seg mkSegf(float x1, float y1, float x2, float y2) {
    if (y1 > y2) {
        return mkSegf(x2, y2, x1, y1);
    }
    return {y1, y2, x1, (x2 - x1) / (y2 - y1)};
}