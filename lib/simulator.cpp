#include "renderer/renderer.h"
#include "simulator.h"

#include <cassert>
#include <chrono>
#include <iostream>
#include <utility>
#include <queue>

CellData::CellData() {}

CellData::CellData(uint16_t _x, uint16_t _y, uint64_t _grains)
    : x(_x)
    , y(_y)
    , grains(_grains)
{}

InputProperties::InputProperties() {}

void Simulator::Expand(
    int32_t& x,
    int32_t& y,
    std::deque<std::deque<uint64_t>>& table,
    std::deque<std::deque<uint64_t>>& visited
) {
    if (!(0 <= x && x < props_.width)) {
        if (x < 0) {
            ExpandWidthLeft(table, visited);
            ++shift_x;
            ++x;
        } else {
            ExpandWidthRight(table, visited);
        }

        ++props_.width;
    } else {
        if (y < 0) {
            ExpandHeightDown(table, visited);
            ++shift_y;
            ++y;
        } else {
            ExpandHeightUp(table, visited);
        }

        ++props_.height;
    }
}

void Simulator::ExpandWidthLeft(
    std::deque<std::deque<uint64_t>>& table,
    std::deque<std::deque<uint64_t>>& visited
) {
    for (size_t current_row_index = 0; current_row_index < props_.height; ++current_row_index) {
        table[current_row_index].push_front(0);
        visited[current_row_index].push_front(false);
    }
}

void Simulator::ExpandWidthRight(
    std::deque<std::deque<uint64_t>>& table,
    std::deque<std::deque<uint64_t>>& visited
) {
    for (size_t current_row_index = 0; current_row_index < props_.height; ++current_row_index) {
        table[current_row_index].push_back(0);
        visited[current_row_index].push_back(false);
    }
}

void Simulator::ExpandHeightUp(
    std::deque<std::deque<uint64_t>>& table,
    std::deque<std::deque<uint64_t>>& visited
) {
    table.push_back(std::deque<uint64_t>(props_.width));
    visited.push_back(std::deque<uint64_t>(props_.width));
}

void Simulator::ExpandHeightDown(
    std::deque<std::deque<uint64_t>>& table,
    std::deque<std::deque<uint64_t>>& visited
) {
    table.push_front(std::deque<uint64_t>(props_.width));
    visited.push_front(std::deque<uint64_t>(props_.width));
}

bool Simulator::CheckBorders(int32_t x, int32_t y) const {
    return 0 <= y && y < props_.height && 0 <= x && x < props_.width;
}

void Simulator::RenderFrame(const std::deque<std::deque<uint64_t>>& table, uint64_t frame_number) const {
    assert(props_.height == table.size() && table.size() > 0);
    assert(props_.width == table[0].size());

    auto time_before = std::chrono::high_resolution_clock::now();

    std::cout << "Rendering " << frame_number << " frame... " << std::flush;

    Renderer renderer(props_.width, props_.height);

    for (size_t y = 0; y < props_.height; ++y) {
        for (size_t x = 0; x < props_.width; ++x) {
            if (table[y][x] == 0) {
                renderer.SetColor(x, y, kWhiteColor);
            } else if (table[y][x] == 1) {
                renderer.SetColor(x, y, kGreenColor);
            } else if (table[y][x] == 2) {
                renderer.SetColor(x, y, kPurpleColor);
            } else if (table[y][x] == 3) {
                renderer.SetColor(x, y, kYellowColor);
            } else if (table[y][x] >= 4) {
                renderer.SetColor(x, y, kBlackColor);
            }
        }
    }

    std::string output_path = props_.output_path + "frame" + std::to_string(frame_number) + ".bmp";

    renderer.Export(output_path.c_str());

    auto time_after = std::chrono::high_resolution_clock::now();

    std::cout << "Time elapsed: " << std::chrono::duration_cast<std::chrono::milliseconds>(time_after - time_before).count() << " ms" << std::endl;
}

bool Simulator::CheckIfRenderNeeded(uint64_t iteration) const {
    return (props_.render_frequency != 0 && iteration % props_.render_frequency == 0)
         || (props_.render_frequency == 0 && iteration == props_.max_iterations - 1);
}

void Simulator::Run() {
    std::vector<std::pair<int16_t, int16_t>> ways_to_move = {
        {-1, 0},
        {1, 0},
        {0, -1},
        {0, 1}
    };

    std::deque<std::deque<uint64_t>> table(props_.height, std::deque<uint64_t>(props_.width));
    std::vector<std::queue<std::pair<int32_t, int32_t>>> bad_cells(2);

    for (CellData& cell: input_data_) {
        if (!CheckBorders(cell.x, cell.y)) {
            throw std::runtime_error("Segfault: one of the cells is out of borders");
        }
        
        table[cell.y][cell.x] = cell.grains;

        if (cell.grains >= 4) {
            bad_cells[0].push({cell.x, cell.y});
        }
    }

    std::deque<std::deque<uint64_t>> visited(props_.height, std::deque<uint64_t>(props_.width));
    uint64_t last_rendered_frame = 0;

    for (uint64_t iteration = 0;; ++iteration) {
        if (props_.max_iterations != 0 && iteration >= props_.max_iterations) {
            if (iteration - 1 != last_rendered_frame) {
                RenderFrame(table, iteration - 1);
                last_rendered_frame = iteration - 1;
            }
            
            break;
        }

        auto time_before = std::chrono::high_resolution_clock::now();

        std::cout << "Simulating " << iteration + 1 << " iteration... " << std::flush;

        uint8_t queue_index = iteration % 2;
        uint8_t other_queue_index = queue_index ^ 1;
        std::queue<std::pair<int32_t, int32_t>>& current_queue = bad_cells[queue_index];

        if (current_queue.empty()) {
            if (iteration != last_rendered_frame) {
                RenderFrame(table, iteration + 1);
                last_rendered_frame = iteration;
            }

            break;
        }

        assert(bad_cells[other_queue_index].empty());

        while (!current_queue.empty()) {
            auto [x, y] = current_queue.front();
            uint16_t old_shift_x = shift_x;
            uint16_t old_shift_y = shift_y;

            x += shift_x;
            y += shift_y;

            current_queue.pop();

            if (visited[y][x] > iteration || table[y][x] < 4) {
                continue;
            }

            visited[y][x] = iteration + 1;

            uint64_t add_to_neighbours = table[y][x] / 4;
            table[y][x] %= 4;

            for (auto [dx, dy]: ways_to_move) {
                int32_t new_x = x - old_shift_x + shift_x + dx;
                int32_t new_y = y - old_shift_y + shift_y + dy;

                if (!CheckBorders(new_x, new_y)) {
                    Expand(new_x, new_y, table, visited);
                }

                table[new_y][new_x] += add_to_neighbours;

                if (table[new_y][new_x] >= 4) {
                    bad_cells[other_queue_index].push({new_x - shift_x, new_y - shift_y});
                }
            }
        }

        auto time_after = std::chrono::high_resolution_clock::now();

        std::cout << "Time elapsed: " << std::chrono::duration_cast<std::chrono::milliseconds>(time_after - time_before).count() << " ms" << std::endl;

        if (CheckIfRenderNeeded(iteration)) {
            RenderFrame(table, iteration + 1);
            last_rendered_frame = iteration;
        }
    }
}

Simulator::Simulator(const InputProperties& _props, const std::vector<CellData>& _input_data)
    : props_(_props)
    , input_data_(_input_data)
    , shift_x(0)
    , shift_y(0)
{}
