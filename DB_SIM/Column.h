#pragma once

#include <string>

/**
 * @brief Enumeration for basic data types.
 */
enum class DataType {
    INTEGER,
    FLOAT,
    STRING,
    // Additional data types can be added here.
};

/**
 * @brief The Column class represents a column in a table.
 *
 * Responsibilities:
 * - Stores column information: name, data type, nullability, default value, etc.
 *
 * Usage:
 * - Used within Schema to define the structure of a table.
 */
class Column {
public:
    /**
     * @brief Construct a new Column object.
     * @param name The column name.
     * @param type The data type.
     * @param allowNull Whether the column allows NULL values (default is true).
     * @param defaultValue The default value (default is an empty string).
     */
    Column(const std::string& name, DataType type, bool allowNull = true, const std::string& defaultValue = "");

    /**
     * @brief Destroy the Column object.
     */
    ~Column();

    /**
     * @brief Get the column name.
     * @return const std::string& The column name.
     */
    const std::string& getName() const;

    /**
     * @brief Get the column data type.
     * @return DataType The data type.
     */
    DataType getType() const;

    /**
     * @brief Check if the column allows NULL values.
     * @return true if allowed; false otherwise.
     */
    bool isNullable() const;

    /**
     * @brief Get the default value for the column.
     * @return const std::string& The default value.
     */
    const std::string& getDefaultValue() const;

private:
    std::string name;
    DataType type;
    bool allowNull;
    std::string defaultValue;
};
