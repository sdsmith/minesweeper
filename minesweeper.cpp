#include <cassert>
#include <cstdio>
#include <limits>
#include <random>
#include <string>
#include <vector>

constexpr char mine_val = std::numeric_limits<char>::max();

using Board = std::vector<std::vector<char>>;

void update_mine_adjacent_counts(Board& board, int x, int y)
{
    const size_t length = board.size();
    assert(length > 0);
    const size_t width = board[0].size();
    assert(width > 0);

    assert(x >= 0 && static_cast<size_t>(x) < width);
    assert(y >= 0 && static_cast<size_t>(y) < width);

    for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {
            // Don't update self
            if (dx == 0 && dy == 0) { continue; }

            const int adj_x = x + dx;
            const int adj_y = y + dy;

            // Board bounds check
            if ((adj_x < 0) || (static_cast<size_t>(adj_x) >= width) ||
                (adj_y < 0) || (static_cast<size_t>(adj_y) >= length))
            {
                continue;
            }

            // Don't update mine locations
            if (board[adj_y][adj_x] == mine_val) { continue; }

            board[adj_y][adj_x]++;
        }
    }
}

Board gen_board(int length, int width, int num_mines) {
    assert(length * width >= num_mines);

    Board board(length, std::vector<char>(width));

    std::random_device rd;
    std::mt19937 rand_gen(rd());
    std::uniform_int_distribution<> length_dis(0, length - 1);
    std::uniform_int_distribution<> width_dis(0, width - 1);

    for (int n = 0; n < num_mines; ++n) {
        while (1) {
            const int x = width_dis(rand_gen);
            const int y = length_dis(rand_gen);

            if (board[y][x] != mine_val) {
                board[y][x] = mine_val;
                update_mine_adjacent_counts(board, x, y);
                break;
            }
        }
    }

    return board;
}

void print_board(const Board& board) {
    // TODO(stewarts): make Board a class already
    const size_t length = board.size();
    assert(length > 0);
    const size_t width = board[0].size();
    assert(width > 0);

    std::string s;

    for (size_t y = 0; y < length; ++y) {
        for (size_t x = 0; x < width; ++x) {
            char c;
            switch (board[y][x]) {
                case 0:
                    c = ' ';
                    break;
                case mine_val:
                    c = 'X';
                    break;
                default:
                    c = static_cast<char>(board[y][x] + '0');
                    break;
            }

            s += c;
            s += ' ';
        }

        s += '\n';
    }

    printf("%s\n", s.c_str());
}

static constexpr int board_length = 40;
static constexpr int board_width = 80;
static constexpr double mine_percent = 0.1;
static constexpr int num_mines = static_cast<int>(board_length * board_width * mine_percent);

int main(int argc, char* argv[]) {
    Board board = gen_board(board_length, board_width, num_mines);
    print_board(board);
    return 0;
}
