//#include "GBlendMode.h"
//#include "GColor.h"
//#include "GFilter.h"
//#include "blends.cpp"
//
//blendM blendModes_[] = {
//        clearBlend,
//        srcBlend,
//        dstBlend,
//        srcOverBlend,
//        dstOverBlend,
//        srcInBlend,
//        dstInBlend,
//        srcOutBlend,
//        dstOutBlend,
//        srcATopBlend,
//        dstATopBlend,
//        xorBlend,
//};
//
//blendM blendModes[] = {
//        clearBlend,
//        srcBlend,
//        dstBlend,
//        srcOverBlend,
//        dstOverBlend,
//        srcInBlend,
//        dstInBlend,
//        srcOutBlend,
//        dstOutBlend,
//        srcATopBlend,
//        dstATopBlend,
//        xorBlend,
//};
//
//class BlendFilter : public GFilter {
//    bool preservesAlpha() override {
//        return false;
//    }
//
//    void filter(GPixel output[], const GPixel input[], int count) override {
//        auto ind = static_cast<size_t>(this->mode);
//        auto fn = blendModes_[ind];
//
//        for (int i = 0; i < count ; i ++ ) {
//            auto r = fn(input[i], output[i]);
//            output[i] = r;
//        }
//    }
//
//public:
//    GBlendMode mode;
//};
//
//std::unique_ptr<GFilter> GCreateBlendFilter(GBlendMode mode, const GColor& src) {
//    auto filter = new BlendFilter();
//    filter->mode = mode;
//    return std::unique_ptr<GFilter>(filter);
//}