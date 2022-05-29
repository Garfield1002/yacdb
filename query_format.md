
# yacDB Query format
### EXIT

Type any of the following instructions to exit the console :

 - q
 - exit

This is a console instruction only and has nothing to do with the database, hence the absence of ';'.
## Instruction syntax

The yacdb keywords such as instruction types (sel, crt, add) and instruction keywords (where, with, in) need to be written in lowercase letter.

Any identifier, either table or column, can be composed of any letter -upper and lower case- as well as digits.


### CREATE

Instruction for table creation :

crt IDENT_table with IDENT_col=type, ...;

type: (not implemented yet, everything is string)
- "int"
- "string"

Exemple :
- crt users with name=string, age=int, hobby=string;


### INSERT

Instruction to insert a data in an existing table :

add IDENT_col=VAL, ... in IDENT_table;

VAL :
- int (ex: 48)
- string(ex: "a test string")

Exemple :
- add name="Joe", age=42, hobby="Volleyball" in users;

### SELECT

Instruction to select data from the database :

- sel IDENT_col,... from IDENT_table;
- sel * from IDENT_table;

Exemple :
- sel hobby, age from users;
- sel * from users;


### SELECT WHERE

Instruction select with a condition on a column :

- sel IDENT_col,... from IDENT_table cond CONDITION;

Exemple :
- sel hobby from users where name="Joe";
- sel * from users where age=30;

## NOT IMPLEMENTED YET

drop IDENT_table;

del from IDENT_table cond CONDITION;

up IDENT_table with IDENT_col = VAL, ... cond CONDITION;

-- JOIN
-- LIKE
-- ORDER BY
maybe: create database IDENT;
maybe: drop database IDENT;

sel IDENT_col,... from IDENT_table inner join IDENT_table on CONDITION;
