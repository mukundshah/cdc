/**
 * @author  Mukund Shah
 * @note Lab 5: Find the first and follow sets for a given grammar.
 * @file first_follow.c
 */

#include "first_follow.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int num_productions = 0;
Production grammar[MAX_PRODUCTIONS];

char first_sets[MAX_SYMBOLS][MAX_RHS_LENGTH * MAX_PRODUCTIONS];
int first_set_size[MAX_SYMBOLS];

char follow_sets[MAX_SYMBOLS][MAX_RHS_LENGTH * MAX_PRODUCTIONS];
int follow_set_size[MAX_SYMBOLS];

void clear_grammar() {
  num_productions = 0;
  memset(first_sets, 0, sizeof(first_sets));
  memset(first_set_size, 0, sizeof(first_set_size));
  memset(follow_sets, 0, sizeof(follow_sets));
  memset(follow_set_size, 0, sizeof(follow_set_size));
}

void add_production(char lhs, const char *rhs) {
  if (num_productions >= MAX_PRODUCTIONS) {
    fprintf(stderr, "Too many productions\n");
    exit(1);
  }
  grammar[num_productions].lhs = lhs;
  strcpy(grammar[num_productions].rhs, rhs);
  num_productions++;
}

bool has_epsilon(char symbol) {
  int index = symbol - 'A';
  for (int i = 0; i < first_set_size[index]; i++) {
    if (strcmp(&first_sets[index][i * MAX_RHS_LENGTH], "#") == 0) {
      return true;
    }
  }
  return false;
}

bool add_to_first_set(char symbol, const char *terminal) {
  int index = symbol - 'A';
  for (int i = 0; i < first_set_size[index]; i++) {
    if (strcmp(&first_sets[index][i * MAX_RHS_LENGTH], terminal) == 0) {
      return false; // Already in the set
    }
  }
  strcpy(&first_sets[index][first_set_size[index] * MAX_RHS_LENGTH], terminal);
  first_set_size[index]++;
  return true;
}

bool add_to_follow_set(char symbol, const char *terminal) {
  int index = symbol - 'A';
  for (int i = 0; i < follow_set_size[index]; i++) {
    if (strcmp(&follow_sets[index][i * MAX_RHS_LENGTH], terminal) == 0) {
      return false; // Already in the set
    }
  }
  strcpy(&follow_sets[index][follow_set_size[index] * MAX_RHS_LENGTH],
         terminal);
  follow_set_size[index]++;
  return true;
}

const char *get_first_set(char symbol) {
  static char result[MAX_RHS_LENGTH * MAX_PRODUCTIONS];
  int index = symbol - 'A';
  if (index < 0 || index >= MAX_SYMBOLS || first_set_size[index] == 0) {
    return "";
  }

  result[0] = '\0';
  for (int i = 0; i < first_set_size[index]; i++) {
    if (i > 0) {
      strcat(result, ",");
    }
    strcat(result, &first_sets[index][i * MAX_RHS_LENGTH]);
  }
  return result;
}

const char *get_follow_set(char symbol) {
  static char result[MAX_RHS_LENGTH * MAX_PRODUCTIONS];
  int index = symbol - 'A';
  if (index < 0 || index >= MAX_SYMBOLS || follow_set_size[index] == 0) {
    return "";
  }

  result[0] = '\0';
  for (int i = 0; i < follow_set_size[index]; i++) {
    if (i > 0) {
      strcat(result, ",");
    }
    strcat(result, &follow_sets[index][i * MAX_RHS_LENGTH]);
  }
  return result;
}

void compute_first_sets() {
  bool changed;
  do {
    changed = false;
    for (int i = 0; i < num_productions; i++) {
      char lhs = grammar[i].lhs;
      char *rhs = grammar[i].rhs;

      int j = 0;
      bool all_derive_epsilon = true;

      while (rhs[j] != '\0') {
        char terminal[MAX_TERMINAL_LENGTH] = {0};
        int k = 0;
        while (rhs[j] != '\0' &&
               (islower(rhs[j]) || rhs[j] == '$' || rhs[j] == '#')) {
          terminal[k++] = rhs[j++];
          if (k >= MAX_TERMINAL_LENGTH - 1)
            break;
        }
        terminal[k] = '\0';

        if (strlen(terminal) > 0) {
          changed |= add_to_first_set(lhs, terminal);
          all_derive_epsilon = false;
          break;
        } else { // Non-terminal
          char non_terminal[2] = {rhs[j], '\0'};
          int rhs_index = rhs[j] - 'A';
          bool epsilon_in_rhs = false;
          for (int k = 0; k < first_set_size[rhs_index]; k++) {
            if (strcmp(&first_sets[rhs_index][k * MAX_RHS_LENGTH], "#") != 0) {
              changed |= add_to_first_set(
                  lhs, &first_sets[rhs_index][k * MAX_RHS_LENGTH]);
            } else {
              epsilon_in_rhs = true;
            }
          }
          if (!epsilon_in_rhs) {
            all_derive_epsilon = false;
            break;
          }
          j++;
        }
      }

      if (all_derive_epsilon) {
        changed |= add_to_first_set(lhs, "#");
      }
    }
  } while (changed);
}

void compute_follow_sets() {
  // Add $ to FOLLOW(S), where S is the start symbol
  add_to_follow_set(grammar[0].lhs, "$");

  bool changed;
  do {
    changed = false;
    for (int i = 0; i < num_productions; i++) {
      char lhs = grammar[i].lhs;
      char *rhs = grammar[i].rhs;
      int len = strlen(rhs);

      for (int j = 0; j < len; j++) {
        if (!isupper(rhs[j])) {
          continue;
        }

        char B = rhs[j];

        if (j < len - 1) {
          bool all_derive_epsilon = true;
          for (int k = j + 1; k < len; k++) {
            if (isupper(rhs[k])) {
              const char *first_set = get_first_set(rhs[k]);
              char *token = strtok((char *)first_set, ",");
              while (token != NULL) {
                if (strcmp(token, "#") != 0) {
                  changed |= add_to_follow_set(B, token);
                }
                token = strtok(NULL, ",");
              }
              if (!has_epsilon(rhs[k])) {
                all_derive_epsilon = false;
                break;
              }
            } else {
              char terminal[2] = {rhs[k], '\0'};
              changed |= add_to_follow_set(B, terminal);
              all_derive_epsilon = false;
              break;
            }
          }

          if (all_derive_epsilon) {
            const char *follow_A = get_follow_set(lhs);
            char *token = strtok((char *)follow_A, ",");
            while (token != NULL) {
              changed |= add_to_follow_set(B, token);
              token = strtok(NULL, ",");
            }
          }
        } else {
          const char *follow_A = get_follow_set(lhs);
          char *token = strtok((char *)follow_A, ",");
          while (token != NULL) {
            changed |= add_to_follow_set(B, token);
            token = strtok(NULL, ",");
          }
        }
      }
    }
  } while (changed);
}

void print_first_sets() {
  for (int i = 0; i < MAX_SYMBOLS; i++) {
    if (first_set_size[i] > 0) {
      printf("FIRST(%c) = { ", i + 'A');
      for (int j = 0; j < first_set_size[i]; j++) {
        printf("%s", &first_sets[i][j * MAX_RHS_LENGTH]);
        if (j < first_set_size[i] - 1) {
          printf(", ");
        }
      }
      printf(" }\n");
    }
  }
}

void print_follow_sets() {
  for (int i = 0; i < MAX_SYMBOLS; i++) {
    if (follow_set_size[i] > 0) {
      printf("FOLLOW(%c) = { ", i + 'A');
      for (int j = 0; j < follow_set_size[i]; j++) {
        printf("%s", &follow_sets[i][j * MAX_RHS_LENGTH]);
        if (j < follow_set_size[i] - 1) {
          printf(", ");
        }
      }
      printf(" }\n");
    }
  }
}
