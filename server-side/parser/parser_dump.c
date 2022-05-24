#include "include/parser_dump.h"

const char *typeToString[] = {
    "SEL",
    "UPD",
    "DEL",
    "ADD",
    "CRT",
    "DROP"};

const char *keywordToString[] = {
    "where",
    "from",
    "val",
    "set",
    "tbl"};

void dump_instrarray(FILE *f, InstrArray *instrarray)
{
    for (int i = 0; i < instrarray->size; ++i)
    {
        fprintf(f, "\n");
        dump_instr(f, instrarray->arr[i]);
    }
    fprintf(f, "\n");
}

void dump_instr(FILE *f, instr *instr)
{
    // fprintf(f, "   <instr print>\n");
    switch (instr->type)
    {
    case sel:
        dump_selinstr(f, (struct SelInstr *)instr);
        break;
    case crt:
        dump_crtinstr(f, (struct CrtInstr *)instr);
        break;
    case add:
        dump_addinstr(f, (struct AddInstr *)instr);
        break;
    default:
        fprintf(f, "ERROR: Unable to parse this instruction\n");
        break;
    }
    // fprintf(f, "   <instr print/>\n");
}

void dump_selinstr(FILE *f, struct SelInstr *instr)
{
    fprintf(f, "type: %s\ntable: %s\ncolumns: ", typeToString[instr->type], instr->table);
    dump_charray(f, instr->columns);
    if (instr->has_cond)
        dump_condition(f, instr->cond);
}

void dump_crtinstr(FILE *f, struct CrtInstr *instr)
{
    fprintf(f, "type: %s\ntable: %s\ncolumns: ", typeToString[instr->type], instr->table);
    dump_charray(f, instr->columns);
    fprintf(f, "types: ");
    dump_charray(f, instr->types);
}

void dump_addinstr(FILE *f, struct AddInstr *instr)
{
    fprintf(f, "type: %s\ntable: %s\ncolumns: ", typeToString[instr->type], instr->table);
    dump_charray(f, instr->columns);
    fprintf(f, "values: ");
    dump_charray(f, instr->values);
}

void dump_charray(FILE *f, charray *a)
{
    fprintf(f, "%d [", a->size);
    if (a->size > 0)
    {
        for (int i = 0; i < a->size - 1; ++i)
        {
            fprintf(f, "%s,", a->arr[i]);
        }
        fprintf(f, "%s", a->arr[a->size - 1]);
    }
    fprintf(f, "]\n");
}

void dump_condition(FILE *f, struct condition *cond)
{
    fprintf(f, "condition: %s=%s\n", cond->col, cond->val);
}