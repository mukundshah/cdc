#include "ll1_parser.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int num_productions = 0;

Production grammar[MAX_PRODUCTIONS];

TableEntry parsing_table[MAX_NON_TERMINALS][MAX_TERMINALS];

void create_parsing_table() {
  // Initialize table with empty entries
  for (int i = 0; i < MAX_NON_TERMINALS; i++) {
    for (int j = 0; j < MAX_TERMINALS; j++) {
      parsing_table[i][j].production_index = -1;
      parsing_table[i][j].production[0] = '\0';
    }
  }

  for (int i = 0; i < num_productions; i++) {
    char non_terminal = grammar[i].lhs;
    char *rhs = grammar[i].rhs;

    char first_set[MAX_TERMINALS];
    int first_set_size = 0;

    // Compute FIRST set of RHS
    for (int j = 0; rhs[j] != '\0'; j++) {
      if (islower(rhs[j]) || rhs[j] == '#' || rhs[j] == '$') {
        first_set[first_set_size++] = rhs[j];
        break;
      } else {
        const char *symbol_first = get_first_set(rhs[j]);
        for (int k = 0; symbol_first[k] != '\0'; k++) {
          if (symbol_first[k] != '#') {
            first_set[first_set_size++] = symbol_first[k];
          }
        }
        if (!has_epsilon(rhs[j])) {
          break;
        }
      }
    }

    // Add entries to parsing table
    for (int j = 0; j < first_set_size; j++) {
      if (first_set[j] != '#') {
        int row = non_terminal - 'A';
        int col = first_set[j] - 'a';
        parsing_table[row][col].production_index = i;
        strcpy(parsing_table[row][col].production, rhs);
      }
    }

    // If epsilon is in FIRST set, add entries for FOLLOW set
    if (has_epsilon(non_terminal)) {
      const char *follow_set = get_follow_set(non_terminal);
      for (int j = 0; follow_set[j] != '\0'; j++) {
        int row = non_terminal - 'A';
        int col = follow_set[j] - 'a';
        parsing_table[row][col].production_index = i;
        strcpy(parsing_table[row][col].production, "#");
      }
    }
  }
}

bool ll1_parse(const char *input) {
  char stack[MAX_STACK_SIZE];
  int top = 0;
  stack[top++] = '$';
  stack[top++] = grammar[0].lhs; // Start symbol

  int input_index = 0;

  while (top > 0) {
    char stack_top = stack[--top];
    char current_input = input[input_index];

    if (stack_top == current_input) {
      if (current_input == '$') {
        return true; // Successful parse
      }
      input_index++;
    } else if (islower(stack_top) || stack_top == '#' || stack_top == '$') {
      return false; // Mismatch
    } else {
      int row = stack_top - 'A';
      int col = current_input - 'a';
      if (parsing_table[row][col].production_index == -1) {
        return false; // No production found
      }
      char *production = parsing_table[row][col].production;
      if (production[0] != '#') {
        for (int i = strlen(production) - 1; i >= 0; i--) {
          stack[top++] = production[i];
        }
      }
    }
  }

  return false; // Input consumed but stack not empty
}
