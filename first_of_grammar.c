#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PRODUCTIONS 100
#define MAX_SYMBOLS 26
#define MAX_RHS_LENGTH 20

typedef struct {
  char lhs;
  char rhs[MAX_RHS_LENGTH];
} Production;

Production grammar[MAX_PRODUCTIONS];
int num_productions = 0;

char first_sets[MAX_SYMBOLS][MAX_SYMBOLS * MAX_RHS_LENGTH];
int first_set_size[MAX_SYMBOLS];

bool is_terminal(const char *symbol) {
  return islower(symbol[0]) || symbol[0] == '$' || symbol[0] == '#' ||
         !isalpha(symbol[0]);
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

bool has_epsilon(char symbol) {
  int index = symbol - 'A';
  for (int i = 0; i < first_set_size[index]; i++) {
    if (strcmp(&first_sets[index][i * MAX_RHS_LENGTH], "#") == 0) {
      return true;
    }
  }
  return false;
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
        if (is_terminal(&rhs[j])) {
          char terminal[MAX_RHS_LENGTH] = {0};
          int k = 0;
          while (rhs[j + k] != '\0' && !isupper(rhs[j + k])) {
            terminal[k] = rhs[j + k];
            k++;
          }
          changed |= add_to_first_set(lhs, terminal);
          all_derive_epsilon = false;
          break;
        } else { // Non-terminal
          int rhs_index = rhs[j] - 'A';
          bool added_non_epsilon = false;
          for (int k = 0; k < first_set_size[rhs_index]; k++) {
            if (strcmp(&first_sets[rhs_index][k * MAX_RHS_LENGTH], "#") != 0) {
              changed |= add_to_first_set(
                  lhs, &first_sets[rhs_index][k * MAX_RHS_LENGTH]);
              added_non_epsilon = true;
            }
          }
          if (!has_epsilon(rhs[j])) {
            all_derive_epsilon = false;
            break;
          }
          if (!added_non_epsilon) {
            j++; // Move to the next symbol only if we didn't add any
                 // non-epsilon
          } else {
            break; // If we added non-epsilon, we're done with this production
          }
        }
      }

      if (all_derive_epsilon && rhs[0] != '\0') {
        changed |= add_to_first_set(lhs, "#");
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

void add_production(char lhs, const char *rhs) {
  if (num_productions >= MAX_PRODUCTIONS) {
    fprintf(stderr, "Too many productions\n");
    exit(1);
  }
  grammar[num_productions].lhs = lhs;
  strcpy(grammar[num_productions].rhs, rhs);
  num_productions++;
}

void clear_grammar() {
  num_productions = 0;
  memset(first_sets, 0, sizeof(first_sets));
  memset(first_set_size, 0, sizeof(first_set_size));
}

const char *get_first_set(char symbol) {
  static char result[MAX_SYMBOLS * MAX_RHS_LENGTH];
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

int main() {
  // Example grammar
  add_production('S', "ABC");
  add_production('A', "aA");
  add_production('A', "#");
  add_production('B', "bB");
  add_production('B', "#");
  add_production('C', "c");

  compute_first_sets();
  print_first_sets();

  return 0;
}
