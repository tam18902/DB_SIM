#include <iostream>
#include <sstream>
#include <string>
#include "QueryProcessor.h"

int main() {
    // Create a QueryProcessor instance (it handles all commands).
    QueryProcessor qp;

    std::cout << "Welcome to the Database Management Application." << std::endl;
    std::cout << "Available commands:" << std::endl;
    std::cout << "  CREATE TABLE ...      - Create a new table" << std::endl;
    std::cout << "  DROP TABLE ...        - Drop an existing table" << std::endl;
    std::cout << "  DROP COLUMN ...       - Drop a column from a table" << std::endl;
    std::cout << "  FLUSH <filename> <key>;  - Save database to file" << std::endl;
    std::cout << "  LOAD <filename> <key>;   - Load database from file" << std::endl;
    std::cout << "  (Valid SQL queries: INSERT, SELECT, UPDATE, DELETE)" << std::endl;
    std::cout << "  HELP [command]        - Show usage help" << std::endl;
    std::cout << "  EXIT                  - Exit the application" << std::endl;

    std::string input;
    while (true) {
        std::cout << "\n> ";
        std::getline(std::cin, input);
        if (input == "EXIT" || input == "exit")
            break;
        if (!input.empty()) {
            qp.execute(input);
        }
    }

    std::cout << "Exiting application." << std::endl;
    return 0;
}
