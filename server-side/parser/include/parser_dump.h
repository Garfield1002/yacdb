/**
 * @file parser_dump.h
 * @author Mateo
 * @brief Debug file to dump instructions
 */
#ifndef PARSER_DUMP_H
#define PARSER_DUMP_H

#include <stdio.h>
#include "instruction.h"

/**
 * @brief Dump an Instrarray in a file
 *
 * @param f Where to dump
 * @param Instrarray The InstrArray to dump
 */
void dump_instrarray(FILE *f, InstrArray *Instrarray);
/**
 * @brief Dump an instr in a file
 *
 * @param f Where to dump
 * @param instr The instr to dump
 */
void dump_instr(FILE *f, instr *instr);
/**
 * @brief Dump a SelInstr in a file
 *
 * @param f Where to dump
 * @param Instrarray The SelInstr to dump
 */
void dump_selinstr(FILE *f, struct SelInstr *selinstr);
/**
 * @brief Dump a CrtInstr in a file
 *
 * @param f Where to dump
 * @param Instrarray The CrtInstr to dump
 */
void dump_crtinstr(FILE *f, struct CrtInstr *crtinstr);
/**
 * @brief Dump an AddInstr in a file
 *
 * @param f Where to dump
 * @param Instrarray The AddInstr to dump
 */
void dump_addinstr(FILE *f, struct AddInstr *addinstr);
/**
 * @brief Dump a condition in a file
 *
 * @param f Where to dump
 * @param Instrarray The condition to dump
 */
void dump_condition(FILE *f, struct condition *cond);
// Dump charray on screen
/**
 * @brief Dump a charray in a file
 *
 * @param f Where to dump
 * @param Instrarray The charray to dump
 */
void dump_charray(FILE *f, charray *array);

#endif