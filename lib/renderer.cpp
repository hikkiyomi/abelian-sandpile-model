#include "renderer.h"

Renderer::BMPFileHeader::BMPFileHeader()
    : file_type(0x4D42)
    , file_size(0)
    , reserved1(0)
    , reserved2(0)
    , offset(0)
{}

Renderer::BMPInfoHeader::BMPInfoHeader(int _width, int _height)
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

Renderer::Renderer(int _width, int _height) {
    file_header_ = BMPFileHeader();
    info_header_ = BMPInfoHeader(_width, _height);

    // TODO: correct initialization for renderer
    // * file_header_.offset = sizeof(file_header_) + sizeof(info_header_);
    // * file_header_.file_size = file_header_.offset + color_bits_ * _width * _height;
}
