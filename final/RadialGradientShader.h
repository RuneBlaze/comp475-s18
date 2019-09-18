//
// Created by Baqiao Liu on 5/7/18.
//

#ifndef FINAL_RADIALGRADIENTSHADER_H
#define FINAL_RADIALGRADIENTSHADER_H
#include "GShader.h"
#include "prelude.h"
#include "GMatrix.h"
#include <vector>

class RadialGradientShader : public GShader {
public:
    explicit RadialGradientShader(GPoint center, float radius, const GColor* colors, const int count) {
        this->center = center;
        this->radius = radius;
        for (int i = 0; i < count; i ++) {
            this->cols.push_back(colors[i]);
        }
    }

    bool setContext(const GMatrix& rhs) override {
        return rhs.invert(&rctm);
    }

    bool isOpaque() override {return false;}

    void shadeRow(int x, int y, int count, GPixel res[]) override {
        for (int i = 0; i < count; i ++) {
            GPoint local = rctm.mapXY(x + i, y);
            res[i] = shadePixel(local.x(),local.y());
        }
    }

    GColor blendColor(GColor& lhs, GColor& rhs, float t) {
        return GColor::MakeARGB(
                lhs.fA * (1 - t) + rhs.fA * t,
                lhs.fR * (1 - t) + rhs.fR * t,
                lhs.fG * (1 - t) + rhs.fG * t,
                lhs.fB * (1 - t) + rhs.fB * t
        );
    }

    GPixel shadePixel(float x, float y) {
        auto cc = toUnitCircle({x, y});
        auto fx = cc.fX;
        auto fy = cc.fY;
        return unitCircleColor(fx, fy);
    }

    GPoint toUnitCircle(GPoint p) {
        return {(p.fX - this->center.fX) / radius, (p.fY - this->center.fY) / radius};
    }

    GPixel unitCircleColor(float x, float y) {
        auto dist = sqrt(x * x + y * y);
        if (dist >= 1) dist = 0.99999;

        auto _x = dist;
        auto count = cols.size();
        float seglen = 1.0f / (count - 1);
        float rest = fmod(_x, seglen);
        int firstC = floor(_x / seglen);
        int secondC = firstC + 1;
        GColor lhs = cols[firstC];
        GColor rhs = cols[secondC];
        GColor res = blendColor(lhs, rhs, rest / seglen);
        return color2px(res);
    }

    GPoint center;
    float radius;
    GMatrix rctm;
    std::vector<GColor> cols = {};
};


#endif //FINAL_RADIALGRADIENTSHADER_H
