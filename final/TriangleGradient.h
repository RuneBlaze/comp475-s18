//
// Created by Baqiao Liu on 5/7/18.
//

#ifndef FINAL_TRIANGLEGRADIENT_H
#define FINAL_TRIANGLEGRADIENT_H


#include "GShader.h"
#include "prelude.h"
#include "GMatrix.h"
#include <vector>


class TriangleGradientShader : public GShader {
public:
    explicit TriangleGradientShader(const GPoint* points, const GColor* colors) {
        for (int i = 0; i < 3; i ++) {
            pts.push_back(points[i]);
            cols.push_back(colors[i]);
        }
    }

    bool isOpaque() override {return false;}

    bool setContext(const GMatrix& rhs) override {
        return rhs.invert(&rctm);
    }

    void shadeRow(int x, int y, int count, GPixel res[]) override {
        for (int i = 0; i < count; i ++) {
            GPoint local = rctm.mapXY(x + i, y);
            res[i] = shadePixel(local.x(),local.y());
        }
    }

    GPixel shadePixel(float x, float y) {
        auto c1 = std::get<0>(toTriangleCoord(x, y));
        auto c2 = std::get<1>(toTriangleCoord(x, y));
        auto c3 = 1 - c1 - c2;

        auto l1 = this->cols[0];
        auto l2 = this->cols[1];
        auto l3 = this->cols[2];


        return color2px(GColor::MakeARGB(
                l1.fA * c1 + l2.fA * c2 + l3.fA * c3,
                l1.fR * c1 + l2.fR * c2 + l3.fR * c3,
                l1.fG * c1 + l2.fG * c2 + l3.fG * c3,
                l1.fB * c1 + l2.fB * c2 + l3.fB * c3
        ));
    }

    std::tuple<float, float> toTriangleCoord(float x, float y) {
        float x1 = this->pts[0].fX;
        float x2 = this->pts[1].fX;
        float x3 = this->pts[2].fX;

        float y1 = this->pts[0].fY;
        float y2 = this->pts[1].fY;
        float y3 = this->pts[2].fY;

        auto det = (y2-y3)*(x1-x3) + (x3-x2) * (y1 - y3);
        auto c1 = (y2 - y3) * (x - x3) + (x3 - x2) * (y - y3);
        c1 /= det;

        auto c2 = (y3 - y1) * (x - x3) + (x1 - x3) * (y - y3);
        c2 /= det;

        return std::make_tuple(c1, c2);
    }

    std::vector<GColor> cols = {};
    std::vector<GPoint> pts = {};

    GMatrix rctm;
};


#endif //FINAL_TRIANGLEGRADIENT_H
