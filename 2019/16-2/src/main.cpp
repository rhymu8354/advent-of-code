/**
 * @file main.cpp
 *
 * This module holds the main() function, which is the entrypoint
 * to the program.
 *
 * © 2019 by Richard Walters
 */

#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

#ifdef _WIN32
#include <crtdbg.h>
#endif /* _WIN32 */

#pragma once

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

    // Open the input file and read in the input digits.
    std::ifstream input("input.txt");
    std::string line;
    (void)std::getline(input, line);
    std::vector< int > digits;
    const auto order = line.length();
    digits.reserve(order);
    for (auto ch: line) {
        digits.push_back(ch - '0');
    }

    // Compute the sine waves used in the "FFT".
    int primitiveSine[4] = {0, 1, 0, -1};
    std::vector< std::vector< int > > sine;
    sine.reserve(order);
    for (size_t j = 0; j < order; ++j) {
        std::vector< int > wave;
        wave.reserve(order);
        for (size_t k = 0; k < order; ++k) {
            const auto phase = ((k + 1) / (j + 1)) % 4;
            wave.push_back(primitiveSine[phase]);
        }
        sine.push_back(std::move(wave));
    }

    // Extract the "offset".
    const auto offset = (
        digits[0] * 1000000
        + digits[1] * 100000
        + digits[2] * 10000
        + digits[3] * 1000
        + digits[4] * 100
        + digits[5] * 10
        + digits[6]
    );

    // Apply a strategically small part of the "FFT" many times.
    //
    // Note that this only works if "offset" is at least half
    // order * 10000 (it was for my input).
    //
    // At the "offset" column, base pattern is 1's all the way to the
    // end, so this reduces the problem down to the following:
    //
    // 1. Start with the last digit.  This is also the last new digit,
    //    because the base pattern is all 0's except for the last.
    // 2. Walking backwards, each new previous digit is the next new digit
    //    plus the previous digit.
    //
    // For example, consider this offset which is at least half-way
    // through the digits:
    //
    // digits: 238741892741238946897643
    // offset:              ^
    //
    // The new digits are built from the right.  First the last digit,
    // which is the same always:
    //
    //     digits: 238741892741238946897643
    // new digits:                        3
    //
    // Next, walking back, each digit is the sum of the digits above and
    // to the right:
    //
    //     digits: 238741892741238946897643
    // new digits:                       73
    //                                  ↗
    //                             4 + 3
    //
    // Continue until we reach the offset:
    //
    //     digits: 238741892741238946897643
    // new digits:              74673790373
    //     offset:              ^
    //
    constexpr size_t iterations = 100;
    const auto trailer = order * 10000 - offset;
    decltype(digits) lastDigits;
    lastDigits.reserve(trailer);
    for (size_t i = order * 10000 - 1; i >= offset; --i) {
        lastDigits.push_back(digits[i % order]);
    }
    for (size_t i = 0; i < iterations; ++i) {
        decltype(lastDigits) newLastDigits(lastDigits);
        int last = 0;
        for (size_t j = 0; j < trailer; ++j) {
            last = ((last + newLastDigits[j]) % 10);
            newLastDigits[j] = last;
        }
        lastDigits.swap(newLastDigits);
        printf(
            "After %zu phases, 8 digits starting at offset %d: ",
            i + 1,
            offset
        );
        for (size_t i = trailer - 1; i >= trailer - 8; --i) {
            printf("%d", lastDigits[i]);
        }
        printf("\n");
    }
    return EXIT_SUCCESS;
}
