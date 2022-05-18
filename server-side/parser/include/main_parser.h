#ifndef PARSER_H
#define PARSER_H

#include "instruction.h"

// Parse an instruction and return it
instr *parse_instr(char *);
void dump_instr(instr *);
void free_instr(instr *);

#endif