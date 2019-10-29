#include <cassert>
#include <cstdio>
#include <limits>
#include <random>
#include <string>
#include <vector>

constexpr char mine_val = std::numeric_limits<char>::max();

class Grid {
private:
    std::vector<std::vector<char>> m_board;

public:
    Grid(int length, int width) : m_board(length, std::vector<char>(width)) {
        assert(length > 0);
        assert(width > 0);
    }

    int length() const { return static_cast<int>(m_board.size()); }
    int width() const { return static_cast<int>(m_board[0].size()); }

    char get(int x, int y) const { return m_board[y][x]; }
    char& get(int x, int y) { return m_board[y][x]; }
    void set(int x, int y, char val) { m_board[y][x] = val; }
};

void update_mine_adjacent_counts(Grid& board, int x, int y)
{
    const int width = board.width();
    const int length = board.length();

    assert(x >= 0 && x < width);
    assert(y >= 0 && y < length);

    for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {
            // Don't update self
            if (dx == 0 && dy == 0) { continue; }

            const int adj_x = x + dx;
            const int adj_y = y + dy;

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

Grid gen_board(int length, int width, int num_mines) {
    assert(length * width >= num_mines);

    Grid board(length, width);

    std::random_device rd;
    std::mt19937 rand_gen(rd());
    std::uniform_int_distribution<> length_dis(0, length - 1);
    std::uniform_int_distribution<> width_dis(0, width - 1);

    for (int n = 0; n < num_mines; ++n) {
        while (1) {
            const int x = width_dis(rand_gen);
            const int y = length_dis(rand_gen);

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

    for (int y = 0; y < board.length(); ++y) {
        for (int x = 0; x < board.width(); ++x) {
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

static constexpr int board_length = 40;
static constexpr int board_width = 80;
static constexpr double mine_percent = 0.1;
static constexpr int num_mines = static_cast<int>(board_length * board_width * mine_percent);

int main(int argc, char* argv[]) {
    Grid board = gen_board(board_length, board_width, num_mines);
    print_board(board);
    return 0;
}
