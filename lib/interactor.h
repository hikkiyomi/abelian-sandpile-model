#pragma once

#include <cstdint>
#include <string>
#include <vector>

struct CellData {
    uint16_t x;
    uint16_t y;
    uint64_t grains;

    CellData();
    CellData(uint16_t _x, uint16_t _y, uint64_t _grains);
};

class Interactor {
public:
    Interactor(int argc, char* argv[]);

    void PrintInputData();
private:
    uint16_t height_;
    uint16_t width_;

    uint64_t max_iterations_;
    uint64_t render_frequency_;

    std::string input_path_;
    std::string output_path_;

    std::vector<CellData> input_data_;

    void ParseConsoleArguments(int argc, char* argv[]);
    void ReadInputData();
};
