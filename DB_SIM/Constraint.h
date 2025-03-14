#pragma once

#include <string>
#include <vector>

/**
 * @brief Base class for all constraints.
 *
 * This abstract class defines the interface for schema constraints.
 *
 * Usage:
 * - Derived classes must implement the validate() method.
 */
class Constraint {
public:
    virtual ~Constraint() {}

    /**
     * @brief Validate the constraint using the provided values.
     * @param values A vector of values corresponding to the columns.
     * @return true if the constraint is satisfied; false otherwise.
     */
    virtual bool validate(const std::vector<std::string>& values) const = 0;
};

/**
 * @brief PrimaryKeyConstraint ensures that the values for the specified column(s) are unique and non-null.
 *
 * Usage:
 * - Create a PrimaryKeyConstraint by providing a vector of column names.
 */
class PrimaryKeyConstraint : public Constraint {
public:
    PrimaryKeyConstraint(const std::vector<std::string>& columnNames);
    ~PrimaryKeyConstraint();

    /**
     * @brief Validate the primary key constraint.
     * @param values The values to validate.
     * @return true if all values are non-empty; false otherwise.
     */
    bool validate(const std::vector<std::string>& values) const override;

    /**
     * @brief Get the list of columns that form the primary key.
     * @return const std::vector<std::string>& The vector of primary key column names.
     */
    const std::vector<std::string>& getColumnNames() const;

private:
    std::vector<std::string> columnNames;
};

/**
 * @brief ForeignKeyConstraint links the columns in this table to the primary key of another table.
 *
 * Usage:
 * - Create a ForeignKeyConstraint by specifying local columns, the referenced table, and the referenced columns.
 */
class ForeignKeyConstraint : public Constraint {
public:
    ForeignKeyConstraint(const std::vector<std::string>& columnNames, const std::string& referencedTable, const std::vector<std::string>& referencedColumns);
    ~ForeignKeyConstraint();

    /**
     * @brief Validate the foreign key constraint.
     * @param values The values to validate.
     * @return true if the number of values matches the number of columns; false otherwise.
     */
    bool validate(const std::vector<std::string>& values) const override;

    /**
     * @brief Get the list of local columns in the foreign key.
     * @return const std::vector<std::string>& The vector of local column names.
     */
    const std::vector<std::string>& getColumnNames() const;

    /**
     * @brief Get the name of the referenced table.
     * @return const std::string& The referenced table name.
     */
    const std::string& getReferencedTable() const;

    /**
     * @brief Get the list of referenced columns.
     * @return const std::vector<std::string>& The vector of referenced column names.
     */
    const std::vector<std::string>& getReferencedColumns() const;

private:
    std::vector<std::string> columnNames;
    std::string referencedTable;
    std::vector<std::string> referencedColumns;
};

/**
 * @brief UniqueConstraint ensures that the values for the specified column(s) are unique.
 *
 * Usage:
 * - Create a UniqueConstraint by providing a vector of column names.
 */
class UniqueConstraint : public Constraint {
public:
    UniqueConstraint(const std::vector<std::string>& columnNames);
    ~UniqueConstraint();

    /**
     * @brief Validate the unique constraint.
     * @param values A vector of values to validate.
     * @return true if the values are unique; false otherwise.
     */
    bool validate(const std::vector<std::string>& values) const override;

    /**
     * @brief Get the list of columns that form the unique constraint.
     * @return const std::vector<std::string>& The vector of unique constraint column names.
     */
    const std::vector<std::string>& getColumnNames() const;

private:
    std::vector<std::string> columnNames;
};
