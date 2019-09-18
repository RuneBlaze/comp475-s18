#include "GPath.h"
#include "GMatrix.h"

GPath& GPath::addRect(const GRect& r, Direction dir) {
    auto rect = r;
    if (dir == 0) {
        // CW Dir
        GPoint pts[4];
        pts[0] = GPoint::Make(rect.left(),rect.top());
        pts[1] = GPoint::Make(rect.right(),rect.top());
        pts[2] = GPoint::Make(rect.right(),rect.bottom());
        pts[3] = GPoint::Make(rect.left(),rect.bottom());
        return this->addPolygon(pts, 4);
    } else {
        // CCW Dir
        GPoint pts[4];
        pts[0] = GPoint::Make(rect.left(),rect.top());
        pts[3] = GPoint::Make(rect.right(),rect.top());
        pts[2] = GPoint::Make(rect.right(),rect.bottom());
        pts[1] = GPoint::Make(rect.left(),rect.bottom());
        return this->addPolygon(pts, 4);
    }
}

// moveTo(pts[0]), lineTo(pts[1..count-1])
GPath& GPath::addPolygon(const GPoint pts[], int count) {
    this->moveTo(pts[0]);
    for (int i = 1; i < count; i ++) {
        this->lineTo(pts[i]);
    }
    return *this;
}

GRect GPath::bounds() const {
    float left = 0;
    float right = 0;
    float top = 0;
    float down = 0;

    if (fPts.size() > 0) {
        left = fPts[0].x();
        top = fPts[0].y();
        right = left;
        down = top;
    }

    if (fPts.size() > 1) {
        for (int i = 1; i < fPts.size(); i ++) {
            auto p = fPts[i];
            left = std::min(left, p.fX);
            right = std::max(right, p.fX);
            top = std::min(top, p.fY);
            down = std::max(down, p.fY);
        }
    }
//    printf("%f %f %f %f \n", left, top, right, down);
    return GRect::MakeLTRB(left, top, right, down);
}

void GPath::transform(const GMatrix& m) {
    for (int i = 0; i < fPts.size(); i ++) {
        fPts[i] = m.mapPt(fPts[i]);
    }
}

GPath& GPath::addCircle(GPoint center, float radius, GPath::Direction dir) {
    float t8 = tan(M_PI / 8);
    float sqrt2_2 = sqrt(2) / 2;
    GPoint pts[] = {
            {1, 0},
            {1, t8},
            {sqrt2_2, sqrt2_2},
            {t8, 1},
            {0, 1},
            {-t8, 1},
            {-sqrt2_2, sqrt2_2},
            {-1, t8},
            {-1, 0},
            {-1, -t8},
            {-sqrt2_2, -sqrt2_2},
            {-t8, -1},
            {0, -1},
            {t8, -1},
            {sqrt2_2, -sqrt2_2},
            {1, -t8}
    };

    for (int i = 0; i < 16; i ++) {
        pts[i] = center + radius * pts[i];
    }

    moveTo(pts[0]);
    if (dir == 0) {
        // CW
        for (int i = 1; i <= 15; i += 2) {
            quadTo(pts[i], pts[(i + 1) % 16]);
        }
    } else {
        // CCW
        for (int i = 15; i >= 1; i -= 2) {
            quadTo(pts[i], pts[i - 1]);
        }
    }
    return *this;
}

/**
     *  Given 0 < t < 1, subdivide the src[] quadratic bezier at t into two new quadratics in dst[]
     *  such that
     *  0...t is stored in dst[0..2]
     *  t...1 is stored in dst[2..4]
     */
void GPath::ChopQuadAt(const GPoint *src, GPoint *dst, float t) {
    GPoint p1 = src[0];
    GPoint p2 = src[1];
    GPoint p3 = src[2];

    auto z = t;

    dst[0] = p1;
    dst[1] = z * p2 - (z - 1) * p1;
    dst[2] = static_cast<GPoint>(static_cast<GPoint>(pow(z, 2) * p3) - static_cast<GPoint>(2 * z * (z - 1) * p2)) + static_cast<GPoint>(pow((z - 1), 2) * p1);

    dst[3] = z * p3 - (z - 1) * p2;
    dst[4] = p3;
}


/**
     *  Given 0 < t < 1, subdivide the src[] cubic bezier at t into two new cubics in dst[]
     *  such that
     *  0...t is stored in dst[0..3]
     *  t...1 is stored in dst[3..6]
     */
void GPath::ChopCubicAt(const GPoint *src, GPoint *dst, float t) {
    auto p1 = src[0];
    auto p2 = src[1];
    auto p3 = src[2];
    auto p4 = src[3];
    auto z = t;

    dst[0] = p1;
    dst[1] = z * p2 - (z - 1) * p1;
    dst[2] = static_cast<GPoint>(pow(z, 2) * p3 - 2 * z * (z - 1) * p2) + pow((z - 1), 2) * p1;
    dst[3] = static_cast<GPoint>(pow(z, 3) * p4 - 3 * pow(z, 2) * (z - 1) * p3) + static_cast<GPoint>(3 * z * pow((z - 1), 2) * p2) - pow((z - 1), 3) * p1;
    dst[4] = static_cast<GPoint>(static_cast<GPoint>(pow(z, 2) * p4) - 2 * z * (z - 1) * p3) + pow((z - 1), 2) * p2;
    dst[5] = z * p4 - (z - 1) * p3;
    dst[6] = p4;
}