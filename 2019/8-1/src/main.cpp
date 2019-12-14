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
#include <memory>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <vector>

#ifdef _WIN32
#include <crtdbg.h>
#endif /* _WIN32 */

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

    // Find the layer with the fewest 0 digits.
    // On that layer, count the number of 1 and 2 digits,
    // and multiply them.
    const size_t width = 25;
    const size_t height = 6;
    const auto layerSize = width * height;
    const auto numLayers = line.length() / layerSize;
    if (numLayers * layerSize != line.length()) {
        (void)fprintf(stderr, "Input is not a whole number of layers!\n");
        return EXIT_FAILURE;
    }
    printf("There are %zu image layers.\n", numLayers);
    auto fewestZeroDigits = std::numeric_limits< size_t >::max();
    size_t answer = 0;
    for (size_t i = 0; i < numLayers; ++i) {
        std::map< char, size_t > digitCount;
        for (size_t j = 0; j < layerSize; ++j) {
            ++digitCount[line[i * layerSize + j]];
        }
        const auto zeroDigits = digitCount['0'];
        if (zeroDigits < fewestZeroDigits) {
            fewestZeroDigits = zeroDigits;
            answer = digitCount['1'] * digitCount['2'];
        }
    }
    printf("Answer: %zu\n", answer);
    return EXIT_SUCCESS;
}
