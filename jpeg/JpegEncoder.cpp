//
// Created by 林炳河 on 2021/5/29.
//

#include "JpegEncoder.hpp"
#include "JpegDCT.hpp"
#include "JpegQuant.hpp"
#include "JpegZigzag.hpp"
#include "HuffmanCodec.hpp"

JpegEncoder::JpegEncoder(std::string outputPath) : mOutputPath(outputPath) {

}

JpegEncoder::~JpegEncoder() {

}

void JpegEncoder::encodeRGB24(const uint8_t *r, const uint8_t *g, const uint8_t *b, const unsigned int &w,
                              const unsigned int &h) {
    if (r == nullptr || g == nullptr || b == nullptr || w == 0 || h == 0) {
        printf("encode failed. r/g/b/w/h is error.");
        return;
    }

    // 颜色模式转换
    // RGB -> YUV444
    uint8_t yChannel[w * h];
    uint8_t uChannel[w * h];
    uint8_t vChannel[w * h];
    bool success = rgbToYUV444(r, g, b, w, h, yChannel, uChannel, vChannel);
    if (!success) {
        return;
    }

    // 分块
    int wBlockSize = w / 8 + (w % 8 == 0 ? 0 : 1);
    int hBlockSize = h / 8 + (h % 8 == 0 ? 0 : 1);
    int blockSize = wBlockSize * hBlockSize;
    uint8_t yBlocks[blockSize][8 * 8];
    uint8_t uBlocks[blockSize][8 * 8];
    uint8_t vBlocks[blockSize][8 * 8];
    success = yuvToBlocks(yChannel, uChannel, vChannel, w, h, yBlocks, uBlocks, vBlocks);
    if (!success) {
        return;
    }

    // DCT
    int yDCT[blockSize][64];
    int uDCT[blockSize][64];
    int vDCT[blockSize][64];
    success = blocksToFDCT(yBlocks, uBlocks, vBlocks, w, h, yDCT, uDCT, vDCT);
    if (!success) {
        return;
    }

    // 量化
    success = fdctToQuant(yDCT, uDCT, vDCT, w, h);
    if (!success) {
        return;
    }

    // Zigzag排序一下
    success = quantToZigzag(yDCT, uDCT, vDCT, w, h);
    if (!success) {
        return;
    }

    // DC 系数的 DPCM 与中间格式计算
    // AC 系数的 RLC 与中间格式计算
    // 最终进行熵编码（霍夫曼编码）
    // 以上多个步骤将在下面环节中一起处理
    std::shared_ptr<HuffmanCodec> huffmanCodec = std::make_shared<HuffmanCodec>();
    long dataLength = huffmanCodec->encode(yDCT, uDCT, vDCT, w, h);
    printf("[JpegEncoder] encode length:%ld\n", dataLength);
    if (dataLength <= 0) {
        return;
    }

    // 终于到了写文件的时候了
    success = writeToFile(huffmanCodec->getResult(), dataLength, w, h);
    printf("[JpegEncoder] result:%d\n", success);
}

uint8_t bound(uint8_t min, int value, uint8_t max) {
    if (value <= min) {
        return min;
    }
    if (value >= max) {
        return max;
    }
    return value;
}

bool JpegEncoder::rgbToYUV444(const uint8_t *r, const uint8_t *g, const uint8_t *b,
                              const unsigned int &w, const unsigned int &h,
                              uint8_t *const y, uint8_t *const u, uint8_t *const v) {
    for (int row = 0; row < h; row++) {
        for (int column = 0; column < w; column++) {
            int index = row * w + column;
            // rgb -> yuv 公式
            // 这里在实现的时候踩了个坑
            // 之前直接将cast后的值赋值给了y/u/v数组，y/u/v数组类型是uint8，计算出来比如v是256直接越界数值会被转成其他如0之类的值
            // 导致最后颜色效果错误
            int yValue = static_cast<int>(round(0.299 * r[index] + 0.587 * g[index] + 0.114 * b[index]));
            int uValue = static_cast<int>(round(-0.169 * r[index] - 0.331 * g[index] + 0.500 * b[index] + 128));
            int vValue = static_cast<int>(round(0.500 * r[index] - 0.419 * g[index] - 0.081 * b[index] + 128));
            // 做下边界容错
            y[index] = bound(0, yValue, 255);
            u[index] = bound(0, uValue, 255);
            v[index] = bound(0, vValue, 255);
        }
    }
    return true;
}

bool JpegEncoder::yuvToBlocks(const uint8_t *y, const uint8_t *u, const uint8_t *v,
                              const unsigned int &w, const unsigned int &h,
                              uint8_t yBlocks[][64], uint8_t uBlocks[][64], uint8_t vBlocks[][64]) {
    int wBlockSize = w / 8 + (w % 8 == 0 ? 0 : 1);
    int hBlockSize = h / 8 + (h % 8 == 0 ? 0 : 1);
    for (int blockRow = 0; blockRow < hBlockSize; blockRow++) {
        for (int blockColumn = 0; blockColumn < wBlockSize; blockColumn++) {
            int blockIndex = blockRow * wBlockSize + blockColumn; // 当前子块下标
            uint8_t *yBlock = yBlocks[blockIndex];
            uint8_t *uBlock = uBlocks[blockIndex];
            uint8_t *vBlock = vBlocks[blockIndex];
            for (int row = 0; row < 8; row++) {
                for (int column = 0; column < 8; column++) {
                    int indexInSubBlock = row * 8 + column; // 块中数据位置
                    int realPosX = blockColumn * 8 + column; // 在完整YUV数据中的X位置
                    int realPosY = blockRow * 8 + row; // 在完整YUV数据中的Y位置
                    int indexInOriginData = realPosY * w + realPosX; // 在原始数据中的位置
                    if (realPosX >= w || realPosY >= h) {
                        // 补齐数据
                        yBlock[indexInSubBlock] = 0;
                        uBlock[indexInSubBlock] = 0;
                        vBlock[indexInSubBlock] = 0;
                    } else {
                        yBlock[indexInSubBlock] = y[indexInOriginData];
                        uBlock[indexInSubBlock] = u[indexInOriginData];
                        vBlock[indexInSubBlock] = v[indexInOriginData];
                    }
                }
            }
        }

    }
    return true;
}

bool JpegEncoder::blocksToFDCT(const uint8_t yBlocks[][64], const uint8_t uBlocks[][64], const uint8_t vBlocks[][64],
                               const unsigned int &w, const unsigned int &h,
                               int yDCT[][64], int uDCT[][64], int vDCT[][64]) {
    int wBlockSize = w / 8 + (w % 8 == 0 ? 0 : 1);
    int hBlockSize = h / 8 + (h % 8 == 0 ? 0 : 1);
    int blockSize = wBlockSize * hBlockSize;
    std::shared_ptr<JpegDCT> fdct = std::make_shared<JpegDCT>();
    for (int blockIndex = 0; blockIndex < blockSize; blockIndex++) {
        const uint8_t *yBlock = yBlocks[blockIndex];
        const uint8_t *uBlock = uBlocks[blockIndex];
        const uint8_t *vBlock = vBlocks[blockIndex];
        for (int i = 0; i < 64; i++) {
            yDCT[blockIndex][i] = yBlock[i] - 128;
            uDCT[blockIndex][i] = uBlock[i] - 128;
            vDCT[blockIndex][i] = vBlock[i] - 128;

            yDCT[blockIndex][i] = yDCT[blockIndex][i] << 2;
            uDCT[blockIndex][i] = uDCT[blockIndex][i] << 2;
            vDCT[blockIndex][i] = vDCT[blockIndex][i] << 2;
        }
        fdct->fdct2d8x8(yDCT[blockIndex], nullptr);
        fdct->fdct2d8x8(uDCT[blockIndex], nullptr);
        fdct->fdct2d8x8(vDCT[blockIndex], nullptr);
    }
    return true;
}

bool JpegEncoder::fdctToQuant(int yDCT[][64], int uDCT[][64], int vDCT[][64],
                              const unsigned int &w, const unsigned int &h) {
    int wBlockSize = w / 8 + (w % 8 == 0 ? 0 : 1);
    int hBlockSize = h / 8 + (h % 8 == 0 ? 0 : 1);
    int blockSize = wBlockSize * hBlockSize;
    std::shared_ptr<JpegQuant> quant = std::make_shared<JpegQuant>();
    for (int blockIndex = 0; blockIndex < blockSize; blockIndex++) {
        int *yBlock = yDCT[blockIndex];
        int *uBlock = uDCT[blockIndex];
        int *vBlock = vDCT[blockIndex];
        quant->quantEncode8x8(yBlock, true);
        quant->quantEncode8x8(uBlock, false);
        quant->quantEncode8x8(vBlock, false);
    }
    return true;
}

bool JpegEncoder::quantToZigzag(int yQuant[][64], int uQuant[][64], int vQuant[][64],
                                const unsigned int &w, const unsigned int &h) {
    int wBlockSize = w / 8 + (w % 8 == 0 ? 0 : 1);
    int hBlockSize = h / 8 + (h % 8 == 0 ? 0 : 1);
    int blockSize = wBlockSize * hBlockSize;
    std::shared_ptr<JpegZigzag> zigzag = std::make_shared<JpegZigzag>();
    for (int blockIndex = 0; blockIndex < blockSize; blockIndex++) {
        int *yBlock = yQuant[blockIndex];
        int *uBlock = uQuant[blockIndex];
        int *vBlock = vQuant[blockIndex];
        zigzag->zigzag(yBlock);
        zigzag->zigzag(uBlock);
        zigzag->zigzag(vBlock);
    }
    return true;
}

bool JpegEncoder::writeToFile(char* buffer, long dataLength,
                              const unsigned int &w, const unsigned int &h) {
    FILE *fp = fopen(mOutputPath.data(), "wb");

    // SOI
    fputc(0xff, fp);
    fputc(0xd8, fp);

    // DQT
    const int *pqtab[2] = {JpegQuant::STD_QUANT_TAB_LUMIN, JpegQuant::STD_QUANT_TAB_CHROM};
    for (int i = 0; i < 2; i++) {
        int len = 2 + 1 + 64;
        fputc(0xff, fp);
        fputc(0xdb, fp);
        fputc(len >> 8, fp);
        fputc(len >> 0, fp);
        fputc(i, fp);
        for (int j = 0; j < 64; j++) {
            fputc(pqtab[i][JpegZigzag::ZIGZAG_INDEX[j]], fp);
        }
    }

    // SOF0
    int SOF0Len = 2 + 1 + 2 + 2 + 1 + 3 * 3;
    fputc(0xff, fp);
    fputc(0xc0, fp);
    fputc(SOF0Len >> 8, fp);
    fputc(SOF0Len >> 0, fp);
    fputc(8, fp); // precision 8bit
    fputc(h >> 8, fp); // height
    fputc(h >> 0, fp); // height
    fputc(w >> 8, fp); // width
    fputc(w >> 0, fp); // width
    fputc(3, fp);

    fputc(0x01, fp); fputc(0x11, fp); fputc(0x00, fp);
    fputc(0x02, fp); fputc(0x11, fp); fputc(0x01, fp);
    fputc(0x03, fp); fputc(0x11, fp); fputc(0x01, fp);

    // DHT AC
    const uint8_t *huftabAC[] = {
            HuffmanCodec::STD_HUFTAB_LUMIN_AC, HuffmanCodec::STD_HUFTAB_CHROM_AC
    };
    for (int i = 0; i < 2; i++) {
        fputc(0xff, fp);
        fputc(0xc4, fp);
        int len = 2 + 1 + 16;
        for (int j = 0; j < 16; j++) {
            len += huftabAC[i][j];
        }
        fputc(len >> 8, fp);
        fputc(len >> 0, fp);
        fputc(i + 0x10, fp);
        fwrite(huftabAC[i], len - 3, 1, fp);
    }
    // DHT DC
    const uint8_t *huftabDC[] = {
            HuffmanCodec::STD_HUFTAB_LUMIN_DC, HuffmanCodec::STD_HUFTAB_CHROM_DC
    };
    for (int i = 0; i < 2; i++) {
        fputc(0xff, fp);
        fputc(0xc4, fp);
        int len = 2 + 1 + 16;
        for (int j = 0; j < 16; j++) {
            len += huftabDC[i][j];
        }
        fputc(len >> 8, fp);
        fputc(len >> 0, fp);
        fputc(i + 0x00, fp);
        fwrite(huftabDC[i], len - 3, 1, fp);
    }

    // SOS
    int SOSLen = 2 + 1 + 2 * 3 + 3;
    fputc(0xff, fp);
    fputc(0xda, fp);
    fputc(SOSLen >> 8, fp);
    fputc(SOSLen >> 0, fp);
    fputc(3, fp);

    fputc(0x01, fp); fputc(0x00, fp);
    fputc(0x02, fp); fputc(0x11, fp);
    fputc(0x03, fp); fputc(0x11, fp);

    fputc(0x00, fp);
    fputc(0x3F, fp);
    fputc(0x00, fp);

    // data
    fwrite(buffer, dataLength, 1, fp);

    // EOI
    fputc(0xff, fp);
    fputc(0xd9, fp);

    fflush(fp);
    fclose(fp);

    return true;
}