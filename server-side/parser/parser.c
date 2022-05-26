#include "include/parser.h"

#define NB_TYPE 6

#define RETURN_IF_NULL(cond) \
    if (!(cond))             \
    {                        \
        return 0;            \
    }

#define SAFE_CALL(a, cond) a += (cond) == 0;

// Return true if char c can be used as a letter in a token
int is_alphanum(char c)
{
    //------- lower case a-z --------- upper case A-Z --------- digit 0-9 ---------- dot . --
    return (c >= 97 && c <= 122) || (c >= 65 && c <= 90) || (c >= 48 && c <= 57) /*|| c == 46*/;
}

int is_int(char c)
{
    return (c >= 48 && c <= 57);
}

int count_comma_list(char *data)
{
    if (!data) // If no datas
        return 0;

    char *p = data;
    char count = 1;
    char last = 0;
    char equal = 0;
    char inQuote = 0;
    int startedWithParenthesis = *p == '(';
    for (; *p; ++p)
    {
        if (inQuote)
        {
            if (*p != '\"')
                continue;
            else
            {
                inQuote = 0;
                last = 'a'; // quote counts as ident
            }
        }
        else if (*p == '\"')
        {
            inQuote = 1;
        }
        else if (is_alphanum(*p))
        {
            if (last == ' ' && !equal) // If we find a word after a space, and no equal
            {
                return count;
            }
            last = 'a';
        }
        else if (*p == '=')
        {
            if (equal)
            {
                fprintf(stderr, "Unauthorized token \'%c\' near \"%s\"", *p, p);
                return -1;
            }
            equal = 1;
        }
        else if (*p == ' ')
        {
            if (last == 'a')
            {
                last = ' ';
            }
        }
        else if (*p == ',')
        {
            count++;
            equal = 0;
            last = ',';
        }
        else
        {
            fprintf(stderr, "Unauthorized token \'%c\' near \"%s\"", *p, p);
            return -1;
        }
    }

    return count;
}

// Parse a instruction type, see enum instrType
dataType parse_datatype(char **data)
{
    struct parsingData parsingData[] = {
        {integer, 3, "int"},
        {string, 6, "string"}};

    for (int i = 0; i < 2; ++i)
    {
        if (!strncmp(*data, parsingData[i].inString, parsingData[i].size))
        {
            *data += parsingData[i].size;
            remove_spaces(data);
            return (instrType)i;
        }
    }
    fprintf(stderr, "No dataType found in \'%s\'", *data);
    return unknownDataType;
}

// Parse a instruction type, see enum instrType
instrType parse_instrtype(char **data)
{
    struct parsingData parsingData[] = {
        {sel, 3, "sel"},
        {del, 3, "del"},
        {upd, 3, "upd"},
        {add, 3, "add"},
        {crt, 3, "crt"},
        {drop, 4, "drop"}};

    for (int i = 0; i < 6; ++i)
    {
        if (!strncmp(*data, parsingData[i].inString, parsingData[i].size))
        {
            *data += parsingData[i].size;
            if (**data != ' ') // ensure at least one space after instrType
            {
                *data -= parsingData[i].size;
                break;
            }
            remove_spaces(data);
            return (instrType)i;
        }
    }
    fprintf(stderr, "No instrType found in \'%s\'\n", *data);
    return unknownInstrType;
}

int parse_expected_keyword(char **data, keyword expected)
{
    struct keywordData parsingKeywordData[] = {
        {where, 5, "where"},
        {from, 4, "from"},
        {in, 2, "in"},
        {set, 3, "set"},
        {with, 4, "with"}};

    char *start = *data;

    if (!strncmp(*data, parsingKeywordData[expected].inString, parsingKeywordData[expected].size))
    {
        *data += parsingKeywordData[expected].size;
        remove_spaces(data);
        return 1;
    }
    else
    {
        *data = start;
        fprintf(stderr, "Error while parsing keyword, expected \'%s\', got \'%s\'\n", parsingKeywordData[expected].inString, *data);
        return 0;
    }
}

// Parse an identifier
char *parse_ident(char **data)
{
    char *start = *data;
    while (is_alphanum(**data))
    {
        (*data)++;
    }
    if (*data == start)
    {
        fprintf(stderr, "No ident found in %s\n", *data);
        return NULL;
    }
    int size = (*data - start) / sizeof(char);
    char *ident = malloc((size + 1) * sizeof(char));
    strncpy(ident, start, size);
    ident[size] = '\0';
    remove_spaces(data);
    // printf("ident:%ld:%s:%s:\n", strlen(ident), ident, *data);
    if ((int)size > IDENT_MAX_LEGHT)
    {
        fprintf(stderr, "Error: Ident cannot exceed %d char in lenght\n", IDENT_MAX_LEGHT);
        free(ident);
        return 0;
    }
    return ident;
}

char *parse_value(char **data)
{
    char *start = *data;
    char *ident;
    if (**data == '\"') // is a string
    {
        (*data)++;
        while (**data != '\"' && **data)
        {
            (*data)++;
        }
        if (**data != '\"')
        {
            fprintf(stderr, "Quote open but not closed near \'%s\'\n", start);
            *data = start;
            return NULL;
        }
        (*data)++;
        int size = (*data - start - 2) / sizeof(char);
        ident = malloc((size + 1) * sizeof(char));
        strncpy(ident, start + 1, size);
        ident[size] = '\0';
    }
    else // is an int
    {
        while (is_int(**data))
        {
            (*data)++;
        }
        if (*data == start)
        {
            fprintf(stderr, "No value found in \'%s\'\n", *data);
            return NULL;
        }
        int size = (*data - start) / sizeof(char);
        ident = malloc((size + 1) * sizeof(char));
        strncpy(ident, start, size);
        ident[size] = '\0';
    }
    remove_spaces(data);
    // printf("ident:%ld:%s:%s:\n", strlen(ident), ident, *data);
    return ident;
}

// Remove heading spaces from the char*
void remove_spaces(char **data)
{
    while (**data == ' ' || **data == '\t' || **data == '\n')
    {
        (*data)++;
    }
}

int parse_comma(char **data)
{
    if (**data == ',')
    {
        (*data)++;
        remove_spaces(data);
        return 1;
    }
    else
    {
        fprintf(stderr, "No comma present !");
        return 0;
    }
}

int parse_equalsign(char **data)
{

    if (**data == '=')
    {
        (*data)++;
        remove_spaces(data);
        return 1;
    }
    else
    {
        fprintf(stderr, "No \'=\' present !\n");
        return 0;
    }
}

struct condition *parse_condition(char **data)
{
    struct condition *cond = condition_init();
    int nok = 0;
    SAFE_CALL(nok, cond->col = parse_ident(data))
    SAFE_CALL(nok, parse_equalsign(data))
    SAFE_CALL(nok, cond->val = parse_value(data))
    if (nok)
    {
        free_condition(cond);
        return 0;
    }
    return cond;
}

int parse_comma_list(char **data, charray *array)
{
    int sz = count_comma_list(*data);
    array->size = sz;
    if (!sz)
        return 0;

    array->arr = malloc(sz * sizeof(char *));
    RETURN_IF_NULL(array->arr[0] = parse_ident(data))
    for (int i = 1; i < sz; ++i)
    {
        RETURN_IF_NULL(parse_comma(data))
        RETURN_IF_NULL(array->arr[i] = parse_ident(data))
    }
    return 1;
}

int parse_columndef_list(char **data, charray *col_name, charray *col_type)
{
    int sz = count_comma_list(*data);
    col_name->size = sz;
    col_type->size = sz;
    if (!sz)
    {
        fprintf(stderr, "No column def found\n");
        return 0;
    }
    col_name->arr = calloc(sz, sizeof(char *));
    col_type->arr = calloc(sz, sizeof(char *));

    RETURN_IF_NULL(col_name->arr[0] = parse_ident(data))
    RETURN_IF_NULL(parse_equalsign(data))
    RETURN_IF_NULL(col_type->arr[0] = parse_ident(data))

    for (int i = 1; i < sz; ++i)
    {
        parse_comma(data);
        RETURN_IF_NULL(col_name->arr[i] = parse_ident(data))
        RETURN_IF_NULL(parse_equalsign(data))
        RETURN_IF_NULL(col_type->arr[i] = parse_ident(data))
    }

    return 1;
}

int parse_addvalue_list(char **data, charray *col_name, charray *col_val)
{
    int sz = count_comma_list(*data);
    col_name->size = sz;
    col_val->size = sz;
    if (!sz)
    {
        fprintf(stderr, "No column def found\n");
        return 0;
    }
    col_name->arr = calloc(sz, sizeof(char *));
    col_val->arr = calloc(sz, sizeof(char *));

    RETURN_IF_NULL(col_name->arr[0] = parse_ident(data))
    RETURN_IF_NULL(parse_equalsign(data))
    RETURN_IF_NULL(col_val->arr[0] = parse_value(data))

    for (int i = 1; i < sz; ++i)
    {
        parse_comma(data);
        RETURN_IF_NULL(col_name->arr[i] = parse_ident(data))
        RETURN_IF_NULL(parse_equalsign(data))
        RETURN_IF_NULL(col_val->arr[i] = parse_value(data))
    }
    return 1;
}

instr *parse_selinstr(char **data)
{
    struct SelInstr *selinstr = selinstr_init();
    int nok = 0;
    if (**data == '*') // Either '*' or column list
    {
        selinstr->columns = charray_init(); // An empty column list means '*'
        (*data)++;
        remove_spaces(data);
    }
    else
        SAFE_CALL(nok, parse_comma_list(data, selinstr->columns))
    SAFE_CALL(nok, parse_expected_keyword(data, from))
    SAFE_CALL(nok, selinstr->table = parse_ident(data))
    if (**data != '\0') // not the end of the instrution : there is a where statement
    {
        SAFE_CALL(nok, parse_expected_keyword(data, where))
        SAFE_CALL(nok, selinstr->cond = parse_condition(data))
        selinstr->has_cond = (selinstr->cond != NULL);
    }
    if (nok)
    {
        free_instr((instr *)selinstr);
        return 0;
    }
    return (instr *)selinstr;
}

instr *parse_crtinstr(char **data)
{
    struct CrtInstr *crtinstr = crtinstr_init();
    int nok = 0;
    SAFE_CALL(nok, crtinstr->table = parse_ident(data))
    SAFE_CALL(nok, parse_expected_keyword(data, with))
    SAFE_CALL(nok, parse_columndef_list(data, crtinstr->columns, crtinstr->types))
    if (nok)
    {
        free_instr((instr *)crtinstr);
        return 0;
    }
    return (instr *)crtinstr;
}

instr *parse_addinstr(char **data)
{
    struct AddInstr *crtinstr = addinstr_init();
    int nok = 0;
    SAFE_CALL(nok, parse_addvalue_list(data, crtinstr->columns, crtinstr->values))
    SAFE_CALL(nok, parse_expected_keyword(data, in))
    SAFE_CALL(nok, crtinstr->table = parse_ident(data))
    if (nok)
    {
        free_instr((instr *)crtinstr);
        return 0;
    }
    return (instr *)crtinstr;
}

instr *parse_instr(char *start)
{
    // printf("   <instr parse>\n");
    char *data = start;
    remove_spaces(&data);
    // printf("%s\n", data);
    instr *instr = NULL;
    instrType type = parse_instrtype(&data);
    switch (type)
    {
    case sel:
        instr = parse_selinstr(&data);
        break;
    case crt:
        instr = parse_crtinstr(&data);
        break;
    case add:
        instr = parse_addinstr(&data);
        break;
    case unknownInstrType:
    default:
        instr = unknowninstr_init();
    }
    if(!instr)
        instr = unknowninstr_init();
    // printf("   <instr parse/>\n");
    return instr;
}

InstrArray *parse_user_input(char *data)
{
    InstrArray *instrarray = malloc(sizeof(InstrArray));
    char *temp = data;
    int sz = 0;
    for (; *temp != '\0'; temp++)
        if (*temp == ';')
            sz++;
    instrarray->size = sz;
    instrarray->arr = calloc(sz, sizeof(instr *));
    char *token = data;
    char *semicolon = data;

    for (int i = 0; i < sz; ++i)
    {
        for (; *semicolon != ';'; semicolon++)
            ;
        *semicolon = '\0';
        // printf("-%s-\n", token);
        instrarray->arr[i] = parse_instr(token);
        token = ++semicolon;
    }
    return instrarray;
}