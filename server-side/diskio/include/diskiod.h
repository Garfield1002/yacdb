/**
 * @file diskiod.h
 * @author Jack Royer
 * @brief Debug utils for disk and cache operations.
 *
 * @date 2022-05-29
 */

#pragma once
#include "diskio.h"

void dump_header();

void dump_node(struct node *node);