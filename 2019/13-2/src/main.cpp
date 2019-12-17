/**
 * @file main.cpp
 *
 * This module holds the main() function, which is the entrypoint
 * to the program.
 *
 * © 2019 by Richard Walters
 */

#include <algorithm>
#include <fstream>
#include <functional>
#include <inttypes.h>
#include <map>
#include <memory>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <vector>

#ifdef _WIN32
#include <crtdbg.h>
#endif /* _WIN32 */

struct Position {
    int x = 0;
    int y = 0;

    Position() {
    }

    Position(int x, int y)
        : x(x)
        , y(y)
    {
    }

    bool operator==(const Position& other) const {
        return (
            (x == other.x)
            && (y == other.y)
        );
    }

    bool operator!=(const Position& other) const {
        return !(*this == other);
    }

    bool operator<(const Position& other) const {
        if (x < other.x) {
            return true;
        } else if (x > other.x) {
            return false;
        } else {
            return (y < other.y);
        }
    }

    Position& operator+=(const Position& other) {
        x += other.x;
        y += other.y;
        return *this;
    }
};

intmax_t GetNextNumber(
    const std::string& input,
    size_t& pos
) {
    auto delimiter = input.find(',', pos);
    if (delimiter == std::string::npos) {
        delimiter = input.length();
    }
    intmax_t number;
    if (sscanf(input.substr(pos, delimiter - pos).c_str(), "%" SCNdMAX, &number) != 1) {
        (void)fprintf(stderr, "Bad input detected at position %zu\n", pos);
        exit(1);
    }
    pos = delimiter + 1;
    return number;
}

struct Machine {
    size_t id = 0;
    size_t pos = 0;
    std::vector< intmax_t > numbers;
    std::vector< intmax_t > input;
    bool halted = false;
    intmax_t relativeBase = 0;

    void ExpandToFit(size_t index) {
        if (index >= numbers.size()) {
            numbers.resize(index + 1);
        }
    }

    size_t LoadIndex(
        size_t pos,
        int mode
    ) {
        switch (mode) {
            case 0: { // position
                const auto index = (size_t)numbers[pos];
                return index;
            } break;

            case 2: { // relative
                const auto offset = numbers[pos];
                const auto index = (size_t)(relativeBase + offset);
                return index;
            } break;

            default: {
                (void)fprintf(stderr, "Invalid index mode for offset %zu\n", pos);
                exit(1);
            }
        }
        return 0;
    }

    intmax_t LoadArgument(
        size_t pos,
        int mode
    ) {
        switch (mode) {
            case 0: { // position
                const auto index = (size_t)numbers[pos];
                ExpandToFit(index);
                const auto arg = numbers[index];
                return arg;
            } break;

            case 1: { // immediate
                const auto arg = numbers[pos];
                return arg;
            } break;

            case 2: { // relative
                const auto offset = numbers[pos];
                const auto index = (size_t)(relativeBase + offset);
                ExpandToFit(index);
                const auto arg = numbers[index];
                return arg;
            } break;

            default: {
                (void)fprintf(stderr, "Invalid argument mode for offset %zu\n", pos);
                exit(1);
            }
        }
        return 0;
    }

    void Store(
        size_t index,
        intmax_t value
    ) {
        ExpandToFit(index);
        numbers[index] = value;
    }

    void Run(std::vector< intmax_t >& output) {
        printf("*** Machine %zu Run ***\n", id);
        while (!halted) {
            const auto opcode = numbers[pos] % 100;
            switch (opcode) {
                case 1: { // add
                    const auto arg1 = LoadArgument(pos + 1, (numbers[pos] / 100) % 10);
                    const auto arg2 = LoadArgument(pos + 2, (numbers[pos] / 1000) % 10);
                    const auto index3 = LoadIndex(pos + 3, (numbers[pos] / 10000) % 10);
                    Store(index3, arg1 + arg2);
                    pos += 4;
                } break;

                case 2: { // multiply
                    const auto arg1 = LoadArgument(pos + 1, (numbers[pos] / 100) % 10);
                    const auto arg2 = LoadArgument(pos + 2, (numbers[pos] / 1000) % 10);
                    const auto index3 = LoadIndex(pos + 3, (numbers[pos] / 10000) % 10);
                    Store(index3, arg1 * arg2);
                    pos += 4;
                } break;

                case 3: { // input
                    const auto index = LoadIndex(pos + 1, (numbers[pos] / 100) % 10);
                    if (input.empty()) {
                        printf("*** Machine %zu Needs Input ***\n", id);
                        return;
                    }
                    const auto inputValue = input[0];
                    (void)input.erase(input.begin());
                    Store(index, inputValue);
                    pos += 2;
                } break;

                case 4: { // output
                    const auto outputValue = LoadArgument(pos + 1, (numbers[pos] / 100) % 10);
                    output.push_back(outputValue);
                    pos += 2;
                } break;

                case 5: { // jump-if-true
                    const auto arg1 = LoadArgument(pos + 1, (numbers[pos] / 100) % 10);
                    const auto arg2 = (size_t)LoadArgument(pos + 2, (numbers[pos] / 1000) % 10);
                    if (arg1 != 0) {
                        pos = arg2;
                    } else {
                        pos += 3;
                    }
                } break;

                case 6: { // jump-if-false
                    const auto arg1 = LoadArgument(pos + 1, (numbers[pos] / 100) % 10);
                    const auto arg2 = (size_t)LoadArgument(pos + 2, (numbers[pos] / 1000) % 10);
                    if (arg1 == 0) {
                        pos = arg2;
                    } else {
                        pos += 3;
                    }
                } break;

                case 7: { // less-than
                    const auto arg1 = LoadArgument(pos + 1, (numbers[pos] / 100) % 10);
                    const auto arg2 = LoadArgument(pos + 2, (numbers[pos] / 1000) % 10);
                    const auto index3 = LoadIndex(pos + 3, (numbers[pos] / 10000) % 10);
                    Store(
                        index3,
                        (
                            (arg1 < arg2)
                            ? 1
                            : 0
                        )
                    );
                    pos += 4;
                } break;

                case 8: { // equals
                    const auto arg1 = LoadArgument(pos + 1, (numbers[pos] / 100) % 10);
                    const auto arg2 = LoadArgument(pos + 2, (numbers[pos] / 1000) % 10);
                    const auto index3 = LoadIndex(pos + 3, (numbers[pos] / 10000) % 10);
                    Store(
                        index3,
                        (
                            (arg1 == arg2)
                            ? 1
                            : 0
                        )
                    );
                    pos += 4;
                } break;

                case 9: { // adjust relative base
                    const auto arg1 = LoadArgument(pos + 1, (numbers[pos] / 100) % 10);
                    relativeBase += arg1;
                    pos += 2;
                } break;

                case 99: { // stop
                    halted = true;
                } break;

                default: { // ERROR!!
                    (void)fprintf(stderr, "Invalid opcode (%" PRIdMAX ")\n", opcode);
                    exit(1);
                } break;
            }
        }
        printf("*** Machine %zu Halted ***\n", id);
    }
};

/**
 * This function is the entrypoint of the program.
 *
 * @param[in] argc
 *     This is the number of command-line arguments given to the program.
 *
 * @param[in] argv
 *     This is the array of command-line arguments given to the program.
 */
int main(int argc, char* argv[]) {
#ifdef _WIN32
    //_crtBreakAlloc = 18;
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif /* _WIN32 */
    (void)setbuf(stdout, NULL);

    // Open the input file and read in the input string.
    std::ifstream input("input.txt");
    std::string line;
    (void)std::getline(input, line);

    // Parse the input string into a vector of numbers.
    size_t pos = 0;
    const auto inputLength = line.length();
    std::vector< intmax_t > numbers;
    while (pos < inputLength) {
        const auto number = GetNextNumber(line, pos);
        numbers.push_back(number);
    }

    // Construct machine.
    Machine machine;
    machine.id = 1;
    machine.numbers = std::move(numbers);

    // Construct the tiles to be drawn by the game.
    std::map< Position, int > tiles;

    // Insert quarters into the machine.
    machine.numbers[0] = 2;

    // Run the machine, taking the output as directives to draw
    // into the tiles.  Whenever input is required, display the tiles
    // along with the current score, and ask the user to provide
    // a joystick control direction.
    int score = 0;
    size_t turns = 0;
    while (!machine.halted) {
        // Run the machine until it needs input.
        std::vector< intmax_t > output;
        int ball = 0;
        int paddle = 0;
        machine.Run(output);
        if ((output.size() % 3) != 0) {
            fprintf(stderr, "Improper number of output values\n");
            return EXIT_FAILURE;
        }
        for (size_t i = 0; i < output.size(); i += 3) {
            const auto x = (int)output[i];
            const auto y = (int)output[i+1];
            if (
                (x == -1)
                && (y == 0)
            ) {
                score = (int)output[i+2];
            } else {
                const auto tile = (int)output[i+2];
                tiles[{x, y}] = tile;
                if (tile == 4) {
                    ball = x;
                } else if (tile == 3) {
                    paddle = x;
                }
            }
        }

        // Draw the current state of the tiles.
        int minX = 0;
        int maxX = 0;
        int minY = 0;
        int maxY = 0;
        for (const auto& tile: tiles) {
            minX = std::min(minX, tile.first.x);
            maxX = std::max(maxX, tile.first.x);
            minY = std::min(minY, tile.first.y);
            maxY = std::max(maxY, tile.first.y);
        }
        const auto height = maxY - minY + 1;
        const auto width = maxX - minX + 1;
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                const auto tilesEntry = tiles.find({
                    minX + x,
                    minY + y
                });
                int tile = 0;
                if (tilesEntry != tiles.end()) {
                    tile = tilesEntry->second;
                }
                static const char paints[] = { ' ', '#', '*', '_', 'o' };
                const auto output = paints[(size_t)tile];
                printf("%c", output);
            }
            printf("\n");
        }

        // Draw the current score.
        printf("Current score: %d\n", score);

        // If the machine hasn't yet halted, provide joystick input
        // by finding the ball and paddle, and attempting to keep the
        // paddle directly under the ball.
        if (!machine.halted) {
            int input = (
                (paddle == ball)
                ? 0
                : (
                    (paddle < ball)
                    ? 1
                    : -1
                )
            );
            machine.input.push_back(input);
            ++turns;
        }

        // // If the machine hasn't yet halted, ask for joystick input
        // if (!machine.halted) {
        //     printf("1 <--  2  --> 3  : ");
        //     static char input[2];
        //     (void)gets_s(input, sizeof(input));
        //     machine.input.push_back((int)(input[0] - '2'));
        // }
    }
    printf("Game over man, GAME OVER!\n");
    printf("It took us %zu 'turns' to beat the game!\n", turns);
    return EXIT_SUCCESS;
}
