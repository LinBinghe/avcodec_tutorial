#include <iostream>
#include "JpegEncoder.hpp"

uint32_t rainbowColors[] = {
        0XFF0000, // 红
        0XFFA500, // 橙
        0XFFFF00, // 黄
        0X00FF00, // 绿
        0X007FFF, // 青
        0X0000FF, // 蓝
        0X8B00FF  // 紫
};

int main() {
    // 先自己造一份RGB24的数据
    int width = 270, height = 270;
    uint8_t redData[width * height];
    uint8_t greenData[width * height];
    uint8_t blueData[width * height];
    for (int i = 0; i < width; ++i) {
        for (int j = 0; j < height; j++) {
            int randomIndex = rand() % 7;
            int currentIndex = (i * height + j);
            uint32_t currentColor = rainbowColors[randomIndex];
            // 当前颜色 R 分量
            uint8_t R = (currentColor & 0xFF0000) >> 16;
            // 当前颜色 G 分量
            uint8_t G = (currentColor & 0x00FF00) >> 8;
            // 当前颜色 B 分量
            uint8_t B = currentColor & 0x0000FF;
            redData[currentIndex] = R;
            greenData[currentIndex] = G;
            blueData[currentIndex] = B;
        }
    }
    const char *homeDir = getenv("HOME");
    if (homeDir) {
        printf("[JpegEncoder] outputPath is %s\n", (std::string(homeDir) + "/Desktop/avcodec_tutorial_jpeg_E.jpeg").data());
    } else {
        printf("[JpegEncoder] can't get HOME path.");
        return -1;
    }
    std::shared_ptr<JpegEncoder> jpegEncoder =
            std::make_shared<JpegEncoder>((std::string(homeDir) + "/Desktop/avcodec_tutorial_jpeg_E.jpeg"));
    jpegEncoder->encodeRGB24(redData, blueData, greenData, width, height);
    return 0;
}
