#include "renderer/renderer.h"
#include "simulator.h"

#include <cassert>
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

void Simulator::expand(
    uint16_t x,
    uint16_t y,
    std::deque<std::deque<uint64_t>>& table,
    std::deque<std::deque<bool>>& visited
) {
    if (!(0 <= x && x < props_.width_)) {
        if (x < 0) {
            expand_width_left(table, visited);
            ++shift_x;
        } else {
            expand_width_right(table, visited);
        }

        ++props_.width_;
    } else {
        if (y < 0) {
            expand_height_up(table, visited);
        } else {
            expand_height_down(table, visited);
            ++shift_y;
        }

        ++props_.height_;
    }
}

void Simulator::expand_width_left(
    std::deque<std::deque<uint64_t>>& table,
    std::deque<std::deque<bool>>& visited
) {
    for (size_t current_row_index = 0; current_row_index < props_.height_; ++current_row_index) {
        table[current_row_index].push_front(0);
        visited[current_row_index].push_front(false);
    }
}

void Simulator::expand_width_right(
    std::deque<std::deque<uint64_t>>& table,
    std::deque<std::deque<bool>>& visited
) {
    for (size_t current_row_index = 0; current_row_index < props_.height_; ++current_row_index) {
        table[current_row_index].push_back(0);
        visited[current_row_index].push_back(false);
    }
}

void Simulator::expand_height_up(
    std::deque<std::deque<uint64_t>>& table,
    std::deque<std::deque<bool>>& visited
) {
    table.push_back(std::deque<uint64_t>(props_.width_));
    visited.push_back(std::deque<bool>(props_.width_));
}

void Simulator::expand_height_down(
    std::deque<std::deque<uint64_t>>& table,
    std::deque<std::deque<bool>>& visited
) {
    table.push_front(std::deque<uint64_t>(props_.width_));
    visited.push_front(std::deque<bool>(props_.width_));
}

bool Simulator::check_borders(uint16_t x, uint16_t y) const {
    return 0 <= y && y < props_.height_ && 0 <= x && x < props_.width_;
}

void Simulator::RenderFrame(const std::deque<std::deque<uint64_t>>& table, uint64_t frame_number) const {
    assert(props_.height_ == table.size() && table.size() > 0);
    assert(props_.width_ == table[0].size());

    Renderer renderer(props_.width_, props_.height_);

    for (size_t y = 0; y < props_.height_; ++y) {
        for (size_t x = 0; x < props_.width_; ++x) {
            if (table[y][x] == 0) {
                renderer.SetColor(x, y, kWhiteColor);
            } else if (table[y][x] == 1) {
                renderer.SetColor(x, y, kGreenColor);
            } else if (table[y][x] == 2) {
                renderer.SetColor(x, y, kPurpleColor);
            } else if (table[y][x] == 3) {
                renderer.SetColor(x, y, kPurpleColor);
            } else if (table[y][x] >= 4) {
                renderer.SetColor(x, y, kBlackColor);
            }
        }
    }

    std::string output_path = props_.output_path_ + "frame" + std::to_string(frame_number) + ".bmp";

    renderer.Export(output_path.c_str());
}

Simulator::Simulator(const InputProperties& _props, const std::vector<CellData>& _input_data)
    : props_(_props)
    , input_data_(_input_data)
    , shift_x(0)
    , shift_y(0)
{
    std::vector<std::pair<uint16_t, uint16_t>> ways_to_move = {
        {-1, 0},
        {1, 0},
        {0, -1},
        {0, 1}
    };

    std::deque<std::deque<uint64_t>> table(props_.height_, std::deque<uint64_t>(props_.width_));
    std::vector<std::queue<std::pair<uint16_t, uint16_t>>> bad_cells(2);

    for (CellData& cell: input_data_) {
        if (!check_borders(cell.x, cell.y)) {
            throw std::runtime_error("Segfault: one of the cells is out of borders");
        }
        
        table[cell.y][cell.x] = cell.grains;

        if (cell.grains >= 4) {
            bad_cells[0].push({cell.x, cell.y});
        }
    }

    std::deque<std::deque<bool>> visited(props_.height_, std::deque<bool>(props_.width_));
    uint64_t last_rendered_frame = 0;

    for (uint64_t iteration = 0;; ++iteration) {
        if (props_.max_iterations_ != 0 && iteration >= props_.max_iterations_) {
            if (iteration != last_rendered_frame) {
                RenderFrame(table, iteration);
                last_rendered_frame = iteration - 1;
            }
            
            break;
        }

        std::cout << "Rendering " << iteration + 1 << " frame..." << std::endl;

        uint8_t queue_index = iteration % 2;
        uint8_t other_queue_index = queue_index ^ 1;
        std::queue<std::pair<uint16_t, uint16_t>>& current_queue = bad_cells[queue_index];

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
                uint16_t new_x = x + dx;
                uint16_t new_y = y + dy;

                if (!check_borders(new_x, new_y)) {
                    expand(new_x, new_y, table, visited);
                }

                table[new_y][new_x] += add_to_neighbours;

                if (table[new_y][new_x] >= 4) {
                    bad_cells[other_queue_index].push({new_x, new_y});
                }
            }
        }

        if ((props_.render_frequency_ != 0 && iteration % props_.render_frequency_ == 0)
         || (props_.render_frequency_ == 0 && iteration == props_.max_iterations_ - 1)) {
            RenderFrame(table, iteration + 1);
            last_rendered_frame = iteration;
        }
    }
}
