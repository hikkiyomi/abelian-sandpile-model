#include "lib/renderer.h"

#include <iostream>

int main() {
    Renderer renderer(640, 480);
    int channels = 3;

    for (int y = 20; y < 220; ++y) {
        for (int x = 50; x < 150; ++x) {
            renderer.table_[channels * (y * renderer.info_header_.width + x) + 0] = 100;
            renderer.table_[channels * (y * renderer.info_header_.width + x) + 1] = 130;
            renderer.table_[channels * (y * renderer.info_header_.width + x) + 2] = 255;
        }
    }

    renderer.Export("../../test.bmp");

    return 0;
}
