//
// Created by 林炳河 on 2021/5/29.
//

#ifndef JPEG_JPEGENCODER_HPP
#define JPEG_JPEGENCODER_HPP

#include <string>

class JpegEncoder {
public:
    JpegEncoder(std::string outputPath);

    ~JpegEncoder();

    void encodeRGB24(const uint8_t* r, const uint8_t* g, const uint8_t* b,
                     const unsigned int &w, const unsigned int &h);

private:
    bool rgbToYUV444(const uint8_t* r, const uint8_t* g, const uint8_t* b,
                     const unsigned int &w, const unsigned int &h,
                     uint8_t* const y, uint8_t* const u, uint8_t* const v);

    bool yuvToBlocks(const uint8_t* y, const uint8_t* u, const uint8_t* v,
                     const unsigned int &w, const unsigned int &h,
                     uint8_t yBlocks[][64], uint8_t uBlocks[][64], uint8_t vBlocks[][64]);

    bool blocksToFDCT(const uint8_t yBlocks[][64], const uint8_t uBlocks[][64], const uint8_t vBlocks[][64],
                      const unsigned int &w, const unsigned int &h,
                      int yDCT[][64], int uDCT[][64], int vDCT[][64]);

    bool fdctToQuant(int yDCT[][64], int uDCT[][64], int vDCT[][64],
                     const unsigned int &w, const unsigned int &h);

    bool quantToZigzag(int yQuant[][64], int uQuant[][64], int vQuant[][64],
                       const unsigned int &w, const unsigned int &h);

    bool writeToFile(char* buffer, long dataLength,
                     const unsigned int &w, const unsigned int &h);

private:
    std::string mOutputPath;
};


#endif //JPEG_JPEGENCODER_HPP
