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
#include <limits>
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
};

bool RayCast(
    const std::vector< std::string >& lines,
    const Position& origin,
    const Position& target,
    std::set< Position >& scanned,
    Position& first
) {
    bool seen = false;
    const auto height = (int)lines.size();
    const auto width = (int)lines[0].length();
    const auto dx = target.x - origin.x;
    const auto adx = abs(dx);
    const auto dy = target.y - origin.y;
    const auto ady = abs(dy);
    const auto directionX = (
        (adx == 0)
        ? 1
        : dx / adx
    );
    const auto directionY = (
        (ady == 0)
        ? 1
        : dy / ady
    );
    if (adx > ady) {
        Position hit = origin;
        int i = 0;
        for (;;) {
            hit.x += directionX;
            ++i;
            if (
                (hit.x < 0)
                || (hit.x >= width)
            ) {
                break;
            }
            if (((i * ady) % adx) == 0) {
                hit.y = origin.y + i * directionY * ady / adx;
                if (
                    (hit.y < 0)
                    || (hit.y >= height)
                ) {
                    break;
                }
                (void)scanned.insert(hit);
                if (lines[hit.y][hit.x] == '#') {
                    if (!seen) {
                        first = hit;
                    }
                    seen = true;
                }
            }
        }
    } else {
        Position hit = origin;
        int i = 0;
        for (;;) {
            hit.y += directionY;
            ++i;
            if (
                (hit.y < 0)
                || (hit.y >= height)
            ) {
                break;
            }
            if (((i * adx) % ady) == 0) {
                hit.x = origin.x + i * directionX * adx / ady;
                if (
                    (hit.x < 0)
                    || (hit.x >= width)
                ) {
                    break;
                }
                (void)scanned.insert(hit);
                if (lines[hit.y][hit.x] == '#') {
                    if (!seen) {
                        first = hit;
                    }
                    seen = true;
                }
            }
        }
    }
    return seen;
}

size_t Scan(
    const std::vector< std::string >& lines,
    const Position& origin
) {
    size_t seen = 0;
    std::set< Position > scanned;
    (void)scanned.insert(origin);
    const auto height = lines.size();
    const auto width = lines[0].length();
    for (size_t y = 0; y < height; ++y) {
        for (size_t x = 0; x < width; ++x) {
            Position target;
            target.x = (int)x;
            target.y = (int)y;
            if (scanned.find(target) != scanned.end()) {
                continue;
            }
            Position first;
            if (RayCast(lines, origin, target, scanned, first)) {
                ++seen;
            }
        }
    }
    return seen;
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
    printf("Map is %zux%zu\n", width, height);

    // For each asteroid, ray-cast to every other position in the map,
    // counting the number of asteroids seen.
    //
    // Remember which asteroid sees the most.
    size_t most = 0;
    Position mostOrigin;
    for (size_t y = 0; y < height; ++y) {
        for (size_t x = 0; x < width; ++x) {
            if (lines[y][x] != '#') {
                continue;
            }
            Position origin;
            origin.x = (int)x;
            origin.y = (int)y;
            const auto detected = Scan(lines, origin);
            if (detected > most) {
                most = detected;
                mostOrigin = origin;
            }
        }
    }
    printf(
        "Best is %zu,%zu with %zu other asteroids detected.\n",
        mostOrigin.x, mostOrigin.y,
        most
    );

    // For every possible target, compute the angle from the origin,
    // and sort them.  Drop duplicate angles.
    std::map< double, Position > targetsByAngle;
    static const auto halfPi = atan(1.0) * 2.0;
    for (size_t y = 0; y < height; ++y) {
        for (size_t x = 0; x < width; ++x) {
            Position target;
            target.x = (int)x;
            target.y = (int)y;
            if (target == mostOrigin) {
                continue;
            }
            const auto angle = fmod(
                atan2(
                    (double)(target.y - mostOrigin.y),
                    (double)(target.x - mostOrigin.x)
                ) + halfPi * 5,
                halfPi * 4
            );
            auto targetsByAngleEntry = targetsByAngle.find(angle);
            if (targetsByAngleEntry == targetsByAngle.end()) {
                targetsByAngle[angle] = target;
            }
        }
    }

    // Rotate clockwise through all target angles, firing until we have
    // destroyed 200 asteroids.
    size_t hits = 0;
    bool firing = true;
    while (firing) {
        for (const auto& targetsByAngleEntry: targetsByAngle) {
            std::set< Position > scanned;
            Position targetHit;
            if (
                RayCast(
                    lines,
                    mostOrigin,
                    targetsByAngleEntry.second,
                    scanned,
                    targetHit
                )
            ) {
                ++hits;
                printf("Destroyed asteroid #%zu at %zux%zu\n", hits, targetHit.x, targetHit.y);
                lines[targetHit.y][targetHit.x] = '.';
                if (hits == 200) {
                    firing = false;
                    break;
                }
            }
        }
    }
    return EXIT_SUCCESS;
}
