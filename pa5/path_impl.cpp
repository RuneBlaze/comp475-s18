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