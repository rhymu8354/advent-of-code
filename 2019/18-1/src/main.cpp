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
#include <sstream>
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

    /**
     * This method finds all the places that are reachable from the
     * given starting position, and the total cost to reach them.
     *
     * @param[in] startingPosition
     *     This is the starting position from which to find
     *     all reachable places.
     *
     * @param[in] findNeighbors
     *     This function is used to find the neighbors of given positions.
     *
     * @param[in] moveCost
     *     This function computes the cost of moving from one position
     *     to an adjacent position.
     *
     * @param[in] maxCost
     *     If non-zero, this sets a maximum cost after which no more
     *     reachable places are sought after.
     *
     * @return
     *     A list of places and the total cost to reach them is returned.
     *     This list is sorted by increasing total cost.
     */
    static std::vector< SearchStep > FindReachablePlaces(
        const T& startingPosition,
        NeighborFunction findNeighbors,
        CostFunction moveCost,
        int maxCost = 0
    ) {
        std::vector< SearchStep > places;
        std::priority_queue< SearchStep > frontier;
        std::map< T, SearchStep > steps;
        frontier.emplace(startingPosition, startingPosition, 0);
        steps[startingPosition] = frontier.top();
        while (!frontier.empty()) {
            auto lastStep = frontier.top();
            frontier.pop();
            for (const auto& nextStepPosition: findNeighbors(lastStep.position)) {
                auto nextStepCost = moveCost(lastStep.position, nextStepPosition);
                nextStepCost += steps[lastStep.position].cost;
                if (
                    (maxCost > 0)
                    && (nextStepCost > maxCost)
                ) {
                    continue;
                }
                auto step = steps.find(nextStepPosition);
                if (
                    (step == steps.end())
                    || (nextStepCost < step->second.cost)
                ) {
                    auto& nextStep = steps[nextStepPosition];
                    nextStep.position = nextStepPosition;
                    nextStep.previous = lastStep.position;
                    nextStep.cost = nextStepCost;
                    frontier.emplace(
                        nextStepPosition,
                        lastStep.position,
                        nextStepCost
                    );
                }
            }
        }
        for (const auto& step: steps) {
            if (step.first != startingPosition) {
                places.push_back(step.second);
            }
        }
        std::sort(
            places.begin(),
            places.end(),
            [](
                const SearchStep& a,
                const SearchStep& b
            ){
                return a.cost < b.cost;
            }
        );
        return places;
    }
};

enum class Cell {
    Unexplored,
    Floor,
    Wall,
    OxygenSystem,
    Path,
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

    Position operator+(const Position& other) {
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
    const std::vector< std::string >& lines,
    const std::set< char >& keys,
    const Position& end
) {
    const auto height = lines.size();
    const auto width = lines[0].length();
    const auto cell = lines[end.y][end.x];
    if (cell == '#') {
        return 1000001;
    } else if (cell == '.') {
        return 1;
    } else if (cell == '@') {
        return 1;
    } else if (
        (cell >= 'a')
        && (cell <= 'z')
    ) {
        return 1;
    } else if (
        (cell >= 'A')
        && (cell <= 'Z')
    ) {
        if (keys.find(cell + 'a' - 'A') == keys.end()) {
            return 1000001;
        } else {
            return 1;
        }
    } else {
        return 0;
    }
}

int PositionHeuristic(const Position& start, const Position& end) {
    return (
        abs(end.x - start.x)
        + abs(end.y - start.y)
    );
}

std::string EncodePath(
    const std::set< char >& keys,
    const Position& position,
    int steps
) {
    std::ostringstream buffer;
    buffer << position.x << ',' << position.y;
    buffer << ',' << steps;
    buffer << ',';
    for (auto key: keys) {
        buffer << key;
    }
    return buffer.str();
}

void FindShortestPath(
    const std::vector< std::string >& lines,
    const std::set< char >& keys,
    const Position& position,
    std::map< std::string, int >& pathsTried,
    int& bestSteps,
    int steps
) {
    const auto path = EncodePath(keys, position, steps);
    const auto pathsTriedEntry = pathsTried.find(path);
    if (
        (pathsTriedEntry != pathsTried.end())
        && (steps >= pathsTriedEntry->second)
    ) {
        return;
    }
    printf("%s - %d\n", path.c_str(), bestSteps);
    const auto height = lines.size();
    const auto width = lines[0].length();
    const auto places = PathFinding< Position >::FindReachablePlaces(
        position,
        Neighbors,
        [&](const Position& start, const Position& end){ return Cost(lines, keys, end); },
        1000000
    );
    bool foundKey = false;
    for (const auto& place: places) {
        const auto cell = lines[place.position.y][place.position.x];
        if (
            (cell >= 'a')
            && (cell <= 'z')
            && (keys.find(cell) == keys.end())
        ) {
            foundKey = true;
            if (
                (bestSteps > 0)
                && (steps + place.cost >= bestSteps)
            ) {
                break;
            }
            std::set< char > nextKeys = keys;
            (void)nextKeys.insert(cell);
            FindShortestPath(
                lines,
                nextKeys,
                place.position,
                pathsTried,
                bestSteps,
                steps + place.cost
            );
        }
    }
    if (!foundKey) {
        printf("Found path with %d steps.\n", steps);
        bestSteps = (
            (bestSteps == 0)
            ? steps
            : std::min(bestSteps, steps)
        );
    }
    pathsTried[path] = steps;
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

    // Open the input file and read in the map.
    std::ifstream input("input.txt");
    std::vector< std::string > lines;
    std::string line;
    while (std::getline(input, line)) {
        lines.push_back(std::move(line));
        line.clear();
    }
    const auto height = lines.size();
    const auto width = lines[0].length();
    Position position;
    for (size_t y = 0; y < height; ++y) {
        for (size_t x = 0; x < width; ++x) {
            if (lines[y][x] == '@') {
                position.x = (int)x;
                position.y = (int)y;
            }
        }
    }
    printf("Map is %zux%zu and entrance is at %dx%d.\n", width, height, position.x, position.y);

    // Find the shortest path that obtains all the keys.
    std::map< std::string, int > pathsTried;
    std::set< char > keys;
    int steps = 0;
    FindShortestPath(lines, keys, position, pathsTried, steps, 0);
    printf("Shortest path is %d steps.\n", steps);
    return EXIT_SUCCESS;
}
