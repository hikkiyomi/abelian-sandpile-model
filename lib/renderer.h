#pragma once

#include <cstdint>

class Renderer {
    struct BMPFileHeader {
        uint16_t file_type;
        uint32_t file_size;
        uint16_t reserved1;
        uint16_t reserved2;
        uint32_t offset;

        BMPFileHeader();
    };

    struct BMPInfoHeader {
        uint32_t size;
        int32_t width;
        int32_t height;
        uint16_t planes;
        uint16_t bit_count;
        uint32_t compression;
        uint32_t size_image;
        int32_t xpels_per_meter;
        int32_t ypels_per_meter;
        uint32_t colors_used;
        uint32_t colors_important;

        BMPInfoHeader();
        BMPInfoHeader(int _width, int _height);
    };
public:
    Renderer(int _width, int _height);
private:
    BMPFileHeader file_header_;
    BMPInfoHeader info_header_;

    static const uint8_t color_bits_ = 3;
};
