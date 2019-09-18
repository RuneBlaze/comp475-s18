#include "GColor.h"
#include "GMath.h"
#include "GPoint.h"
#include "GRect.h"
#include "GMatrix.h"


void GMatrix::setIdentity() {
    GMatrix::set6(1, 0, 0,
                  0, 1, 0);
}

void GMatrix::setTranslate(float tx, float ty) {
    GMatrix::set6(1,0,tx,
                  0,1,ty);
}

void GMatrix::setScale(float sx, float sy) {
    GMatrix::set6(sx, 0, 0,
                  0, sy, 0);
}

void GMatrix::setRotate(float radians) {
    auto t = radians;
    GMatrix::set6(cos(t),-sin(t),0,
                  sin(t),cos(t),0);
}

void GMatrix::setConcat(const GMatrix& secundo, const GMatrix& primo) {
    auto A = secundo.fMat[0];
    auto B = secundo.fMat[1];
    auto C = secundo.fMat[2];
    auto D = secundo.fMat[3];
    auto E = secundo.fMat[4];
    auto F = secundo.fMat[5];

    auto a = primo.fMat[0];
    auto b = primo.fMat[1];
    auto c = primo.fMat[2];
    auto d = primo.fMat[3];
    auto e = primo.fMat[4];
    auto f = primo.fMat[5];

    GMatrix::set6(A*a+B*d,A*b+B*e,A*c+B*f+C,D*a+E*d,D*b+E*e,D*c+E*f+F);
}

bool GMatrix::invert(GMatrix* inverse) const {
    auto a = GMatrix::fMat[0];
    auto b = GMatrix::fMat[1];
    auto c = GMatrix::fMat[2];
    auto d = GMatrix::fMat[3];
    auto e = GMatrix::fMat[4];
    auto f = GMatrix::fMat[5];
    auto det = a * e - b * d;
    if (det == 0) {
        return false;
    }

    if (inverse != nullptr) {
        inverse->set6(e / det, -b / det, (b * f - e * c) / det,
                      -d / det, a / det, (d * c - a * f) / det);
    }
    return true;
}


void GMatrix::mapPoints(GPoint dst[], const GPoint src[], int count) const {
    for (int i = 0; i < count; i++ ) {
        auto s = src[i];
        auto x = s.fX;
        auto y = s.fY;
        auto a = GMatrix::fMat[0];
        auto b = GMatrix::fMat[1];
        auto c = GMatrix::fMat[2];
        auto d = GMatrix::fMat[3];
        auto e = GMatrix::fMat[4];
        auto f = GMatrix::fMat[5];
        dst[i].fX = a * x + b * y + c;
        dst[i].fY = d * x + e * y + f;
    }
}