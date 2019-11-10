#include "input.h"
#include "platform/platform.h"
#include "platform/sdl2.h"
#include "renderer/opengl.h"
#include "types.h"
#include <cassert>
#include <cstdio>
#include <limits>
#include <memory>
#include <random>
#include <string>
#include <vector>

constexpr char mine_val = std::numeric_limits<char>::max();

class Grid {
private:
    std::vector<std::vector<char>> m_board;

public:
    Grid(s32 length, s32 width) : m_board(static_cast<u32>(length),
                                          std::vector<char>(static_cast<u32>(width))) {
        assert(length > 0);
        assert(width > 0);
    }

    s32 length() const { return static_cast<s32>(m_board.size()); }
    s32 width() const { return static_cast<s32>(m_board[0].size()); }

    char get(s32 x, s32 y) const {
        assert(x >= 0);
        assert(y >= 0);
        return m_board[static_cast<u32>(y)][static_cast<u32>(x)];
    }
    char& get(s32 x, s32 y) {
        assert(x >= 0);
        assert(y >= 0);
        return m_board[static_cast<u32>(y)][static_cast<u32>(x)];
    }
    void set(s32 x, s32 y, char val) {
        assert(x >= 0);
        assert(y >= 0);
        m_board[static_cast<u32>(y)][static_cast<u32>(x)] = val;
    }
};

void update_mine_adjacent_counts(Grid& board, s32 x, s32 y)
{
    const s32 width = board.width();
    const s32 length = board.length();

    assert(x >= 0 && x < width);
    assert(y >= 0 && y < length);

    for (s32 dy = -1; dy <= 1; ++dy) {
        for (s32 dx = -1; dx <= 1; ++dx) {
            // Don't update self
            if (dx == 0 && dy == 0) { continue; }

            const s32 adj_x = x + dx;
            const s32 adj_y = y + dy;

            // Board bounds check
            if ((adj_x < 0) || (adj_x >= width) ||
                (adj_y < 0) || (adj_y >= length))
            {
                continue;
            }

            // Don't update mine locations
            if (board.get(adj_x, adj_y) == mine_val) { continue; }

            board.get(adj_x, adj_y)++;
        }
    }
}

Grid gen_board(s32 length, s32 width, s32 num_mines) {
    assert(length * width >= num_mines);

    Grid board(length, width);

    std::random_device rd;
    std::mt19937 rand_gen(rd());
    std::uniform_int_distribution<> length_dis(0, length - 1);
    std::uniform_int_distribution<> width_dis(0, width - 1);

    for (s32 n = 0; n < num_mines; ++n) {
        while (true) {
            const s32 x = width_dis(rand_gen);
            const s32 y = length_dis(rand_gen);

            if (board.get(x, y) != mine_val) {
                board.set(x, y, mine_val);
                update_mine_adjacent_counts(board, x, y);
                break;
            }
        }
    }

    return board;
}

void print_board(const Grid& board) {
    std::string s;

    for (s32 y = 0; y < board.length(); ++y) {
        for (s32 x = 0; x < board.width(); ++x) {
            char c;
            switch (board.get(x, y)) {
                case 0:
                    c = ' ';
                    break;
                case mine_val:
                    c = 'X';
                    break;
                default:
                    c = static_cast<char>(board.get(x, y) + '0');
                    break;
            }

            s += c;
            s += ' ';
        }

        s += '\n';
    }

    printf("%s\n", s.c_str());
}

void update(const Game_Input*) {
    // TODO(stewarts):
}

void render() {
    // TODO(stewarts):
}


static constexpr s32 board_length = 40;
static constexpr s32 board_width = 80;
static constexpr f64 mine_percent = 0.1;
static constexpr s32 num_mines = static_cast<s32>(board_length * board_width * mine_percent);

int main(int, char*[]) {
    Grid board = gen_board(board_length, board_width, num_mines);
    //print_board(board);

    std::unique_ptr<Platform> platform = std::make_unique<Sdl2>();
    std::unique_ptr<Renderer> renderer = std::make_unique<OpenGl>("Minesweeper", platform.get());
    Game_Input* input;
    bool running = true;

    // performance stats
    u64 perf_sys_start_frame;
    u64 perf_sys_count;
    f64 perf_sys_time_ms;
    u64 perf_process_start_frame;
    u64 perf_process_count;
    f64 perf_process_time_ms;
    u64 perf_end_frame;
    s32 perf_fps;
    u64 perf_frequency = platform->get_performance_frequency();
    perf_sys_start_frame = platform->get_performance_counter();

    renderer->proto_setup();

    while (running) {
        // System process specific performance logging
        perf_process_start_frame = platform->get_performance_counter();

        // Collect system event information
        platform->process_sys_event_queue();
        input = platform->get_input();
        running = !input->state.request_quit;

        // Update game state and render
        update(input);
        render();

        // Performance logging
        perf_end_frame = platform->get_performance_counter();
        perf_sys_count = perf_end_frame - perf_sys_start_frame;
        perf_process_count = perf_end_frame - perf_process_start_frame;
        perf_sys_time_ms = (static_cast<f64>(perf_sys_count) * 1000.0) / static_cast<f64>(perf_frequency);
        perf_process_time_ms = (static_cast<f64>(perf_process_count) * 1000.0) / static_cast<f64>(perf_frequency);
        perf_fps = static_cast<s32>(1 / (perf_sys_time_ms / 1000));
        perf_sys_start_frame = perf_end_frame;
        //printf("frame: %fms (%fms in process) (%d fps)\n", perf_sys_time_ms, perf_process_time_ms, perf_fps);
    }

    return 0;
}
