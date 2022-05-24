#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "parser_mallocs_frees.h"
#include "instruction.h"

// Max lenght of a user line

struct dataTypeData
{
    enum dataType type;
    int size;
    char *inString;
};

struct parsingData
{
    int corresponding_enum;
    int size;
    char *inString;
};

struct keywordData
{
    enum keyword keyword;
    int size;
    char *inString;
};

// parsing stuff

// Increase the cursor past leading spaces
void remove_spaces(char **);
// Increase the cursor past a comma
int parse_comma(char **);
// Increase the cursor past an equalsign
int parse_equalsign(char **);
// Parse the identifier at the start of the data, increase cursor
char *parse_ident(char **);
// Parse a value, either an int or a "string"
char *parse_value(char **);
// Parse the instrType at the start of the data, increase cursor
instrType parse_instrtype(char **);
// Parse the expected keyword, return 1 with the cursor increased if correct parsing, 0 and cursor not increase if incorrect
int parse_expected_keyword(char **, keyword);
// Counts the size of the comma separated list at the stat of the string given
int count_comma_list(char *);
// Parse a comma separated list and stores it in the charray parameter
int parse_comma_list(char **, charray *);
// Parse a select where statement
struct condition *parse_condition(char **);
// Parse an instruction and return it
instr *parse_instr(char *);
// Parse a SelInstr and return it
instr *parse_selinstr(char **);
// Parse the user input and return an array of instruct to be executed
// User input should be a semicolon separated list of instruction
InstrArray *parse_user_input(char *data);

#endif