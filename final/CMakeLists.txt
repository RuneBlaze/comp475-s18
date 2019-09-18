cmake_minimum_required(VERSION 3.10)
project(final)

set(CMAKE_CXX_STANDARD 11)

include_directories(.)
include_directories(apps)
include_directories(include)

add_executable(pa6
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
        src/GBitmap.cpp
        src/GPath.cpp
        blends.h
        clip.h
        color_gradient.cpp
        empty_canvas.cpp
        layer.h
        MyFilter.h
        MyMatrix.cpp
        MyShader.cpp
        MyShader.h
        path_impl.cpp
        prelude.cpp
        prelude.h
        seg.h bezier.h tiling.h tiling.cpp RadialGradientShader.h TriangleGradient.h stroked_line.h)
