#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#define IDENT_MAX_LEGHT 20
#define COLUMNS_MAX_NB 10

typedef enum dataType
{
    integer,
    string,
    unknownDataType
} dataType;

typedef enum instrType
{
    sel,
    upd,
    del,
    add,
    crt,
    drop,
    unknownInstrType
} instrType;

typedef enum keyword
{
    where,
    from,
    in,
    set,
    with
} keyword;

// Array of strings
typedef struct charray
{
    char **arr;
    int size;
} charray;

struct condition
{
    char *col;
    char *val;
};

// One yacDB instruction, this type is meant to be casted into other instr type
typedef struct instr
{
    instrType type;
} instr;

struct SelInstr
{
    instrType type;
    char *table;
    charray *columns; // empty = * = all cols
    int has_cond;
    struct condition *cond;
};

struct CrtInstr
{
    instrType type;
    char *table;
    charray *columns;
    charray *types;
};

struct AddInstr
{
    instrType type;
    char *table;
    charray *columns;
    charray *values;
};

// One line entered by the user, can be several instruction splited by ';'
typedef struct InstrArray
{
    instr **arr;
    int size;
} InstrArray;

#endif