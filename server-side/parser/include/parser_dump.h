#ifndef PARSER_DUMP_H
#define PARSER_DUMP_H

#include <stdio.h>
#include "instruction.h"

// Dump instr on streen
void dump_instr(FILE*, instr *);

void dump_selinstr(FILE*, struct SelInstr *);
void dump_crtinstr(FILE*, struct CrtInstr *);
void dump_addinstr(FILE*, struct AddInstr *);

// Dump charray on screen
void dump_charray(FILE*, charray *);

#endif