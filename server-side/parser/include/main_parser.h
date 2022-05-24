#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include "instruction.h"

// Parse an instruction and return it
instr *parse_instr(char *);
void dump_instr(FILE*, instr *);
void free_instr(instr *);

#endif