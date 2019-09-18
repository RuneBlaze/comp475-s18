//
// Created by Baqiao Liu on 1/23/18.
//

//#include "empty_canvas.h"
#include <GColor.h>
#include <GPoint.h>
#include "GCanvas.h"
#include "GBitmap.h"
#include <GRect.h>
//#include "seg.h"
#include "clip.h"
#include <functional>


inline unsigned int flt255 (float x) {
    auto r = rnd(x * 255);
    GASSERT(r >= 0); GASSERT(r <= 255);
    return r;
}



void printAllSegs(const std::vector<Seg> &segs) {
    for (int i = 0; i < segs.size(); i ++) {
        printSeg(segs[i]);
    }
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
    return (x * y + 127)/255;
}


GPixel srcOutBlend(const GPixel& src, const GPixel& dest) {
    //!< [Sa * (1 - Da), Sc * (1 - Da)]
    auto rhs =  255 - A(dest);
    return GPixel_PackARGB(
            mulByte(A(src), rhs),
            mulByte(R(src), rhs),
            mulByte(G(src), rhs),
            mulByte(B(src), rhs)
    );
}

GPixel srcBlend(const GPixel& src, const GPixel& dest) {
    return src;
}

GPixel alphaBlend(const GPixel& src, const GPixel& dest) {
    int wt = 255 - A(src);
    if (A(src) == 255) {
        return srcBlend(src, dest);
    }
    return pixelAdd(src,
             GPixel_PackARGB(
                     (unsigned) mulByte(A(dest), wt),
                     (unsigned) mulByte(R(dest), wt),
                     (unsigned) mulByte(G(dest), wt),
                     (unsigned) mulByte(B(dest), wt)
             )
    );
}


/*
 *
 *
 *     kClear,    //!< [0, 0]
    kSrc,      //!< [Sa, Sc]
    kDst,      //!< [Da, Dc]
    kSrcOver,  //!< [Sa + Da * (1 - Sa), Sc + Dc * (1 - Sa)]
    kDstOver,  //!< [Da + Sa * (1 - Da), Dc + Sc * (1 - Da)]
    kSrcIn,    //!< [Sa * Da, Sc * Da]
    kDstIn,    //!< [Da * Sa, Dc * Sa]
    kSrcOut,   //!< [Sa * (1 - Da), Sc * (1 - Da)]
    kDstOut,   //!< [Da * (1 - Sa), Dc * (1 - Sa)]
    kSrcATop,  //!< [Da, Sc * Da + Dc * (1 - Sa)]
    kDstATop,  //!< [Sa, Dc * Sa + Sc * (1 - Da)]
    kXor,      //!< [Sa + Da - 2 * Sa * Da, Sc * (1 - Da) + Dc * (1 - Sa)]
 */

GPixel clearBlend(const GPixel& src, const GPixel& dest) {
    return GPixel_PackARGB(0,0,0,0);
}

GPixel dstBlend(const GPixel& src, const GPixel& dest) {
    return dest;
}

GPixel srcOverBlend(const GPixel& src, const GPixel& dest) {
    return alphaBlend(src, dest);
}

GPixel dstOverBlend(const GPixel& src, const GPixel& dest) {
    return alphaBlend(dest, src);
}

GPixel srcInBlend(const GPixel& src, const GPixel& dest) {
    //!< [Sa * Da, Sc * Da]
    return GPixel_PackARGB(
            mulByte(A(src), A(dest)),
            mulByte(R(src), A(dest)),
            mulByte(G(src), A(dest)),
            mulByte(B(src), A(dest))
    );
}

GPixel dstInBlend(const GPixel& src, const GPixel& dest) {
    return srcInBlend(dest, src);
}

GPixel dstOutBlend(const GPixel& src, const GPixel& dest) {
    return srcOutBlend(dest, src);
}

GPixel ACManipulate(const GPixel& src, const GPixel& dest, unsigned int alpha,
                    std::function<unsigned int(int, int)> fn) {
    return GPixel_PackARGB(
            alpha,
            fn(R(src), R(dest)),
            fn(G(src), G(dest)),
            fn(B(src), B(dest))
    );
}


GPixel srcATopBlend(const GPixel& src, const GPixel& dest) {
    //!< [Da, Sc * Da + Dc * (1 - Sa)]
    return ACManipulate(
            src, dest,
            static_cast<unsigned int>(A(dest)),
            [&src, &dest](int s, int d) -> unsigned int {
                return static_cast<unsigned int>(mulByte(s, A(dest)) + mulByte(d, (255 - A(src))));
            }
    );
}

GPixel dstATopBlend(const GPixel& src, const GPixel& dest) {
    return srcATopBlend(dest, src);
}





GPixel xorBlend(const GPixel& src, const GPixel& dest) {
    //!< [Sa + Da - 2 * Sa * Da, Sc * (1 - Da) + Dc * (1 - Sa)]
    return ACManipulate(
            src, dest,
            static_cast<unsigned int>(
                    A(dest) + A(src) - 2 * mulByte(A(src), A(dest))
            ),
            [&src, &dest](int s, int d) -> unsigned int {
                return static_cast<unsigned int>(
                        mulByte(s, 255 - A(dest)) + mulByte(d, 255 - A(src))
                );
            }
    );
}

typedef std::function<GPixel(const GPixel&, const GPixel&)> blendFn;


typedef GPixel (*blendM) (const GPixel&, const GPixel&);
blendM blendModes[] = {
        clearBlend,
        srcBlend,
        dstBlend,
        srcOverBlend,
        dstOverBlend,
        srcInBlend,
        dstInBlend,
        srcOutBlend,
        dstOutBlend,
        srcATopBlend,
        dstATopBlend,
        xorBlend,
};


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

    void drawPaint(const GPaint& paint) override {
        auto color = paint.getColor();
        auto p = color2px(color);
        for (int x = 0 ; x < fDevice.width() ; x ++) {
            for (int y = 0 ; y < fDevice.height() ; y ++) {
                auto addr = fDevice.getAddr(x, y);
                *addr = p;
            }
        }
    }

    void drawRect(const GRect& rect, const GPaint& paint) override {
        auto bid = static_cast<size_t>(paint.getBlendMode());
        blendM fn = blendModes[bid];
        auto color = paint.getColor();
        auto r = rect.round();
        auto src = color2px(color);
        clip(r, fDevice.width(), fDevice.height());

        if (!isValidRect(r)) return;

        for (int y = r.top() ; y < r.bottom(); y ++) {
            for (int x = r.left(); x < r.right(); x ++) {
                GPixel * addr = fDevice.getAddr(x, y);
                *addr = fn(src, *addr);
            }
        }
    }

    std::vector<Seg> pts2segs(const GPoint pts[], int count) {
        std::vector<Seg> buf = {};
        for (int i = 0; i < count; i ++) {
            if (i == count - 1) {
                buf.push_back(mkSegf(
                        pts[count-1].fX,
                        pts[count-1].fY,
                        pts[0].fX,
                        pts[0].fY
                ));
            } else {
                auto pt = pts[i];
                auto pt2 = pts[i+1];
                buf.push_back(mkSegf(pt.fX, pt.fY, pt2.fX, pt2.fY));
            }
        }
        return buf;
    }

    float getIntersection(int y, Seg &seg) {
        return seg.curX + ((y + 0.5f) - seg.y1) * seg.k;
    }

    inline int urnd(float x) {
        return static_cast<int>(floor(x + 0.5f));
    }

    bool isIntersecting(int y, Seg &seg) {
        return (y >= urnd(seg.y1)) && (y < urnd(seg.y2));
    }

    std::vector<float> rayCast(int y, std::vector<Seg>& segmented_segs) {
        std::vector<float> xs = {};
        xs.reserve(2);
        int i = 0;
        int j = 0;
//        for (Seg &s : segmented_segs) {
//            if (isIntersecting(y, s)) {
//                if (j == 0) j = i;
//                xs.push_back(getIntersection(y, s));
//                if (xs.size() >= 2) break;
//            }
//            i++;
//        }

        auto it = segmented_segs.begin();
        while (it != segmented_segs.end()) {
            if (y > urnd((*it).y2)) {
                it = segmented_segs.erase(it);
            } else {
                auto s = (*it);
                if (isIntersecting(y, s)) {
                    xs.push_back(getIntersection(y, s));
                    if (xs.size() >= 2) break;
                }
                ++it;
            }
        }
        return xs;
    }

    void drawConvexPolygon(const GPoint pts[], int count, const GPaint& paint) override  {
        GIRect r;
        r.setXYWH(0, 0, fDevice.width(), fDevice.height());
        auto bid = static_cast<size_t>(paint.getBlendMode());
        blendM fn = blendModes[bid];
        auto raw_segs = pts2segs(pts, count);
        auto color = paint.getColor();
        auto src = color2px(color);
        std::vector<Seg> segmented_segs = {};
        for (auto s : raw_segs) {
            if (!std::isfinite(s.k)) continue;
//            for (auto _s : clipSeg(r, s)) {
//                segmented_segs.push_back(_s);
//            }
            segmented_segs.push_back(s);
        }
//        printAllSegs(segmented_segs);
        std::sort(segmented_segs.begin(), segmented_segs.end(),
                  [](Seg const& a, Seg const& b) -> bool {
                      if (a.y1 != b.y1) return a.y1 < b.y1;
                      if (a.curX != b.curX) return a.curX < b.curX;
                      return a.k < b.k;
                  }
        );
        int initialY = segmented_segs.empty() ? 0 : std::max(urnd(segmented_segs[0].y1),0);
        auto finalY = fDevice.height();
        if (!segmented_segs.empty()) {
            auto finalY_ = std::max_element(segmented_segs.begin(), segmented_segs.end(), [](const Seg &lhs, const Seg &rhs) {
                return lhs.y2 < rhs.y2;
            });
            finalY = urnd(std::min((*finalY_).y2, static_cast<float>(fDevice.height())));
        }
        for (int y = initialY; y < urnd(finalY); y ++) {
            auto rayCastRes = rayCast(y, segmented_segs);
            std::sort(rayCastRes.begin(),rayCastRes.end());
            if (rayCastRes.size() == 1) {
                GPixel * addr = fDevice.getAddr(urnd(rayCastRes[0]), y);
                *addr = fn(src, *addr);
                continue;
            };
            if (rayCastRes.size() == 2) {
                rayCastRes[0] = std::max(.0f, rayCastRes[0]);
                rayCastRes[1] = std::min(static_cast<float>(fDevice.width()), rayCastRes[1]);
//                printf("rc: %f, %f\n", rayCastRes[0], rayCastRes[1]);
                for (int x = urnd(rayCastRes[0]); x < urnd(rayCastRes[1]); x ++) {
                    GPixel * addr = fDevice.getAddr(x, y);
                    *addr = fn(src, *addr);
                }
            } else if (!rayCastRes.empty()) {
                GASSERT(false);
                printf("%zu\n", rayCastRes.size());
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
//    auto r = GPixel_PackARGB(120, 80, 0, 0);
//    auto g = GPixel_PackARGB(120, 0, 110, 0);
//    auto rs = srcATopBlend(r, g);
//    showPixel(rs);


    GIRect rect;
    rect.setXYWH(0,0,100,100);
    printf("%d !!!\n", rect.fRight);
//    auto s1 = mkSeg(30, 30, 50, 50);
//    GASSERT(clipSeg(rect, s1).size() == 1);

    auto s2 = mkSeg(50, 0, -50, 100);
    printSeg(s2);
    auto c2 = clipSeg(rect, s2);
    printAllSegs(c2);
//    GASSERT(c2.size() == 2);
    printf("==== TESTENDD ====\n");
}

std::unique_ptr<GCanvas> GCreateCanvas(const GBitmap& device) {
    if (!device.pixels()) {
        return nullptr;
    }

//    testCases();
    return std::unique_ptr<GCanvas>(new EmptyCanvas(device));
}
