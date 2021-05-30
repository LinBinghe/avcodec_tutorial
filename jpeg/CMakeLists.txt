cmake_minimum_required(VERSION 3.19)
project(jpeg)

set(CMAKE_CXX_STANDARD 14)

add_executable(jpeg_encoder encoder.cpp
        JpegEncoder.cpp JpegEncoder.hpp
        JpegDCT.cpp JpegDCT.hpp
        JpegQuant.cpp JpegQuant.hpp
        JpegZigzag.cpp JpegZigzag.hpp
        HuffmanCodec.cpp HuffmanCodec.hpp
        3rdparty/bitstr.h 3rdparty/bitstr.c)
