#include "include/parser_mallocs_frees.h"

void freenull(void *p)
{
    free(p);
    p = NULL;
}

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
        freenull(arrayToFree->arr[i]);
    }
    freenull(arrayToFree->arr);
    freenull(arrayToFree);
}

void free_instrarray(InstrArray *arrayToFree)
{
    for (int i = 0; i < arrayToFree->size; ++i)
    {
        free_instr(arrayToFree->arr + i);
    }
    freenull(arrayToFree->arr);
    freenull(arrayToFree);
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
    freenull(selinstr->table);
    free_charray(selinstr->columns);
    if (selinstr->hasCond)
    {
        freenull(selinstr->cond.col);
        freenull(selinstr->cond.val);
    }
    freenull(selinstr);
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
    freenull(crtinstr->table);
    free_charray(crtinstr->columns);
    free_charray(crtinstr->types);
    freenull(crtinstr);
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
    freenull(addinstr->table);
    free_charray(addinstr->columns);
    free_charray(addinstr->values);
    freenull(addinstr);
}