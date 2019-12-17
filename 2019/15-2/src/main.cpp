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
#include <limits>
#include <map>
#include <memory>
#include <queue>
#include <set>
#include <stdlib.h>
#include <stdio.h>
#include <stack>
#include <string>
#include <vector>

#ifdef _WIN32
#include <crtdbg.h>
#endif /* _WIN32 */

/**
 * This template is used to find a path from one position
 * to another, where the type of position is a template argument.
 */
template< typename T > struct PathFinding {
    /**
     * This structure represents a place reachable along a path from
     * a starting position.
     */
    struct SearchStep {
        // Properties

        /**
         * This is the position of this step.
         */
        T position;

        /**
         * This is the previous position that led to this position.
         */
        T previous;

        /**
         * This is the cost incurred so far to reach this step.
         */
        int cost = 0;

        // Methods

        /**
         * This is the default constructor of the class.
         */
        SearchStep() = default;

        /**
         * This is used to construct the object in place
         * from its properties.
         *
         * @param[in] position
         *     This is the position of this step.
         *
         * @param[in] previous
         *     This is the previous position that led to this position.
         *
         * @param[in] cost
         *     This is the cost incurred so far to reach this step.
         */
        SearchStep(
            const T& position,
            const T& previous,
            unsigned int cost
        )
            : position(position)
            , previous(previous)
            , cost(cost)
        {
        }

        /**
         * This is the sorting order comparison operator.
         *
         * @param[in] other
         *     This is the other object to which to compare this object.
         *
         * @return
         *     An indication of whether or not this object comes before
         *     the other object is returned.
         */
        bool operator<(const SearchStep& other) const {
            return cost > other.cost;
        }
    };

    /**
     * This is the type of function which determines the immediate
     * neighbors of a given position.
     *
     * @param[in] position
     *     This is the position for which to find neighbors.
     *
     * @return
     *     The immediate neighbors of the given position are returned.
     */
    typedef std::function<
        std::vector< T >(const T& position)
    > NeighborFunction;

    /**
     * This is the type of function which computes the cost
     * of moving from one position to another.
     *
     * @param[in] start
     *     This is the starting position.
     *
     * @param[in] end
     *     This is the end position.
     *
     * @return
     *     The cost of moving from the start position to the end
     *     position is returned.
     */
    typedef std::function<
        int(
            const T& start,
            const T& end
        )
    > CostFunction;

    /**
     * This structure holds information returned by the FindPath function.
     */
    struct Path {
        /**
         * This indicates whether or not the destination can be reached.
         */
        bool reachable = false;

        /**
         * This is the total estimated cost of reaching the
         * destination.
         */
        int cost = 0;

        /**
         * These are the positions of each step from the starting
         * position to the destination.
         */
        std::vector< T > steps;
    };

    /**
     * This method finds a path from a given starting position
     * to a given destination.
     *
     * @param[in] startingPosition
     *     This is the starting position from which to find
     *     a path to the given destination.
     *
     * @param[in] destination
     *     This is the position to which to find a path.
     *
     * @param[in] findNeighbors
     *     This function is used to find the neighbors of given positions.
     *
     * @param[in] moveCost
     *     This function computes the cost of moving from one position
     *     to an adjacent position.
     *
     * @param[in] heuristic
     *     This function estimates the remaining cost of moving from
     *     a given position to another.
     *
     * @param[in] maxCost
     *     If non-zero, this sets a maximum cost for the path to
     *     find.
     *
     * @return
     *     Information about the path from the given starting position
     *     to the given destination is returned.
     */
    static Path FindPath(
        const T& startingPosition,
        const T& destination,
        NeighborFunction findNeighbors,
        CostFunction moveCost,
        CostFunction heuristic,
        int maxCost = 0
    ) {
        std::priority_queue< SearchStep > frontier;
        std::map< T, SearchStep > steps;
        frontier.emplace(startingPosition, startingPosition, 0);
        steps[startingPosition] = frontier.top();
        while (!frontier.empty()) {
            auto lastStep = frontier.top();
            if (lastStep.position == destination) {
                Path path;
                path.reachable = true;
                path.cost = steps[lastStep.position].cost;
                for (auto position = lastStep.position; position != startingPosition; position = steps[position].previous) {
                    path.steps.push_back(position);
                }
                std::reverse(path.steps.begin(), path.steps.end());
                return path;
            }
            frontier.pop();
            for (const auto& nextStepPosition: findNeighbors(lastStep.position)) {
                auto nextStepCost = moveCost(lastStep.position, nextStepPosition);
                const auto nextStepHeuristic = heuristic(nextStepPosition, destination);
                nextStepCost += steps[lastStep.position].cost;
                if (
                    (maxCost > 0)
                    && (nextStepCost + nextStepHeuristic > maxCost)
                ) {
                    continue;
                }
                auto step = steps.find(nextStepPosition);
                if (
                    (step == steps.end())
                    || (nextStepCost < step->second.cost)
                ) {
                    steps[nextStepPosition].cost = nextStepCost;
                    steps[nextStepPosition].previous = lastStep.position;
                    frontier.emplace(
                        nextStepPosition,
                        lastStep.position,
                        nextStepCost + nextStepHeuristic
                    );
                }
            }
        }
        return Path();
    }

};

enum class Cell {
    Unexplored,
    Floor,
    Wall,
    OxygenSystem,
    Path,
    Oxygen,
};

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

    Position operator+(const Position& other) const {
        Position sum = *this;
        sum.x += other.x;
        sum.y += other.y;
        return sum;
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

std::vector< Position > Neighbors(const Position& position) {
    return {
        {position.x - 1, position.y},
        {position.x + 1, position.y},
        {position.x, position.y - 1},
        {position.x, position.y + 1},
    };
}

int Cost(
    const std::map< Position, Cell >& cells,
    const Position& end
) {
    const auto cellsEntry = cells.find(end);
    if (cellsEntry == cells.end()) {
        return 0;
    } else if (cellsEntry->second == Cell::Wall) {
        return 1000001;
    } else {
        return 1;
    }
}

int PositionHeuristic(const Position& start, const Position& end) {
    return (
        abs(end.x - start.x)
        + abs(end.y - start.y)
    );
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

    // Explore the section of the ship until the oxygen system is found.
    std::map< Position, Cell > cells;
    Position robotPosition;
    cells[robotPosition] = Cell::Floor;

    // Run the machine, providing as input the a command to move in one of
    // the cardinal directions, and taking the output the response from
    // the robot about what it encountered.
    int minX = 0;
    int maxX = 0;
    int minY = 0;
    int maxY = 0;
    Position oxygenSystem;
    std::stack< size_t > trail;
    struct Direction {
        Position delta;
        intmax_t input;
        size_t back;
    };
    static std::vector< Direction > directions{
        {{ 0, -1}, 1, 1}, // 1: north
        {{ 0,  1}, 2, 0}, // 2: south
        {{-1,  0}, 3, 3}, // 3: west
        {{ 1,  0}, 4, 2}, // 4: east
    };
    for (;;) {
        // Look for an unexplored cell next to the robot.
        size_t direction = 0;
        bool backTracking = true;
        for (size_t i = 0; i < directions.size(); ++i) {
            const auto cellsEntry = cells.find(robotPosition + directions[i].delta);
            if (cellsEntry == cells.end()) {
                direction = i;
                backTracking = false;
                break;
            }
        }

        // If we did not find an unexplored cell next the robot,
        // we need to back-track.
        if (backTracking) {
            if (trail.empty()) {
                break;
            }
            printf("Robot is back-tracking (trail is length %zu)\n", trail.size());
            direction = directions[trail.top()].back;
            trail.pop();
        }

        // Provide the robot with its instruction, and run the machine
        // to get the next output.
        machine.input.push_back(directions[direction].input);
        std::vector< intmax_t > output;
        machine.Run(output);
        if (output.size() != 1) {
            fprintf(stderr, "Robot did not provide correct output!\n");
            return EXIT_FAILURE;
        }

        // The machine's output indicates what happened to the robot,
        // and if it located the oxygen system.
        const auto targetPosition = robotPosition + directions[direction].delta;
        switch (output[0]) {
            case 0: { // hit a wall
                printf("Robot found a wall at %dx%d\n", targetPosition.x, targetPosition.y);
                cells[targetPosition] = Cell::Wall;
            } break;

            case 1:   // robot moved
            case 2: { // robot moved and found oxygen system
                printf("Robot moved to %dx%d\n", targetPosition.x, targetPosition.y);
                if (!backTracking) {
                    trail.push(direction);
                }
                robotPosition = targetPosition;
                if (output[0] == 1) {
                    cells[targetPosition] = Cell::Floor;
                } else {
                    cells[targetPosition] = Cell::OxygenSystem;
                    oxygenSystem = targetPosition;
                    printf("Found the oxygen system!\n");
                }
            } break;

            default: {
                fprintf(stderr, "MrDestructoid - robot is insane.  Beware, human!\n");
                return EXIT_FAILURE;
            } break;
        }
        minX = std::min(minX, targetPosition.x);
        maxX = std::max(maxX, targetPosition.x);
        minY = std::min(minY, targetPosition.y);
        maxY = std::max(maxY, targetPosition.y);
    }

    // Use the A* algorithm to find the shortest path to the oxygen system.
    const auto path = PathFinding< Position >::FindPath(
        {0, 0},
        oxygenSystem,
        Neighbors,
        [&cells](const Position& start, const Position& end){ return Cost(cells, end); },
        PositionHeuristic,
        1000000
    );
    printf("Shortest distance: %d\n", path.cost);

    // Plot the shortest path.
    for (const auto& step: path.steps) {
        if (step != oxygenSystem) {
            cells[step] = Cell::Path;
        }
    }

    // Display the cells in the ship.
    printf("-----------------------------------------\n");
    const auto height = maxY - minY + 1;
    const auto width = maxX - minX + 1;
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            const Position pos(
                minX + x,
                minY + y
            );
            const auto cellsEntry = cells.find(pos);
            int color = 0;
            if (cellsEntry != cells.end()) {
                color = (int)cellsEntry->second;
            }
            if (robotPosition == pos) {
                color = 5;
            }
            if (
                (pos.x == 0)
                && (pos.y == 0)
            ) {
                color = 6;
            }
            static const char paints[] = { '?', '.', '#', '@', '+', '%', '*' };
            const auto output = paints[(size_t)color];
            printf("%c", output);
        }
        printf("\n");
    }
    printf("-----------------------------------------\n");

    // Flood-fill oxygen throughout the ship.
    cells[oxygenSystem] = Cell::Oxygen;
    std::set< Position > 🌊;
    (void)🌊.insert(oxygenSystem);
    size_t minutes = 0;
    for (;;) {
        // Advance the frontier one more step.
        auto next🌊 = decltype(🌊)();
        for (const auto& edge: 🌊) {
            for (const auto& direction: directions) {
                const auto nextEdge = edge + direction.delta;
                if (
                    (cells[nextEdge] == Cell::Floor)
                    || (cells[nextEdge] == Cell::Path)
                ) {
                    cells[nextEdge] = Cell::Oxygen;
                    (void)next🌊.insert(nextEdge);
                }
            }
        }
        🌊.swap(next🌊);

        // If we could not advance the frontier, then it means we've
        // filled the entire ship, so we were done already.
        if (🌊.empty()) {
            break;
        }

        // We're still filling the ship with oxygen, so advance time one
        // minute.
        ++minutes;
    
        // Display the cells in the ship.
        printf("-----------------------------------------\n");
        printf("After %zu minutes:\n", minutes);
        const auto height = maxY - minY + 1;
        const auto width = maxX - minX + 1;
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                const Position pos(
                    minX + x,
                    minY + y
                );
                const auto cellsEntry = cells.find(pos);
                int color = 0;
                if (cellsEntry != cells.end()) {
                    color = (int)cellsEntry->second;
                }
                if (robotPosition == pos) {
                    color = 6;
                }
                if (
                    (pos.x == 0)
                    && (pos.y == 0)
                ) {
                    color = 7;
                }
                static const char paints[] = { '?', '.', '#', '@', '+', 'O', '%', '*' };
                const auto output = paints[(size_t)color];
                printf("%c", output);
            }
            printf("\n");
        }
        printf("-----------------------------------------\n");
    }
    printf("It took %zu minutes to 🌊 the ship with oxygen.\n", minutes);
    return EXIT_SUCCESS;
}
