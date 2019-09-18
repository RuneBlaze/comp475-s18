cmake_minimum_required(VERSION 3.9)
project(pa4)

set(CMAKE_CXX_STANDARD 11)


include_directories(include)
add_executable(pa4
        apps/bench.cpp
        apps/bench.h
        apps/bench_recs.cpp
        apps/bounce.cpp
        apps/draw.cpp
        apps/GProxyCanvas.h
        apps/GRandom.h
        apps/GTime.cpp
        apps/GWindow.cpp
        apps/GWindow.h
        apps/image.cpp
        apps/image.h
        apps/image_pa1.cpp
        apps/image_pa2.cpp
        apps/image_pa3.cpp
        apps/image_pa4.cpp
        apps/image_recs.cpp
        apps/paint.cpp
        apps/tests.cpp
        apps/tests.h
        apps/tests_pa4.cpp
        apps/tests_recs.cpp
        apps/viewer.cpp
        layer.h
        include/GBitmap.h
        include/GBlendMode.h
        include/GCanvas.h
        include/GColor.h
        include/GFilter.h
        include/GMath.h
        include/GMatrix.h
        include/GPaint.h
        include/GPixel.h
        include/GPoint.h
        include/GRect.h
        include/GShader.h
        include/GTime.h
        include/GTypes.h
        include/GWindow.h
        src/GBitmap.cpp
        clip.h
        empty_canvas.cpp
        blends.h
        MyMatrix.cpp
        MyShader.cpp
        MyFilter.h
        MyShader.h
        seg.h)
