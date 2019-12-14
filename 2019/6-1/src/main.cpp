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
#include <map>
#include <memory>
#include <set>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <vector>

#ifdef _WIN32
#include <crtdbg.h>
#endif /* _WIN32 */

struct Body {
    std::string name;
    Body* parent = nullptr;
    std::set< Body* > children;
};

size_t CountOrbits(
    const std::map< std::string, std::unique_ptr< Body > >& bodies,
    const Body* parent,
    size_t parentOrbits = 0
) {
    size_t orbits = 0;
    for (const auto& child: parent->children) {
        orbits += CountOrbits(bodies, child, parentOrbits + 1) + parentOrbits + 1;
    }
    return orbits;
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

    // Open the input file and read in and parse each line as an
    // orbital relationship.
    std::ifstream input("input.txt");
    std::string line;
    std::map< std::string, std::unique_ptr< Body > > bodies;
    while (std::getline(input, line)) {
        auto delimiter = line.find(')');
        if (delimiter == std::string::npos) {
            (void)fprintf(stderr, "Invalid input: %s\n", line.c_str());
            return EXIT_FAILURE;
        }
        const auto parentName = line.substr(0, delimiter);
        const auto childName = line.substr(delimiter + 1);
        auto& parent = bodies[parentName];
        if (parent == nullptr) {
            parent.reset(new Body);
            parent->name = parentName;
        }
        auto& child = bodies[childName];
        if (child == nullptr) {
            child.reset(new Body);
            child->name = childName;
        }
        if (child->parent != nullptr) {
            (void)fprintf(
                stderr,
                "%s cannot directly orbit both %s and %s\n",
                childName.c_str(),
                parentName.c_str(),
                child->parent->name.c_str()
            );
            return EXIT_FAILURE;
        }
        child->parent = parent.get();
        (void)parent->children.insert(child.get());
    }

    // Beginning with the "universal center of mass", count the orbits.
    const auto com = bodies.find("COM");
    if (com == bodies.end()) {
        (void)fprintf(stderr, "Universal center of mass not found!\n");
        return EXIT_FAILURE;
    }
    printf("Total orbits: %zu\n", CountOrbits(bodies, com->second.get()));
    return EXIT_SUCCESS;
}
