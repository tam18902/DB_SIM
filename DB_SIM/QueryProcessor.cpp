#include "QueryProcessor.h"
#include "Database.h"
#include "Utility.h"
#include "Schema.h"
#include "Column.h"
#include "Record.h"
#include "Table.h"

#include <iostream>
#include <regex>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <cctype>

using namespace Utility;

// Structure to store help information.
struct QueryHelp {
    std::string usage;
    std::string example;
};

// Static map to store help info for each command.
static const std::unordered_map<std::string, QueryHelp> helpMap = {
    {"create table",
        {"CREATE TABLE <tableName> (<columnName> <dataType> [NOT NULL], [PRIMARY KEY (<col(s)>)] [; UNIQUE (<col(s)>]);",
         "CREATE TABLE users (id INTEGER NOT NULL, name STRING, age INTEGER, PRIMARY KEY (id), UNIQUE (name));"}},
    {"drop table",
        {"DROP TABLE <tableName>;",
         "DROP TABLE users;"}},
    {"drop column",
        {"DROP COLUMN <tableName> <columnName>;",
         "DROP COLUMN users age;"}},
    {"flush",
        {"FLUSH <filename> <key>;",
         "FLUSH database.db mysecretkey;"}},
    {"load",
        {"LOAD <filename> <key>;",
         "LOAD database.db mysecretkey;"}},
    {"insert",
        {"INSERT INTO <tableName> (col1, col2, ...) VALUES (val1, val2, ...);",
         "INSERT INTO users (id, name, age) VALUES ('1', 'Alice', '30');"}},
    {"select",
        {"SELECT <col1, col2, ...> FROM <tableName> [WHERE <condition>];",
         "SELECT * FROM users WHERE id = 1;"}},
    {"update",
        {"UPDATE <tableName> SET <col1> = <val1>, <col2> = <val2>, ... WHERE <condition>;",
         "UPDATE users SET name = 'Alicia', age = '31' WHERE id = 1;"}},
    {"delete",
        {"DELETE FROM <tableName> WHERE <condition>;",
         "DELETE FROM users WHERE id = 1;"}}
};

// Function to handle help command.
static void handleQueryHelp(const std::string& input) {
    // Split input by spaces.
    std::vector<std::string> tokens = split(input, ' ');
    if (tokens[0] == "help") {
        tokens.erase(tokens.begin());
    }
    if (tokens.empty()) {
        // If only "help" is entered, list all available commands.
        std::cout << "Available commands and their usage:" << std::endl;
        for (const auto& pair : helpMap) {
            std::cout << "Command: " << pair.first << std::endl;
            std::cout << "  Usage: " << pair.second.usage << std::endl;
            std::cout << "  Example: " << pair.second.example << std::endl;
            std::cout << std::endl;
        }
        return;
    }
    // Otherwise, the second token specifies the command.
    std::string key;
    for (size_t i = 0; i < tokens.size(); ++i) {
        if (i > 0)
            key += " ";
        key += toUpper(tokens[i]);
    }
    std::transform(key.begin(), key.end(), key.begin(), ::tolower);
    auto it = helpMap.find(key);
    if (it != helpMap.end()) {
        std::cout << "Help for command '" << key << "':" << std::endl;
        std::cout << "  Usage: " << it->second.usage << std::endl;
        std::cout << "  Example: " << it->second.example << std::endl;
    }
    else {
        std::cerr << "No help available for command: " << key << std::endl;
    }
}

// Constructor.
QueryProcessor::QueryProcessor() {
    // Initialize if needed.
}

// Destructor.
QueryProcessor::~QueryProcessor() {
    // Cleanup if needed.
}

/**
 * @brief Execute a command/query by dispatching to the appropriate parser.
 *
 * Supported commands:
 * - HELP [command]
 * - CREATE TABLE ...
 * - DROP TABLE ...
 * - DROP COLUMN ...
 * - FLUSH <filename> <key>;
 * - LOAD <filename> <key>;
 * - Standard SQL queries: INSERT, SELECT, UPDATE, DELETE.
 */
bool QueryProcessor::execute(const std::string& sqlQuery) {
    std::string query = trim(sqlQuery);
    std::string lowerQuery = query;
    std::transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(), ::tolower);

    if (lowerQuery.find("help") == 0) {
        handleQueryHelp(query);
    }
    else if (lowerQuery.find("create table") == 0) {
        parseCreate(query);
    }
    else if (lowerQuery.find("drop table") == 0) {
        parseDropTable(query);
    }
    else if (lowerQuery.find("drop column") == 0) {
        parseDropColumn(query);
    }
    else if (lowerQuery.find("flush") == 0) {
        parseFlush(query);
    }
    else if (lowerQuery.find("load") == 0) {
        parseLoad(query);
    }
    else if (lowerQuery.find("insert") == 0) {
        parseInsert(query);
    }
    else if (lowerQuery.find("select") == 0) {
        parseSelect(query);
    }
    else if (lowerQuery.find("update") == 0) {
        parseUpdate(query);
    }
    else if (lowerQuery.find("delete") == 0) {
        parseDelete(query);
    }
    else {
        std::cerr << "Error: Unsupported command/query type." << std::endl;
        return false;
    }
    return true;
}

/**
 * @brief Parse and execute a DROP TABLE command.
 * Expected syntax: DROP TABLE <tableName>;
 */
void QueryProcessor::parseDropTable(const std::string& query) {
    std::regex dropTablePattern(R"(DROP\s+TABLE\s+(\w+);)", std::regex::icase);
    std::smatch match;
    if (std::regex_match(query, match, dropTablePattern)) {
        std::string tableName = match[1];
        if (Database::getInstance().dropTable(tableName))
            std::cout << "DROP TABLE: Table '" << tableName << "' dropped successfully." << std::endl;
        else
            std::cerr << "Error: Failed to drop table '" << tableName << "'." << std::endl;
    }
    else {
        std::cerr << "Error: Invalid DROP TABLE query format." << std::endl;
        handleQueryHelp("drop table");
    }
}

/**
 * @brief Parse and execute a DROP COLUMN command.
 * Expected syntax: DROP COLUMN <tableName> <columnName>;
 */
void QueryProcessor::parseDropColumn(const std::string& query) {
    std::regex dropColumnPattern(R"(DROP\s+COLUMN\s+(\w+)\s+(\w+);)", std::regex::icase);
    std::smatch match;
    if (std::regex_match(query, match, dropColumnPattern)) {
        std::string tableName = match[1];
        std::string columnName = match[2];
        auto table = Database::getInstance().getTable(tableName);
        if (!table) {
            std::cerr << "Error: Table '" << tableName << "' not found." << std::endl;
        }
        else {
            if (table->dropColumn(columnName))
                std::cout << "DROP COLUMN: Column '" << columnName << "' dropped from table '" << tableName << "'." << std::endl;
            else
                std::cerr << "Error: Failed to drop column '" << columnName << "' from table '" << tableName << "'." << std::endl;
        }
    }
    else {
        std::cerr << "Error: Invalid DROP COLUMN query format." << std::endl;
        auto it = helpMap.find("drop column");
        if (it != helpMap.end()) {
            std::cerr << "Usage: " << it->second.usage << std::endl;
            std::cerr << "Example: " << it->second.example << std::endl;
        }
    }
}

/**
 * @brief Parse and execute a CREATE TABLE command.
 * Expected syntax:
 *   CREATE TABLE <tableName> (<columnDef_or_constraintDef>, ...);
 * Column definition: <columnName> <dataType> [NOT NULL]
 * Constraint definitions:
 *   PRIMARY KEY (<col1>, <col2>, ...)
 *   UNIQUE (<col1>, <col2>, ...)
 * Example:
 *   CREATE TABLE users (id INTEGER NOT NULL, name STRING, age INTEGER, PRIMARY KEY (id), UNIQUE (name));
 */
void QueryProcessor::parseCreate(const std::string& query) {
    std::regex createPattern(R"(CREATE\s+TABLE\s+(\w+)\s*\((.+)\);)", std::regex::icase);
    std::smatch match;
    if (std::regex_match(query, match, createPattern)) {
        std::string tableName = match[1];
        std::string body = match[2];

        // Check if table already exists.
        if (Database::getInstance().getTable(tableName) != nullptr) {
            std::cerr << "Error: Table '" << tableName << "' already exists." << std::endl;
            return;
        }

        Schema schema;
        // Split the body by comma; note: this is a simple split and may not handle complex definitions.
        std::vector<std::string> tokens = split(body, ',');
        for (const auto& token : tokens) {
            std::string t = trim(token);
            // Check if the token defines a PRIMARY KEY constraint.
            if (t.size() >= 11 && std::equal(t.begin(), t.begin() + 11, "PRIMARY KEY",
                [](char a, char b) { return std::tolower(a) == std::tolower(b); })) {
                std::regex pkPattern(R"(PRIMARY\s+KEY\s*\((.+)\))", std::regex::icase);
                std::smatch pkMatch;
                if (std::regex_match(t, pkMatch, pkPattern)) {
                    std::string pkCols = pkMatch[1];
                    std::vector<std::string> pkColumns = split(pkCols, ',');
                    auto pkConstraint = std::make_shared<PrimaryKeyConstraint>(pkColumns);
                    schema.addConstraint(pkConstraint);
                }
                else {
                    std::cerr << "Error: Invalid PRIMARY KEY constraint definition: \"" << t << "\"" << std::endl;
                    std::cerr << "Usage: PRIMARY KEY (<column1>, <column2>, ...)" << std::endl;
                    std::cerr << "Example: PRIMARY KEY (id)" << std::endl;
                    return;
                }
            }
            // Check if the token defines a UNIQUE constraint.
            else if (t.size() >= 6 && std::equal(t.begin(), t.begin() + 6, "UNIQUE",
                [](char a, char b) { return std::tolower(a) == std::tolower(b); })) {
                std::regex uqPattern(R"(UNIQUE\s*\((.+)\))", std::regex::icase);
                std::smatch uqMatch;
                if (std::regex_match(t, uqMatch, uqPattern)) {
                    std::string uqCols = uqMatch[1];
                    std::vector<std::string> uqColumns = split(uqCols, ',');
                    auto uqConstraint = std::make_shared<UniqueConstraint>(uqColumns);
                    schema.addConstraint(uqConstraint);
                }
                else {
                    std::cerr << "Error: Invalid UNIQUE constraint definition: \"" << t << "\"" << std::endl;
                    std::cerr << "Usage: UNIQUE (<column1>, <column2>, ...)" << std::endl;
                    std::cerr << "Example: UNIQUE (name)" << std::endl;
                    return;
                }
            }
            // Otherwise, assume it is a column definition.
            else {
                // Expected format: <columnName> <dataType> [NOT NULL]
                std::regex colPattern(R"((\w+)\s+(INTEGER|FLOAT|STRING)(\s+NOT\s+NULL)?\s*)", std::regex::icase);
                std::smatch colMatch;
                if (std::regex_match(t, colMatch, colPattern)) {
                    std::string colName = colMatch[1];
                    std::string typeStr = colMatch[2];
                    std::string notNullStr = colMatch[3];
                    DataType colType = DataType::STRING;
                    std::transform(typeStr.begin(), typeStr.end(), typeStr.begin(), ::toupper);
                    if (typeStr == "INTEGER")
                        colType = DataType::INTEGER;
                    else if (typeStr == "FLOAT")
                        colType = DataType::FLOAT;
                    else if (typeStr == "STRING")
                        colType = DataType::STRING;
                    // "NOT NULL" means allowNull is false.
                    bool allowNull = notNullStr.empty();
                    schema.addColumn(Column(colName, colType, allowNull));
                }
                else {
                    std::cerr << "Error: Invalid column definition: \"" << t << "\"" << std::endl;
                    handleQueryHelp("create table");
                    return;
                }
            }
        }
        Database::getInstance().addTable(tableName, std::make_shared<Table>(tableName, schema));
        std::cout << "CREATE: Table '" << tableName << "' created successfully." << std::endl;
    }
    else {
        std::cerr << "Error: Invalid CREATE TABLE query format." << std::endl;
        handleQueryHelp("create table");
    }
}

/**
 * @brief Parse and execute a FLUSH command.
 * Expected syntax:
 *   FLUSH <filename> <key>;
 * Example:
 *   FLUSH database.db mysecretkey;
 */
void QueryProcessor::parseFlush(const std::string& query) {
    std::string q = query;
    if (!q.empty() && q.back() == ';')
        q.pop_back();

    std::istringstream iss(q);
    std::string command, filename, key;
    iss >> command >> filename >> key;
    if (!filename.empty() && !key.empty()) {
        Database::getInstance().flushToFile(filename, key);
        std::cout << "FLUSH: Database saved to file '" << filename << "'." << std::endl;
    }
    else {
        std::cerr << "Error: Invalid FLUSH command format." << std::endl;
        auto it = helpMap.find("flush");
        handleQueryHelp("flush");
    }
}

/**
 * @brief Parse and execute a LOAD command.
 * Expected syntax:
 *   LOAD <filename> <key>;
 * Example:
 *   LOAD database.db mysecretkey;
 */
void QueryProcessor::parseLoad(const std::string& query) {
    std::string q = query;
    if (!q.empty() && q.back() == ';')
        q.pop_back();

    std::istringstream iss(q);
    std::string command, filename, key;
    iss >> command >> filename >> key;
    if (!filename.empty() && !key.empty()) {
        Database::getInstance().loadFromFile(filename, key);
        std::cout << "LOAD: Database loaded from file '" << filename << "'." << std::endl;
    }
    else {
        std::cerr << "Error: Invalid LOAD command format." << std::endl;
        handleQueryHelp("load");
    }
}

/**
 * @brief Parse and execute an INSERT query.
 * Expected syntax:
 *   INSERT INTO <tableName> (col1, col2, ...) VALUES (val1, val2, ...);
 * Example:
 *   INSERT INTO users (id, name, age) VALUES ('1', 'Alice', '30');
 */
void QueryProcessor::parseInsert(const std::string& query) {
    std::regex insertPattern(R"(INSERT INTO (\w+)\s*\(([^)]+)\)\s*VALUES\s*\(([^)]+)\);)", std::regex::icase);
    std::smatch match;
    if (std::regex_match(query, match, insertPattern)) {
        std::string table = match[1];
        std::string columnsStr = match[2];
        std::string valuesStr = match[3];

        std::vector<std::string> columns = split(columnsStr, ',');
        std::vector<std::string> values = split(valuesStr, ',');

        std::cout << "INSERT: Table = " << table << "\nColumns: ";
        for (const auto& col : columns)
            std::cout << col << " ";
        std::cout << "\nValues: ";
        for (const auto& val : values)
            std::cout << val << " ";
        std::cout << std::endl;

        if (!Database::getInstance().insert(table, columns, values))
            std::cerr << "Error: Insert operation failed." << std::endl;
    }
    else {
        std::cerr << "Error: Invalid INSERT query format." << std::endl;
        handleQueryHelp("insert");
    }
}

/**
 * @brief Parse and execute a SELECT query.
 * Expected syntax:
 *   SELECT <col1, col2, ...> FROM <tableName> [WHERE <condition>];
 * Examples:
 *   SELECT * FROM users;
 *   SELECT id, name FROM users WHERE id = 1;
 */
void QueryProcessor::parseSelect(const std::string& query) {
    std::regex selectPattern(R"(SELECT (.+) FROM (\w+)(?: WHERE (.+))?;)", std::regex::icase);
    std::smatch match;
    if (std::regex_match(query, match, selectPattern)) {
        std::string columnsStr = match[1];
        std::string table = match[2];
        std::string condition;
        if (match.size() > 3)
            condition = match[3];

        std::vector<std::string> columns;
        if (trim(columnsStr) == "*")
            columns.push_back("*");
        else
            columns = split(columnsStr, ',');

        std::cout << "SELECT: Table = " << table << "\nColumns: ";
        for (const auto& col : columns)
            std::cout << col << " ";
        std::cout << "\nCondition: " << condition << std::endl;

        if (!Database::getInstance().select(table, columns, condition))
            std::cerr << "Error: Select operation failed." << std::endl;
    }
    else {
        std::cerr << "Error: Invalid SELECT query format." << std::endl;
        handleQueryHelp("select");
    }
}

/**
 * @brief Parse and execute an UPDATE query.
 * Expected syntax:
 *   UPDATE <tableName> SET <col1> = <val1>, <col2> = <val2>, ... WHERE <condition>;
 * Example:
 *   UPDATE users SET name = 'Alicia', age = '31' WHERE id = 1;
 */
void QueryProcessor::parseUpdate(const std::string& query) {
    std::regex updatePattern(R"(UPDATE (\w+)\s+SET\s+(.+)\s+WHERE\s+(.+);)", std::regex::icase);
    std::smatch match;
    if (std::regex_match(query, match, updatePattern)) {
        std::string table = match[1];
        std::string setClause = match[2];
        std::string condition = match[3];

        std::vector<std::string> assignmentsStr = split(setClause, ',');
        std::vector<std::pair<std::string, std::string>> assignments;
        for (const auto& assignment : assignmentsStr) {
            size_t posEqual = assignment.find('=');
            if (posEqual != std::string::npos) {
                std::string col = trim(assignment.substr(0, posEqual));
                std::string val = trim(assignment.substr(posEqual + 1));
                if (!val.empty() && val.front() == '\'' && val.back() == '\'')
                    val = val.substr(1, val.size() - 2);
                assignments.push_back(std::make_pair(col, val));
            }
        }

        std::cout << "UPDATE: Table = " << table << "\nAssignments: ";
        for (const auto& p : assignments)
            std::cout << "(" << p.first << " = " << p.second << ") ";
        std::cout << "\nCondition: " << condition << std::endl;

        if (!Database::getInstance().update(table, assignments, condition))
            std::cerr << "Error: Update operation failed." << std::endl;
    }
    else {
        std::cerr << "Error: Invalid UPDATE query format." << std::endl;
        handleQueryHelp("update");
    }
}

/**
 * @brief Parse and execute a DELETE query.
 * Expected syntax:
 *   DELETE FROM <tableName> WHERE <condition>;
 * Example:
 *   DELETE FROM users WHERE id = 1;
 */
void QueryProcessor::parseDelete(const std::string& query) {
    std::regex deletePattern(R"(DELETE FROM (\w+)\s+WHERE\s+(.+);)", std::regex::icase);
    std::smatch match;
    if (std::regex_match(query, match, deletePattern)) {
        std::string table = match[1];
        std::string condition = match[2];

        std::cout << "DELETE: Table = " << table << "\nCondition: " << condition << std::endl;

        if (!Database::getInstance().remove(table, condition))
            std::cerr << "Error: Delete operation failed." << std::endl;
    }
    else {
        std::cerr << "Error: Invalid DELETE query format." << std::endl;
        handleQueryHelp("delete");
    }
}
