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
#include <unordered_map>
#include <vector>

#ifdef _WIN32
#include <crtdbg.h>
#endif /* _WIN32 */

template< int N > struct Vector {
    std::vector< int > m;

    Vector() {
        m.resize(N);
    }

    bool operator==(const Vector& other) const {
        for (size_t i = 0; i < m.size(); ++i) {
            if (m[i] != other.m[i]) {
                return false;
            }
        }
        return true;
    }

    bool operator!=(const Vector& other) const {
        return !(*this == other);
    }

    bool operator<(const Vector& other) const {
        for (size_t i = 0; i < m.size(); ++i) {
            if (m[i] < other.m[i]) {
                return true;
            } else if (m[i] > other.m[i]) {
                return false;
            }
        }
        return false;
    }

    Vector& operator+=(const Vector& other) {
        for (size_t i = 0; i < m.size(); ++i) {
            m[i] += other.m[i];
        }
        return *this;
    }
};

struct Moon {
    Vector<3> position;
    Vector<3> velocity;
};

int Direction(int magnitude) {
    if (magnitude == 0) {
        return 0;
    } else if (magnitude > 0) {
        return 1;
    } else {
        return -1;
    }
}

template< typename T > std::unordered_map< T, size_t > PrimeFactors(T value) {
    static std::vector< T > primes;
    static T nextPrimeCandidate = 2;
    std::unordered_map< T, size_t > primeFactors;
    size_t nextPrime = 0;
    T remainder = value;
    while (remainder > 1) {
        if (nextPrime >= primes.size()) {
            for (;;) {
                if (value == nextPrimeCandidate) {
                    primes.push_back(nextPrimeCandidate);
                    ++nextPrimeCandidate;
                    break;
                } else if (PrimeFactors(nextPrimeCandidate).empty()) {
                    break;
                } else {
                    ++nextPrimeCandidate;
                }
            }
        }
        if (value == primes[nextPrime]) {
            break;
        } else if ((remainder % primes[nextPrime]) == 0) {
            ++primeFactors[primes[nextPrime]];
            remainder /= primes[nextPrime];
        } else {
            if (
                (value != remainder)
                && (primes[nextPrime] * primes[nextPrime] > remainder)
            ) {
                ++primeFactors[remainder];
                break;
            }
            ++nextPrime;
        }
    }
    return primeFactors;
}

template< typename T > T FindLeastCommonMultiple(const std::vector< T > values) {
    std::vector< std::unordered_map< T, size_t > > primeFactorsForValues;
    for (auto value: values) {
        primeFactorsForValues.push_back(PrimeFactors(value));
    }
    std::unordered_map< T, size_t > primeFactorsForLCM;
    for (const auto& primeFactors: primeFactorsForValues) {
        for (const auto& primeFactorsEntry: primeFactors) {
            const auto prime = primeFactorsEntry.first;
            const auto power = primeFactorsEntry.second;
            auto& primeFactorForLCM = primeFactorsForLCM[prime];
            primeFactorForLCM = std::max(primeFactorForLCM, power);
        }
    }
    T lcm = 1;
    for (const auto& primeFactor: primeFactorsForLCM) {
        const auto prime = primeFactor.first;
        const auto power = primeFactor.second;
        for (size_t i = 0; i < power; ++i) {
            lcm *= prime;
        }
    }
    return lcm;
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
                &moon.position.m[0],
                &moon.position.m[1],
                &moon.position.m[2]
            ) != 3
        ) {
            fprintf(stderr, "Invalid input!\n");
            return EXIT_FAILURE;
        }
        moons.push_back(std::move(moon));
        line.clear();
    }

    // Simulate the motion of the moons until we see them in the
    // same state twice for each dimension.
    auto originalMoons = moons;
    std::vector< Moon > newMoons;
    newMoons.reserve(moons.size());
    std::vector< size_t > periods;
    for (size_t d = 0; d < 3; ++d) {
        size_t stepCount = 0;
        for(;;) {
            // Simulate one dimension of the U N I V E R S E.
            newMoons.clear();
            bool allMoonsReturned = true;
            for (size_t m = 0; m < moons.size(); ++m) {
                const auto& moon = moons[m];
                int deltaVelocity = 0;
                for (const auto& otherMoon: moons) {
                    const auto delta = otherMoon.position.m[d] - moon.position.m[d];
                    deltaVelocity += Direction(delta);
                }
                Moon newMoon(moon);
                newMoon.velocity.m[d] += deltaVelocity;
                newMoon.position.m[d] += newMoon.velocity.m[d];
                if (
                    (newMoon.velocity.m[d] != originalMoons[m].velocity.m[d])
                    || (newMoon.position.m[d] != originalMoons[m].position.m[d])
                ) {
                    allMoonsReturned = false;
                }
                newMoons.push_back(std::move(newMoon));
            }
            moons.swap(newMoons);

            // Count this step.
            ++stepCount;

            // Check if the current state of the stepped dimension of
            // the U N I V E R S E has returned back to the beginning.
            if (allMoonsReturned) {
                break;
            }
        }
        printf(
            "Every %zu steps, the moons return to their original positions and velocities along the %c dimension.\n",
            stepCount,
            (char)('X' + d)
        );
        periods.push_back(stepCount);
    }

    // Find the least common multiple of the periods of each dimension.
    const auto lcm = FindLeastCommonMultiple(periods);
    printf("The moons return to their original positions and velocities after %zu steps.\n", lcm);
    return EXIT_SUCCESS;
}
