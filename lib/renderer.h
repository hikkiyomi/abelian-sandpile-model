#pragma once

#include <cstdint>
#include <fstream>
#include <vector>

#pragma pack(push, 1)
struct BMPFileHeader {
    uint16_t file_type;
    uint32_t file_size;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t offset;

    BMPFileHeader();
};
#pragma pack(pop)

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
    BMPInfoHeader(int32_t _width, int32_t _height);
};

class Renderer {
public:
    BMPFileHeader file_header_;
    BMPInfoHeader info_header_;

    std::vector<uint8_t> table_;

    Renderer(int32_t _width, int32_t _height);

    void Export(const char* file_path);
private:
    static const uint8_t kColorBytes_ = 3;
    static const uint8_t kAlignWidth_ = 4;

    uint32_t row_length_;

    uint32_t GetPadding() const;
    uint32_t GetAlignedLength() const;
    void WriteHeaders(std::ofstream& stream);
};
