#include "include/parser.h"

#define NB_TYPE 6

#define RETURN_IF_NULL(cond) \
    if (!(cond))             \
    {                        \
        return 0;            \
    }

#define ISOK(a, cond) a += (cond) == 0;

// Return true if char c can be used as a letter in a token
int isAlphaOk(char c)
{
    //------- lower case a-z --------- upper case A-Z --------- digit 0-9 ---------- dot . --
    return (c >= 97 && c <= 122) || (c >= 65 && c <= 90) || (c >= 48 && c <= 57) || c == 46;
}

int isNumeric(char c)
{
    return (c >= 48 && c <= 57);
}

// Count the number of tokenArr needed to store the instruction
int tokenArrCounter(char *data)
{
    if (!data) // If no datas
        return 0;

    char *p = data;
    int count = 0;
    char alphaStreak = 0;
    char startWithAlpha = 0;
    char endWithAlpha = 0;
    char bracketOpened = 0;
    char hasEqual = 0;
    char last = *data;
    for (; *p; ++p)
    {
        if (isAlphaOk(*p))
        {
            last = *p;
            if (!alphaStreak) // First letter after some spaces
            {
                alphaStreak = 1; // plus one tokenArr if started with an okChar
            }
        }
        else
        {
            if (alphaStreak && !bracketOpened)
            {
                count++;
            }
            alphaStreak = 0;
            if (*p == ' ')
            {
                continue;
            }
            last = *p;
            if (*p == '(')
            {
                bracketOpened = 1;
            }
            else if (*p == ')')
            {
                count = count + 1 + hasEqual;
                bracketOpened = 0;
                hasEqual = 0;
            }
            else if (*p == ',')
            {
                count -= !bracketOpened * (1 + hasEqual);
                hasEqual = 0;
            }
            else if (*p == '=')
            {
                hasEqual = 1;
            }
            else
            {
                fprintf(stderr, "Unauthorized token \'%c\' near \"%s\"", *p, p);
                return -1;
            }
        }
    }
    return count + (last != ')');
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
        else if (isAlphaOk(*p))
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
    fprintf(stderr, "No instrType found in \'%s\'", *data);
    return unknownInstrType;
}

// Parse a instruction keyword, see enum instrType
instrType parseKeyword(char **data)
{
    // for (int i = 0; i < NB_type; ++i)
    // {
    //     if (!strncmp(*data, parsingTypeData[i].inString, parsingTypeData[i].size))
    //     {
    //         *data += parsingTypeData[i].size;
    //         return parsingTypeData[i].type;
    //     }
    // }
    // remove_spaces(data);
    // return unknownType;
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
    while (isAlphaOk(**data))
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
            fprintf(stderr, "Quote open but not closed near %s\n", start);
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
        while (isNumeric(**data))
        {
            (*data)++;
        }
        if (*data == start)
        {
            fprintf(stderr, "No value found in %s\n", *data);
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
void remove_spaces(char **currentData)
{
    while (**currentData == ' ' || **currentData == '\t')
    {
        (*currentData)++;
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
        fprintf(stderr, "No \'=\' present !");
        return 0;
    }
}

int parse_comma_list(char **data, charray *array)
{
    int sz = count_comma_list(*data);
    array->size = sz;
    printf(" comma_list_count:%d:\n", sz);
    if (!sz)
        return 0;

    array->arr = malloc(sz * sizeof(char *));
    RETURN_IF_NULL(array->arr[0] = parse_ident(data))
    for (int i = 1; i < sz; ++i)
    {
        RETURN_IF_NULL(parse_comma(data))
        RETURN_IF_NULL(array->arr[i] = parse_ident(data))
    }
    printf(" end_comma_list\n");
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
    ISOK(nok, parse_comma_list(data, selinstr->columns))
    ISOK(nok, parse_expected_keyword(data, from))
    ISOK(nok, selinstr->table = parse_ident(data))
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
    ISOK(nok, crtinstr->table = parse_ident(data))
    ISOK(nok, parse_expected_keyword(data, with))
    ISOK(nok, parse_columndef_list(data, crtinstr->columns, crtinstr->types))
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
    ISOK(nok, parse_addvalue_list(data, crtinstr->columns, crtinstr->values))
    ISOK(nok, parse_expected_keyword(data, in))
    ISOK(nok, crtinstr->table = parse_ident(data))
    if (nok)
    {
        free_instr((instr *)crtinstr);
        return 0;
    }
    return (instr *)crtinstr;
}

instr *parse_instr(char *start)
{
    printf("   <instr parse>\n");
    char *data = start;
    remove_spaces(&data);
    // printf("%s\n", data);
    instr *instr;
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
    default:
        instr->type = unknownInstrType;
        parse_expected_keyword(&data, from);
        parse_expected_keyword(&data, where);
        parse_expected_keyword(&data, set);
        printf("%s\n", data);
        break;
    }
    if (instr == NULL)
    {
        printf("Error during parsing\n");
    }
    printf("   <instr parse/>\n");
    return instr;
}

InstrArray *parse_user_input(char *data)
{
    InstrArray *instrarray = malloc(sizeof(InstrArray));
    instrarray->size = 0;
    const char s[2] = ";";
    char *token;

    /* get the first token */
    token = strtok(data, s);

    /* walk through other tokens */
    while (token != NULL)
    {
        printf("-%s-\n", token);
        instrarray->arr = parse_instr(token); // does not work
        instrarray->size++;
        token = strtok(NULL, s);
    }
}
