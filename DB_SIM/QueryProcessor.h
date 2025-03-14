#pragma once

#include <string>

/**
 * @brief The QueryProcessor class is responsible for parsing and executing SQL queries and commands.
 *
 * Responsibilities:
 * - Parse user commands/queries (CREATE TABLE, FLUSH, LOAD, INSERT, SELECT, UPDATE, DELETE).
 * - Dispatch commands to the Database accordingly.
 *
 * Usage:
 * - Create a QueryProcessor object and call execute() with a query string.
 */
class QueryProcessor {
public:
    /**
     * @brief Construct a new QueryProcessor object.
     */
    QueryProcessor();

    /**
     * @brief Destroy the QueryProcessor object.
     */
    ~QueryProcessor();

    /**
     * @brief Execute a command or SQL query entered by the user.
     * @param sqlQuery The command/query string.
     * @return true if the query is executed successfully; false otherwise.
     */
    bool execute(const std::string& sqlQuery);

private:
    // Parsing functions for various commands.
    void parseCreate(const std::string& query);
    void parseFlush(const std::string& query);
    void parseLoad(const std::string& query);
    void parseInsert(const std::string& query);
    void parseSelect(const std::string& query);
    void parseUpdate(const std::string& query);
    void parseDelete(const std::string& query);
    void parseDropColumn(const std::string& query);
    void parseDropTable(const std::string& query);

    // Additional helper functions can be declared here if needed.
};
