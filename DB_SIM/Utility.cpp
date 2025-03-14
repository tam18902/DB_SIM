#include "Utility.h"
#include <random>
#include <sstream>
#include <iomanip>

namespace Utility {

    std::string generateUUID() {
        // Create a random number generator
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dist(0, 255);

        // Create a 16-byte array for UUID
        unsigned char uuid[16];
        for (int i = 0; i < 16; i++) {
            uuid[i] = static_cast<unsigned char>(dist(gen));
        }

        // Set version 4: bits 4-7 of the 7th byte are set to 0100 (0x40)
        uuid[6] = (uuid[6] & 0x0F) | 0x40;

        // Set variant: bits 6 and 7 of the 9th byte are set to 10 (0x80)
        uuid[8] = (uuid[8] & 0x3F) | 0x80;

        // Use stringstream to format UUID as a string in the standard format: 8-4-4-4-12
        std::stringstream ss;
        ss << std::hex << std::setfill('0');
        for (int i = 0; i < 16; i++) {
            ss << std::setw(2) << static_cast<int>(uuid[i]);
            // Add a hyphen after the 4th, 6th, 8th, and 10th byte.
            if (i == 3 || i == 5 || i == 7 || i == 9) {
                ss << "-";
            }
        }

        return ss.str();
    }

    // Helper function: remove leading and trailing whitespace from a string.
    std::string trim(const std::string& s) {
        size_t start = s.find_first_not_of(" \t\n\r");
        if (start == std::string::npos)
            return "";
        size_t end = s.find_last_not_of(" \t\n\r");
        return s.substr(start, end - start + 1);
    }

    // Helper function: split a string by a delimiter and remove whitespace.
    std::vector<std::string> split(const std::string& s, char delimiter) {
        std::vector<std::string> tokens;
        std::istringstream iss(s);
        std::string token;
        while (std::getline(iss, token, delimiter)) {
            tokens.push_back(trim(token));
        }
        return tokens;
    }

    std::string removeApostrophe(const std::string& s) {
        // Remove single quotes if present
        if (!s.empty() && s.front() == '\'' && s.back() == '\'') {
            return s.substr(1, s.size() - 2);
        }
        return s;
    }

    // Helper function to convert a string to uppercase.
    std::string toUpper(const std::string& s) {
        std::string result = s;
        for (auto& c : result) {
            c = toupper(c);
        }
        return result;
    }
} // namespace Utility
