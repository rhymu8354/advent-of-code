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

int LoadArgument(
    std::vector< int >& numbers,
    size_t pos,
    int mode
) {
    switch (mode) {
        case 0: { // position
            const auto index = (size_t)numbers[pos];
            const auto arg = numbers[index];
            printf("%d (from %zu)", arg, index);
            return arg;
        } break;

        case 1: { // immediate
            const auto arg = numbers[pos];
            printf("%d (immediate)", arg);
            return arg;
        } break;

        default: {
            (void)fprintf(stderr, "Invalid mode for offset %zu\n", pos);
            exit(1);
        }
    }
    return 0;
}

std::vector< int > RunMachine(
    std::vector< int > numbers,
    const std::vector< int >& input
) {
    size_t inputPos = 0;
    std::vector< int > output;
    printf("*** Machine Start ***\n");
    size_t pos = 0;
    while (pos < numbers.size()) {
        const auto opcode = numbers[pos] % 100;
        printf("[%zu] ", pos);
        switch (opcode) {
            case 1: { // add
                printf("Adding ");
                const auto arg1 = LoadArgument(numbers, pos + 1, (numbers[pos] / 100) % 10);
                printf(" to ");
                const auto arg2 = LoadArgument(numbers, pos + 2, (numbers[pos] / 1000) % 10);
                const auto index3 = numbers[pos+3];
                printf(" and storing at %zu\n", index3);
                numbers[index3] = arg1 + arg2;
                pos += 4;
            } break;

            case 2: { // multiply
                printf("Multiplying ");
                const auto arg1 = LoadArgument(numbers, pos + 1, (numbers[pos] / 100) % 10);
                printf(" by ");
                const auto arg2 = LoadArgument(numbers, pos + 2, (numbers[pos] / 1000) % 10);
                const auto index3 = numbers[pos+3];
                printf(" and storing at %zu\n", index3);
                numbers[index3] = arg1 * arg2;
                pos += 4;
            } break;

            case 3: { // input
                const auto index = numbers[pos+1];
                if (inputPos >= input.size()) {
                    (void)fprintf(stderr, "Input exhausted!\n");
                    return output;
                }
                const auto inputValue = input[inputPos++];
                printf("Input value: %d\n", inputValue);
                numbers[index] = inputValue;
                pos += 2;
            } break;

            case 4: { // output
                printf("Output: ");
                const auto outputValue = LoadArgument(numbers, pos + 1, (numbers[pos] / 100) % 10);
                printf("\n");
                output.push_back(outputValue);
                pos += 2;
            } break;

            case 5: { // jump-if-true
                printf("Jumping, if ");
                const auto arg1 = LoadArgument(numbers, pos + 1, (numbers[pos] / 100) % 10);
                printf(" is non-zero, to ");
                const auto arg2 = (size_t)LoadArgument(numbers, pos + 2, (numbers[pos] / 1000) % 10);
                printf("\n");
                if (arg1 != 0) {
                    pos = arg2;
                } else {
                    pos += 3;
                }
            } break;

            case 6: { // jump-if-false
                printf("Jumping, if ");
                const auto arg1 = LoadArgument(numbers, pos + 1, (numbers[pos] / 100) % 10);
                printf(" is zero, to ");
                const auto arg2 = (size_t)LoadArgument(numbers, pos + 2, (numbers[pos] / 1000) % 10);
                printf("\n");
                if (arg1 == 0) {
                    pos = arg2;
                } else {
                    pos += 3;
                }
            } break;

            case 7: { // less-than
                printf("Testing if ");
                const auto arg1 = LoadArgument(numbers, pos + 1, (numbers[pos] / 100) % 10);
                printf(" is less than ");
                const auto arg2 = LoadArgument(numbers, pos + 2, (numbers[pos] / 1000) % 10);
                const auto index3 = (size_t)numbers[pos+3];
                printf(" and storing result to %zu\n", index3);
                numbers[index3] = (
                    (arg1 < arg2)
                    ? 1
                    : 0
                );
                pos += 4;
            } break;

            case 8: { // equals
                printf("Testing if ");
                const auto arg1 = LoadArgument(numbers, pos + 1, (numbers[pos] / 100) % 10);
                printf(" equals ");
                const auto arg2 = LoadArgument(numbers, pos + 2, (numbers[pos] / 1000) % 10);
                const auto index3 = (size_t)numbers[pos+3];
                printf(" and storing result to %zu\n", index3);
                numbers[index3] = (
                    (arg1 == arg2)
                    ? 1
                    : 0
                );
                pos += 4;
            } break;

            case 99: { // stop
                printf("Done.\n");
                pos = numbers.size();
            } break;

            default: { // ERROR!!
                (void)fprintf(stderr, "Invalid opcode (%d)\n", opcode);
                exit(1);
            } break;
        }
    }
    printf("*** Machine Stop ***\n");
    return output;
}

void PrintPhases(const std::vector< int >& phases) {
    bool first = true;
    for (auto phase: phases) {
        if (!first) {
            printf(", ");
        }
        first = false;
        printf("%d", phase);
    }
    printf("\n");
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

    // Try all 32 combinations of phase settings and save the
    // setting that yields the largest output.
    std::vector< int > phases{0, 1, 2, 3, 4};
    std::vector< int > largestOutputPhases;
    int largestOutput = 0;
    do {
        int input = 0;
        printf("------------------------------------------\n");
        printf("Running machines with phases: ");
        PrintPhases(phases);
        for (auto phase: phases) {
            auto output = RunMachine(numbers, {phase, input});
            if (output.size() != 1) {
                fprintf(stderr, "Unexpected number of machine outputs!\n");
                return EXIT_FAILURE;
            }
            input = output[0];
        }
        if (input > largestOutput) {
            largestOutput = input;
            largestOutputPhases = phases;
        }
    } while (std::next_permutation(phases.begin(), phases.end()));
    printf("Largest output is %zu from phases: ", largestOutput);
    PrintPhases(largestOutputPhases);
    return EXIT_SUCCESS;
}
