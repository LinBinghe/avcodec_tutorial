//
// Created by 林炳河 on 2021/5/30.
//

#ifndef JPEG_HUFFMANCODEC_HPP
#define JPEG_HUFFMANCODEC_HPP

#include <cstdint>

/**
 * RLE 数据定义
 */
typedef struct {
    unsigned runlen: 4;
    unsigned codesize: 4;
    unsigned codedata: 16;
} RLEITEM;

/**
 * 霍夫曼编码表项类型定义
 */
typedef struct {
    int symbol; /* 符号 */
    int freq;   /* 频率 */
    int group;  /* 分组 */
    int depth;  /* 码长 */
    int code;   /* 码字 */
} HUFCODEITEM;

class HuffmanCodec {
public:
    HuffmanCodec();

    ~HuffmanCodec();

    long encode(const int yBlocks[][64], const int uBlocks[][64], const int vBlocks[][64],
                const unsigned int &w, const unsigned int &h);

    char* getResult();
private:
    void initCoddList(bool dc, bool luminance);

    void encodeBlock(const int *const block, int &dc, bool luminance);

    void categoryEncode(int &code, int &size);

    bool huffmanEncode(HUFCODEITEM *codeList, int size);

public:
    static const uint8_t STD_HUFTAB_LUMIN_AC[];
    static const uint8_t STD_HUFTAB_LUMIN_DC[];
    static const uint8_t STD_HUFTAB_CHROM_AC[];
    static const uint8_t STD_HUFTAB_CHROM_DC[];

private:
    char *mBuffer;
    void *mBitStream;
    HUFCODEITEM mCodeListDCLumin[256]; // 编码表-DC-Chrominance
    HUFCODEITEM mCodeListDCChrom[256]; // 编码表-DC-Chrominance
    HUFCODEITEM mCodeListACLumin[256]; // 编码表-AC-Luminance
    HUFCODEITEM mCodeListACChrom[256]; // 编码表-AC-Chrominance

    const uint8_t MAX_HUFFMAN_CODE_LEN = 16;
};


#endif //JPEG_HUFFMANCODEC_HPP
