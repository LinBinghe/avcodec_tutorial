//
// Created by 林炳河 on 2021/5/30.
//

#ifndef JPEG_JPEGDCT_HPP
#define JPEG_JPEGDCT_HPP

/**
 * DCT变换器
 */
class JpegDCT {
public:
    JpegDCT();

    ~JpegDCT();

    /**
     * 将8x8的子块数据进行FDCT变换
     * @param data8x8 8x8的子块数据
     * @param ftab fdct表，可为空
     */
    void fdct2d8x8(int *data8x8, int *ftab);

private:
    int float2Fix(float value);

private:
    const int DCT_SIZE = 8;
    const int FIX_Q = 11;
    int mFDCTFactor[64];
};


#endif //JPEG_JPEGDCT_HPP
