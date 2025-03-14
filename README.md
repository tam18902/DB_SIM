# Database Management System

## Overview
This is a simple, file-based database management system implemented in C++. 

It supports basic SQL-like queries for creating, inserting, selecting, updating, and deleting records. 

The database also supports saving to and loading from encrypted files.

## Features
- SQL-like query execution:
  - `CREATE TABLE <tableName> (col1 TYPE, col2 TYPE, ...);`
  - `INSERT INTO <tableName> (col1, col2, ...) VALUES (val1, val2, ...);`
  - `SELECT * FROM <tableName>;`
  - `UPDATE <tableName> SET col1=val1 WHERE condition;`
  - `DELETE FROM <tableName> WHERE condition;`
- Database persistence:
  - `FLUSH <filename> <key>;` - Save database to a file with encryption
  - `LOAD <filename> <key>;` - Load an encrypted database from a file
- Table and column management:
  - `DROP TABLE <tableName>;`
  - `DROP COLUMN <columnName> FROM <tableName>;`
- Command help system:
  - `HELP [command]` - Display usage and examples for commands
- Interactive CLI for executing queries

## Installation
1. Clone the repository:
   ```sh
   git clone https://github.com/tam18902/DB_SIM.git
   cd DB_SIM
   ```
2. Compile the project:
   ```sh
   g++ -std=c++17 -o database main.cpp QueryProcessor.cpp Database.cpp -lcrypto
   ```
   *(Ensure OpenSSL is installed for encryption support.)*
3. Run the application:
   ```sh
   ./database
   ```

## Usage
When you start the application, the available commands will be displayed. You can enter queries interactively:

```sh
> CREATE TABLE users (id INT, name STRING, age INT);
> INSERT INTO users (id, name, age) VALUES (1, 'Alice', 30);
> SELECT * FROM users;
> FLUSH data.db mysecretkey;
> EXIT
```

## Example Commands
### Creating a Table
```sql
CREATE TABLE employees (id INT, name STRING, department STRING, salary FLOAT);
```

### Inserting Data
```sql
INSERT INTO employees (id, name, department, salary) VALUES (1, 'John Doe', 'HR', 50000);
```

### Selecting Data
```sql
SELECT * FROM employees;
```

### Updating Data
```sql
UPDATE employees SET salary = 55000 WHERE id = 1;
```

### Deleting Data
```sql
DELETE FROM employees WHERE id = 1;
```

### Dropping a Table
```sql
DROP TABLE employees;
```

### Saving and Loading the Database
```sh
FLUSH mydatabase.db mypassword;
LOAD mydatabase.db mypassword;
```

### Getting Help
```sh
HELP INSERT;
```

## Contributing
1. Fork the repository
2. Create a new branch (`git checkout -b feature-branch`)
3. Commit your changes (`git commit -am 'Add new feature'`)
4. Push to the branch (`git push origin feature-branch`)
5. Open a Pull Request

## License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Contact
For any issues or feature requests, please open an issue in the repository or contact the maintainer at `tam18902@github.com`.

