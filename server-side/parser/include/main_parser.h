#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include "instruction.h"

// Parse an instruction and return it
instr *parse_instr(char *);
InstrArray *parse_user_input(char *);
void dump_instr(FILE *, instr *);
void dump_instrarray(FILE *, InstrArray *);
void free_instr(instr *);
void free_instrarray(InstrArray *);

#endif