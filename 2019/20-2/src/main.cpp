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
#include <queue>
#include <stdlib.h>
#include <stdio.h>
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

struct Position {
    int x = 0;
    int y = 0;
    int z = 0;

    Position() {
    }

    Position(
        int x,
        int y,
        int z
    )
        : x(x)
        , y(y)
        , z(z)
    {
    }

    bool operator==(const Position& other) const {
        return (
            (x == other.x)
            && (y == other.y)
            && (z == other.z)
        );
    }

    bool EqualDisregardingZ(const Position& other) const {
        return (
            (x == other.x)
            && (y == other.y)
        );
    }

    bool operator!=(const Position& other) const {
        return !(*this == other);
    }

    bool operator<(const Position& other) const {
        if (z < other.z) {
            return true;
        } else if (z > other.z) {
            return false;
        } else if (y < other.y) {
            return true;
        } else if (y > other.y) {
            return false;
        } else {
            return (x < other.x);
        }
    }

    Position& operator+=(const Position& other) {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    Position operator+(const Position& other) const {
        Position sum = *this;
        sum.x += other.x;
        sum.y += other.y;
        sum.z += other.z;
        return sum;
    }

    Position operator-(const Position& other) const {
        Position sum = *this;
        sum.x -= other.x;
        sum.y -= other.y;
        sum.z -= other.z;
        return sum;
    }
};

bool InBounds(
    const Position& position,
    size_t width,
    size_t height
) {
    return (
        (position.x >= 0)
        && (position.x < (int)width)
        && (position.y >= 0)
        && (position.y < (int)height)
    );
}

struct CompareDisregardingZ {
    bool operator()(
        const Position& lhs,
        const Position& rhs
    ) const {
        if (lhs.y < rhs.y) {
            return true;
        } else if (lhs.y > rhs.y) {
            return false;
        } else {
            return (lhs.x < rhs.x);
        }
    }
};

using PortalPositions = std::map<
    Position,
    std::string,
    CompareDisregardingZ
>;

std::vector< Position > Neighbors(
    const std::vector< std::string >& lines,
    const std::map< std::string, std::pair< Position, Position > >& portals,
    const PortalPositions& portalPositions,
    const Position& position
) {
    std::vector< Position > neighbors;
    static const std::vector< Position > directions = {
        {-1,  0, 0},
        { 1,  0, 0},
        { 0, -1, 0},
        { 0,  1, 0},
    };
    const auto height = lines.size();
    const auto width = lines[0].length();
    for (const auto& direction: directions) {
        const auto neighbor = position + direction;
        if (InBounds(neighbor, width, height)) {
            const auto cell = lines[neighbor.y][neighbor.x];
            if (
                (cell == '.')
                || (cell == '#')
            ) {
                neighbors.push_back(neighbor);
            }
        }
    }
    const auto portalPositionsEntry = portalPositions.find(position);
    if (portalPositionsEntry != portalPositions.end()) {
        const auto& portalsEntry = portals.find(portalPositionsEntry->second);
        const auto& portal = portalsEntry->second;
        const auto goingOut = position.EqualDisregardingZ(portal.first);
        auto otherSideOfPortal = (
            goingOut
            ? portal.second
            : portal.first
        );
        otherSideOfPortal.z = position.z + (goingOut ? -1 : 1);
        if (otherSideOfPortal.z >= 0) {
            neighbors.push_back(otherSideOfPortal);
        }
    }
    return neighbors;
}

int Cost(
    const std::vector< std::string >& lines,
    const std::map< std::string, std::pair< Position, Position > >& portals,
    const Position& start,
    const Position& end
) {
    const auto height = lines.size();
    const auto width = lines[0].length();
    const auto cell = lines[end.y][end.x];
    if (cell == '#') {
        return 1000001;
    } else if (cell == '.') {
        return 1;
    }
    return 0;
}

int PositionHeuristic(const Position& start, const Position& end) {
    // Normally I would use manhattan distance for A* heuristic,
    // but in this puzzle, it is not admissible.
    // So just devolve to Dijkstra's Algorithm.
    //
    // [11:14] igroc: https://en.wikipedia.org/wiki/Admissible_heuristic
    // -------------------------------------------------------------------
    //
    // return (
    //     abs(end.x - start.x)
    //     + abs(end.y - start.y)
    // );
    return 0;
}

char CheckForPortal(
    const std::vector< std::string >& lines,
    const Position& position
) {
    const auto height = lines.size();
    const auto width = lines[0].length();
    if (!InBounds(position, width, height)) {
        return 0;
    }
    const auto cell = lines[position.y][position.x];
    if (
        (cell >= 'A')
        && (cell <= 'Z')
    ) {
        return cell;
    } else {
        return 0;
    }
}

bool IsFloor(
    const std::vector< std::string >& lines,
    const Position& position
) {
    const auto height = lines.size();
    const auto width = lines[0].length();
    if (!InBounds(position, width, height)) {
        return 0;
    }
    const auto cell = lines[position.y][position.x];
    return (cell == '.');
}

bool IsOuterEdge(
    const Position& position,
    size_t width,
    size_t height
) {
    if (position.x < 3) {
        return true;
    } else if (position.x + 3 >= (int)width) {
        return true;
    } else if (position.y < 3) {
        return true;
    } else if (position.y + 3 >= (int)height) {
        return true;
    } else {
        return false;
    }
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

    // Scan the map for the starting position, ending position,
    // and the positions of all portals.
    const auto height = lines.size();
    const auto width = lines[0].length();
    Position startingPosition, endingPosition;
    std::map< std::string, std::pair< Position, Position > > portals;
    PortalPositions portalPositions;
    for (size_t y = 0; y < height; ++y) {
        for (size_t x = 0; x < width; ++x) {
            const Position cellPosition((int)x, (int)y, 0);
            const auto cell = CheckForPortal(lines, cellPosition);
            if (cell) {
                static std::vector< Position > directions{
                    { 0,  1, 0},
                    { 1,  0, 0},
                };
                for (const auto& direction: directions) {
                    const auto neighbor = cellPosition + direction;
                    const auto cell2 = CheckForPortal(lines, neighbor);
                    if (cell2) {
                        const auto before = cellPosition - direction;
                        const auto after = neighbor + direction;
                        const auto portalFloor = (
                            IsFloor(lines, before)
                            ? before
                            : after
                        );
                        const auto portalLabel = (
                            std::string(1, cell)
                            + std::string(1, cell2)
                        );
                        if (portalLabel == "AA") {
                            startingPosition = portalFloor;
                        } else if (portalLabel == "ZZ") {
                            endingPosition = portalFloor;
                        } else {
                            portalPositions[portalFloor] = portalLabel;
                            auto portalsEntry = portals.find(portalLabel);
                            if (portalsEntry == portals.end()) {
                                portals[portalLabel] = {portalFloor, portalFloor};
                            } else {
                                if (IsOuterEdge(portalFloor, width, height)) {
                                    portalsEntry->second.first = portalFloor;
                                } else {
                                    portalsEntry->second.second = portalFloor;
                                }
                            }
                        }
                        break;
                    }
                }
            }
        }
    }
    printf(
        "Map is %zux%zu, entrance is at %dx%d, exit is at %dx%d, and there are %zu portals.\n",
        width, height,
        startingPosition.x, startingPosition.y,
        endingPosition.x, endingPosition.y,
        portals.size()
    );
    for (const auto& portal: portals) {
        printf(
            "  %s: %dx%d <-> %dx%d\n",
            portal.first.c_str(),
            portal.second.first.x, portal.second.first.y,
            portal.second.second.x, portal.second.second.y
        );
    }

    // Use the A* path-finding algorithm to find the shortest path
    // from the entrance to the exit.
    const auto path = PathFinding< Position >::FindPath(
        startingPosition,
        endingPosition,
        [&](const Position& position){ return Neighbors(lines, portals, portalPositions, position); },
        [&](const Position& start, const Position& end){ return Cost(lines, portals, start, end); },
        PositionHeuristic,
        1000000
    );
    printf("The shortest path through the maze is %d steps.\n", path.cost);
    int maxZ = 0;
    for (const auto& step: path.steps) {
        maxZ = std::max(maxZ, step.z);
    }
    printf("The deepest we went was %d steps down.\n", maxZ);
    return EXIT_SUCCESS;
}
