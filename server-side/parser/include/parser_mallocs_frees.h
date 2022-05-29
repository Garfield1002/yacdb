/**
 * @file parser_mallocs_frees.h
 * @author Mateo
 * @brief Frees and mallocs helper for the structures in instruction.h
 *
 */
#ifndef PARSER_MALLOCS_FREES_H
#define PARSER_MALLOCS_FREES_H

#include <stdlib.h>
#include "instruction.h"
/**
 * @brief Initialize a charray
 *
 * @return charray*
 */
charray *charray_init();
/**
 * @brief Free a charray
 *
 * @param array The charray to free
 */
void free_charray(charray *array);
/**
 * @brief Initialize an InstrArray
 *
 * @param sz The number of instruction stored in this InstrArray
 * @return InstrArray*
 */
InstrArray *instrarray_init(int sz);
/**
 * @brief Free an InstrArray
 *
 * @param instrarray The InstrArray to free
 */
void free_instrarray(InstrArray * instrarray);
/**
 * @brief Initialize an instr of instrType unknownInstrType
 *
 * @return instr*
 */
instr *unknowninstr_init();
/**
 * @brief Initialize a SelInstr
 *
 * @return struct SelInstr*
 */
struct SelInstr *selinstr_init();
/**
 * @brief Free a SelInstr
 *
 * @param selinstr The SelInstr to free
 */
void free_selinstr(struct SelInstr *selinstr);
/**
 * @brief Initialize a CrtInstr
 *
 * @return struct CrtInstr*
 */
struct CrtInstr *crtinstr_init();
/**
 * @brief Free a CrtInstr
 *
 * @param crtinstr The CrtInstr to free
 */

void free_crtinstr(struct CrtInstr *crtinstr);
/**
 * @brief Initialize an AddInstr
 *
 * @return struct AddInstr*
 */
struct AddInstr *addinstr_init();
/**
 * @brief Free an AddInstr
 *
 * @param addinstr The AddInstr to free
 */
void free_addinstr(struct AddInstr *addinstr);
/**
 * @brief Free an instruction of any type
 *
 * @param instr The instr to free
 */
void free_instr(instr *instr);
/**
 * @brief Initialize a struct condition
 *
 * @return struct condition*
 */
struct condition *condition_init();
/**
 * @brief Free a struct condition
 *
 * @param cond The struct condition to free
 */
void free_condition(struct condition *cond);
#endif