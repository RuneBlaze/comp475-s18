//
// Created by Baqiao Liu on 1/23/18.
//

//#include "empty_canvas.h"
#include <GColor.h>
#include "GCanvas.h"
#include "GBitmap.h"
#include "GRect.h"

inline unsigned int rnd (float x) {
    GASSERT(x >= 0.0);
    auto res = (unsigned int) floor(x + 0.5);
    GASSERT(x >= 0);
    return res;
}

inline unsigned int flt255 (float x) {
    auto r = rnd(x * 255);
    GASSERT(r >= 0); GASSERT(r <= 255);
    return r;
}

void unpackPixel(const GPixel& p, int* a, int* r, int* g, int* b) {
    *a = GPixel_GetA(p);
    *r = GPixel_GetR(p);
    *g = GPixel_GetG(p);
    *b = GPixel_GetB(p);
}

GPixel color2px(const GColor& color_) {
    float a, r, g, b;
    auto color = color_.pinToUnit();
    a = color.fA;
    r = color.fR * a;
    g = color.fG * a;
    b = color.fB * a;
    return GPixel_PackARGB(flt255(a), flt255(r), flt255(g), flt255(b));
}

GPixel pixelAdd(const GPixel& lhs, const GPixel& rhs) {
    int a,r,g,b;
    int a_,r_,g_,b_ ;
    unpackPixel(lhs, &a, &r, &g, &b);
    unpackPixel(rhs, &a_, &r_, &g_, &b_);
    return GPixel_PackARGB(unsigned (a + a_), unsigned (r + r_),
                           unsigned (g + g_), unsigned (b + b_));
}

inline int A(const GPixel& p) {return GPixel_GetA(p);}
inline int R(const GPixel& p) {return GPixel_GetR(p);}
inline int G(const GPixel& p) {return GPixel_GetG(p);}
inline int B(const GPixel& p) {return GPixel_GetB(p);}

inline int mulByte(const int x, const int y) {
    GASSERT(x >= 0 && x <= 255);
    GASSERT(y >= 0 && y <= 255);
    return (x * y + 127)/255;
}


GPixel alphaBlend(const GPixel& src, const GPixel& dest) {
    int wt = 255 - A(src);
    return pixelAdd(src,
             GPixel_PackARGB(
                     (unsigned) mulByte(A(dest), wt),
                     (unsigned) mulByte(R(dest), wt),
                     (unsigned) mulByte(G(dest), wt),
                     (unsigned) mulByte(B(dest), wt)
             )
    );
}

// mutable!
void clip(GIRect& rect, int w, int h) {
    rect.fLeft = std::max(0, rect.fLeft);
    rect.fLeft = std::min(w, rect.fLeft);

    rect.fRight = std::min(w, rect.fRight);
    rect.fRight = std::max(0, rect.fRight);

    rect.fBottom = std::min(h, rect.fBottom);
    rect.fBottom = std::max(0, rect.fBottom);

    rect.fTop = std::max(0, rect.fTop);
    rect.fTop = std::min(h, rect.fTop);
}

bool isValidRect(GIRect& rect) {
    return rect.left() < rect.right() && rect.top() <= rect.bottom();
}


class EmptyCanvas : public GCanvas {
public:
    EmptyCanvas(const GBitmap& device) : fDevice(device) {}

    void clear(const GColor& color) override {
        auto p = color2px(color);
        for (int x = 0 ; x < fDevice.width() ; x ++) {
            for (int y = 0 ; y < fDevice.height() ; y ++) {
                auto addr = fDevice.getAddr(x, y);
                *addr = p;
            }
        }
    }

    void fillRect(const GRect& rect, const GColor& color) override {
        auto r = rect.round();
        auto src = color2px(color);
        clip(r, fDevice.width(), fDevice.height());
//        GASSERT();
        if (!isValidRect(r)) return;
        for (int x = r.left(); x < r.right(); x ++) {
            for (int y = r.top() ; y < r.bottom(); y ++) {
                GPixel * addr = fDevice.getAddr(x, y);
                *addr = alphaBlend(src, *addr);
            }
        }
    }

private:
    const GBitmap fDevice;
};


void showPixel(GPixel& p) {
    printf("%d %d %d %d\n", A(p), R(p), G(p), B(p));
}

void testCases() {
    printf("==== TESTCASES ====\n");
    auto r = GPixel_PackARGB(120, 80, 0, 0);
    auto g = GPixel_PackARGB(120, 0, 110, 0);
    auto rs = alphaBlend(r, g);
    showPixel(rs);
    printf("==== TESTENDD ====\n");
}

std::unique_ptr<GCanvas> GCreateCanvas(const GBitmap& device) {
    if (!device.pixels()) {
        return nullptr;
    }

//    testCases();
    return std::unique_ptr<GCanvas>(new EmptyCanvas(device));
}
