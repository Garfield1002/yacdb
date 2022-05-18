#include "parser/include/main_parser.h"
#include "../utils/utils.h"
#include <string.h>

int main(int argc, const char **argv)
{
    printf("\n-- Welcome to the yacDB console ! -- \n");
    printf("Available command are :\n\n");
    printf("crt IDENT_table with IDENT_col=TYPE, ...\n");
    printf("sel IDENT_col, ... from IDENT_table\n");
    printf("add IDENT_col=VAL, .. in IDENT_table\n\n");
    printf("type q or exit to quit the console\n\n");

    char l[LINE_SIZE];
    while (1)
    {
        printf("-> ");
        strcpy(l, "");
        // scanf("%s", l);
        fgets(l, LINE_SIZE, stdin);
        if (l[0] == '\n') // if strats with a \n, nothing to do, restart
        {
            continue;
        }
        l[strlen(l) - 1] = '\0'; // remove trailing \n
        printf("<- %s-%ld\n", l, strlen(l));
        if (!strcmp(l, "q") || !strcmp(l, "exit"))
        {
            return 0;
        }
        instr *i = parse_instr(l);
        if (i)
        {
            dump_instr(i);
            free_instr(i);
        }
    }
}