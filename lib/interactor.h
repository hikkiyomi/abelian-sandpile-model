#pragma once

#include "simulator.h"

#include <cstdint>
#include <string>
#include <vector>

class Interactor {
public:
    Interactor(int argc, char* argv[]);

    void PrintInputData();
    void Run();
private:
    InputProperties props_;
    std::vector<CellData> input_data_;

    void ParseConsoleArguments(int argc, char* argv[]);
    void ReadInputData();
};
