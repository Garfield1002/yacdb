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

void dump_instr(instr *instr)
{

    printf("   <instr print>\n");
    switch (instr->type)
    {
    case sel:
        dump_selinstr((struct SelInstr *)instr);
        break;
    case crt:
        dump_crtinstr((struct CrtInstr *)instr);
        break;
    case add:
        dump_addinstr((struct AddInstr *)instr);
        break;
    default:
        printf("type: unknown\nThis instruction has an unknown type, cannor dump :%d:\n", instr->type);
        break;
    }
    printf("   <instr print/>\n");
}

void dump_selinstr(struct SelInstr *instr)
{
    printf("type: %s\ntable: %s\ncolumns: ", typeToString[instr->type], instr->table);
    dump_charray(instr->columns);
    printf("not cond yet\n");
}

void dump_crtinstr(struct CrtInstr *instr)
{
    printf("type: %s\ntable: %s\ncolumns: ", typeToString[instr->type], instr->table);
    dump_charray(instr->columns);
    printf("types: ");
    dump_charray(instr->types);
}

void dump_addinstr(struct AddInstr *instr)
{
    printf("type: %s\ntable: %s\ncolumns: ", typeToString[instr->type], instr->table);
    dump_charray(instr->columns);
    printf("values: ");
    dump_charray(instr->values);
}

void dump_charray(charray *a)
{
    printf("%d [", a->size);
    if (a->size > 0)
    {
        for (int i = 0; i < a->size - 1; ++i)
        {
            printf("%s,", a->arr[i]);
        }
        printf("%s", a->arr[a->size - 1]);
    }
    printf("]\n");
}