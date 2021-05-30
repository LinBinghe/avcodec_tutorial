//
// Created by 林炳河 on 2021/5/30.
//

#ifndef JPEG_JPEGZIGZAG_HPP
#define JPEG_JPEGZIGZAG_HPP

/**
 * Zigzag重排序
 */
class JpegZigzag {
public:
    JpegZigzag();

    ~JpegZigzag();

    /**
     * 将8x8的子块数据进行Zigzag重排序
     */
    void zigzag(int* const data8x8);

public:
    static const int ZIGZAG_INDEX[64]; // 重排序的下标索引
};


#endif //JPEG_JPEGZIGZAG_HPP
