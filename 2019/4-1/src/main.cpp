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

std::vector< int > BreakIntoDigits(int number) {
    std::vector< int > digits;
    digits.reserve(6);
    digits.push_back(number / 100000);
    digits.push_back((number / 10000) % 10);
    digits.push_back((number / 1000) % 10);
    digits.push_back((number / 100) % 10);
    digits.push_back((number / 10) % 10);
    digits.push_back(number % 10);
    return digits;
}

bool IsPossiblePassword(
    const std::vector< int >& digits,
    int candidate
) {
    bool foundSameNeighbors = false;
    for (size_t i = 0; i < 5; ++i) {
        if (digits[i] == digits[i+1]) {
            foundSameNeighbors = true;
        } else if (digits[i] > digits[i+1]) {
            return false;
        }
    }
    return foundSameNeighbors;
}

int NextCandidate(
    const std::vector< int >& digits,
    int candidate
) {
    // From right to left, find the first digit that is not a '9'.
    // If we simply added one, it will roll over to 0, which would not
    // yield a valid candidate.
    //
    // Example:  136799 -> 136800 is not a valid candidate
    //                  -> 136888 is the next valid candidate
    //
    // While searching for the last "non-nine", build up a variable "place"
    // which will be 10^N where N is the place of the "non-nine"
    //
    // Example:  136799
    //              ^
    //               \   place = 100
    //                -- first non-nine
    //
    // Then add one, multiple "place" by the digit that rolled over,
    // and add that digit to the rest of the digits back towards the right.
    //
    // Example:  136799 -> 136800  (place = 100)
    //                             (place *= 10 -> 800)
    //           136800 -> 136880  (place /= 10 -> 80)
    //           136880 -> 136888  (place /= 10 -> 8)
    //                             (place /= 10 -> 0 : stop!)
    size_t pos = 5;
    int place = 1;
    for (;;) {
        if (digits[pos] != 9) {
            ++candidate;
            place *= (digits[pos] + 1);
            while (place > 10) {
                place /= 10;
                candidate += place;
            }
            return candidate;
        }
        place *= 10;
        if (pos == 0) {
            break;
        }
        --pos;
    }
    return 1000000;
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

    // Read and parse from the input file the starting and ending numbers.
    std::string line;
    (void)std::getline(input, line);
    int start, end;
    if (sscanf(line.c_str(), "%d-%d", &start, &end) != 2) {
        (void)fprintf(stderr, "Bad input detected\n");
    }
    printf("Range: %d to %d\n", start, end);

    // Beginning with the first candidate, check the rules to see
    // if it's a valid password.
    //
    // After checking the password, advance to the next candidate.
    //
    // Stop when we get past the last candidate.
    size_t numPasswordsConsidered = 0;
    size_t numPossiblePasswords = 0;
    int candidate = start;
    while (candidate <= end) {
        const auto digits = BreakIntoDigits(candidate);
        if (IsPossiblePassword(digits, candidate)) {
            ++numPossiblePasswords;
        }
        printf("Candidate: %d\n", candidate);
        candidate = NextCandidate(digits, candidate);
        ++numPasswordsConsidered;
    }
    printf("There are %zu possible passwords.\n", numPossiblePasswords);
    printf("%zu passwords were considered.\n", numPasswordsConsidered);
    return EXIT_SUCCESS;
}
