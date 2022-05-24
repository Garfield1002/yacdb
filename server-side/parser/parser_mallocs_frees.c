#include "include/parser_mallocs_frees.h"

charray *charray_init()
{
    charray *array = malloc(sizeof(charray));
    array->arr = NULL;
    array->size = 0;
}

void free_charray(charray *arrayToFree)
{
    for (int i = 0; i < arrayToFree->size; ++i)
    {
        free(arrayToFree->arr[i]);
    }
    free(arrayToFree->arr);
    free(arrayToFree);
}

void free_instrarray(InstrArray *arrayToFree)
{
    for (int i = 0; i < arrayToFree->size; ++i)
    {
        free_instr(arrayToFree->arr + i);
    }
    free(arrayToFree->arr);
    free(arrayToFree);
}

void free_instr(instr *instr)
{
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
    selinstr->hasCond = 0;
    selinstr->cond.col = NULL;
    selinstr->cond.val = NULL;
    return selinstr;
}

void free_selinstr(struct SelInstr *selinstr)
{
    free(selinstr->table);
    free_charray(selinstr->columns);
    if (selinstr->hasCond)
    {
        free(selinstr->cond.col);
        free(selinstr->cond.val);
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

instr* unknowninstr_init(){
    instr* instr = malloc(sizeof(instr));
    instr->type = unknownInstrType;
    return instr;
}