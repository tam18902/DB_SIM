#include "Schema.h"
#include "Column.h"

// Constructor: Initialize a Schema object; vectors are default-initialized.
Schema::Schema() {
    // No additional initialization needed since vectors are auto-initialized.
}

// Destructor: Clean up resources if needed; here, the vectors are automatically released.
Schema::~Schema() {
    // No special actions required.
}

// Add a new column to the schema.
void Schema::addColumn(const Column& column) {
    columns.push_back(column);
}

// Add a constraint to the schema.
void Schema::addConstraint(const std::shared_ptr<Constraint>& constraint) {
    constraints.push_back(constraint);
}

// Retrieve the list of columns.
const std::vector<Column>& Schema::getColumns() const {
    return columns;
}

// Retrieve the list of constraints.
const std::vector<std::shared_ptr<Constraint>>& Schema::getConstraints() const {
    return constraints;
}

// Check if a column with the specified name exists; returns true if found.
bool Schema::hasColumn(const std::string& columnName) const {
    for (const auto& col : columns) {
        if (col.getName() == columnName) {
            return true;
        }
    }
    return false;
}
