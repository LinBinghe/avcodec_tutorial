//
// Created by 林炳河 on 2021/5/30.
//

#include "JpegQuant.hpp"

const int JpegQuant::STD_QUANT_TAB_LUMIN[64] = {
        16, 11, 10, 16, 24, 40, 51, 61,
        12, 12, 14, 19, 26, 58, 60, 55,
        14, 13, 16, 24, 40, 57, 69, 56,
        14, 17, 22, 29, 51, 87, 80, 62,
        18, 22, 37, 56, 68, 109, 103, 77,
        24, 35, 55, 64, 81, 104, 113, 92,
        49, 64, 78, 87, 103, 121, 120, 101,
        72, 92, 95, 98, 112, 100, 103, 99,
};
const int JpegQuant::STD_QUANT_TAB_CHROM[64] = {
        17, 18, 24, 47, 99, 99, 99, 99,
        18, 21, 26, 66, 99, 99, 99, 99,
        24, 26, 56, 99, 99, 99, 99, 99,
        47, 66, 99, 99, 99, 99, 99, 99,
        99, 99, 99, 99, 99, 99, 99, 99,
        99, 99, 99, 99, 99, 99, 99, 99,
        99, 99, 99, 99, 99, 99, 99, 99,
        99, 99, 99, 99, 99, 99, 99, 99,
};

JpegQuant::JpegQuant() {

}

JpegQuant::~JpegQuant() {

}

void JpegQuant::quantEncode8x8(int *data8x8, bool luminance) {
    for (int i = 0; i < 64; i++) {
        if (luminance) {
            data8x8[i] /= STD_QUANT_TAB_LUMIN[i];
        } else {
            data8x8[i] /= STD_QUANT_TAB_CHROM[i];
        }
    }
}