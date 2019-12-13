/**
 * @file main.cpp
 *
 * This module holds the main() function, which is the entrypoint
 * to the program.
 *
 * © 2019 by Richard Walters
 */

#include <fstream>
#include <functional>
#include <memory>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <vector>

#ifdef _WIN32
#include <crtdbg.h>
#endif /* _WIN32 */

int GetNextNumber(
    const std::string& input,
    size_t& pos
) {
    auto delimiter = input.find(',', pos);
    if (delimiter == std::string::npos) {
        delimiter = input.length();
    }
    int number;
    if (sscanf(input.substr(pos, delimiter - pos).c_str(), "%d", &number) != 1) {
        (void)fprintf(stderr, "Bad input detected at position %zu\n", pos);
        exit(1);
    }
    pos = delimiter + 1;
    return number;
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

    // Open the input file and read in the input string.
    std::ifstream input("input.txt");
    std::string line;
    (void)std::getline(input, line);
    printf("Accepted input line: \"%s\" (length: %zu)\n", line.c_str(), line.length());

    // Parse the input string into a vector of numbers.
    size_t pos = 0;
    const auto inputLength = line.length();
    std::vector< int > numbers;
    while (pos < inputLength) {
        const auto number = GetNextNumber(line, pos);
        numbers.push_back(number);
        printf("Next number (%zu): %d (pos=%zu)\n", numbers.size(), number, pos);
    }

    // Add in the "trick" hidden in the instructions. SwiftRage
    //
    // Actually what we're doing here is "repairing" the program,
    // placing the "magic smoke" back into the computer.  4Head
    numbers[1] = 12;
    numbers[2] = 2;

    // Run our state machine on the input string until it's exhausted.
    for (pos = 0; pos < numbers.size(); pos += 4) {
        const auto opcode = numbers[pos];
        printf("Opcode: %d\n", opcode);
        switch (opcode) {
            case 1: { // add
                const auto index1 = numbers[pos+1];
                const auto arg1 = numbers[index1];
                const auto index2 = numbers[pos+2];
                const auto arg2 = numbers[index2];
                const auto index3 = numbers[pos+3];
                printf(
                    "Adding %d (from %zu) to %d (from %zu) and storing at %zu\n",
                    arg1, index1,
                    arg2, index2,
                    index3
                );
                numbers[index3] = arg1 + arg2;
            } break;

            case 2: { // multiply
                const auto index1 = numbers[pos+1];
                const auto arg1 = numbers[index1];
                const auto index2 = numbers[pos+2];
                const auto arg2 = numbers[index2];
                const auto index3 = numbers[pos+3];
                printf(
                    "Multiplying %d (from %zu) to %d (from %zu) and storing at %zu\n",
                    arg1, index1,
                    arg2, index2,
                    index3
                );
                numbers[index3] = arg1 * arg2;
            } break;

            case 99: { // stop
                pos = numbers.size();
            } break;

            default: { // ERROR!!
                (void)fprintf(stderr, "Invalid opcode at offset %zu\n", pos);
                exit(1);
            } break;
        }
    }

    // Display all of the numbers.
    for (pos = 0; pos < numbers.size(); ++pos) {
        if (pos != 0) {
            printf(",");
        }
        printf("%d", numbers[pos]);
    }
    printf("\n");

    // Output the value left at position zero.
    printf("Output: %d\n", numbers[0]);
    return EXIT_SUCCESS;
}
