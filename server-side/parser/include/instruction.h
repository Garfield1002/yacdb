/**
 * @file instruction.h
 * @author Mateo
 * @brief Structures for instruction management
 *
 */
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

/**
 * @brief The different instruction types, such as select or add
 *
 */
typedef enum instrType
{
    /// Select, same as SELECT in sql
    sel,
    /// Update (not implemented)
    upd,
    /// Delete (not implemented)
    del,
    /// Add, for data insertion
    add,
    /// Create, for table creation
    crt,
    /// Drop (not implemented)
    drop,
    /// Unknown, when there is a parsing error
    unknownInstrType
} instrType;


/**
 * @brief An array of char* (string)
 *
 */
typedef struct charray
{
    /// The array
    char **arr;
    /// The size of the array
    int size;
} charray;
/**
 * @brief An equality condition
 *
 */
struct condition
{
    /// Column name
    char *col;
    /// Column value
    char *val;
};


/// One yacDB instruction, this type is meant to be casted into other instr type
typedef struct instr
{
    instrType type;
} instr;

/**
 * @brief Select instruction
 *
 */
struct SelInstr
{
    /// Always "sel"
    instrType type;
    /// Table name
    char *table;
    /// Columns name, if empty represents all columns
    charray *columns;
    /// 1 if there is a where statement in the instruction
    int has_cond;
    /// The condition in the where
    struct condition *cond;
};

/**
 * @brief Create instruction
 *
 */
struct CrtInstr
{
    /// Always "crt"
    instrType type;
    /// Table name
    char *table;
    /// Columns name
    charray *columns;
    /// Columns type (useless)
    charray *types;
};

/**
 * @brief Add instruction
 *
 */
struct AddInstr
{
    /// Always "add"
    instrType type;
    /// Table name
    char *table;
    /// Columns name
    charray *columns;
    /// Columns values
    charray *values;
};

/**
 * @brief An array of instruction
 *
 */
typedef struct InstrArray
{
    // The array
    instr **arr;
    // The size of the array
    int size;
} InstrArray;

#endif