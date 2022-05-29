#ifndef CORE_H
#define CORE_H

#include "../../parser/include/main_parser.h"
#include "../../schema/include/record.h"
#include "../../schema/include/db_ops.h"

void db_handle(FILE* f, char* line);
void init_database();


#endif