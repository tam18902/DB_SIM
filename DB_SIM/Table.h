#pragma once

#include <string>
#include <vector>
#include <memory>
#include "Schema.h"
#include "Record.h"

/**
 * @brief The Table class represents a table (relation) in the database.
 *
 * Responsibilities:
 * - Manages the schema (structure) of the table.
 * - Stores the records (rows) of the table.
 * - Provides CRUD operations: insert, update, delete records.
 *
 * Usage:
 * - Create a new table with a specified schema.
 * - Insert a record using insertRecord().
 * - Search, update, or delete records based on criteria.
 */
class Table {
public:
    /**
     * @brief Construct a new Table object.
     * @param tableName The name of the table.
     * @param schema The schema defining the structure of the table.
     */
    Table(const std::string& tableName, const Schema& schema);

    /**
     * @brief Destroy the Table object.
     */
    ~Table();

    /**
     * @brief Insert a record into the table after validating constraints.
     * @param record The record to insert.
     * @return true if the record was successfully inserted; false otherwise.
     */
    bool insertRecord(const Record& record);

    /**
     * @brief Update records in the table based on a condition.
     * @param record The new record data to apply.
     * @param condition The condition (e.g., "column = value" or "all") for updating.
     * @return true if at least one record was updated; false otherwise.
     */
    bool updateRecord(const Record& record, const std::string& condition);

    /**
     * @brief Delete records from the table based on a condition.
     * @param condition The condition (e.g., "column = value" or "all") for deletion.
     * @return true if at least one record was deleted; false otherwise.
     */
    bool deleteRecord(const std::string& condition);

    /**
     * @brief Get all records in the table.
     * @return const std::vector<Record>& A reference to the vector of records.
     */
    const std::vector<Record>& getRecords() const;

    /**
     * @brief Get the schema of the table.
     * @return const Schema& The table's schema.
     */
    const Schema& getSchema() const;

    bool dropColumn(const std::string& columnName);

private:
    std::string name;
    Schema schema;
    std::vector<Record> records; // List of records in the table.

    // Indexes for optimization can be added later.
};
