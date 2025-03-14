#include "Column.h"

// Constructor: Initialize the Column with the given name, data type, nullability, and default value.
Column::Column(const std::string& name, DataType type, bool allowNull, const std::string& defaultValue)
    : name(name), type(type), allowNull(allowNull), defaultValue(defaultValue)
{
    // Additional checks or logging can be added here if needed.
}

// Destructor: No special handling is needed because std::string automatically manages its memory.
Column::~Column() {
    // No dynamic resources to release.
}

// Return the name of the column.
const std::string& Column::getName() const {
    return this->name;
}

// Return the data type of the column.
DataType Column::getType() const {
    return this->type;
}

// Check if the column allows NULL values.
bool Column::isNullable() const {
    return this->allowNull;
}

// Return the default value of the column.
const std::string& Column::getDefaultValue() const {
    return this->defaultValue;
}
