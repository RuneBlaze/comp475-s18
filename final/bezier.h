//
// Created by Baqiao Liu on 4/21/18.
//

#ifndef PA6_BEZIER_H
#define PA6_BEZIER_H

#include "GPath.h"
#include "seg.h"


// citation: TechNote TN-06-005, Jim Armstrong
bool cubicTolerance(GPoint p0, GPoint p1, GPoint p2, GPoint p3, float tol) {
    auto ux = 3 * p1.fX - 2 * p0.fX - p3.fX;
    auto uy = 3 * p1.fY - 2 * p0.fY - p3.fY;
    auto vx = 3 * p2.fX - p0.fX - 2*p3.fX;
    auto vy = 3 * p2.fY - p0.fY - 2*p3.fY;

    ux *= ux;
    uy *= uy;
    vx *= vx;
    vy *= vy;

    ux = std::max(ux, vx);
    uy = std::max(uy, vy);
    return (ux + uy <= tol);
}

std::vector<Seg> mkCubic(GPoint p0, GPoint p1, GPoint p2, GPoint p3) {
    if (cubicTolerance(p0, p1, p2, p3, 3)) {
        return {mkSegf(p0.fX,p0.fY, p3.fX, p3.fY)};
    } else {
        GPoint dst[7];
        GPoint src[] = {p0, p1, p2, p3};
        GPath::ChopCubicAt(src, dst, 0.5);
        std::vector<Seg> res = {};
        auto lhs = mkCubic(dst[0], dst[1], dst[2], dst[3]);
        auto rhs = mkCubic(dst[3], dst[4], dst[5], dst[6]);
        for (auto s : lhs) {
            res.push_back(s);
        }
        for (auto s : rhs) {
            res.push_back(s);
        }
        return res;
    }
}

#endif //PA6_BEZIER_H
