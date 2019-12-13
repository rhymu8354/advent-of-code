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
#include <map>
#include <math.h>
#include <memory>
#include <set>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <vector>

#ifdef _WIN32
#include <crtdbg.h>
#endif /* _WIN32 */

enum class Direction {
    Up,
    Down,
    Left,
    Right,
};

struct Instruction {
    Direction direction;
    size_t steps;
};

struct Position {
    int x = 0;
    int y = 0;

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

    bool IsOrigin() const {
        return (
            (x == 0)
            && (y == 0)
        );
    }

    size_t Distance() const {
        return (
            abs(x)
            + abs(y)
        );
    }
};

Instruction GetNextInstruction(
    const std::string& input,
    size_t& pos
) {
    Instruction instruction;
    auto delimiter = input.find(',', pos);
    if (delimiter == std::string::npos) {
        delimiter = input.length();
    }
    switch (input[pos]) {
        case 'U': {
            instruction.direction = Direction::Up;
        } break;

        case 'D': {
            instruction.direction = Direction::Down;
        } break;

        case 'L': {
            instruction.direction = Direction::Left;
        } break;

        case 'R': {
            instruction.direction = Direction::Right;
        } break;

        default: {
            (void)fprintf(stderr, "Bad input detected at position %zu\n", pos);
            exit(1);
        } break;
    }
    if (sscanf(input.substr(pos + 1, delimiter - pos - 1).c_str(), "%zu", &instruction.steps) != 1) {
        (void)fprintf(stderr, "Bad input detected at position %zu\n", pos);
        exit(1);
    }
    pos = delimiter + 1;
    return instruction;
}

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

    // Open the input file.
    std::ifstream input("input.txt");

    // Read and parse from the input file all the wire paths.
    std::vector< std::vector< Instruction > > paths;
    std::string line;
    while (std::getline(input, line)) {
        size_t pos = 0;
        const auto inputLength = line.length();
        std::vector< Instruction > path;
        while (pos < inputLength) {
            const auto instruction = GetNextInstruction(line, pos);
            path.push_back(instruction);
        }
        paths.push_back(std::move(path));
    }

    // Lay out the wires: trace each wire path, adding positions touched
    // by each wire into a set, one per wire.  For each position, we store
    // the number of steps it took the wire to reach that position.
    //
    // If a wire crosses itself, we end up adding the same position twice,
    // which for a std::set, is a no-op.
    std::vector< std::map< Position, size_t > > wirePositions;
    for (const auto& path: paths) {
        std::map< Position, size_t > positions;
        Position position;
        size_t steps = 0;
        for (const auto& instruction: path) {
            int dx = 0;
            int dy = 0;
            switch (instruction.direction) {
                case Direction::Up: {
                    dy = -1;
                } break;

                case Direction::Down: {
                    dy = 1;
                } break;

                case Direction::Left: {
                    dx = -1;
                } break;

                case Direction::Right: {
                    dx = 1;
                } break;

                default: {
                    (void)fprintf(stderr, "Bad instruction detected\n");
                    exit(1); 
                } break;
            }
            for (size_t _ = 0; _ < instruction.steps; ++_) {
                ++steps;
                position.x += dx;
                position.y += dy;
                auto& positionsEntry = positions[position];
                if (positionsEntry == 0) {
                    positionsEntry = steps;
                }
            }
        }
        (void)wirePositions.push_back(std::move(positions));
    }

    // Start with the positions touched by the first wire.
    // For all other wires, check if each candidate position is also
    // touched by that wire.  If not, remove the candidate position.
    // By the end, the candidate positions are all the positions
    // touched by all the wires.
    //
    // For every candidate, sum together the step counters for each wire.
    auto wirePositionsEntry = wirePositions.begin();
    auto candidatePositions = *wirePositionsEntry;
    while (++wirePositionsEntry != wirePositions.end()) {
        const auto& otherWirePositions = *wirePositionsEntry;
        auto candidatePositionsEntry = candidatePositions.begin();
        while (candidatePositionsEntry != candidatePositions.end()) {
            const auto& position = *candidatePositionsEntry;
            const auto otherWirePositionsEntry = otherWirePositions.find(position.first);
            if (otherWirePositionsEntry == otherWirePositions.end()) {
                candidatePositionsEntry = candidatePositions.erase(candidatePositionsEntry);
            } else {
                candidatePositionsEntry->second += otherWirePositionsEntry->second;
                ++candidatePositionsEntry;
            }
        }
    }

    // Go through all positions touched by all wires, and find the one
    // with the lowest step count.
    Position closestPosition;
    size_t closestStepCount = 0;
    for (const auto& candidatePositionsEntry: candidatePositions) {
        const auto& position = candidatePositionsEntry.first;
        const auto stepCount = candidatePositionsEntry.second;
        if (closestStepCount == 0) {
            closestPosition = position; // FIRST!!
            closestStepCount = stepCount;
        } else if (stepCount < closestStepCount) {
            closestStepCount = stepCount;
        }
    }

    // Output the closet position.
    printf(
        "Closest:  x=%d, y=%d -> step count: %zu\n",
        closestPosition.x,
        closestPosition.y,
        closestStepCount
    );
    return EXIT_SUCCESS;
}
