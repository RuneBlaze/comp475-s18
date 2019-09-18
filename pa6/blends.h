#include <GColor.h>
#include <functional>

#include "GBitmap.h"
#include "GPixel.h"
#include "GBlendMode.h"
#include "GFilter.h"
#include "prelude.h"

inline int A(const GPixel& p) {return GPixel_GetA(p);}
inline int R(const GPixel& p) {return GPixel_GetR(p);}
inline int G(const GPixel& p) {return GPixel_GetG(p);}
inline int B(const GPixel& p) {return GPixel_GetB(p);}

void showPixel(GPixel& pixel) {
    printf("%d %d %d %d \n", A(pixel), R(pixel), G(pixel), B(pixel));
}

typedef std::function<GPixel(const GPixel&, const GPixel&)> blendFn;

typedef GPixel (*blendM) (const GPixel&, const GPixel&);



void unpackPixel(const GPixel& p, int* a, int* r, int* g, int* b) {
    *a = GPixel_GetA(p);
    *r = GPixel_GetR(p);
    *g = GPixel_GetG(p);
    *b = GPixel_GetB(p);
}



static inline GPixel GPixel_PackARGB_(unsigned a, unsigned r, unsigned g, unsigned b) {
    r = std::min(a, r);
    g = std::min(a, g);
    b = std::min(a, b);
    return GPixel_PackARGB(a, r, g, b);
}

GPixel pixelAdd(const GPixel& lhs, const GPixel& rhs) {
    int a,r,g,b;
    int a_,r_,g_,b_ ;
    unpackPixel(lhs, &a, &r, &g, &b);
    unpackPixel(rhs, &a_, &r_, &g_, &b_);
    return GPixel_PackARGB_(unsigned (a + a_), unsigned (r + r_),
                           unsigned (g + g_), unsigned (b + b_));
}



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

class BlendFilter : public GFilter {
public:
    explicit BlendFilter(GBlendMode& mode, const GPixel& color) : color(color) {
        this->mode = mode;
    }

    bool preservesAlpha() override {
        return false;
    }

    void filter(GPixel output[], const GPixel input[], int count) override {
        auto ind = static_cast<size_t>(this->mode);
        auto fn = blendModes[ind];
        for (int i = 0; i < count ; i ++ ) {
            output[i] = fn(this->color, input[i]);
        }
    }

public:
    GBlendMode mode;
    const GPixel color;
};

std::unique_ptr<GFilter> GCreateBlendFilter(GBlendMode mode, const GColor& src) {
    auto filter = new BlendFilter(mode, color2px(src));
    return std::unique_ptr<GFilter>(filter);
}
