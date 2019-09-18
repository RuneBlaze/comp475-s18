//
// Created by Baqiao Liu on 2/10/18.
//

#include <GColor.h>
#include <GPoint.h>
#include "GCanvas.h"
#include "GBitmap.h"
#include "GRect.h"
#include "seg.h"
#include <vector>

/*
 * //

struct Seg {
    int y1;
    int y2;
    float curX;
    float k;
};
 *
 * */


bool isInRect(const GIRect& rect, float x, float y) {
    return (x >= rect.fLeft && x <= rect.fRight && y >= rect.fTop && y <= rect.fBottom);
}

void clipFromTop(const GIRect& rect, Seg &seg) {
    printf("fromTop\n");
    GASSERT(seg.y1 < rect.fTop);
    auto dy = rect.fTop - seg.y1;
    auto a1 = seg.curX + dy * seg.k;
    seg.y1 = rect.fTop;
    seg.curX = a1;
}

void clipFromBottom(const GIRect& rect, Seg &seg) {
    printf("fromBottom\n");
    GASSERT(seg.y2 > rect.fBottom && seg.y1 >= rect.fTop);
    auto dy = rect.fBottom - seg.y1;
    auto a1 = seg.curX + dy * seg.k;
    seg.y2 = rect.fBottom;
}

Seg mkVertSeg(float y1, float y2, int x) {
    return {y1, y2, static_cast<float>(x), 0};
}

Seg clipX2FromLeft(const GIRect& rect, Seg &seg) {
    printf("x2 - left\n");
    auto dx = rect.fLeft - seg.curX;
    auto ny = seg.y1 + (1 / seg.k) * dx;
    Seg nseg = mkVertSeg(ny, seg.y2, rect.fLeft);
    seg.y2 = ny;
    return nseg;
}



Seg clipX2FromRight(const GIRect& rect, Seg &seg) {
    printf("x2 - right\n");
    auto dx = rect.fRight - seg.curX;
    auto ny = seg.y1 + (1 / seg.k) * dx;
    Seg nseg = mkVertSeg(ny, seg.y2, rect.fRight);
    seg.y2 = ny;
    return nseg;
}

Seg clipX1FromLeft(const GIRect& rect, Seg &seg) {
    printf("x1 - left\n");
    auto dx = rect.fLeft - seg.curX;
    auto ny = seg.y1 + (1 / seg.k) * dx;
    Seg nseg = mkVertSeg(seg.y1, ny, rect.fLeft);
    seg.y1 = ny;
    seg.curX = rect.fLeft;
    return nseg;
}

Seg clipX1FromRight(const GIRect& rect, Seg &seg) {
    printf("x1 - right\n");
    printSeg(seg);
    auto dx = seg.curX - rect.fRight;
    auto ny = seg.y1 + (1 / seg.k) * dx;
    Seg nseg = mkVertSeg(seg.y1, ny, rect.fRight);
    seg.y1 = ny;
    seg.curX = rect.fRight;
    if (nseg.y1 > nseg.y2) {
        printf("%d\n", rect.fRight);
        printSeg(nseg);
        GASSERT(false);
    }
    return nseg;
}


std::vector<Seg> clipSeg(const GIRect& rect, Seg &seg) {
//    printf("%f\n", seg.k);
    auto x1 = seg.curX;
    auto x2 = seg.k * (seg.y2 - seg.y1) + seg.curX;
//    printf("%f\n", x2);
    if (seg.y1 > rect.fBottom) return {};
    if (seg.y2 < rect.fTop) return {};
//    printf("2\n");
    if (x2 <= rect.fLeft && x1 <= rect.fLeft) return {
                {seg.y1, seg.y2, static_cast<float>(rect.fLeft), 0}
        };

    printf("EMERGEN: %f, %d, %f, %d\n", x1, rect.fRight, x2, rect.fRight);
    printSeg(seg);
    printf("%d, %d\n", (x1 > rect.fRight), (x2 > rect.fRight));
    if (x1 >= rect.fRight && x2 >= rect.fRight) {
        printf("simply returned\n");
        return {
                {seg.y1, seg.y2, static_cast<float>(rect.fRight), 0}
        };
    }
//    printf("3\n");
    if (isInRect(rect, x1, seg.y1) && isInRect(rect, x2, seg.y2)) return {seg};

    if (seg.y1 < rect.fTop) clipFromTop(rect, seg);
    if (seg.y2 > rect.fBottom) clipFromBottom(rect, seg);
    printf("afterbottom\n");
    printSeg(seg);

    x1 = seg.curX;
    x2 = seg.k * (seg.y2 - seg.y1) + seg.curX;
//    printf("%f\n", x2);
    if (seg.y1 > rect.fBottom) return {};
    if (seg.y2 < rect.fTop) return {};
//    printf("2\n");
    if (x2 <= rect.fLeft && x1 <= rect.fLeft) return {
                {seg.y1, seg.y2, static_cast<float>(rect.fLeft), 0}
        };

    std::vector<Seg> buf = {};
    printf("%f, %d\n", seg.curX, rect.fLeft);
    if (seg.curX < rect.fLeft) {
        buf.push_back(clipX1FromLeft(rect, seg));
    }

    if (seg.curX > rect.fRight) {
        printf("%f, %d <=>\n", seg.curX, rect.fRight);
        buf.push_back(clipX1FromRight(rect, seg));
    }

    if (gx2(seg) < rect.fLeft) {
        buf.push_back(clipX2FromLeft(rect, seg));
    }

    if (gx2(seg) > rect.fRight) {
        buf.push_back(clipX2FromRight(rect, seg));
    }
    buf.push_back(seg);
    return buf;
}