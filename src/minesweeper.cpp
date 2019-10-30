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
    Grid(s32 length, s32 width) : m_board(length, std::vector<char>(width)) {
        assert(length > 0);
        assert(width > 0);
    }

    s32 length() const { return static_cast<s32>(m_board.size()); }
    s32 width() const { return static_cast<s32>(m_board[0].size()); }

    char get(s32 x, s32 y) const { return m_board[y][x]; }
    char& get(s32 x, s32 y) { return m_board[y][x]; }
    void set(s32 x, s32 y, char val) { m_board[y][x] = val; }
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
        while (1) {
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

void update(const Game_Input* input) {
    // TODO(stewarts):
}

void render() {
    // TODO(stewarts):
}


static constexpr s32 board_length = 40;
static constexpr s32 board_width = 80;
static constexpr f64 mine_percent = 0.1;
static constexpr s32 num_mines = static_cast<s32>(board_length * board_width * mine_percent);

int main(int argc, char* argv[]) {
    Grid board = gen_board(board_length, board_width, num_mines);
    print_board(board);

    std::unique_ptr<Platform> platform = std:: make_unique<Sdl2>();
    std::unique_ptr<Renderer> renderer = std::make_unique<OpenGl>(platform.get());
    Game_Input* input;
    bool running = true;

    // Performance stats
    u64 perfSysStartFrame;
    u64 perfSysCount;
    f64 perfSysTimeMs;
    u64 perfProcessStartFrame;
    u64 perfProcessCount;
    f64 perfProcessTimeMs;
    u64 perfEndFrame;
    s32 perfFps;
    u64 perfFrequency = platform->getPerformanceFrequency();
    perfSysStartFrame = platform->getPerformanceCounter();

    renderer->protoSetup();

    while (running) {
        // System process specific performance logging
        perfProcessStartFrame = platform->getPerformanceCounter();

        // Collect system event information
        platform->processSysEventQueue();
        input = platform->getInput();
        running = !input->state.request_quit;

        // Update game state and render
        update(input);
        render();

        // Performance logging
        perfEndFrame = platform->getPerformanceCounter();
        perfSysCount = perfEndFrame - perfSysStartFrame;
        perfProcessCount = perfEndFrame - perfProcessStartFrame;
        perfSysTimeMs = (static_cast<f64>(perfSysCount) * 1000.0) / static_cast<f64>(perfFrequency);
        perfProcessTimeMs = (static_cast<f64>(perfProcessCount) * 1000.0) / static_cast<f64>(perfFrequency);
        perfFps = static_cast<s32>(1 / (perfSysTimeMs / 1000));
        perfSysStartFrame = perfEndFrame;
        printf("frame: %fms (%fms in process) (%d fps)\n", perfSysTimeMs, perfProcessTimeMs, perfFps);
    }

    return 0;
}
