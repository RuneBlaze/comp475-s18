//
// Created by Baqiao Liu on 5/7/18.
//

#ifndef FINAL_STROKED_LINE_H
#define FINAL_STROKED_LINE_H

#include "GPath.h"
#include "GPoint.h"
#include "GMatrix.h"
#include "cmath"

float getK(GPoint p0, GPoint p1) {
    if (p1.fX - p0.fX == 0) {
        return (p1.fY - p0.fY) / 0.0001;
    }
    return (p1.fY - p0.fY) / (p1.fX - p0.fX);
}

GPoint avg(GPoint lhs, GPoint rhs) {
    return {(lhs.fX + rhs.fX) / 2, (lhs.fY + rhs.fY) / 2};
}

void addStrokedLine(GPath* path, GPoint p0, GPoint p1,
                          float width, bool roundCap) {


    if (p0.x() > p1.x()) {
        addStrokedLine(path, p1, p0, width, roundCap);
        return;
    }

    auto k = getK(p0, p1);

    auto deg = atan(k);


    auto l = hypot((p1.fX - p0.fX), p1.fY - p0.fY);

    GPoint pts[4] = {{0, -width/2}, {0, width/2}, {1, width/2}, {1, -width/2}};

    auto matrix = GMatrix::MakeScale(l, 1).postConcat(GMatrix::MakeRotate(deg)).postConcat(GMatrix::MakeTranslate(p0.fX, p0.fY));
    matrix.mapPoints(pts, 4);

    if (roundCap) {
        path->moveTo(pts[0]);
        path->lineTo(pts[1]);
        path->lineTo(pts[2]);
        path->lineTo(pts[3]);
        path->addCircle(avg(pts[0], pts[1]), width/2, GPath::Direction::kCCW_Direction);
        path->addCircle(avg(pts[2], pts[3]), width/2, GPath::Direction::kCCW_Direction);

    } else {
        path->addPolygon(pts, 4);
    }

}


#endif //FINAL_STROKED_LINE_H
