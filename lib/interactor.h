#pragma once

#include <cstdint>
#include <string>

class Interactor {
public:
    Interactor(int argc, char* argv[]);
private:
    uint16_t height_;
    uint16_t width_;

    uint64_t max_iterations_;
    uint64_t render_frequency_;

    std::string input_path_;
    std::string output_path_;

    void ParseConsoleArguments(int argc, char* argv[]);
    void ReadInputData();
};
