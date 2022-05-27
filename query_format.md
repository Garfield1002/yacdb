
### yacDB Query format
#### EXIT

q;

exit;


#### CREATE / DROP

crt IDENT_table with IDENT_col=type, ...;

type:
- "int"
- "string"

drop IDENT_table;

#### INSERT

add IDENT_col=VAL, ... in IDENT_table;

VAL:
- int (ex: 48)
- string(ex: "a test string")

#### SELECT
sel IDENT_col,... from IDENT_table;

sel * from IDENT_table;

#### WHERE
sel IDENT_col,... from IDENT_table cond CONDITION;

del from IDENT_table cond CONDITION;

up IDENT_table with IDENT_col = VAL, ... cond CONDITION;

#### maybe:
-- JOIN
-- LIKE
-- ORDER BY
maybe: create database IDENT;
maybe: drop database IDENT;

sel IDENT_col,... from IDENT_table inner join IDENT_table on CONDITION;
