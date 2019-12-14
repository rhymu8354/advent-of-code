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
#include <inttypes.h>
#include <memory>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <vector>

#ifdef _WIN32
#include <crtdbg.h>
#endif /* _WIN32 */

intmax_t GetNextNumber(
    const std::string& input,
    size_t& pos
) {
    auto delimiter = input.find(',', pos);
    if (delimiter == std::string::npos) {
        delimiter = input.length();
    }
    intmax_t number;
    if (sscanf(input.substr(pos, delimiter - pos).c_str(), "%" SCNdMAX, &number) != 1) {
        (void)fprintf(stderr, "Bad input detected at position %zu\n", pos);
        exit(1);
    }
    pos = delimiter + 1;
    return number;
}

struct Machine {
    size_t id = 0;
    size_t pos = 0;
    std::vector< intmax_t > numbers;
    std::vector< intmax_t > input;
    bool halted = false;
    intmax_t relativeBase = 0;

    void ExpandToFit(size_t index) {
        if (index >= numbers.size()) {
            numbers.resize(index + 1);
        }
    }

    size_t LoadIndex(
        size_t pos,
        int mode
    ) {
        switch (mode) {
            case 0: { // position
                const auto index = (size_t)numbers[pos];
                printf("%zu", index);
                return index;
            } break;

            case 2: { // relative
                const auto offset = numbers[pos];
                const auto index = (size_t)(relativeBase + offset);
                printf("%zu (%" PRIdMAX " + %" PRIdMAX ")", index, relativeBase, offset);
                return index;
            } break;

            default: {
                (void)fprintf(stderr, "Invalid index mode for offset %zu\n", pos);
                exit(1);
            }
        }
        return 0;
    }

    intmax_t LoadArgument(
        size_t pos,
        int mode
    ) {
        switch (mode) {
            case 0: { // position
                const auto index = (size_t)numbers[pos];
                ExpandToFit(index);
                const auto arg = numbers[index];
                printf("%" PRIdMAX " (from %zu)", arg, index);
                return arg;
            } break;

            case 1: { // immediate
                const auto arg = numbers[pos];
                printf("%" PRIdMAX " (immediate)", arg);
                return arg;
            } break;

            case 2: { // relative
                const auto offset = numbers[pos];
                const auto index = (size_t)(relativeBase + offset);
                ExpandToFit(index);
                const auto arg = numbers[index];
                printf("%" PRIdMAX " (from %" PRIdMAX " + %" PRIdMAX " = %zu)", arg, relativeBase, offset, index);
                return arg;
            } break;

            default: {
                (void)fprintf(stderr, "Invalid argument mode for offset %zu\n", pos);
                exit(1);
            }
        }
        return 0;
    }

    void Store(
        size_t index,
        intmax_t value
    ) {
        ExpandToFit(index);
        numbers[index] = value;
    }

    void Run(std::vector< intmax_t >& output) {
        printf("*** Machine %zu Run ***\n", id);
        while (!halted) {
            const auto opcode = numbers[pos] % 100;
            printf("[%zu] ", pos);
            switch (opcode) {
                case 1: { // add
                    printf("Adding ");
                    const auto arg1 = LoadArgument(pos + 1, (numbers[pos] / 100) % 10);
                    printf(" to ");
                    const auto arg2 = LoadArgument(pos + 2, (numbers[pos] / 1000) % 10);
                    printf(" and storing at ");
                    const auto index3 = LoadIndex(pos + 3, (numbers[pos] / 10000) % 10);
                    printf("\n");
                    Store(index3, arg1 + arg2);
                    pos += 4;
                } break;

                case 2: { // multiply
                    printf("Multiplying ");
                    const auto arg1 = LoadArgument(pos + 1, (numbers[pos] / 100) % 10);
                    printf(" by ");
                    const auto arg2 = LoadArgument(pos + 2, (numbers[pos] / 1000) % 10);
                    printf(" and storing at ");
                    const auto index3 = LoadIndex(pos + 3, (numbers[pos] / 10000) % 10);
                    printf("\n");
                    Store(index3, arg1 * arg2);
                    pos += 4;
                } break;

                case 3: { // input
                    printf("Input value to ");
                    const auto index = LoadIndex(pos + 1, (numbers[pos] / 100) % 10);
                    if (input.empty()) {
                        printf("(no more input)\n");
                        printf("*** Machine %zu Needs Input ***\n", id);
                        return;
                    }
                    const auto inputValue = input[0];
                    (void)input.erase(input.begin());
                    printf(": %" PRIdMAX "\n", inputValue);
                    Store(index, inputValue);
                    pos += 2;
                } break;

                case 4: { // output
                    printf("Output: ");
                    const auto outputValue = LoadArgument(pos + 1, (numbers[pos] / 100) % 10);
                    printf("\n");
                    output.push_back(outputValue);
                    pos += 2;
                } break;

                case 5: { // jump-if-true
                    printf("Jumping, if ");
                    const auto arg1 = LoadArgument(pos + 1, (numbers[pos] / 100) % 10);
                    printf(" is non-zero, to ");
                    const auto arg2 = (size_t)LoadArgument(pos + 2, (numbers[pos] / 1000) % 10);
                    printf("\n");
                    if (arg1 != 0) {
                        pos = arg2;
                    } else {
                        pos += 3;
                    }
                } break;

                case 6: { // jump-if-false
                    printf("Jumping, if ");
                    const auto arg1 = LoadArgument(pos + 1, (numbers[pos] / 100) % 10);
                    printf(" is zero, to ");
                    const auto arg2 = (size_t)LoadArgument(pos + 2, (numbers[pos] / 1000) % 10);
                    printf("\n");
                    if (arg1 == 0) {
                        pos = arg2;
                    } else {
                        pos += 3;
                    }
                } break;

                case 7: { // less-than
                    printf("Testing if ");
                    const auto arg1 = LoadArgument(pos + 1, (numbers[pos] / 100) % 10);
                    printf(" is less than ");
                    const auto arg2 = LoadArgument(pos + 2, (numbers[pos] / 1000) % 10);
                    printf(" and storing result to ");
                    const auto index3 = LoadIndex(pos + 3, (numbers[pos] / 10000) % 10);
                    printf("\n");
                    Store(
                        index3,
                        (
                            (arg1 < arg2)
                            ? 1
                            : 0
                        )
                    );
                    pos += 4;
                } break;

                case 8: { // equals
                    printf("Testing if ");
                    const auto arg1 = LoadArgument(pos + 1, (numbers[pos] / 100) % 10);
                    printf(" equals ");
                    const auto arg2 = LoadArgument(pos + 2, (numbers[pos] / 1000) % 10);
                    printf(" and storing result to ");
                    const auto index3 = LoadIndex(pos + 3, (numbers[pos] / 10000) % 10);
                    printf("\n");
                    Store(
                        index3,
                        (
                            (arg1 == arg2)
                            ? 1
                            : 0
                        )
                    );
                    pos += 4;
                } break;

                case 9: { // adjust relative base
                    printf("Adjust relative base by ");
                    const auto arg1 = LoadArgument(pos + 1, (numbers[pos] / 100) % 10);
                    relativeBase += arg1;
                    printf(" to %" PRIdMAX "\n", relativeBase);
                    pos += 2;
                } break;

                case 99: { // stop
                    printf("Halt!\n");
                    halted = true;
                } break;

                default: { // ERROR!!
                    (void)fprintf(stderr, "Invalid opcode (%" PRIdMAX ")\n", opcode);
                    exit(1);
                } break;
            }
        }
        printf("*** Machine %zu Halted ***\n", id);
    }
};

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
    std::vector< intmax_t > numbers;
    while (pos < inputLength) {
        const auto number = GetNextNumber(line, pos);
        numbers.push_back(number);
    }

    // Load machine with input.
    Machine machine;
    machine.id = 1;
    machine.numbers = std::move(numbers);
    printf("Input: ");
    intmax_t inputValue;
    if (scanf("%" SCNdMAX, &inputValue) != 1) {
        (void)fprintf(stderr, "Invalid input\n");
        exit(1);
    }
    machine.input.push_back(inputValue);

    // Run the machine and print its output.
    std::vector< intmax_t > output;
    machine.Run(output);
    printf("Output: ");
    bool first = true;
    for (auto value: output) {
        if (!first) {
            printf(", ");
        }
        first = false;
        printf("%" PRIdMAX, value);
    }
    printf("\n");
    return EXIT_SUCCESS;
}
