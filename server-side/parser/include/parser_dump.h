#ifndef PARSER_DUMP_H
#define PARSER_DUMP_H

#include <stdio.h>
#include "instruction.h"

// Dump instrarray on streen
void dump_instrarray(FILE *, InstrArray *);
// Dump instr on streen
void dump_instr(FILE *, instr *);

void dump_selinstr(FILE *, struct SelInstr *);
void dump_crtinstr(FILE *, struct CrtInstr *);
void dump_addinstr(FILE *, struct AddInstr *);
void dump_condition(FILE *, struct condition *);
// Dump charray on screen
void dump_charray(FILE *, charray *);

#endif