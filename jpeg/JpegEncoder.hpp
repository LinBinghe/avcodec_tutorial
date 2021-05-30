//
// Created by 林炳河 on 2021/5/29.
//

#ifndef JPEG_JPEGENCODER_HPP
#define JPEG_JPEGENCODER_HPP

#include <string>
/**
 * jpeg格式编码器
 */
class JpegEncoder {
public:
    JpegEncoder(std::string outputPath);

    ~JpegEncoder();

    /**
     * @brief 编码RGB888格式的数据
     */
    void encodeRGB24(const uint8_t* r, const uint8_t* g, const uint8_t* b,
                     const unsigned int &w, const unsigned int &h);

private:
    /**
     * RGB24转码为YUV444
     */
    bool rgbToYUV444(const uint8_t* r, const uint8_t* g, const uint8_t* b,
                     const unsigned int &w, const unsigned int &h,
                     uint8_t* const y, uint8_t* const u, uint8_t* const v);

    /**
     * YUV444数据进行8x8分块
     */
    bool yuvToBlocks(const uint8_t* y, const uint8_t* u, const uint8_t* v,
                     const unsigned int &w, const unsigned int &h,
                     uint8_t yBlocks[][64], uint8_t uBlocks[][64], uint8_t vBlocks[][64]);

    /**
     * 8x8分块后的数据逐块进行FDCT变换
     */
    bool blocksToFDCT(const uint8_t yBlocks[][64], const uint8_t uBlocks[][64], const uint8_t vBlocks[][64],
                      const unsigned int &w, const unsigned int &h,
                      int yDCT[][64], int uDCT[][64], int vDCT[][64]);

    /**
     * FDCT变换后的数据块逐一进行量化处理
     */
    bool fdctToQuant(int yDCT[][64], int uDCT[][64], int vDCT[][64],
                     const unsigned int &w, const unsigned int &h);

    /**
     * 将量化后的子块数据逐一按照Zigzag排序
     */
    bool quantToZigzag(int yQuant[][64], int uQuant[][64], int vQuant[][64],
                       const unsigned int &w, const unsigned int &h);

    /**
     * 将最终压缩后的数据写入文件
     */
    bool writeToFile(char* buffer, long dataLength,
                     const unsigned int &w, const unsigned int &h);

private:
    std::string mOutputPath;
};


#endif //JPEG_JPEGENCODER_HPP
