#pragma once

#include <cstdint>
#include <deque>
#include <string>
#include <vector>

struct CellData {
    uint16_t x;
    uint16_t y;
    uint64_t grains;

    CellData();
    CellData(uint16_t _x, uint16_t _y, uint64_t _grains);
};

struct InputProperties {
    uint16_t height;
    uint16_t width;

    uint64_t max_iterations;
    uint64_t render_frequency;

    std::string input_path;
    std::string output_path;

    InputProperties();
};

class Simulator {
public:
    Simulator(const InputProperties& _props, const std::vector<CellData>& _input_data);
private:
    InputProperties props_;
    std::vector<CellData> input_data_;

    uint16_t shift_x;
    uint16_t shift_y;

    bool CheckBorders(int32_t x, int32_t y) const;
    
    void Expand(int32_t& x, int32_t& y, std::deque<std::deque<uint64_t>>& table, std::deque<std::deque<uint64_t>>& visited);
    void ExpandWidthLeft(std::deque<std::deque<uint64_t>>& table, std::deque<std::deque<uint64_t>>& visited);
    void ExpandWidthRight(std::deque<std::deque<uint64_t>>& table, std::deque<std::deque<uint64_t>>& visited);
    void ExpandHeightUp(std::deque<std::deque<uint64_t>>& table, std::deque<std::deque<uint64_t>>& visited);
    void ExpandHeightDown(std::deque<std::deque<uint64_t>>& table, std::deque<std::deque<uint64_t>>& visited);

    void RenderFrame(const std::deque<std::deque<uint64_t>>& table, uint64_t frame_number) const;
};
