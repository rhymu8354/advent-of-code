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

};

enum class Cell {
    Unexplored,
    Floor,
    Wall,
    OxygenSystem,
    Path,
};

enum class Turn {
    Left,
    Right,
    Subroutine,
};

struct Move {
    Turn turn = Turn::Left;
    size_t steps = 0;
    char subroutine = 'A';

    bool operator==(const Move& other) const {
        if (turn != other.turn) {
            return false;
        }
        if (turn == Turn::Subroutine) {
            return (subroutine == other.subroutine);
        } else {
            return (steps == other.steps);
        }
    }

    bool operator!=(const Move& other) const {
        return !(*this == other);
    }
};

using Moves = std::vector< Move >;

using Subroutines = std::map< char, Moves >;

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

bool InBounds(size_t width, size_t height, const Position& position) {
    if (position.x < 0) {
        return false;
    }
    if (position.y < 0) {
        return false;
    }
    if ((size_t)position.x >= width) {
        return false;
    }
    if ((size_t)position.y >= height) {
        return false;
    }
    return true;
}

bool MoveIfScaffold(
    const std::vector< std::vector< char > >& image,
    Position& robot,
    const Position& delta
) {
    const auto height = image.size();
    const auto width = image[0].size();
    const auto next = robot + delta;
    if (!InBounds(width, height, next)) {
        return false;
    }
    if (image[next.y][next.x] != '#') {
        return false;
    }
    robot = next;
    return true;
}

std::string FormatMoves(const Moves& moves) {
    std::ostringstream buffer;
    bool first = true;
    for (const auto& move: moves) {
        if (!first) {
            buffer << ',';
        }
        first = false;
        switch (move.turn) {
            case Turn::Left: {
                buffer << "L," << move.steps;
            } break;

            case Turn::Right: {
                buffer << "R," << move.steps;
            } break;

            case Turn::Subroutine: {
                buffer << move.subroutine;
            } break;

            default: {
                buffer << "???";
            } break;
        }
    }
    return buffer.str();
}

template< typename T > bool Match(
    const std::vector< T >& v1,
    size_t i1,
    const std::vector< T >& v2,
    size_t i2,
    size_t n
) {
    for (size_t j = 0; j < n; ++j) {
        if (v1[i1 + j] != v2[i2 + j]) {
            return false;
        }
    }
    return true;
}

template< typename T > void Splice(
    std::vector< T >& v1,
    size_t offset,
    size_t length,
    const std::vector< T >& v2
) {
    while (length < v2.size()) {
        v1.insert(
            v1.begin() + offset + length,
            v2[length]
        );
        ++length;
    }
    while (length > v2.size()) {
        v1.erase(v1.begin() + offset + length - 1);
        --length;
    }
    for (size_t i = 0; i < length; ++i) {
        v1[offset + i] = v2[i];
    }
}

char FindFreeSubroutine(const Subroutines& subroutines) {
    for (const auto& subroutinesEntry: subroutines) {
        if (subroutinesEntry.second.empty()) {
            return subroutinesEntry.first;
        }
    }
    return 0;
}

bool ContainsSubroutine(const Moves& moves) {
    for (const auto& move: moves) {
        if (move.turn == Turn::Subroutine) {
            return true;
        }
    }
    return false;
}

bool ContainsOnlySubroutines(const Moves& moves) {
    for (const auto& move: moves) {
        if (move.turn != Turn::Subroutine) {
            return false;
        }
    }
    return true;
}

Moves Reduce(
    const Moves& moves,
    Subroutines& subroutines,
    size_t maxMoves
) {
    const auto subroutine = FindFreeSubroutine(subroutines);
    if (!subroutine) {
        if (ContainsOnlySubroutines(moves)) {
            return moves;
        } else {
            return {};
        }
    }
    Moves reduction;
    for (size_t i = maxMoves; i > 1; --i) {
        for (size_t j = 0; j + i * 2 <= moves.size(); ++j) {
            subroutines[subroutine] = Moves(
                moves.begin() + j,
                moves.begin() + j + i
            );
            if (ContainsSubroutine(subroutines[subroutine])) {
                continue;
            }
            if (FormatMoves(subroutines[subroutine]).length() > 20) {
                continue;
            }
            for (size_t k = j + i; k + i <= moves.size(); ++k) {
                if (Match(subroutines[subroutine], 0, moves, k, i)) {
                    Moves subroutineMoves;
                    Move subroutineMove;
                    subroutineMove.turn = Turn::Subroutine;
                    subroutineMove.subroutine = subroutine;
                    subroutineMoves.push_back(std::move(subroutineMove));
                    Moves reduction = moves;
                    Splice(reduction, j, i, subroutineMoves);
                    Splice(reduction, k - i + 1, i, subroutineMoves);
                    for (size_t l = k - i + 2; l + i <= reduction.size(); ++l) {
                        if (Match(subroutines[subroutine], 0, reduction, l, i)) {
                            Splice(reduction, l, i, subroutineMoves);
                        }
                    }
                    reduction = Reduce(reduction, subroutines, maxMoves);
                    if (!reduction.empty()) {
                        return reduction;
                    }
                }
            }
        }
    }
    subroutines[subroutine].clear();
    return {};
}

std::vector< char > ImageLine(const std::string& line) {
    std::vector< char > imageLine;
    for (auto ch: line) {
        imageLine.push_back(ch);
    }
    return imageLine;
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
    machine.numbers = numbers;

    // Run the machine until it halts.
    // The machine output is drawing an image from a video camera.
    std::vector< intmax_t > output;
    machine.Run(output);
    std::vector< std::vector< char > > image;
    std::vector< char > imageLine;
    for (auto value: output) {
        if (value == 10) {
            if (!imageLine.empty()) {
                image.push_back(std::move(imageLine));
            }
            imageLine.clear();
            continue;
        }
        const auto ch = (char)value;
        imageLine.push_back(ch);
    }

    // image.clear();
    // image.push_back(ImageLine("#######...#####"));
    // image.push_back(ImageLine("#.....#...#...#"));
    // image.push_back(ImageLine("#.....#...#...#"));
    // image.push_back(ImageLine("......#...#...#"));
    // image.push_back(ImageLine("......#...###.#"));
    // image.push_back(ImageLine("......#.....#.#"));
    // image.push_back(ImageLine("^########...#.#"));
    // image.push_back(ImageLine("......#.#...#.#"));
    // image.push_back(ImageLine("......#########"));
    // image.push_back(ImageLine("........#...#.."));
    // image.push_back(ImageLine("....#########.."));
    // image.push_back(ImageLine("....#...#......"));
    // image.push_back(ImageLine("....#...#......"));
    // image.push_back(ImageLine("....#...#......"));
    // image.push_back(ImageLine("....#####......"));

    // Find robot starting position.
    const auto height = image.size();
    const auto width = image[0].size();
    static const std::string robotCharacters = "<>^v";
    Position robot;
    for (size_t y = 0; y < height; ++y) {
        for (size_t x = 0; x < width; ++x) {
            if (robotCharacters.find(image[y][x]) != std::string::npos) {
                robot.x = (int)x;
                robot.y = (int)y;
                break;
            }
        }
    }

    // Display the image.
    printf("-----------------------------------------\n");
    for (const auto& imageLine: image) {
        for (auto ch: imageLine) {
            printf("%c", ch);
        }
        printf("\n");
    }
    printf("-----------------------------------------\n");

    // Determine the path through the scaffolds.
    struct Direction {
        Position delta;
        char robotCharacter;
        size_t left;
        size_t right;
    };
    static std::vector< Direction > directions{
        {{ 0, -1}, '^', 2, 3}, // 1: north
        {{ 0,  1}, 'v', 3, 2}, // 2: south
        {{-1,  0}, '<', 1, 0}, // 3: west
        {{ 1,  0}, '>', 0, 1}, // 4: east
    };
    size_t robotDirection = 0;
    while (image[robot.y][robot.x] != directions[robotDirection].robotCharacter) {
        ++robotDirection;
    }
    Moves moves;
    for (;;) {
        // Check if we should turn left, turn right, or stop.
        Move move;
        const Position left = robot + directions[directions[robotDirection].left].delta;
        const Position right = robot + directions[directions[robotDirection].right].delta;
        if (
            InBounds(width, height, left)
            && (image[left.y][left.x] == '#')
        ) {
            move.turn = Turn::Left;
        } else if (
            InBounds(width, height, right)
            && (image[right.y][right.x] == '#')
        ) {
            move.turn = Turn::Right;
        } else {
            break;
        }

        // Turn the robot and move until the next edge is reached.
        robotDirection = (
            (move.turn == Turn::Left)
            ? directions[robotDirection].left
            : directions[robotDirection].right
        );
        while (MoveIfScaffold(image, robot, directions[robotDirection].delta)) {
            ++move.steps;
        }
        moves.push_back(std::move(move));
    }

    // Display moves.
    printf("\nOriginal Path:\n");
    printf("%s\n", FormatMoves(moves).c_str());

    // Reduce the path to subroutine calls.
    Subroutines subroutines;
    subroutines['A'] = Moves();
    subroutines['B'] = Moves();
    subroutines['C'] = Moves();
    moves = Reduce(moves, subroutines, 7);
    printf("\nFinal Path:\n");
    printf("%s\n", FormatMoves(moves).c_str());
    for (const auto& subroutine: subroutines) {
        printf("%c:\n", subroutine.first);
        printf("%s\n", FormatMoves(subroutine.second).c_str());
    }

    // Reset the machine, and "wake up" the robot.
    machine = Machine();
    machine.id = 1;
    machine.numbers = numbers;
    machine.numbers[0] = 2;

    // Input main movement routine, followed by the movement functions.
    for (auto ch: FormatMoves(moves)) {
        machine.input.push_back((intmax_t)ch);
    }
    machine.input.push_back(10);
    for (const auto& subroutinesEntry: subroutines) {
        for (auto ch: FormatMoves(subroutinesEntry.second)) {
            machine.input.push_back((intmax_t)ch);
        }
        machine.input.push_back(10);
    }

    // Do we want to see a continuous video feed?
    machine.input.push_back((intmax_t)'n');
    machine.input.push_back(10);
    printf("\n");

    // Run the machine until it halts.  The final output should
    // be the amount of dust collected.
    output.clear();
    machine.Run(output);
    if (!machine.halted) {
        fprintf(stderr, "Robot needs more input!\n");
        return EXIT_FAILURE;
    }
    if (output.empty()) {
        fprintf(stderr, "Robot gave no output!\n");
        return EXIT_FAILURE;
    }
    printf("Robot produced %zu output values.\n", output.size());   
    printf("Amount of dust collected: %" PRIdMAX "\n", output[output.size() - 1]);
    return EXIT_SUCCESS;
}
