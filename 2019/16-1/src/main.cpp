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

    // Apply the "FFT" many times.
    constexpr size_t iterations = 100;
    for (size_t i = 0; i < iterations; ++i) {
        decltype(digits) newDigits;
        newDigits.reserve(order);
        for (size_t j = 0; j < order; ++j) {
            int sum = 0;
            for (size_t k = 0; k < order; ++k) {
                sum += digits[k] * sine[j][k];
            }
            newDigits.push_back(abs(sum % 10));
        }
        digits.swap(newDigits);
        printf("After %zu phases: ", i + 1);
        for (auto digit: digits) {
            printf("%d", digit);
        }
        printf("\n");
    }
    return EXIT_SUCCESS;
}
