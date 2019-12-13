/**
 * @file main.cpp
 *
 * This module holds the main() function, which is the entrypoint
 * to the program.
 *
 * © 2019 by Richard Walters
 */

#include <functional>
#include <memory>
#include <stdlib.h>
#include <stdio.h>
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

    // Open the input file and read in the mass of each module.
    std::vector< int > masses;
    std::unique_ptr< FILE, std::function< void(FILE*) > > inputOwner(
        fopen("input.txt", "rt"),
        [](FILE* file){ (void)fclose(file); }
    );
    const auto input = inputOwner.get();
    while (!feof(input)) {
        static char buffer[256];
        if (fgets(buffer, sizeof(buffer), input) == NULL) {
            if (feof(input)) {
                break;
            }
            (void)fprintf(stderr, "Error reading input line!\n");
            return EXIT_FAILURE;
        }
        int mass;
        if (sscanf(buffer, "%d", &mass) != 1) {
            (void)fprintf(stderr, "Error parsing input line!\n");
            return EXIT_FAILURE;
        }
        masses.push_back(mass);
        printf("Accepted input mass: %d\n", mass);
    }

    // Calculate and sum the fuel requirements for each module.
    int totalFuelRequirement = 0;
    for (auto mass: masses) {
        totalFuelRequirement += (mass / 3) - 2;
    }

    // Output the result.
    printf("Total fuel requirement: %d\n", totalFuelRequirement);
    return EXIT_SUCCESS;
}
