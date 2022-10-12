#include "renderer.h"

#include <fstream>
#include <stdexcept>

BMPFileHeader::BMPFileHeader()
    : file_type(0x4D42)
    , file_size(0)
    , reserved1(0)
    , reserved2(0)
    , offset(0)
{}

BMPInfoHeader::BMPInfoHeader() {}

BMPInfoHeader::BMPInfoHeader(int32_t _width, int32_t _height)
    : size(sizeof(BMPInfoHeader))
    , width(_width)
    , height(_height)
    , planes(1)
    , bit_count(24)
    , compression(0)
    , size_image(0)
    , xpels_per_meter(0)
    , ypels_per_meter(0)
    , colors_used(0)
    , colors_important(0)
{}

Color::Color()
    : r(0)
    , g(0)
    , b(0)
{}

Color::Color(uint8_t _r, uint8_t _g, uint8_t _b)
    : r(_r)
    , g(_g)
    , b(_b)
{}

Renderer::Renderer(int32_t _width, int32_t _height) {
    file_header_ = BMPFileHeader();
    info_header_ = BMPInfoHeader(_width, _height);
    
    file_header_.offset = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader);
    row_length_ = _width * kColorBytes_;
    table_.resize(row_length_ * _height);

    file_header_.file_size = file_header_.offset + table_.size() + _height * GetPadding();
}

uint32_t Renderer::GetPadding() const {
    return ((kAlignWidth_ - row_length_) % kAlignWidth_ + kAlignWidth_) % kAlignWidth_;
}

uint32_t Renderer::GetAlignedLength() const {
    return row_length_ + GetPadding();
}

void Renderer::SetColor(uint16_t x, uint16_t y, const Color& color_mask) {
    uint32_t b_channel_index = kColorBytes_ * (y * info_header_.width + x);
    uint32_t g_channel_index = b_channel_index + 1;
    uint32_t r_channel_index = b_channel_index + 2;

    if (!(0 <= b_channel_index && r_channel_index < table_.size())) {
        throw std::runtime_error("Segfault: trying to access non-existing cell");
    }

    table_[b_channel_index] = color_mask.b;
    table_[g_channel_index] = color_mask.g;
    table_[r_channel_index] = color_mask.r;
}

void Renderer::Export(const char* file_path) const {
    std::ofstream stream(file_path, std::ios::binary);

    if (stream.is_open()) {
        WriteHeaders(stream);

        uint32_t padding_length = GetPadding();

        if (padding_length == 0) {
            stream.write(reinterpret_cast<const char*>(table_.data()), table_.size());
        } else {
            std::vector<uint8_t> padding(padding_length);

            for (size_t y = 0; y < info_header_.height; ++y) {
                stream.write(reinterpret_cast<const char*>(table_.data() + row_length_ * y), row_length_);
                stream.write(reinterpret_cast<const char*>(padding.data()), padding_length);
            }
        }
    } else {
        throw std::runtime_error("Could not open or create the file.");
    } 
}

void Renderer::WriteHeaders(std::ofstream& stream) const {
    stream.write(reinterpret_cast<const char*>(&file_header_), sizeof(file_header_));
    stream.write(reinterpret_cast<const char*>(&info_header_), sizeof(info_header_));
}
