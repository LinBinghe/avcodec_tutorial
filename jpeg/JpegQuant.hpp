//
// Created by 林炳河 on 2021/5/30.
//

#ifndef JPEG_JPEGQUANT_HPP
#define JPEG_JPEGQUANT_HPP


class JpegQuant {
public:
    JpegQuant();

    ~JpegQuant();

    void quantEncode8x8(int *data8x8, bool luminance);

public:
    static const int STD_QUANT_TAB_LUMIN[64];
    static const int STD_QUANT_TAB_CHROM[64];
};


#endif //JPEG_JPEGQUANT_HPP
