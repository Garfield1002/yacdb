/**
 * @file parser.h
 * @author Mateo
 * @brief Main parsing file, transform a user input into instruction from instruction.h
 *
 */
#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "parser_mallocs_frees.h"
#include "instruction.h"

/**
 * @brief The different keyword in the yacdb language, used for parsing
 */
typedef enum keyword
{
    /// where (in SelInstr)
    where,
    /// from (in SelInstr)
    from,
    /// in (in AddInstr)
    in,
    /// Not implemented yet
    set,
    /// with (in CrtInstr)
    with
} keyword;
/**
 * @brief Internal struct used when parsing
 */
struct parsingData
{
    int corresponding_enum;
    int size;
    char *inString;
};

/**
 * @brief Increase data pointer past leading spaces
 */
void remove_spaces(char **data);

/**
 * @brief Increase data pointer past a comma
 *
 * @param data Pointer to current position in user input
 * @return 1 on success, 0 on failure
 */
int parse_comma(char **data);
/**
 * @brief Increase data pointer past an equalsign
 *
 * @param data Pointer to current position in user input
 * @return 1 on success, 0 on failure
 */
int parse_equalsign(char **data);
/**
 * @brief Parse the identifier at the start of the data, increase data pointer on success
 *
 * @param data Pointer to current position in user input
 * @return char* A pointer to the identifier parsed
 */
char *parse_ident(char **data);
/**
 * @brief Parse a value, either an int or a "string", increase data pointer on success
 *
 * @param data Pointer to current position in user input
 * @return char* A pointer to the value parsed
 */
char *parse_value(char **data);
/**
 * @brief Parse the instrType at the start of the data, increase data pointer on success
 *
 * @param data Pointer to current position in user input
 * @return instrType
 */
instrType parse_instrtype(char **data);
/**
 * @brief Parse the expected keyword, increase data pointer on success
 *
 * @param data Pointer to current position in user input
 * @param expected Expected keyword to be parsed
 * @return 1 on success, 0 on failure
 */
int parse_expected_keyword(char **data, keyword expected);
//
/**
 * @brief Counts the size of the comma s
//  and return an array of instruct to be executed
//eparated list at the stat of the string given
 *
 * @param data Pointer to current position in user input
 * @return int The number of element in the list
 */
int count_comma_list(char *data);
/**
 * @brief Parse a comma separated list, increase data pointer on success
 *
 * @param data Pointer to current position in user input
 * @param res Stores the result of the operation
 * @return 1 on success, 0 on failure
 */
int parse_comma_list(char **data, charray *res);
/**
 * @brief Parse a single condition (COL = VALUE), increase data pointer on success
 *
 * @param data Pointer to current position in user input
 * @return struct condition*
 */
struct condition *parse_condition(char **data);
//
/**
 * @brief Parse an instruction
 *
 * @param data User input
 * @return instr* The instruction parsed (casted in instr*)
 */
instr *parse_instr(char *data);
/**
 * @brief Parse a SelInstr
 *
 * @param data Pointer to current position in user input
 * @return instr* The instruction parsed (casted in instr*)
 */
instr *parse_selinstr(char **data);
/**
 * @brief Parse a CrtInstr
 *
 * @param data Pointer to current position in user input
 * @return instr* The instruction parsed (casted in instr*)
 */
instr *parse_crtinstr(char **data);
/**
 * @brief Parse a AddInstr
 *
 * @param data Pointer to current position in user input
 * @return instr* The instruction parsed (casted in instr*)
 */
instr *parse_addinstr(char **data);
/**
 * @brief Parse the user input
 *
 * User input should be a semicolon separated list of instruction
 *
 * @param data User input
 * @return InstrArray* The array of instruction parsed
 */
InstrArray *parse_user_input(char *data);

#endif