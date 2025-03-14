#pragma once

#include <string>
#include <unordered_map>

/**
 * @brief The Record class represents a row in a table.
 *
 * Responsibilities:
 * - Stores data as key-value pairs, where the key is the column name and the value is the data (as a string).
 * - Can be extended to support other data types (e.g., using std::variant).
 *
 * Usage:
 * - Create a Record and set values using setValue().
 * - Retrieve values with getValue().
 */
class Record {
public:
    /**
     * @brief Construct a new Record object.
     */
    Record();

    /**
     * @brief Destroy the Record object.
     */
    ~Record();

    /**
     * @brief Set the value for a specified column.
     * @param columnName The column name.
     * @param value The value to set.
     */
    void setValue(const std::string& columnName, const std::string& value);

    /**
     * @brief Get the value of the specified column.
     * @param columnName The column name.
     * @return const std::string& The value associated with the column.
     * @throws std::runtime_error if the column is not found.
     */
    const std::string& getValue(const std::string& columnName) const;

    /**
     * @brief Check if the record has the specified column.
     * @param columnName The column name.
     * @return true if the column exists; false otherwise.
     */
    bool hasColumn(const std::string& columnName) const;

    /**
     * @brief Get the entire data map of the record.
     * @return const std::unordered_map<std::string, std::string>& The record's data.
     */
    const std::unordered_map<std::string, std::string>& getData() const;

private:
    std::unordered_map<std::string, std::string> data;
};
