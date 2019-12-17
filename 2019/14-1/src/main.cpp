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
#include <math.h>
#include <memory>
#include <set>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <unordered_map>
#include <vector>

#ifdef _WIN32
#include <crtdbg.h>
#endif /* _WIN32 */

struct Chemical {
    std::string name;
    size_t quantity;
};

struct Recipe {
    Chemical output;
    std::vector< Chemical > inputs;
};

Chemical ParseChemical(const std::string& encoding) {
    Chemical chemical;
    const auto delimiter = encoding.find(' ');
    (void)sscanf(encoding.substr(0, delimiter).c_str(), "%zu", &chemical.quantity);
    chemical.name = encoding.substr(delimiter + 1);
    return chemical;
}

std::string FormatChemical(const Chemical& chemical) {
    std::ostringstream buffer;
    buffer << chemical.quantity;
    buffer << ' ';
    buffer << chemical.name;
    return buffer.str();
}

bool OnlyOre(const std::unordered_map< std::string, size_t >& quantities) {
    for (const auto& quantitiesEntry: quantities) {
        if (quantitiesEntry.first != "ORE") {
            return false;
        }
    }
    return true;
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

    // Open the input file and read in the moon positions.
    std::ifstream input("input.txt");
    std::unordered_map< std::string, Recipe > recipes;
    std::string line;
    while (std::getline(input, line)) {
        const auto outputDelimiter = line.find(" => ");
        if (outputDelimiter == std::string::npos) {
            fprintf(stderr, "BibleThump -- bad recipe!\n");
            return EXIT_FAILURE;
        }
        Recipe recipe;
        recipe.output = ParseChemical(line.substr(outputDelimiter + 4));
        auto inputs = line.substr(0, outputDelimiter);
        while (!inputs.empty()) {
            auto inputDelimiter = inputs.find(", ");
            std::string remainder;
            if (inputDelimiter == std::string::npos) {
                inputDelimiter = inputs.length();
            } else {
                remainder = inputs.substr(inputDelimiter + 2);
            }
            const auto nextInput = inputs.substr(0, inputDelimiter);
            recipe.inputs.push_back(ParseChemical(nextInput));
            inputs = remainder;
        }
        recipes[recipe.output.name] = recipe;
    }

    // Beginning with 1 FUEL, use recipes to work backwards until we
    // are left only with ORE.
    std::unordered_map< std::string, size_t > quantitiesNeeded;
    std::unordered_map< std::string, size_t > inventory;
    quantitiesNeeded["FUEL"] = 1;
    while (!OnlyOre(quantitiesNeeded)) {
        // Find the first chemical needed that isn't ORE,
        // and remove it from the needed list.
        auto firstQuantity = quantitiesNeeded.begin();
        while (firstQuantity->first == "ORE") {
            ++firstQuantity;
        }
        const auto name = firstQuantity->first;
        auto quantityNeeded = firstQuantity->second;
        quantitiesNeeded.erase(firstQuantity);

        // Pull from existing inventory first.
        const auto inventoryEntry = inventory.find(name);
        if (inventoryEntry != inventory.end()) {
            const auto amountFromInventory = std::min(
                inventoryEntry->second,
                quantityNeeded
            );
            quantityNeeded -= amountFromInventory;
            inventoryEntry->second -= amountFromInventory;
            if (inventoryEntry->second == 0) {
                (void)inventory.erase(inventoryEntry);
            }
            if (quantityNeeded == 0) {
                continue;
            }
        }

        // For any more needed, run batches of the appropriate
        // recipe, adding the inputs onto the quantities needed.
        const auto recipesEntry = recipes.find(name);
        const auto quantityPerBatch = recipesEntry->second.output.quantity;
        const auto numBatches = (quantityNeeded + quantityPerBatch - 1) / quantityPerBatch;
        printf(
            "Needing %zu %s -- making %zu batches\n",
            quantityNeeded,
            name.c_str(),
            numBatches
        );
        for (const auto& input: recipesEntry->second.inputs) {
            quantitiesNeeded[input.name] += numBatches * input.quantity;
            printf(
                "Producing %zu %s\n",
                numBatches * input.quantity,
                input.name.c_str()
            );
        }
        const auto leftover = numBatches * quantityPerBatch - quantityNeeded;
        if (leftover > 0) {
            inventory[name] = leftover;
        }
    }
    printf("We will need %zu ORE\n", quantitiesNeeded["ORE"]);
    return EXIT_SUCCESS;
}
