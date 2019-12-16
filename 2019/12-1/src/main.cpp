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
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <vector>

#ifdef _WIN32
#include <crtdbg.h>
#endif /* _WIN32 */

struct Vector3D {
    int x = 0;
    int y = 0;
    int z = 0;

    bool operator==(const Vector3D& other) const {
        return (
            (x == other.x)
            && (y == other.y)
            && (z == other.z)
        );
    }

    bool operator!=(const Vector3D& other) const {
        return !(*this == other);
    }

    bool operator<(const Vector3D& other) const {
        if (x < other.x) {
            return true;
        } else if (x > other.x) {
            return false;
        } else if (y < other.y) {
            return true;
        } else if (y > other.y) {
            return false;
        } else {
            return (z < other.z);
        }
    }

    Vector3D& operator+=(const Vector3D& other) {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    int Energy() const {
        return abs(x) + abs(y) + abs(z);
    }

    std::string Report() const {
        std::ostringstream buffer;
        buffer << "<";
        buffer << "x=" << x;
        buffer << ", y=" << y;
        buffer << ", z=" << z;
        buffer << ">";
        return buffer.str();
    }

};

struct Moon {
    Vector3D position;
    Vector3D velocity;

    std::string Report() const {
        std::ostringstream buffer;
        buffer << "pos=" << position.Report();
        buffer << ", vel=" << velocity.Report();
        return buffer.str();
    }
};

int Direction(int magnitude) {
    return (
        (magnitude == 0)
        ? 0
        : magnitude / abs(magnitude)
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

    // Open the input file and read in the moon positions.
    std::ifstream input("input.txt");
    std::vector< Moon > moons;
    std::string line;
    while (std::getline(input, line)) {
        Moon moon;
        if (
            sscanf(
                line.c_str(),
                "<x=%d, y=%d, z=%d>",
                &moon.position.x,
                &moon.position.y,
                &moon.position.z
            ) != 3
        ) {
            fprintf(stderr, "Invalid input!\n");
            return EXIT_FAILURE;
        }
        moons.push_back(std::move(moon));
        line.clear();
    }

    // Simulate the motion of the moons over a certain number of steps.
    constexpr size_t maxSteps = 1000;
    std::vector< Moon > newMoons;
    newMoons.reserve(moons.size());
    for (size_t step = 0; step < maxSteps; ++step) {
        printf("After %zu steps:\n", step + 1);
        newMoons.clear();
        for (const auto& moon: moons) {
            Vector3D deltaVelocity;
            for (const auto& otherMoon: moons) {
                const auto dx = otherMoon.position.x - moon.position.x;
                const auto dy = otherMoon.position.y - moon.position.y;
                const auto dz = otherMoon.position.z - moon.position.z;
                deltaVelocity.x += Direction(dx);
                deltaVelocity.y += Direction(dy);
                deltaVelocity.z += Direction(dz);
            }
            Moon newMoon(moon);
            newMoon.velocity += deltaVelocity;
            newMoon.position += newMoon.velocity;
            printf("%s\n", newMoon.Report().c_str());
            newMoons.push_back(std::move(newMoon));
        }
        moons.swap(newMoons);
        printf("\n");
    }

    // Calculate the total energy in the system.
    int totalEnergy = 0;
    for (const auto& moon: moons) {
        totalEnergy += (
            moon.position.Energy()
            * moon.velocity.Energy()
        );
    }
    printf("Total energy in the system: %d\n", totalEnergy);
    return EXIT_SUCCESS;
}
