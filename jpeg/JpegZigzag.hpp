//
// Created by 林炳河 on 2021/5/30.
//

#ifndef JPEG_JPEGZIGZAG_HPP
#define JPEG_JPEGZIGZAG_HPP


class JpegZigzag {
public:
    JpegZigzag();

    ~JpegZigzag();

    void zigzag(int* const data8x8);

public:
    static const int ZIGZAG_INDEX[64];
};


#endif //JPEG_JPEGZIGZAG_HPP
