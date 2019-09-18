cmake_minimum_required(VERSION 3.8)
project(pa2)

set(CMAKE_CXX_STANDARD 11)

include_directories(include)

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
        apps/image_recs.cpp
        apps/paint.cpp
        apps/test_recs.cpp
        apps/tests.cpp
        apps/tests.h
        apps/tests_recs.cpp
        apps/viewer.cpp
        include/GBitmap.h
        include/GCanvas.h
        include/GColor.h
        include/GMath.h
        include/GPaint.h
        include/GPixel.h
        include/GPoint.h
        include/GRect.h
        include/GTime.h
        include/GTypes.h
        include/GWindow.h
        src/GBitmap.cpp
        empty_canvas.cpp clip.h seg.h)

add_executable(pa2 ${SOURCE_FILES})