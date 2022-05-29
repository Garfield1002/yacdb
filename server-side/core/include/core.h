/**
 * @file core.h
 * @author Mateo
 * @brief Core YACDB managment, from user input to execution
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef CORE_H
#define CORE_H

#include "../../parser/include/main_parser.h"
#include "../../schema/include/record.h"
#include "../../schema/include/db_ops.h"

/**
 * @brief Core function that transform a line of user input into yacdb instruction and execute them
 *
 * @param f The file in which to write the result
 * @param line The line entered by the user
 */
void db_handle(FILE* f, char* line);

/**
 * @brief Function that needs to be called before being able to call db_handle()
 *
 */
void init_database();

/**
 * @brief Debug function to test the parser
 *
 * @param f Output file (stdout)
 * @param line User input
 */
void debug_parser(FILE* f, char* line);
#endif