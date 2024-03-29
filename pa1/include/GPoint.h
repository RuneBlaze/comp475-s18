/**
 *  Copyright 2015 Mike Reed
 */

#ifndef GPoint_DEFINED
#define GPoint_DEFINED

#include "GMath.h"

struct GVector {
    float fX;
    float fY;

    float x() const { return fX; }
    float y() const { return fY; }
    float length() const { return sqrtf(fX*fX + fY*fY); }

    void set(float x, float y) {
        fX = x;
        fY = y;
    }

    GVector operator+(const GVector& v) const {
        return { fX + v.fX, fY + v.fY };
    }

    GVector operator*(float s) {
        return { fX * s, fY * s };
    }
};

class GPoint {
public:
    float fX;
    float fY;

    float x() const { return fX; }
    float y() const { return fY; }

    void set(float x, float y) {
        fX = x;
        fY = y;
    }

    static GPoint Make(float x, float y) {
        GPoint pt = { x, y };
        return pt;
    }

    GPoint operator+(const GVector& v) {
        return { fX + v.fX, fY + v.fY };
    }
    GPoint& operator+=(const GVector& v) {
        *this = *this + v;
        return *this;
    }

    GVector operator-(const GPoint& p) {
        return { fX - p.fX, fY - p.fY };
    }
};

#endif
