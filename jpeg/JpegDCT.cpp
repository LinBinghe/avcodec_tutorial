//
// Created by 林炳河 on 2021/5/30.
//

#include "JpegDCT.hpp"

JpegDCT::JpegDCT() {
    int i, j;
    float factor[64];

    // 初始化fdct factor
    const float AAN_DCT_FACTOR[8] = {
            1.0f, 1.387039845f, 1.306562965f, 1.175875602f,
            1.0f, 0.785694958f, 0.541196100f, 0.275899379f,
    };
    for (i = 0; i < DCT_SIZE; i++) {
        for (j = 0; j < DCT_SIZE; j++) {
            factor[i * 8 + j] = 1.0f / (AAN_DCT_FACTOR[i] * AAN_DCT_FACTOR[j] * 8);
        }
    }
    for (i = 0; i < 64; i++) {
        mFDCTFactor[i] = float2Fix(factor[i]);
    }
}

JpegDCT::~JpegDCT() {

}

int JpegDCT::float2Fix(float value) {
    return static_cast<int>((value) * (1 << FIX_Q));
}

void JpegDCT::fdct2d8x8(int *data8x8, int *ftab) {
    int tmp0, tmp1, tmp2, tmp3;
    int tmp4, tmp5, tmp6, tmp7;
    int tmp10, tmp11, tmp12, tmp13;
    int z1, z2, z3, z4, z5, z11, z13;
    int *dataptr;
    int ctr;

    /* Pass 1: process rows. */
    dataptr = data8x8;
    for (ctr = 0; ctr < DCT_SIZE; ctr++) {
        tmp0 = dataptr[0] + dataptr[7];
        tmp7 = dataptr[0] - dataptr[7];
        tmp1 = dataptr[1] + dataptr[6];
        tmp6 = dataptr[1] - dataptr[6];
        tmp2 = dataptr[2] + dataptr[5];
        tmp5 = dataptr[2] - dataptr[5];
        tmp3 = dataptr[3] + dataptr[4];
        tmp4 = dataptr[3] - dataptr[4];

        /* Even part */
        tmp10 = tmp0 + tmp3;  /* phase 2 */
        tmp13 = tmp0 - tmp3;
        tmp11 = tmp1 + tmp2;
        tmp12 = tmp1 - tmp2;

        dataptr[0] = tmp10 + tmp11;  /* phase 3 */
        dataptr[4] = tmp10 - tmp11;

        z1 = (tmp12 + tmp13) * float2Fix(0.707106781f) >> FIX_Q; /* c4 */
        dataptr[2] = tmp13 + z1;  /* phase 5 */
        dataptr[6] = tmp13 - z1;

        /* Odd part */
        tmp10 = tmp4 + tmp5;  /* phase 2 */
        tmp11 = tmp5 + tmp6;
        tmp12 = tmp6 + tmp7;

        /* The rotator is modified from fig 4-8 to avoid extra negations. */
        z5 = (tmp10 - tmp12) * float2Fix(0.382683433f) >> FIX_Q;  /* c6 */
        z2 = (float2Fix(0.541196100f) * tmp10 >> FIX_Q) + z5;     /* c2-c6 */
        z4 = (float2Fix(1.306562965f) * tmp12 >> FIX_Q) + z5;     /* c2+c6 */
        z3 = tmp11 * float2Fix(0.707106781f) >> FIX_Q;            /* c4 */

        z11 = tmp7 + z3;        /* phase 5 */
        z13 = tmp7 - z3;

        dataptr[5] = z13 + z2;  /* phase 6 */
        dataptr[3] = z13 - z2;
        dataptr[1] = z11 + z4;
        dataptr[7] = z11 - z4;

        dataptr += DCT_SIZE;     /* advance pointer to next row */
    }

    /* Pass 2: process columns. */
    dataptr = data8x8;
    for (ctr = 0; ctr < DCT_SIZE; ctr++) {
        tmp0 = dataptr[DCT_SIZE * 0] + dataptr[DCT_SIZE * 7];
        tmp7 = dataptr[DCT_SIZE * 0] - dataptr[DCT_SIZE * 7];
        tmp1 = dataptr[DCT_SIZE * 1] + dataptr[DCT_SIZE * 6];
        tmp6 = dataptr[DCT_SIZE * 1] - dataptr[DCT_SIZE * 6];
        tmp2 = dataptr[DCT_SIZE * 2] + dataptr[DCT_SIZE * 5];
        tmp5 = dataptr[DCT_SIZE * 2] - dataptr[DCT_SIZE * 5];
        tmp3 = dataptr[DCT_SIZE * 3] + dataptr[DCT_SIZE * 4];
        tmp4 = dataptr[DCT_SIZE * 3] - dataptr[DCT_SIZE * 4];

        /* Even part */
        tmp10 = tmp0 + tmp3;  /* phase 2 */
        tmp13 = tmp0 - tmp3;
        tmp11 = tmp1 + tmp2;
        tmp12 = tmp1 - tmp2;

        dataptr[DCT_SIZE * 0] = tmp10 + tmp11;  /* phase 3 */
        dataptr[DCT_SIZE * 4] = tmp10 - tmp11;

        z1 = (tmp12 + tmp13) * float2Fix(0.707106781f) >> FIX_Q; /* c4 */
        dataptr[DCT_SIZE * 2] = tmp13 + z1;  /* phase 5 */
        dataptr[DCT_SIZE * 6] = tmp13 - z1;

        /* Odd part */
        tmp10 = tmp4 + tmp5;  /* phase 2 */
        tmp11 = tmp5 + tmp6;
        tmp12 = tmp6 + tmp7;

        /* The rotator is modified from fig 4-8 to avoid extra negations. */
        z5 = (tmp10 - tmp12) * float2Fix(0.382683433f) >> FIX_Q;  /* c6 */
        z2 = (float2Fix(0.541196100f) * tmp10 >> FIX_Q) + z5;     /* c2-c6 */
        z4 = (float2Fix(1.306562965f) * tmp12 >> FIX_Q) + z5;     /* c2+c6 */
        z3 = tmp11 * float2Fix(0.707106781f) >> FIX_Q;            /* c4 */

        z11 = tmp7 + z3;  /* phase 5 */
        z13 = tmp7 - z3;

        dataptr[DCT_SIZE * 5] = z13 + z2; /* phase 6 */
        dataptr[DCT_SIZE * 3] = z13 - z2;
        dataptr[DCT_SIZE * 1] = z11 + z4;
        dataptr[DCT_SIZE * 7] = z11 - z4;

        dataptr++;  /* advance pointer to next column */
    }

    ftab = ftab ? ftab : mFDCTFactor;
    for (ctr = 0; ctr < 64; ctr++) {
        data8x8[ctr] *= ftab[ctr];
        data8x8[ctr] >>= FIX_Q + 2;
    }
}