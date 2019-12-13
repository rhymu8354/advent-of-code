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

bool TryVerb(
    const std::vector< int >& intcode,
    int verb,
    int& noun,
    size_t& pos
) {
    // Walk backwards to find the end opcode.
    pos = intcode.size();
    while (intcode[--pos] != 99) {
    }

    // Run our state machine backwards until we either reach the statement
    // containing the noun, or we hit a multiply we can't reverse (the
    // dividend is not an integer).
    int foobar = 19690720;
    while (pos >= 4) {
        pos -= 4;
        const auto opcode = intcode[pos];
        const auto index1 = intcode[pos + 1];
        const auto index2 = intcode[pos + 2];
        const auto index3 = intcode[pos + 3];
        int constIndex = index1;
        int varIndex = index2;
        if (constIndex == pos - 1) {
            std::swap(constIndex, varIndex);
        }
        const auto constValue = (
            (constIndex == 2)
            ? verb
            : intcode[constIndex]
        );
        if (opcode == 1) {
            const auto previousFoobar = foobar - constValue;
            printf(
                "[%zu] %d (from %zu) + %d (from %zu) = %d\n",
                pos,
                previousFoobar, varIndex,
                constValue, constIndex,
                foobar
            );
            foobar = previousFoobar;
            if (varIndex == 1) { // noun
                noun = foobar;
                return true;
            }
        } else {
            const auto previousFoobar = foobar / constValue;
            if (previousFoobar * constValue == foobar) {
                printf(
                    "[%zu] %d (from %zu) * %d (from %zu) = %d\n",
                    pos,
                    previousFoobar, varIndex,
                    constValue, constIndex,
                    foobar
                );
                foobar = previousFoobar;
                if (varIndex == 1) { // noun
                    noun = foobar;
                    return true;
                }
            } else {
                printf(
                    "[%zu] %.10lg (from %zu) * %d (from %zu) = %d\n",
                    pos,
                    (double)foobar / constValue, varIndex,
                    constValue, constIndex,
                    foobar
                );
                return false;
            }
        }
    }
    return false;
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

    // Parse the input string into a vector of numbers.
    size_t pos = 0;
    const auto inputLength = line.length();
    std::vector< int > numbers;
    while (pos < inputLength) {
        const auto number = GetNextNumber(line, pos);
        numbers.push_back(number);
    }

    // Brute-force try each verb until we find a solution.
    for (int verb = 0; verb < 100; ++verb) {
        int noun;
        if (TryVerb(numbers, verb, noun, pos)) {
            printf("SUCCESS -> Noun: %d, Verb: %d\n", noun, verb);
            break;
        } else {
            printf("FAIL -> Verb: %d (at pos %zu)\n", verb, pos);
        }
    }
    return EXIT_SUCCESS;
}
