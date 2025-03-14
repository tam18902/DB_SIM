#pragma once

#include <vector>
#include <string>
#include <memory>
#include "Column.h"
#include "Constraint.h"

/**
 * @brief The Schema class defines the structure of a table.
 *
 * Responsibilities:
 * - Stores a list of columns in the table.
 * - Manages constraints such as primary key, foreign key, and unique.
 *
 * Usage:
 * - Create a new schema by adding columns using addColumn() and constraints using addConstraint().
 */
class Schema {
public:
    /**
     * @brief Construct a new Schema object.
     */
    Schema();

    /**
     * @brief Destroy the Schema object.
     */
    ~Schema();

    /**
     * @brief Add a new column to the schema.
     * @param column The Column object to add.
     */
    void addColumn(const Column& column);

    /**
     * @brief Add a constraint to the schema.
     * @param constraint A shared pointer to the Constraint object.
     */
    void addConstraint(const std::shared_ptr<Constraint>& constraint);

    /**
     * @brief Retrieve the list of columns.
     * @return const std::vector<Column>& The vector of columns.
     */
    const std::vector<Column>& getColumns() const;

    /**
     * @brief Retrieve the list of constraints.
     * @return const std::vector<std::shared_ptr<Constraint>>& The vector of constraints.
     */
    const std::vector<std::shared_ptr<Constraint>>& getConstraints() const;

    /**
     * @brief Check if the schema has a column with the specified name.
     * @param columnName The name of the column.
     * @return true if found; false otherwise.
     */
    bool hasColumn(const std::string& columnName) const;

private:
    std::vector<Column> columns;
    std::vector<std::shared_ptr<Constraint>> constraints;
};
