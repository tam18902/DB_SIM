#include "Table.h"
#include "Utility.h"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <stdexcept>

using namespace Utility;

// Constructor: initialize table with name and given schema.
Table::Table(const std::string& tableName, const Schema& schema)
    : name(tableName), schema(schema) {
    // Additional initialization if needed.
    std::cout << "Table '" << name << "' created with the provided schema.\n";
}

// Destructor: cleanup resources if any.
Table::~Table() {
    // No dynamic resource to clean up.
}

// Insert a record into the table after validating constraints.
bool Table::insertRecord(const Record& record) {
    // Check each constraint defined in the schema.
    const auto& constraints = schema.getConstraints();
    for (const auto& constraint : constraints) {
        // Only enforce PrimaryKeyConstraint and UniqueConstraint here.
        if (auto pk = dynamic_cast<PrimaryKeyConstraint*>(constraint.get())) {
            // Get the list of columns for the primary key constraint.
            std::vector<std::string> keyColumns = pk->getColumnNames();
            std::vector<std::string> newValues;

            // Gather values from the new record.
            for (const auto& col : keyColumns) {
                if (!record.hasColumn(col)) {
                    std::cerr << "Error: Record is missing required column '" << col
                        << "' for primary key constraint." << std::endl;
                    return false;
                }
                std::string value = record.getValue(col);
                // Ensure that primary key value is not empty.
                if (value.empty()) {
                    std::cerr << "Error: Primary key column '" << col << "' cannot be empty." << std::endl;
                    return false;
                }
                newValues.push_back(value);
            }

            // Check against existing records for duplicate key values.
            for (const auto& existingRecord : records) {
                std::vector<std::string> existingValues;
                bool missing = false;
                for (const auto& col : keyColumns) {
                    if (!existingRecord.hasColumn(col)) {
                        missing = true;
                        break;
                    }
                    existingValues.push_back(existingRecord.getValue(col));
                }
                if (missing)
                    continue;

                if (existingValues == newValues) {
                    std::cerr << "Error: Duplicate entry for primary key on columns:";
                    for (const auto& col : keyColumns)
                        std::cerr << " " << col;
                    std::cerr << std::endl;
                    return false;
                }
            }
        }
        else if (auto uq = dynamic_cast<UniqueConstraint*>(constraint.get())) {
            // Get the list of columns for the unique constraint.
            std::vector<std::string> keyColumns = uq->getColumnNames();
            std::vector<std::string> newValues;

            // Gather values from the new record.
            for (const auto& col : keyColumns) {
                if (!record.hasColumn(col)) {
                    std::cerr << "Error: Record is missing required column '" << col
                        << "' for unique constraint." << std::endl;
                    return false;
                }
                newValues.push_back(record.getValue(col));
            }

            // Check against existing records for duplicate key values.
            for (const auto& existingRecord : records) {
                std::vector<std::string> existingValues;
                bool missing = false;
                for (const auto& col : keyColumns) {
                    if (!existingRecord.hasColumn(col)) {
                        missing = true;
                        break;
                    }
                    existingValues.push_back(existingRecord.getValue(col));
                }
                if (missing)
                    continue;

                if (existingValues == newValues) {
                    std::cerr << "Error: Duplicate entry for unique constraint on columns:";
                    for (const auto& col : keyColumns)
                        std::cerr << " " << col;
                    std::cerr << std::endl;
                    return false;
                }
            }
        }
        // Other constraint types (e.g., ForeignKeyConstraint) can be handled here.
    }

    // Additional validation against column definitions can be added here if needed.

    // All checks passed; insert the record.
    records.push_back(record);
    std::cout << "Record inserted into table '" << name << "'." << std::endl;
    return true;
}

/**
 * @brief Update records in the table based on a condition.
 *
 * If condition is empty or "all", update all records.
 * Otherwise, expects condition in the form "column = value".
 * For each matching record, all key-value pairs in newRecord are applied.
 */
bool Table::updateRecord(const Record& newRecord, const std::string& condition) {
    bool updated = false;
    std::string cond = trim(condition);

    // If condition is empty or "all", update all records.
    if (cond.empty() || cond == "all") {
        for (auto& record : records) {
            for (const auto& pair : newRecord.getData()) {
                record.setValue(pair.first, pair.second);
            }
        }
        std::cout << "Updated all records in table '" << name << "'.\n";
        return true;
    }

    // Otherwise, assume condition is of the form "column = value".
    size_t pos = cond.find('=');
    if (pos == std::string::npos) {
        std::cerr << "Error: Invalid condition format in updateRecord.\n";
        return false;
    }
    std::string condCol = trim(cond.substr(0, pos));
    std::string condVal = trim(cond.substr(pos + 1));

    // Iterate through records, updating those that meet the condition.
    for (auto& record : records) {
        try {
            const std::string& value = record.getValue(condCol);
            if (value == condVal) {
                for (const auto& pair : newRecord.getData()) {
                    record.setValue(pair.first, pair.second);
                }
                updated = true;
            }
        }
        catch (const std::runtime_error& e) {
            // If the record does not have the column, ignore.
        }
    }

    if (updated) {
        std::cout << "Updated records satisfying condition: " << condition << "\n";
    }
    else {
        std::cout << "No records match condition: " << condition << "\n";
    }
    return updated;
}

/**
 * @brief Delete records from the table based on a condition.
 *
 * If condition is "all", delete all records.
 * Otherwise, expects condition in the form "column = value".
 */
bool Table::deleteRecord(const std::string& condition) {
    std::string cond = trim(condition);
    if (cond == "all") {
        records.clear();
        std::cout << "All records in table '" << name << "' have been deleted.\n";
        return true;
    }

    // Assume condition is of the form "column = value".
    size_t pos = cond.find('=');
    if (pos == std::string::npos) {
        std::cerr << "Error: Invalid condition format in deleteRecord.\n";
        return false;
    }
    std::string condCol = trim(cond.substr(0, pos));
    std::string condVal = trim(cond.substr(pos + 1));
    if (!condVal.empty() && condVal.front() == '\'' && condVal.back() == '\'') {
        condVal = condVal.substr(1, condVal.size() - 2);
    }

    size_t beforeSize = records.size();
    // Remove records that satisfy the condition.
    auto newEnd = std::remove_if(records.begin(), records.end(), [&](const Record& record) {
        try {
            return record.getValue(condCol) == condVal;
        }
        catch (const std::runtime_error&) {
            return false;
        }
        });
    records.erase(newEnd, records.end());
    size_t afterSize = records.size();

    bool deleted = (beforeSize > afterSize);
    if (deleted) {
        std::cout << "Deleted " << (beforeSize - afterSize)
            << " record(s) from table '" << name << "' matching condition: " << condition << "\n";
    }
    else {
        std::cout << "No records match condition: " << condition << " in table '" << name << "'\n";
    }
    return deleted;
}

// Get all records in the table.
const std::vector<Record>& Table::getRecords() const {
    return records;
}

// Get the schema of the table.
const Schema& Table::getSchema() const {
    return schema;
}

// Drop a column from the table.
bool Table::dropColumn(const std::string& columnName) {
    // Check if the column exists in the schema.
    if (!getSchema().hasColumn(columnName)) {
        std::cerr << "Error: Column '" << columnName << "' does not exist in table '" << name << "'." << std::endl;
        return false;
    }
    // Remove the column from the schema.
    auto& cols = const_cast<std::vector<Column>&>(schema.getColumns());
    cols.erase(std::remove_if(cols.begin(), cols.end(), [columnName](const Column& col) {
        return col.getName() == columnName;
        }), cols.end());
    // Remove the column from all records.
    for (auto& record : records) {
        auto& data = const_cast<std::unordered_map<std::string, std::string>&>(record.getData());
        data.erase(columnName);
    }
    std::cout << "DROP COLUMN: Column '" << columnName << "' dropped from table '" << name << "'." << std::endl;
    return true;
}
