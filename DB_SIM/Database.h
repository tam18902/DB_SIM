#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
#include <utility>

// Forward declaration of Table to avoid circular dependency.
class Table;

/**
 * @brief The Database class represents the database system.
 *
 * Responsibilities:
 * - Manage a collection of tables.
 * - Perform basic operations (insert, select, update, delete) on tables; inputs are pre-parsed by the QueryProcessor.
 * - Load data from a file (with encryption) and flush data to a file.
 * - Manage relationships between tables through constraints.
 *
 * Usage:
 * - Obtain the database instance using getInstance().
 * - Add or retrieve tables by name.
 * - The insert, select, update, and delete functions are called by the QueryProcessor.
 */
class Database {
public:
    /**
     * @brief Get the singleton Database instance.
     * @return Database& The single Database instance.
     */
    static Database& getInstance();

    /**
     * @brief Load the database from a file using the provided encryption key.
     * @param filename The file name.
     * @param key The encryption key.
     * @return true if successful; false otherwise.
     */
    bool loadFromFile(const std::string& filename, const std::string& key);

    /**
     * @brief Flush (save) the database to a file using the provided encryption key.
     * @param filename The file name.
     * @param key The encryption key.
     * @return true if successful; false otherwise.
     */
    bool flushToFile(const std::string& filename, const std::string& key);

    /**
     * @brief Execute an SQL query string.
     * @param sql The SQL query.
     * @return true if executed successfully; false otherwise.
     * @note This function is deprecated if QueryProcessor is used.
     */
    bool executeQuery(const std::string& sql);

    /**
     * @brief Add a table to the database.
     * @param tableName The name of the table.
     * @param table A shared pointer to the Table object.
     */
    void addTable(const std::string& tableName, std::shared_ptr<Table> table);

    /**
     * @brief Retrieve a table by its name.
     * @param tableName The table name.
     * @return std::shared_ptr<Table> A pointer to the table, or nullptr if not found.
     */
    std::shared_ptr<Table> getTable(const std::string& tableName);

    /**
     * @brief Remove a table from the database.
     * @param tableName The table name.
     */
    bool dropTable(const std::string& tableName);

    // Functions called by QueryProcessor after parsing.
    /**
     * @brief Insert a record into the specified table.
     * @param tableName The table name.
     * @param columns A vector of column names.
     * @param values A vector of corresponding values.
     * @return true if insertion is successful; false otherwise.
     */
    bool insert(const std::string& tableName, const std::vector<std::string>& columns, const std::vector<std::string>& values);

    /**
     * @brief Select records from the specified table.
     * @param tableName The table name.
     * @param columns A vector of column names to retrieve (or \"*\" for all).
     * @param condition A condition string.
     * @return true if selection is successful; false otherwise.
     */
    bool select(const std::string& tableName, const std::vector<std::string>& columns, const std::string& condition);

    /**
     * @brief Update records in the specified table.
     * @param tableName The table name.
     * @param assignments A vector of (column, new value) pairs.
     * @param condition A condition string.
     * @return true if update is successful; false otherwise.
     */
    bool update(const std::string& tableName, const std::vector<std::pair<std::string, std::string>>& assignments, const std::string& condition);

    /**
     * @brief Delete records from the specified table.
     * @param tableName The table name.
     * @param condition A condition string.
     * @return true if deletion is successful; false otherwise.
     */
    bool remove(const std::string& tableName, const std::string& condition);

private:
    // Private constructor and destructor for singleton pattern.
    Database();
    ~Database();

    // Map of table names to Table objects.
    std::unordered_map<std::string, std::shared_ptr<Table>> tables;

    // Internal helper functions for encryption and decryption.
    std::string encryptData(const std::string& data, const std::string& key);
    std::string decryptData(const std::string& data, const std::string& key);

    // Disable copying.
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;
};
