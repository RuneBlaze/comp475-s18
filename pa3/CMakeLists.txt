cmake_minimum_required(VERSION 3.8)
project(pa3)
include_directories(include)
set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
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
        apps/image_recs.cpp
        apps/paint.cpp
        apps/tests.cpp
        apps/tests.h
        apps/tests_recs.cpp
        apps/viewer.cpp
        include/GBitmap.h
        include/GCanvas.h
        include/GColor.h
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
        seg.h MyShader.cpp MyShader.h)

add_executable(pa3 ${SOURCE_FILES})