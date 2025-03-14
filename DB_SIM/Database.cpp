#include "Database.h"
#include "Table.h"
#include "Record.h"
#include "Utility.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <regex>
#include <algorithm>
#include <cctype>
#include <vector>

using namespace Utility;


// Helper function to evaluate a simple condition of the form "column = value" for a record.
static bool evaluateCondition(const Record& record, const std::string& condition) {
    std::string cond = trim(condition);
    if (cond.empty()) return true; // No condition means record qualifies.

    // Assume condition is in the form "column = value"
    size_t pos = cond.find('=');
    if (pos == std::string::npos) {
        std::cerr << "Error: Invalid condition format: " << condition << std::endl;
        return false;
    }
    std::string col = trim(cond.substr(0, pos));
    std::string val = trim(cond.substr(pos + 1));
    // Remove surrounding apostrophes if present.
    val = removeApostrophe(val);

    try {
        const std::string& recordVal = record.getValue(col);
        return recordVal == val;
    }
    catch (const std::runtime_error&) {
        return false;
    }
}

//---------------------------------------------------------------------
// Singleton Instance
//---------------------------------------------------------------------
Database& Database::getInstance() {
    static Database instance;
    return instance;
}

//---------------------------------------------------------------------
// Constructor & Destructor
//---------------------------------------------------------------------
Database::Database() {
    // Initialize if necessary.
}

Database::~Database() {
    // Clean up resources if necessary.
}

//---------------------------------------------------------------------
// File IO & Encryption: Flush (serialize) database to a file (including schema constraints)
//---------------------------------------------------------------------
bool Database::flushToFile(const std::string& filename, const std::string& key) {
    std::ostringstream oss;

    // Iterate over each table in the database.
    for (const auto& tablePair : tables) {
        const std::string& tableName = tablePair.first;
        std::shared_ptr<Table> table = tablePair.second;
        // Write the table marker.
        oss << "TABLE:" << tableName << "\n";

        // Write the list of columns (in the order defined in the schema).
        const auto& columns = table->getSchema().getColumns();
        oss << "COLUMNS:";
        for (size_t i = 0; i < columns.size(); ++i) {
            oss << columns[i].getName();
            if (i != columns.size() - 1)
                oss << ",";
        }
        oss << "\n";

        // Write the list of constraints.
        const auto& constraints = table->getSchema().getConstraints();
        oss << "CONSTRAINTS:";
        bool firstConstraint = true;
        for (const auto& constraint : constraints) {
            if (auto pk = dynamic_cast<PrimaryKeyConstraint*>(constraint.get())) {
                if (!firstConstraint) oss << ";";
                oss << "PK(";
                const auto& cols = pk->getColumnNames();
                for (size_t i = 0; i < cols.size(); ++i) {
                    oss << cols[i];
                    if (i != cols.size() - 1)
                        oss << ",";
                }
                oss << ")";
                firstConstraint = false;
            }
            else if (auto uq = dynamic_cast<UniqueConstraint*>(constraint.get())) {
                if (!firstConstraint) oss << ";";
                oss << "UQ(";
                const auto& cols = uq->getColumnNames();
                for (size_t i = 0; i < cols.size(); ++i) {
                    oss << cols[i];
                    if (i != cols.size() - 1)
                        oss << ",";
                }
                oss << ")";
                firstConstraint = false;
            }
            else if (auto fk = dynamic_cast<ForeignKeyConstraint*>(constraint.get())) {
                if (!firstConstraint) oss << ";";
                oss << "FK(";
                const auto& localCols = fk->getColumnNames();
                for (size_t i = 0; i < localCols.size(); ++i) {
                    oss << localCols[i];
                    if (i != localCols.size() - 1)
                        oss << ",";
                }
                oss << ")->" << fk->getReferencedTable() << "(";
                const auto& refCols = fk->getReferencedColumns();
                for (size_t i = 0; i < refCols.size(); ++i) {
                    oss << refCols[i];
                    if (i != refCols.size() - 1)
                        oss << ",";
                }
                oss << ")";
                firstConstraint = false;
            }
        }
        oss << "\n";

        // Write the number of records.
        const auto& records = table->getRecords();
        oss << "RECORDS:" << records.size() << "\n";

        // Write each record: output the values in the order of the columns.
        for (const auto& record : records) {
            for (size_t i = 0; i < columns.size(); ++i) {
                std::string value;
                if (record.hasColumn(columns[i].getName()))
                    value = record.getValue(columns[i].getName());
                oss << value;
                if (i != columns.size() - 1)
                    oss << "|";
            }
            oss << "\n";
        }
        oss << "END_TABLE\n";
    }

    std::string serializedData = oss.str();
    // Encrypt the serialized data.
    std::string encryptedData = encryptData(serializedData, key);

    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Error: Cannot open file for writing: " << filename << std::endl;
        return false;
    }

    file << encryptedData;
    file.close();

    std::cout << "Database flushed to file: " << filename << std::endl;
    return true;
}

//---------------------------------------------------------------------
// File IO & Encryption: Load (deserialize) database from a file (including schema constraints)
//---------------------------------------------------------------------
bool Database::loadFromFile(const std::string& filename, const std::string& key) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Error: Cannot open file for reading: " << filename << std::endl;
        return false;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    std::string encryptedData = buffer.str();
    std::string decryptedData = decryptData(encryptedData, key);

    // Clear existing tables before loading new data.
    tables.clear();

    // Use istringstream to parse the data.
    std::istringstream iss(decryptedData);
    std::string line;
    while (std::getline(iss, line)) {
        line = trim(line);
        if (line.empty())
            continue;

        // Look for the table marker: "TABLE:<tableName>"
        if (line.rfind("TABLE:", 0) == 0) {
            std::string tableName = trim(line.substr(6));

            // Read the "COLUMNS:" line.
            if (!std::getline(iss, line)) break;
            line = trim(line);
            if (line.rfind("COLUMNS:", 0) != 0) {
                std::cerr << "Error: Expected COLUMNS: line" << std::endl;
                return false;
            }
            std::string columnsStr = trim(line.substr(8));
            std::vector<std::string> columnNames = split(columnsStr, ',');

            // Create a schema for the table (defaulting to STRING type for columns).
            Schema schema;
            for (const auto& colName : columnNames) {
                schema.addColumn(Column(colName, DataType::STRING));
            }

            // Read the "CONSTRAINTS:" line.
            if (!std::getline(iss, line)) break;
            line = trim(line);
            if (line.rfind("CONSTRAINTS:", 0) != 0) {
                std::cerr << "Error: Expected CONSTRAINTS: line" << std::endl;
                return false;
            }
            std::string constraintsStr = trim(line.substr(12));
            if (!constraintsStr.empty()) {
                // Constraints are separated by semicolon.
                std::vector<std::string> constraintTokens = split(constraintsStr, ';');
                for (const auto& token : constraintTokens) {
                    // For primary key: format: PK(col1,col2)
                    if (token.rfind("PK(", 0) == 0) {
                        std::string cols = token.substr(3, token.size() - 4);
                        std::vector<std::string> pkCols = split(cols, ',');
                        auto pkConstraint = std::make_shared<PrimaryKeyConstraint>(pkCols);
                        schema.addConstraint(pkConstraint);
                    }
                    // For unique constraint: format: UQ(col1,col2)
                    else if (token.rfind("UQ(", 0) == 0) {
                        std::string cols = token.substr(3, token.size() - 4);
                        std::vector<std::string> uqCols = split(cols, ',');
                        auto uqConstraint = std::make_shared<UniqueConstraint>(uqCols);
                        schema.addConstraint(uqConstraint);
                    }
                    // For foreign key: format: FK(col1,col2)->referencedTable(refCol1,refCol2)
                    else if (token.rfind("FK(", 0) == 0) {
                        size_t posArrow = token.find(")->");
                        if (posArrow == std::string::npos) {
                            std::cerr << "Error: Invalid foreign key constraint format: " << token << std::endl;
                            continue;
                        }
                        std::string localPart = token.substr(3, posArrow - 3);
                        std::vector<std::string> localCols = split(localPart, ',');
                        std::string rest = token.substr(posArrow + 3); // referencedTable(refCols)
                        size_t posParen = rest.find('(');
                        if (posParen == std::string::npos) {
                            std::cerr << "Error: Invalid foreign key constraint format: " << token << std::endl;
                            continue;
                        }
                        std::string refTable = trim(rest.substr(0, posParen));
                        std::string refColsStr = rest.substr(posParen + 1, rest.size() - posParen - 2);
                        std::vector<std::string> refCols = split(refColsStr, ',');
                        auto fkConstraint = std::make_shared<ForeignKeyConstraint>(localCols, refTable, refCols);
                        schema.addConstraint(fkConstraint);
                    }
                    else {
                        std::cerr << "Warning: Unknown constraint format: " << token << std::endl;
                    }
                }
            }

            // Read the "RECORDS:" line.
            if (!std::getline(iss, line)) break;
            line = trim(line);
            if (line.rfind("RECORDS:", 0) != 0) {
                std::cerr << "Error: Expected RECORDS: line" << std::endl;
                return false;
            }
            int recordCount = std::stoi(trim(line.substr(8)));

            // Create a new table with the loaded schema.
            std::shared_ptr<Table> table = std::make_shared<Table>(tableName, schema);

            // Read each record.
            for (int i = 0; i < recordCount; ++i) {
                if (!std::getline(iss, line)) break;
                line = trim(line);
                std::vector<std::string> values = split(line, '|');
                Record record;
                for (size_t j = 0; j < columnNames.size(); ++j) {
                    std::string value = (j < values.size()) ? values[j] : "";
                    record.setValue(columnNames[j], value);
                }
                table->insertRecord(record);
            }

            // Read the table termination marker "END_TABLE".
            if (!std::getline(iss, line)) break;
            line = trim(line);
            if (line != "END_TABLE") {
                std::cerr << "Error: Expected END_TABLE line" << std::endl;
                return false;
            }

            // Add the newly created table to the database.
            addTable(tableName, table);
            std::cout << "Loaded table: " << tableName << " with " << recordCount << " record(s).\n";
        }
    }

    std::cout << "Database loaded from file: " << filename << std::endl;
    return true;
}

//---------------------------------------------------------------------
// Execute Query (only used if needed)
//---------------------------------------------------------------------
bool Database::executeQuery(const std::string& sql) {
    // If a QueryProcessor is already handling query parsing, this function can simply call:
    // QueryProcessor qp; qp.execute(sql);
    // Alternatively, it can be deprecated since query processing is separated.
    std::cerr << "executeQuery() is deprecated. Use QueryProcessor to parse and execute queries." << std::endl;
    return false;
}

//---------------------------------------------------------------------
// Table Operations (input parameters are already parsed)
//---------------------------------------------------------------------

// Insert: Add a record to the specified table.
bool Database::insert(const std::string& tableName, const std::vector<std::string>& columns, const std::vector<std::string>& values) {
    auto table = getTable(tableName);
    if (!table) {
        std::cerr << "Error: Table not found: " << tableName << std::endl;
        return false;
    }
    if (columns.size() != values.size()) {
        std::cerr << "Error: Number of columns and values do not match." << std::endl;
        return false;
    }
    Record newRecord;
    for (size_t i = 0; i < columns.size(); ++i) {
        newRecord.setValue(columns[i], removeApostrophe(values[i]));
    }
    if (table->insertRecord(newRecord)) {
        std::cout << "Record inserted into table " << tableName << std::endl;
        return true;
    }
    else {
        std::cerr << "Error: Failed to insert record into table " << tableName << std::endl;
        return false;
    }
}// Select: Retrieve records from the specified table, filtering by condition if provided.
bool Database::select(const std::string& tableName, const std::vector<std::string>& columns, const std::string& condition) {
    auto table = getTable(tableName);
    if (!table) {
        std::cerr << "Error: Table not found: " << tableName << std::endl;
        return false;
    }
    const auto& records = table->getRecords();
    std::cout << "Selected records from table " << tableName << ":" << std::endl;
    for (const auto& record : records) {
        // If a condition is provided, evaluate it.
        if (!condition.empty() && !evaluateCondition(record, condition))
            continue;
        const auto& data = record.getData();
        // If columns contain only "*" then print all columns.
        if (columns.size() == 1 && columns[0] == "*") {
            for (const auto& p : data) {
                std::cout << p.first << ": " << p.second << " | ";
            }
        }
        else {
            for (const auto& col : columns) {
                auto it = data.find(col);
                if (it != data.end()) {
                    std::cout << col << ": " << it->second << " | ";
                }
            }
        }
        std::cout << std::endl;
    }
    return true;
}

// Update: Update records in the specified table that match the condition.
bool Database::update(const std::string& tableName, const std::vector<std::pair<std::string, std::string>>& assignments, const std::string& condition) {
    auto table = getTable(tableName);
    if (!table) {
        std::cerr << "Error: Table not found: " << tableName << std::endl;
        return false;
    }
    Record updateRecord;
    for (const auto& assignment : assignments) {
        updateRecord.setValue(assignment.first, assignment.second);
    }
    // The Table::updateRecord function is expected to iterate through records and apply the update
    // based on the condition. Here we pass the condition as is; the Table class will use our helper function.
    if (table->updateRecord(updateRecord, condition)) {
        std::cout << "Records updated in table " << tableName << std::endl;
        return true;
    }
    else {
        std::cerr << "Error: Failed to update records in table " << tableName << std::endl;
        return false;
    }
}

// Remove: Delete records from the specified table that match the condition.
bool Database::remove(const std::string& tableName, const std::string& condition) {
    auto table = getTable(tableName);
    if (!table) {
        std::cerr << "Error: Table not found: " << tableName << std::endl;
        return false;
    }
    // The Table::deleteRecord function will handle deletion using the condition.
    if (table->deleteRecord(condition)) {
        std::cout << "Records deleted from table " << tableName << std::endl;
        return true;
    }
    else {
        std::cerr << "Error: Failed to delete records from table " << tableName << std::endl;
        return false;
    }
}

//---------------------------------------------------------------------
// Table Management
//---------------------------------------------------------------------
void Database::addTable(const std::string& tableName, std::shared_ptr<Table> table) {
    tables[tableName] = table;
    std::cout << "Table added: " << tableName << std::endl;
}

std::shared_ptr<Table> Database::getTable(const std::string& tableName) {
    auto it = tables.find(tableName);
    if (it != tables.end()) {
        return it->second;
    }
    std::cerr << "Error: Table not found: " << tableName << std::endl;
    return nullptr;
}

bool Database::dropTable(const std::string& tableName) {
    auto table = getTable(tableName);
    if (!table) {
        std::cerr << "Error: Table '" << tableName << "' not found." << std::endl;
        return false;
    }
    // Iterate over all tables to remove foreign key constraints referencing this table.
    for (auto& pair : tables) {
        auto otherTable = pair.second;
        // Access the constraints of the other table's schema.
        auto& constraints = const_cast<std::vector<std::shared_ptr<Constraint>>&>(otherTable->getSchema().getConstraints());
        constraints.erase(std::remove_if(constraints.begin(), constraints.end(),
            [tableName](const std::shared_ptr<Constraint>& c) {
                if (auto fk = dynamic_cast<ForeignKeyConstraint*>(c.get())) {
                    return fk->getReferencedTable() == tableName;
                }
                return false;
            }), constraints.end());
    }
    tables.erase(tableName);
    std::cout << "DROP TABLE: Table '" << tableName << "' dropped." << std::endl;
    return true;
}


//---------------------------------------------------------------------
// Simple XOR Encryption/Decryption (Demo Only)
//---------------------------------------------------------------------
std::string Database::encryptData(const std::string& data, const std::string& key) {
    std::string encrypted = data;
    for (size_t i = 0; i < data.size(); ++i) {
        encrypted[i] = data[i] ^ key[i % key.size()];
    }
    return encrypted;
}

std::string Database::decryptData(const std::string& data, const std::string& key) {
    std::string decrypted = data;
    for (size_t i = 0; i < data.size(); ++i) {
        decrypted[i] = data[i] ^ key[i % key.size()];
    }
    return decrypted;
}
