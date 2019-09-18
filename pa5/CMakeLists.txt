cmake_minimum_required(VERSION 3.9)
project(pa5)

set(CMAKE_CXX_STANDARD 11)

include_directories(include)

add_executable(pa5
        apps/bench.cpp
        apps/bench.h
        apps/bench_recs.cpp
        apps/bounce.cpp
        apps/draw.cpp
        apps/draw_path.cpp
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
        apps/image_pa5.cpp
        apps/image_recs.cpp
        apps/paint.cpp
        apps/tests.cpp
        apps/tests.h
        apps/tests_pa4.cpp
        apps/tests_pa5.cpp
        apps/tests_recs.cpp
        apps/viewer.cpp
        include/GBitmap.h
        include/GBlendMode.h
        include/GCanvas.h
        include/GColor.h
        include/GFilter.h
        include/GMath.h
        include/GMatrix.h
        include/GPaint.h
        include/GPath.h
        include/GPixel.h
        include/GPoint.h
        include/GRect.h
        include/GShader.h
        include/GTime.h
        include/GTypes.h
        include/GWindow.h
        src/GBitmap.cpp
        src/GPath.cpp
        blends.h
        clip.h
        empty_canvas.cpp
        layer.h
        MyFilter.h
        MyMatrix.cpp
        MyShader.cpp
        MyShader.h
        seg.h color_gradient.cpp path_impl.cpp prelude.h prelude.cpp)
