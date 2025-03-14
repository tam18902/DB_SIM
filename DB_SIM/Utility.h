#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <iomanip>

/**
 * @brief The Utility namespace contains helper functions for the database system.
 *
 * Usage:
 * - generateUUID() returns a unique UUID string.
 * - trim() removes leading and trailing whitespace from a string.
 * - split() splits a string by a delimiter and trims each token.
 * - removeApostrophe() removes surrounding apostrophes from a string.
 * - toUpper() converts a string to uppercase.
 */
namespace Utility {

    /**
     * @brief Generate a UUID string.
     *
     * Uses random number generation to create a unique UUID.
     * @return std::string The generated UUID.
     */
    std::string generateUUID();

    /**
     * @brief Remove leading and trailing whitespace from a string.
     * @param s The input string.
     * @return std::string The trimmed string.
     */
    std::string trim(const std::string& s);

    /**
     * @brief Split a string by the given delimiter and trim each token.
     * @param s The input string.
     * @param delimiter The delimiter character.
     * @return std::vector<std::string> A vector of trimmed tokens.
     */
    std::vector<std::string> split(const std::string& s, char delimiter);

    /**
     * @brief Remove surrounding apostrophes from a string if present.
     * @param s The input string.
     * @return std::string The string without surrounding apostrophes.
     */
    std::string removeApostrophe(const std::string& s);

    /**
     * @brief Convert a string to uppercase.
     * @param s The input string.
     * @return std::string The uppercase version of the string.
     */
    std::string toUpper(const std::string& s);
}
