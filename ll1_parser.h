#ifndef LL1_PARSER_H
#define LL1_PARSER_H

#include "first_follow.h" // This will provide MAX_RHS_LENGTH
#include <stdbool.h>

#define MAX_TERMINALS 26
#define MAX_NON_TERMINALS 26

#define MAX_STACK_SIZE 100

typedef struct {
  int production_index;
  char production[MAX_RHS_LENGTH];
} TableEntry;

extern TableEntry parsing_table[MAX_NON_TERMINALS][MAX_TERMINALS];

void create_parsing_table();
bool ll1_parse(const char *input);

#endif // LL1_PARSER_H
