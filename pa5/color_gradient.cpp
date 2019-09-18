#include <GBitmap.h>
#include <vector>
#include "GShader.h"
#include "GMatrix.h"
#include "prelude.h"


class LGradientShader : public GShader {
public:
    explicit LGradientShader(
            const GPoint p0, const GPoint p1, const GColor* colors, const int count)
    : p0(p0), p1(p1), count(count) {
        auto dx = p1.fX - p0.fX;
        auto dy = p1.fY - p0.fY;
        auto m = new GMatrix(dx, -dy, p0.fX, dy, dx, p0.fY);
//        this->colors = static_cast<GColor *>(calloc(count, sizeof(GColor*)));
        for (int i = 0; i < count; i ++) {
            this->cols.push_back(colors[i]);
        }
        m->invert(&this->pt2interval);
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

    GPixel shadePixel(float x, float y) {
        auto finalMatrix = pt2interval;
        auto _x = finalMatrix.mapPt(GPoint::Make(x, y)).fX;
//        printf("%f\n",_x);
        if (_x < 0) _x = 0;
        if (_x >= 1) _x = 0.99999;
        float seglen = 1.0f / (count - 1);
        float rest = fmod(_x, seglen);
        int firstC = floor(_x / seglen);
        int secondC = firstC + 1;

        GColor lhs = cols[firstC];
        GColor rhs = cols[secondC];
        GColor res = blendColor(lhs, rhs, rest / seglen);
//        printf("%f %f %f %f\n", colors[0].fA, colors[0].fR, colors[0].fG, colors[0].fB);
//        printf("%f [%d, %d, %f] (%f %f %f %f)\n", _x, firstC, cols.size(), rest / seglen, res.fA, res.fB, res.fG, res.fR);
//        if (firstC == cols.size() - 1) {
//            GASSERT(false);
//        }
        return color2px(res);
    }

    GColor blendColor(GColor& lhs, GColor& rhs, float t) {
        return GColor::MakeARGB(
                lhs.fA * (1 - t) + rhs.fA * t,
                lhs.fR * (1 - t) + rhs.fR * t,
                lhs.fG * (1 - t) + rhs.fG * t,
                lhs.fB * (1 - t) + rhs.fB * t
        );
    }

    const GPoint p0;
    const GPoint p1;
    GColor* colors;
    std::vector<GColor> cols = {};
    const int count;
    GMatrix rctm;
    GMatrix pt2interval;
};

std::unique_ptr<GShader> GCreateLinearGradient(
        GPoint p0, GPoint p1, const GColor colors[], int count) {
    auto shader = new LGradientShader(p0, p1, colors, count);
    return std::unique_ptr<GShader>(shader);
}