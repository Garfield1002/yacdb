#ifndef PARSER_MALLOCS_FREES_H
#define PARSER_MALLOCS_FREES_H

#include <stdlib.h>
#include "instruction.h"

// free and inits
charray *charray_init();
void free_charray(charray *);

InstrArray *instrarray_init(int);
void free_instrarray(InstrArray *);

struct SelInstr *selinstr_init();
void free_selinstr(struct SelInstr *);

struct CrtInstr *crtinstr_init();
void free_crtinstr(struct CrtInstr *);

struct AddInstr *addinstr_init();
void free_addinstr(struct AddInstr *);

void free_instr(instr *);
struct condition *condition_init();
void free_condition(struct condition *);
#endif