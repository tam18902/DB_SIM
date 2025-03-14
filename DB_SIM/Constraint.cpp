#include "Constraint.h"
#include <algorithm>
#include <iostream>

//----------------------------------------
// PrimaryKeyConstraint Implementation
//----------------------------------------

PrimaryKeyConstraint::PrimaryKeyConstraint(const std::vector<std::string>& columnNames)
    : columnNames(columnNames)
{
    // Initialize the list of columns that form the primary key.
}

PrimaryKeyConstraint::~PrimaryKeyConstraint() {
    // Destructor: No dynamic resources need to be released.
}

bool PrimaryKeyConstraint::validate(const std::vector<std::string>& values) const {
    // Simple check: all values for the primary key must be non-empty.
    // In a real system, uniqueness across the entire table should also be checked.
    for (const auto& val : values) {
        if (val.empty()) {
            std::cerr << "PrimaryKeyConstraint validation failed: found empty value." << std::endl;
            return false;
        }
    }
    return true;
}

const std::vector<std::string>& PrimaryKeyConstraint::getColumnNames() const {
    return columnNames;
}

//----------------------------------------
// ForeignKeyConstraint Implementation
//----------------------------------------

ForeignKeyConstraint::ForeignKeyConstraint(const std::vector<std::string>& columnNames,
    const std::string& referencedTable,
    const std::vector<std::string>& referencedColumns)
    : columnNames(columnNames), referencedTable(referencedTable), referencedColumns(referencedColumns)
{
    // Initialize properties for the foreign key.
}

ForeignKeyConstraint::~ForeignKeyConstraint() {
    // Destructor: No dynamic resources need to be released.
}

bool ForeignKeyConstraint::validate(const std::vector<std::string>& values) const {
    // Simple check: the number of input values must match the number of columns defined for the foreign key.
    // In a real system, it should also be verified that these values exist in the referenced table.
    if (values.size() != columnNames.size()) {
        std::cerr << "ForeignKeyConstraint validation failed: number of values does not match number of columns." << std::endl;
        return false;
    }
    return true;
}

const std::vector<std::string>& ForeignKeyConstraint::getColumnNames() const {
    return columnNames;
}

const std::string& ForeignKeyConstraint::getReferencedTable() const {
    return referencedTable;
}

const std::vector<std::string>& ForeignKeyConstraint::getReferencedColumns() const {
    return referencedColumns;
}

//----------------------------------------
// UniqueConstraint Implementation
//----------------------------------------

UniqueConstraint::UniqueConstraint(const std::vector<std::string>& columnNames)
    : columnNames(columnNames)
{
    // Initialize the columns for the unique constraint.
}

UniqueConstraint::~UniqueConstraint() {
    // Destructor: No dynamic resources need to be released.
}

bool UniqueConstraint::validate(const std::vector<std::string>& values) const {
    // Simple check: ensure that there are no duplicate values within the given vector.
    // In a real system, this should compare with other records in the table.
    std::vector<std::string> copy = values;
    std::sort(copy.begin(), copy.end());
    for (size_t i = 1; i < copy.size(); ++i) {
        if (copy[i] == copy[i - 1]) {
            std::cerr << "UniqueConstraint validation failed: duplicate value found (" << copy[i] << ")." << std::endl;
            return false;
        }
    }
    return true;
}

const std::vector<std::string>& UniqueConstraint::getColumnNames() const {
    return columnNames;
}
