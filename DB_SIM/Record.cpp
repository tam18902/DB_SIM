#include "Record.h"
#include <stdexcept>

// Constructor: Initialize a Record object with an empty data map.
Record::Record() {
    // No special initialization needed.
}

// Destructor: Clean up the Record object.
// Since std::unordered_map manages its own memory, no manual cleanup is necessary.
Record::~Record() {
    // No special actions required.
}

// Set the value for the specified column.
void Record::setValue(const std::string& columnName, const std::string& value) {
    data[columnName] = value;
}

// Retrieve the value of the specified column.
// Throws std::runtime_error if the column does not exist.
const std::string& Record::getValue(const std::string& columnName) const {
    auto it = data.find(columnName);
    if (it != data.end()) {
        return it->second;
    }
    throw std::runtime_error("Record::getValue - Column not found: " + columnName);
}

// Check if the specified column exists in the record.
bool Record::hasColumn(const std::string& columnName) const {
    return data.find(columnName) != data.end();
}

// Retrieve the entire data map of the record.
const std::unordered_map<std::string, std::string>& Record::getData() const {
    return data;
}
