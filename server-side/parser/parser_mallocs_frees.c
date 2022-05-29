#include "include/parser_mallocs_frees.h"

charray *charray_init()
{
    charray *array = malloc(sizeof(charray));
    array->arr = NULL;
    array->size = 0;
}

void free_charray(charray *array)
{
    for (int i = 0; i < array->size; ++i)
    {
        free(array->arr[i]);
    }
    free(array->arr);
    free(array);
}

InstrArray* instrarray_init(int sz){

    InstrArray *instrarray = malloc(sizeof(InstrArray));
    instrarray->size = sz;
    instrarray->arr = calloc(sz, sizeof(instr *));
    return instrarray;
}

void free_instrarray(InstrArray *instrarray)
{
    for (int i = 0; i < instrarray->size; ++i)
    {
        free_instr(instrarray->arr[i]);
    }
    free(instrarray->arr);
    free(instrarray);
}

void free_instr(instr *instr)
{
    if (!instr)
        return;
    switch (instr->type)
    {
    case sel:
        free_selinstr((struct SelInstr *)instr);
        break;
    case crt:
        free_crtinstr((struct CrtInstr *)instr);
        break;
    case add:
        free_addinstr((struct AddInstr *)instr);
        break;
    case unknownInstrType:
        free(instr);
        break;
    }
}

struct SelInstr *selinstr_init()
{
    struct SelInstr *selinstr = malloc(sizeof(struct SelInstr));
    selinstr->type = sel;
    selinstr->table = NULL;
    selinstr->columns = charray_init();
    selinstr->has_cond = 0;
    selinstr->cond = NULL;
    return selinstr;
}

void free_selinstr(struct SelInstr *selinstr)
{
    free(selinstr->table);
    free_charray(selinstr->columns);
    if (selinstr->has_cond)
    {
        free_condition(selinstr->cond);
    }
    free(selinstr);
}

struct CrtInstr *crtinstr_init()
{
    struct CrtInstr *crtinstr = malloc(sizeof(struct CrtInstr));
    crtinstr->type = crt;
    crtinstr->table = NULL;
    crtinstr->columns = charray_init();
    crtinstr->types = charray_init();
    return crtinstr;
}

void free_crtinstr(struct CrtInstr *crtinstr)
{
    free(crtinstr->table);
    free_charray(crtinstr->columns);
    free_charray(crtinstr->types);
    free(crtinstr);
}

struct AddInstr *addinstr_init()
{
    struct AddInstr *addinstr = malloc(sizeof(struct AddInstr));
    addinstr->type = add;
    addinstr->table = NULL;
    addinstr->columns = charray_init();
    addinstr->values = charray_init();
    return addinstr;
}

void free_addinstr(struct AddInstr *addinstr)
{
    free(addinstr->table);
    free_charray(addinstr->columns);
    free_charray(addinstr->values);
    free(addinstr);
}

instr *unknowninstr_init()
{
    instr *instr = malloc(sizeof(instr));
    instr->type = unknownInstrType;
    return instr;
}

struct condition *condition_init()
{
    struct condition *cond = malloc(sizeof(struct condition));
    cond->col = 0;
    cond->val = 0;
    return cond;
}

void free_condition(struct condition *cond)
{
    free(cond->val);
    cond->val = NULL;
    free(cond->col);
    cond->col = NULL;
    free(cond);
}