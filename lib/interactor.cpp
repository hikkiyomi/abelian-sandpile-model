#include "interactor.h"
#include "simulator.h"

#include <cstring>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>

std::vector<std::string> ParseMonoOption(char* arg) {
    std::vector<std::string> result;
    std::string buff;
    bool equalSignSeen = false;
    
    for (size_t i = 0; arg[i] != '\0'; ++i) {
        if (arg[i] == '=' && !equalSignSeen) {
            result.emplace_back(buff);
            buff.clear();

            equalSignSeen = true;
        } else {
            buff += std::string(1, arg[i]);
        }
    }

    if (!buff.empty()) {
        result.emplace_back(buff);
    }

    if (result.size() != 2) {
        throw std::runtime_error("Wrong arguments.");
    }

    return result;
}

uint16_t ToUInt16(const std::string& arg) {
    uint16_t result = 0;

    for (size_t i = 0; arg[i] != '\0'; ++i) {
        result = result * 10 + static_cast<uint8_t>(arg[i] - '0');
    }

    return result;
}

uint64_t ToUInt64(const std::string& arg) {
    uint64_t result = 0;

    for (size_t i = 0; arg[i] != '\0'; ++i) {
        result = result * 10 + static_cast<uint8_t>(arg[i] - '0');
    }

    return result;
}

void Interactor::ParseConsoleArguments(int argc, char* argv[]) {
    uint16_t arguments_bitmask = 0;

    for (int i = 1; i < argc;) {
        if (argv[i][0] == '-') {
            std::vector<std::string> params;
            
            if (i == argc - 1 || argv[i + 1][0] == '-') {
                params = ParseMonoOption(argv[i]);
                ++i;
            } else {
                params = {argv[i], argv[i + 1]};
                i += 2;
            }

            if (params[0] == "-l" || params[0] == "--length") {
                props_.height_ = ToUInt16(params[1]);
                arguments_bitmask |= (1 << 0);
            } else if (params[0] == "-w" || params[0] == "--width") {
                props_.width_ = ToUInt16(params[1]);
                arguments_bitmask |= (1 << 1);
            } else if (params[0] == "-i" || params[0] == "--input") {
                props_.input_path_ = params[1];
                arguments_bitmask |= (1 << 2);
            } else if (params[0] == "-o" || params[0] == "--output") {
                props_.output_path_ = params[1];
                arguments_bitmask |= (1 << 3);
            } else if (params[0] == "-m" || params[0] == "--max-iter") {
                props_.max_iterations_ = ToUInt64(params[1]);
            } else if (params[0] == "-f" || params[0] == "--freq") {
                props_.render_frequency_ = ToUInt64(params[1]);
                arguments_bitmask |= (1 << 4);
            } else {
                std::runtime_error("Unknown argument " + std::string(params[0]) + ".");
            }
        } else {
            throw std::runtime_error("Wrong arguments.");
        }
    }

    if (arguments_bitmask != (1 << 5) - 1) {
        throw std::runtime_error("Not enough arguments.");
    }
}

void Interactor::ReadInputData() {
    std::ifstream stream(props_.input_path_);
    
    if (stream.is_open()) {
        uint16_t input_x;
        uint16_t input_y;
        uint64_t input_grains;

        while (stream >> input_x >> input_y >> input_grains) {
            input_data_.emplace_back(input_x, input_y, input_grains);
        }
    } else {
        throw std::runtime_error("Cannot read input file.");
    }
}

void Interactor::PrintInputData() {
    for (auto& cell : input_data_) {
        std::cout << cell.x << "\t" << cell.y << "\t" << cell.grains << std::endl;
    }
}

Interactor::Interactor(int argc, char* argv[]) 
    : props_()
{
    props_.max_iterations_ = 0;
    props_.render_frequency_ = 0;

    std::cout << "Parsing arguments..." << std::endl;
    ParseConsoleArguments(argc, argv);
    std::cout << "Done parsing..." << std::endl;

    std::cout << "Reading input data..." << std::endl;
    ReadInputData();
    std::cout << "Done reading..." << std::endl;

    std::cout << "Starting simulation and rendering..." << std::endl;
    Simulator(props_, input_data_);
    std::cout << "Rendering done." << std::endl;
}
