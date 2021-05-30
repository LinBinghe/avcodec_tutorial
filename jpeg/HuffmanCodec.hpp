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

/**
 * 霍夫曼编码器
 */
class HuffmanCodec {
public:
    HuffmanCodec();

    ~HuffmanCodec();

    /**
     * 将yuv数据进行熵编码，其中包含了DC的DPCM、AC的RLE以及DC/AC的中间格式计算，最后再进行霍夫曼编码
     * @return 压缩后数据长度
     */
    long encode(const int yBlocks[][64], const int uBlocks[][64], const int vBlocks[][64],
                const unsigned int &w, const unsigned int &h);

    /**
     * 返回 #encode 接口处理后的最终数据
     */
    char* getResult();
private:
    /**
     * 初始化编码表
     * @param dc 是否是DC系数
     * @param luminance 是否是亮度通道
     */
    void initCodeList(bool dc, bool luminance);

    /**
     * 对单个8x8的子块进行编码
     * @param block 8x8子块数据
     * @param dc 上一次的DC系数，处理后将更新为当前子块的DC系数
     * @param luminance 是否亮度通道
     */
    void encodeBlock(const int *const block, int &dc, bool luminance);

    /**
     * DC/AC中间格式计算，本质是为其"分类"到一个"组"中
     * @param code 个人理解是要编码的值如DC的差值，内部会做再次计算
     * @param size 该值是内部计算给外部的，个人理解是对应霍夫曼编码表中的索引
     */
    void categoryEncode(int &code, int &size);

    /**
     * 进行霍夫曼编码
     * @param codeList 编码表
     * @param size 个人理解就是编码表的索引，编码该索引对应的编码表中的值
     */
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
