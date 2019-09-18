#include <GColor.h>
#include <GPoint.h>
#include <functional>
#include <stack>

#include "GCanvas.h"
#include "GBitmap.h"
#include "GShader.h"
#include "clip.h"
#include "blends.h"
#include "layer.h"

enum class CTMType {
    matrixC,
    layerC
};

void BitmapSlice::blendSinglePix(blendM fn, GPixel src, int x, int y) {
    auto bnds = this->bounds;
    if (x < bnds.left() || x >= bnds.right() || y < bnds.top() || y >= bnds.bottom()) {
        return;
    }
    GPixel * addr = this->getAddr(x, y);
    *addr = fn(src, *addr);
}

void BitmapSlice::blendRow(blendM fn, int startX, int startY, GPixel storage[], int cnt) {
    for (int i = 0; i < cnt; i ++) {
        this->blendSinglePix(fn, storage[i], startX + i, startY);
    }
}

void printAllSegs(const std::vector<Seg> &segs) {
    for (int i = 0; i < segs.size(); i ++) {
        printSeg(segs[i]);
    }
}

GRect prevRect(const BitmapSlice& slice) {
    return slice.bounds;
}

GRect prevRect(const GBitmap& slice) {
    return GRect::MakeWH(slice.width(), slice.height());
}

template <class T>
void blendLayer(const T& dst, BitmapSlice& slice) {
    auto filter = slice.paint.getFilter();
    for (int x = 0; x < slice.width(); x ++) {
        for (int y = 0 ; y < slice.height(); y ++) {

//            if (x < 0 || x >= dst.width() || y < 0 || y >= dst.height()) {
//                continue;
//            }

            auto bnds = prevRect(dst);
            if (x < bnds.left() || x >= bnds.right() || y < bnds.left() || y >= bnds.right()) {
                continue;
            }

            auto s_ = *slice.getAddr(x, y);
            GPixel s = 0;
            if (filter != nullptr) {
                filter->filter(&s, &s_, 1);
            } else {
                s = s_;
            }
            auto d = dst.getAddr(x, y);
            auto bm = slice.paint.getBlendMode();
            auto ind = static_cast<int>(bm);
            auto r = blendModes[ind](s, *d);
            *d = r;
        }
    }
}

void blendLayer1(const GBitmap& dst, BitmapSlice& slice) {
    blendLayer(dst, slice);
}

void blendLayer2(const BitmapSlice& dst, BitmapSlice& slice) {
    blendLayer(dst, slice);
}

class EmptyCanvas : public GCanvas {
public:
    explicit EmptyCanvas(const GBitmap& device) : fDevice(device) {
        auto m = new GMatrix(1,0,0,0,1,0);
        auto l = new Layer(*m);
        history.push(CTMType::matrixC);
        matrices.push(*m);
    }

    void drawPaint(const GPaint& paint) override {
        drawRect(GRect::MakeWH(fDevice.width(), fDevice.height()), paint);
    }

    void drawRect(const GRect& rect, const GPaint& paint) override {
        GPoint pts[4];
        pts[0] = GPoint::Make(rect.left(),rect.top());
        pts[1] = GPoint::Make(rect.right(),rect.top());
        pts[2] = GPoint::Make(rect.right(),rect.bottom());
        pts[3] = GPoint::Make(rect.left(),rect.bottom());
        drawConvexPolygon(pts, 4, paint);
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

    void showPixel(GPixel& p) {
        printf("%d %d %d %d\n", A(p), R(p), G(p), B(p));
    }

    void blendSinglePix(blendM fn, GPixel src, int x, int y) {
        if (x < 0 || x >= fDevice.width() || y < 0 || y >= fDevice.height()) {
            return;
        }
        GPixel * addr = fDevice.getAddr(x, y);
        *addr = fn(src, *addr);
    }

    void blendRow(blendM fn, int startX, int startY, GPixel storage[], int cnt) {
        for (int i = 0; i < cnt; i ++) {
            blendSinglePix(fn, storage[i], startX + i, startY);
        }
    }

    void onSaveLayer(const GRect* bounds, const GPaint& paint) override {
        BitmapSlice * slice;

        if (bounds == nullptr) {
            if (slices.empty()) {
                auto b = GRect::MakeWH(fDevice.width(), fDevice.height());
                slice = new BitmapSlice(
                        fDevice.width(), fDevice.height(), b, paint);
            } else {
                slice = new BitmapSlice(
                        fDevice.width(), fDevice.height(), slices.top().bounds, paint);
            }
        } else {
            slice = new BitmapSlice(
                    fDevice.width(), fDevice.height(), *bounds, paint);
        }
        history.push(CTMType::layerC);
        slices.push(*slice);
    }

    void drawConvexPolygon(const GPoint pts_[], int count, const GPaint& paint) override  {
        if (count <= 1) return;
        GPoint pts[count];

        matrices.top().mapPoints(pts, pts_, count);

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
            segmented_segs.push_back(s);
        }

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

        auto filter = paint.getFilter();
        for (int y = initialY; y < urnd(finalY); y ++) {
            auto rayCastRes = rayCast(y, segmented_segs);
            std::sort(rayCastRes.begin(),rayCastRes.end());

            if (rayCastRes.size() == 2) {
                rayCastRes[0] = std::max(.0f, rayCastRes[0]);
                rayCastRes[1] = std::min(static_cast<float>(fDevice.width()), rayCastRes[1]);

                if (paint.getShader() != nullptr) {
                    int rowL = urnd(rayCastRes[0]);
                    int rowR = urnd(rayCastRes[1]);
                    auto cnt = rowR - rowL;
                    GPixel storage[cnt];
                    auto shader = paint.getShader();
                    shader->setContext(matrices.top());
                    shader->shadeRow(rowL, y, cnt, storage);

                    GPixel nstorage[cnt];
                    if (filter != nullptr) {
                        filter->filter(nstorage, storage, cnt);
                    } else {
                        for (int i = 0 ; i < cnt ; i ++) {
                            nstorage[i] = storage[i];
                        }
                    }

                    if (!slices.empty()) {
                        slices.top().blendRow(fn, rowL, y, nstorage, cnt);
                    } else {
                        blendRow(fn, rowL, y, nstorage, cnt);
                    }
                } else {
                    for (int x = urnd(rayCastRes[0]); x < urnd(rayCastRes[1]); x ++) {
                        GPixel px = src;
                        if (filter != nullptr) {
                            filter->filter(&px,&src,1);
                        }
                        if (!slices.empty()) {
                            slices.top().blendSinglePix(fn, px, x, y);
                        } else {
                            blendSinglePix(fn, px, x, y);
                        }
                    }
                }
            } else if (!rayCastRes.empty()) {
                GASSERT(false);
                printf("%zu\n", rayCastRes.size());
            }
        }
    }

    void save() override {
        auto peek = matrices.top();
        history.push(CTMType::matrixC);
        matrices.push(peek);
    }

    void restore() override {
        auto tp = history.top();
        history.pop();
        if (tp == CTMType::matrixC) {
            matrices.pop();
        } else {
            auto src = slices.top();
            slices.pop();
            if (slices.empty()) {
                auto dst = fDevice;
                blendLayer1(dst, src);
            } else {
                auto dst = slices.top();
                blendLayer2(dst, src);
            }
            delete[] src.canvas;
        }
    }

    void concat(const GMatrix& matrix) override {
        matrices.top().preConcat(matrix);
    }

private:
    const GBitmap fDevice;
    std::stack<Layer> ctms;

    std::stack<CTMType> history;
    std::stack<BitmapSlice> slices;
    std::stack<GMatrix> matrices;
};


void testCases() {
    printf("==== TESTCASES ====\n");
    GIRect rect;
    rect.setXYWH(0,0,100,100);
    printf("%d !!!\n", rect.fRight);
    auto s2 = mkSeg(50, 0, -50, 100);
    printSeg(s2);
    auto c2 = clipSeg(rect, s2);
    printAllSegs(c2);
    printf("==== TESTENDD ====\n");
}

std::unique_ptr<GCanvas> GCreateCanvas(const GBitmap& device) {
    if (!device.pixels()) {
        return nullptr;
    }
    return std::unique_ptr<GCanvas>(new EmptyCanvas(device));
}
