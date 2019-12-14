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

    // Blend the pixels on each layer to produce an output image.
    const size_t width = 25;
    const size_t height = 6;
    const auto layerSize = width * height;
    const auto numLayers = line.length() / layerSize;
    if (numLayers * layerSize != line.length()) {
        (void)fprintf(stderr, "Input is not a whole number of layers!\n");
        return EXIT_FAILURE;
    }
    for (size_t y = 0; y < height; ++y) {
        for (size_t x = 0; x < width; ++x) {
            char output = ' ';
            for (size_t z = 0; z < numLayers; ++z) {
                const auto pixel = line[z * layerSize + y * width + x];
                if (pixel == '0') { // black
                    output = '.';
                    break;
                } else if (pixel == '1') { // white
                    output = 'X';
                    break;
                } else if (pixel == '2') { // transparent
                    continue;
                } else {
                    (void)fprintf(stderr, "Illegal pixel value, layer %zu, x=%zu, y=%zu\n", z, x, y);
                    return EXIT_FAILURE;
                }
            }
            printf("%c", output);
        }
        printf("\n");
    }
    return EXIT_SUCCESS;
}
