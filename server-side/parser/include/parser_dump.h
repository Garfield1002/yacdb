#ifndef PARSER_DUMP_H
#define PARSER_DUMP_H

#include <stdio.h>
#include "instruction.h"

// Dump instr on streen
void dump_instr(instr *);
void dump_selinstr(struct SelInstr *);
void dump_crtinstr(struct CrtInstr *);
void dump_addinstr(struct AddInstr *);

// Dump charray on screen
void dump_charray(charray *);

#endif