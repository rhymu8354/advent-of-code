/**
 * @file main.cpp
 *
 * This module holds the main() function, which is the entrypoint
 * to the program.
 *
 * © 2019 by Richard Walters
 */

#include <algorithm>
#include <chrono>
#include <fstream>
#include <functional>
#include <map>
#include <queue>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <string.h>
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

template< int N > struct Position {
    int m[N];

    Position() {
        (void)memset(m, 0, sizeof(m));
    }

    Position(std::initializer_list< int > list) {
        size_t i = 0;
        for (auto element: list) {
            m[i++] = element;
        }
    }

    Position(const Position<N-1>& other) {
        (void)memset(m, 0, sizeof(m));
        for (size_t i = 0; i < N - 1; ++i) {
            m[i] = other.m[i];
        }
    }

    template< int M > Position<M> Reduce() const {
        Position<M> reduction;
        for (size_t i = 0; i < M; ++i) {
            if (i >= N) {
                break;
            }
            reduction.m[i] = m[i];
        }
        return reduction;
    }

    bool operator==(const Position& other) const {
        for (size_t i = 0; i < N; ++i) {
            if (m[i] != other.m[i]) {
                return false;
            }
        }
        return true;
    }

    bool operator!=(const Position& other) const {
        return !(*this == other);
    }

    bool operator<(const Position& other) const {
        for (size_t i = 0; i < N; ++i) {
            if (m[i] < other.m[i]) {
                return true;
            } else if (m[i] > other.m[i]) {
                return false;
            }
        }
        return false;
    }

    Position& operator+=(const Position& other) {
        for (size_t i = 0; i < N; ++i) {
            m[i] += other.m[i];
        }
        return *this;
    }

    Position& operator-=(const Position& other) {
        for (size_t i = 0; i < N; ++i) {
            m[i] -= other.m[i];
        }
        return *this;
    }

    Position operator+(const Position& other) const {
        Position sum = *this;
        return sum += other;
    }

    Position operator-(const Position& other) const {
        Position sum = *this;
        return sum -= other;
    }
};

using PortalPositions = std::map<
    Position<2>,
    std::string
>;

using Portals = std::map<
    std::string,
    std::pair< Position<2>, Position<2> >
>;

struct Maze {
    // Properties

    std::vector< std::string > lines;
    size_t width = 0;
    size_t height = 0;

    // Methods

    void Input(std::string&& line) {
        lines.push_back(std::move(line));
        if (lines.size() == 1) {
            width = lines[0].length();
        }
        height = lines.size();
    }

    bool InBounds(const Position<2>& position) const {
        return (
            (position.m[0] >= 0)
            && (position.m[0] < (int)width)
            && (position.m[1] >= 0)
            && (position.m[1] < (int)height)
        );
    }

    char CellAt(const Position<2>& position) const {
        if (InBounds(position)) {
            return lines[position.m[1]][position.m[0]];
        } else {
            return ' ';
        }
    }

    char CheckForPortal(const Position<2>& position) const {
        const auto cell = CellAt(position);
        if (
            (cell >= 'A')
            && (cell <= 'Z')
        ) {
            return cell;
        } else {
            return 0;
        }
    }

    bool IsFloor(const Position<2>& position) const {
        const auto cell = CellAt(position);
        return (cell == '.');
    }

    bool IsOuterEdge(const Position<2>& position) const {
        if (position.m[0] < 3) {
            return true;
        } else if (position.m[0] + 3 >= (int)width) {
            return true;
        } else if (position.m[1] < 3) {
            return true;
        } else if (position.m[1] + 3 >= (int)height) {
            return true;
        } else {
            return false;
        }
    }

};

std::vector< Position<3> > Neighbors(
    const Maze& maze,
    const Portals& portals,
    const PortalPositions& portalPositions,
    const Position<3>& position,
    int& deepestNeighborZ
) {
    std::vector< Position<3> > neighbors;
    static const std::vector< Position<2> > directions = {
        {-1,  0},
        { 1,  0},
        { 0, -1},
        { 0,  1},
    };

    // Adjacent positions which have a floor are always neighbors.
    for (const auto& direction: directions) {
        const auto neighbor = position + direction;
        const auto cell = maze.CellAt(neighbor.Reduce<2>());
        if (cell == '.') {
            neighbors.push_back(neighbor);
        }
    }

    // If the position is next to a portal, add the other side
    // of the portal as a neighbor.
    const auto reducedPosition = position.Reduce<2>();
    const auto portalPositionsEntry = portalPositions.find(reducedPosition);
    if (portalPositionsEntry != portalPositions.end()) {
        const auto& portalsEntry = portals.find(portalPositionsEntry->second);
        const auto& portal = portalsEntry->second;
        const auto goingOut = reducedPosition == portal.first;
        Position<3> otherSideOfPortal = (
            goingOut
            ? portal.second
            : portal.first
        );
        otherSideOfPortal.m[2] = position.m[2] + (goingOut ? -1 : 1);

        // You can't use a portal leading out if you're already at
        // the outer-most level.
        if (otherSideOfPortal.m[2] >= 0) {
            neighbors.push_back(otherSideOfPortal);
            deepestNeighborZ = std::max(deepestNeighborZ, otherSideOfPortal.m[2]);
        }
    }
    return neighbors;
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
    Maze maze;
    std::string line;
    while (std::getline(input, line)) {
        maze.Input(std::move(line));
    }

    // Scan the map for the starting position, ending position,
    // and the positions of all portals.
    Position<2> startingPosition, endingPosition;
    Portals portals;
    PortalPositions portalPositions;
    for (size_t y = 0; y < maze.height; ++y) {
        for (size_t x = 0; x < maze.width; ++x) {
            const Position<2> cellPosition{(int)x, (int)y};
            const auto cell = maze.CheckForPortal(cellPosition);
            if (cell) {
                static std::vector< Position<2> > directions{
                    {0,  1},
                    {1,  0},
                };
                for (const auto& direction: directions) {
                    const auto neighbor = cellPosition + direction;
                    const auto cell2 = maze.CheckForPortal(neighbor);
                    if (cell2) {
                        const auto before = cellPosition - direction;
                        const auto after = neighbor + direction;
                        const auto portalFloor = (
                            maze.IsFloor(before)
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
                                if (maze.IsOuterEdge(portalFloor)) {
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
        maze.width, maze.height,
        startingPosition.m[0], startingPosition.m[1],
        endingPosition.m[0], endingPosition.m[1],
        portals.size()
    );
    for (const auto& portal: portals) {
        printf(
            "  %s: %dx%d <-> %dx%d\n",
            portal.first.c_str(),
            portal.second.first.m[0], portal.second.first.m[1],
            portal.second.second.m[0], portal.second.second.m[1]
        );
    }

    // Use the A* path-finding algorithm to find the shortest path
    // from the entrance to the exit.
    const auto startTime = std::chrono::steady_clock::now();
    int deepestNeighborZ = 0;
    const auto path = PathFinding< Position<3> >::FindPath(
        // Desired start and end points of the path
        startingPosition, endingPosition,

        // Function to determine the neighbors of any position
        [&](const Position<3>& position){ return Neighbors(maze, portals, portalPositions, position, deepestNeighborZ); },

        // Cost function (easy since the neighbors function takes
        // care of walls for us).
        [](const Position<3>& start, const Position<3>& end){ return 1; },

        // Heuristic function, which A* uses to optimize the search.
        // I didn't bother making one.  I usually use manhattan distance,
        // however in this puzzle, is inadmissible.
        //
        // [11:14] igroc: https://en.wikipedia.org/wiki/Admissible_heuristic
        //
        // So just devolve to Breadth First Search (Dijkstra's Algorithm
        // considers different costs, but our cost is a constant).
        [](const Position<3>& start, const Position<3>& end){ return 0; }
    );
    const auto endTime = std::chrono::steady_clock::now();
    printf("The shortest path through the maze is %d steps.\n", path.cost);
    int maxZ = 0;
    for (const auto& step: path.steps) {
        maxZ = std::max(maxZ, step.m[2]);
    }
    printf("The deepest we went was %d steps down.\n", maxZ);
    printf("The deepest neighbor we considered was %d steps down.\n", deepestNeighborZ);
    printf(
        "It took %lf seconds to determine this.\n",
        std::chrono::duration< double >(endTime - startTime).count()
    );
    return EXIT_SUCCESS;
}
