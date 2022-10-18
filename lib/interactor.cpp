#include "interactor.h"
#include "simulator.h"

#include <chrono>
#include <cstring>
#include <iostream>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>

void PrintHelpList() {
    std::cout << "While running program, there are some arguments that must be provided." << std::endl;

    std::cout << "-w (--width) - sets the initial width of the output image. - necessary" << std::endl;
    std::cout << "-l (--length) - sets the initial length of the output image. - necessary" << std::endl;
    std::cout << "-i (--input) - reading input file where input data is stored. - necessary" << std::endl;
    std::cout << "-o (--output) - sets the output directory where output data will be saved. - necessary" << std::endl;
    std::cout << "-m (--max-iter) - sets the maximum amount of iterations simulator will do. - optional (standard value is 0 - there is no limit of iterations)" << std::endl;
    std::cout << "-f (--freq) - sets the frequency of rendering images. - necessary (set this to 0 if only image of the last iteration is needed)" << std::endl;

    std::cout << std::endl;
    
    std::cout << "Setting values to parameters can be done like this:" << std::endl;
    std::cout << "--width 640" << std::endl;
    std::cout << "     or    " << std::endl;
    std::cout << "--width=640" << std::endl;
}

std::vector<std::string> ParseMonoOption(char* arg) {
    std::vector<std::string> result;
    std::string buff;
    bool equal_sign_seen = false;

    for (size_t i = 0; arg[i] != '\0'; ++i) {
        if (arg[i] == '=' && !equal_sign_seen) {
            result.emplace_back(buff);
            buff.clear();

            equal_sign_seen = true;
        } else {
            buff += std::string(1, arg[i]);
        }
    }

    if (!buff.empty()) {
        result.emplace_back(buff);
    }

    if (result.size() != 2) {
        throw std::runtime_error("Wrong arguments. Use --help for more information.");
    }

    return result;
}

uint16_t ToUInt16(const std::string& arg) {
    uint16_t result = 0;

    for (size_t i = 0; i < arg.size(); ++i) {
        result = result * 10 + static_cast<uint8_t>(arg[i] - '0');
    }

    return result;
}

uint64_t ToUInt64(const std::string& arg) {
    uint64_t result = 0;

    for (size_t i = 0; i < arg.size(); ++i) {
        result = result * 10 + static_cast<uint8_t>(arg[i] - '0');
    }

    return result;
}

bool CheckIfEachArgumentProvided(uint16_t arguments_bitmask) {
    const uint16_t full_bitmask = (1 << 5) - 1;
    // full_bitmask has this value because (1 << 5) equals to 100000 in binary,
    // and subtracting 1 from this value gives 11111 in binary,
    // which corresponds to the case where each argument was provided.

    return arguments_bitmask == full_bitmask;
}

void Interactor::ParseConsoleArguments(int argc, char* argv[]) {
    uint16_t arguments_bitmask = 0;

    for (int i = 1; i < argc;) {
        if (argv[i][0] == '-') {
            if (strcmp(argv[i], "--help") == 0) {
                PrintHelpList();
                exit(0);
            }

            std::vector<std::string> params;
            
            if (i == argc - 1 || argv[i + 1][0] == '-') {
                params = ParseMonoOption(argv[i]);
                ++i;
            } else {
                params = {argv[i], argv[i + 1]};
                i += 2;
            }

            if (params[0] == "-l" || params[0] == "--length") {
                props_.height = ToUInt16(params[1]);
                arguments_bitmask |= (1 << 0);
            } else if (params[0] == "-w" || params[0] == "--width") {
                props_.width = ToUInt16(params[1]);
                arguments_bitmask |= (1 << 1);
            } else if (params[0] == "-i" || params[0] == "--input") {
                props_.input_path = params[1];
                arguments_bitmask |= (1 << 2);
            } else if (params[0] == "-o" || params[0] == "--output") {
                props_.output_path = params[1];
                arguments_bitmask |= (1 << 3);
            } else if (params[0] == "-m" || params[0] == "--max-iter") {
                props_.max_iterations = ToUInt64(params[1]);
            } else if (params[0] == "-f" || params[0] == "--freq") {
                props_.render_frequency = ToUInt64(params[1]);
                arguments_bitmask |= (1 << 4);
            } else {
                std::runtime_error("Unknown argument " + std::string(params[0]) + ".");
            }
        } else {
            throw std::runtime_error("Wrong arguments. Use --help for more information.");
        }
    }

    if (!CheckIfEachArgumentProvided(arguments_bitmask)) {
        throw std::runtime_error("Not enough arguments.");
    }
}

void Interactor::ReadInputData() {
    std::ifstream stream(props_.input_path);
    
    if (!stream.is_open()) {
        throw std::runtime_error("Cannot read input file.");
    }

    uint16_t input_x;
    uint16_t input_y;
    uint64_t input_grains;

    while (stream >> input_x >> input_y >> input_grains) {
        input_data_.emplace_back(input_x, input_y, input_grains);
    }
}

void Interactor::PrintInputData() {
    for (auto& cell : input_data_) {
        std::cout << cell.x << "\t" << cell.y << "\t" << cell.grains << std::endl;
    }
}

void Interactor::Run() {
    std::cout << "Reading input data..." << std::endl;
    ReadInputData();
    std::cout << "Done reading..." << std::endl;

    auto time_before = std::chrono::high_resolution_clock::now();

    std::cout << "Starting simulation and rendering..." << std::endl;
    Simulator simulator(props_, input_data_);
    simulator.Run();
    std::cout << "Rendering done." << std::endl;

    auto time_after = std::chrono::high_resolution_clock::now();

    std::cout << std::endl << "Total time elapsed: " << std::chrono::duration_cast<std::chrono::milliseconds>(time_after - time_before).count() << " ms" << std::endl;
}

Interactor::Interactor(int argc, char* argv[]) 
    : props_()
{
    props_.max_iterations = 0;
    props_.render_frequency = 0;

    std::cout << "Parsing arguments..." << std::endl;
    ParseConsoleArguments(argc, argv);
    std::cout << "Done parsing..." << std::endl;
}
