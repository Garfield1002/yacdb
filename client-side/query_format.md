
# yacDB Query format
## Console

The console is the place where you write your yacDB instructions, once you press enter the instruction is sent to the server to be treated as a yacDB instruction.
There is 2 exceptions to this : exit instructions and empty lines. If no character appear on the line, no request will be sent to the database. If an exit instruction is detected, the console will exit safely.

The exit instruction are the following :
 - q
 - exit

## YacDB instruction syntax

A yacDB instruction is a series of keyword, identifier and values that end with a semicolon.
Any yacDB keywords such as instruction types (sel, crt, add) and instruction keywords (where, with, in) need to be written in <b>lowercase letter</b>.
A type is a particular type of keyword that refers to yacDB data type stored in the database.
Any identifier, either table name or column name, can be composed of any letter -upper and lower case- as well as digits.
A value is either an int (a series of digits) or a string (a double-quoted list of charaters: "tH1s i5 a string!").

In the following sections:
- IDENT_table and IDENT_col refer to identifiers of either a table or a column
- TYPE refers to a yacDB type : either "int" of "string". (For now, this is not important as everything is considered to be a string)
- VALUE refers to a data value as described earlier
- ... refers to a comma separated list of the previous item
- Anything else is a yacDB keyword

### CREATE

Instruction that create an empty table :

- crt IDENT_table with IDENT_col=TYPE, ...;

Exemple :
- crt users with name=string, age=int, hobby=string;

### INSERT

Instruction that insert a data in an existing table :

- add IDENT_col=VALUE, ... in IDENT_table;

Exemple :
- add name="Joe", age=42, hobby="Volleyball" in users;

### SELECT

Instruction that select and output data from the database :

- sel IDENT_col,... from IDENT_table;
- sel * from IDENT_table;

Exemple :
- sel hobby, age from users;
- sel * from users;

#### SELECT WHERE

Select instruction with an equality condition on a column :

- sel IDENT_col,... from IDENT_table cond CONDITION;

Exemple :
- sel hobby from users where name="Joe";
- sel * from users where age=30;
