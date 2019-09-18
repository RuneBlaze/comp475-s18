cmake_minimum_required(VERSION 3.8)
project(pa1)

set(CMAKE_CXX_STANDARD 11)

include_directories(include)



set(SOURCE_FILES
        apps/bounce.cpp
        apps/draw.cpp
        apps/GProxyCanvas.h
        apps/GRandom.h
        apps/GTime.cpp
        apps/GTime.h
        apps/GWindow.cpp
        apps/GWindow.h
        apps/image.cpp
        apps/image.h
        apps/image_recs.cpp
        apps/viewer.cpp
        include/GBitmap.h
        include/GCanvas.h
        include/GColor.h
        include/GMath.h
        include/GPixel.h
        include/GPoint.h
        include/GRect.h
        include/GTypes.h

        src/GBitmap.cpp empty_canvas.cpp)

add_executable(pa1 ${SOURCE_FILES})