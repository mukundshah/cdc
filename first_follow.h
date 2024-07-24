#ifndef FIRST_FOLLOW_H
#define FIRST_FOLLOW_H

#include <stdbool.h>

#define MAX_PRODUCTIONS 100
#define MAX_SYMBOLS 26
#define MAX_RHS_LENGTH 20
#define MAX_TERMINAL_LENGTH 10

typedef struct {
  char lhs;
  char rhs[MAX_RHS_LENGTH];
} Production;

void clear_grammar();
void add_production(char lhs, const char *rhs);

bool has_epsilon(char symbol);

const char *get_first_set(char symbol);
const char *get_follow_set(char symbol);

void compute_first_sets();
void compute_follow_sets();

#endif // FIRST_FOLLOW_H
