/**
 * @file main_parser.h
 * @author Mateo
 * @brief Small parser include to only add some generic function
 */
#define PARSER_H

#include <stdio.h>
#include "instruction.h"

instr *parse_instr(char *);
InstrArray *parse_user_input(char *);
void dump_instr(FILE *, instr *);
void dump_instrarray(FILE *, InstrArray *);
void free_instr(instr *);
void free_instrarray(InstrArray *);

#endif