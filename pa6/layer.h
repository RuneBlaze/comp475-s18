#include "GCanvas.h"
#include "GBitmap.h"
#include "GMatrix.h"
#include <stdint.h>
class BitmapSlice {
public:
    explicit BitmapSlice(int width, int height, const GRect& bounds, const GPaint& paint) :
            bounds(bounds), paint(paint) {
        this->_width = width;
        this->_height = height;
        this->canvas = static_cast<GPixel *>(calloc(width * height, sizeof(GPixel*)));
    }

    GPixel* getAddr(int x, int y) const {
//        return &this->can[x + y * _width];
        return &this->canvas[x + y * _width];
    }

    int width() const {
        return this->_width;
    }

    int height() const {
        return this->_height;
    }

    GPixel* canvas;
//    GPixel* canvas;
    std::vector<GPixel> can;
    int _width;
    int _height;
    const GRect bounds;
    const GPaint paint;

    virtual void blendSinglePix(blendM fn, GPixel src, int x, int y);
    virtual void blendRow(blendM fn, int startX, int startY, GPixel storage[], int cnt);
};

class Layer {
public:
    explicit Layer(GMatrix matrix, bool newLayer, BitmapSlice* slice) {
        this->isBitmap = newLayer;
        this->matrix = matrix;
        if (isBitmap) {
            this->slice = slice;
        } else {
            this->slice = nullptr;
        }
    }

    explicit Layer(GMatrix matrix) {
        this->isBitmap = false;
        this->matrix = matrix;
    }

    bool isMatrix() {
        return !this->isBitmap;
    }

    bool isLayer() {
        return this->isBitmap;
    }

    void onPop(const GCanvas& canvas) {

    }
public:
    GMatrix matrix;
    BitmapSlice* slice;
    bool isBitmap;
};