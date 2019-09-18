//
// Created by Baqiao Liu on 2/24/18.
//
#include <GBitmap.h>
#include "GShader.h"
#include "GMatrix.h"
#include "MyShader.h"

int rd(float x) {return static_cast<int>(floor(x + 0.5));}
int rd_(float x) {return static_cast<int>(floor(x));}

template <class T>
T clamp(T n, T lhs, T rhs) {
    if (n < lhs) return lhs;
    if (n > rhs) return rhs;
    return n;
}

class MyShader : public GShader{
public:
    explicit MyShader(const GBitmap& rhs) : bmp(rhs) {}
    bool isOpaque() override {return false;}

    bool setContext(const GMatrix& rhs) override {
        return rhs.invert(&rctm);
    }

    void shadeRow(int x, int y, int count, GPixel res[]) override {
        auto finalMatrix = GMatrix();
        finalMatrix.setConcat(localInv,rctm);

        auto bx = x + 0.5f;
        auto by = y + 0.5f;
        GPoint local = finalMatrix.mapXY(bx, by);

        for (int i = 0; i < count; i++) {
            auto _x = local.fX;
            auto _y = local.fY;
            _x = clamp(_x, .0f, bmp.width()-1.0f);
            _y = clamp(_y, .0f, bmp.height()-1.0f);

            auto r = bmp.getAddr(rd_(_x), rd_(_y));

            res[i] = *r;
            local.fX += finalMatrix[GMatrix::SX];
            local.fY += finalMatrix[GMatrix::KY];
        }

    }
    GMatrix rctm;
    GMatrix localInv = GMatrix();
private:
    GBitmap bmp;
};

/**
 *  Return a subclass of GShader that draws the specified bitmap and the inverse of a local matrix.
 *  Returns null if the either parameter is invalid.
 */
std::unique_ptr<GShader> GCreateBitmapShader(const GBitmap& bmp, const GMatrix& localInv) {
    auto sd = new MyShader(bmp);
    sd->localInv = localInv;
    return std::unique_ptr<GShader>(sd);
}