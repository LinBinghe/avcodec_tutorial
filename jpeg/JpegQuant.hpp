//
// Created by 林炳河 on 2021/5/30.
//

#ifndef JPEG_JPEGQUANT_HPP
#define JPEG_JPEGQUANT_HPP

/**
 * 量化器
 */
class JpegQuant {
public:
    JpegQuant();

    ~JpegQuant();

    /**
     * 将8x8的块数据进行量化
     * @param data8x8 子块数据
     * @param luminance 是否亮度通道
     */
    void quantEncode8x8(int *data8x8, bool luminance);

public:
    static const int STD_QUANT_TAB_LUMIN[64]; // 标准亮度量化表
    static const int STD_QUANT_TAB_CHROM[64]; // 标准色度量化表
};


#endif //JPEG_JPEGQUANT_HPP
