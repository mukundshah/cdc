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

char first_sets[MAX_SYMBOLS][MAX_SYMBOLS];
int first_set_size[MAX_SYMBOLS];

bool is_terminal(char symbol) {
  return islower(symbol) || symbol == '$' || symbol == '#' || !isalpha(symbol);
}

bool add_to_first_set(char symbol, char terminal) {
  int index = symbol - 'A';
  for (int i = 0; i < first_set_size[index]; i++) {
    if (first_sets[index][i] == terminal) {
      return false; // Already in the set
    }
  }
  first_sets[index][first_set_size[index]++] = terminal;
  return true;
}

bool has_epsilon(char symbol) {
  int index = symbol - 'A';
  for (int i = 0; i < first_set_size[index]; i++) {
    if (first_sets[index][i] == '#') {
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
      bool should_add_epsilon = true;

      while (rhs[j] != '\0' && should_add_epsilon) {
        if (is_terminal(rhs[j])) {
          changed |= add_to_first_set(lhs, rhs[j]);
          should_add_epsilon = false;
        } else { // Non-terminal
          int rhs_index = rhs[j] - 'A';
          for (int k = 0; k < first_set_size[rhs_index]; k++) {
            if (first_sets[rhs_index][k] != '#') {
              changed |= add_to_first_set(lhs, first_sets[rhs_index][k]);
            }
          }

          if (!has_epsilon(rhs[j])) {
            should_add_epsilon = false;
          }
        }
        j++;
      }

      if (should_add_epsilon) {
        changed |= add_to_first_set(lhs, '#');
      }
    }
  } while (changed);
}

void clear_grammar() {
  num_productions = 0;
  memset(first_sets, 0, sizeof(first_sets));
  memset(first_set_size, 0, sizeof(first_set_size));
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

const char *get_first_set(char symbol) {
  static char result[MAX_SYMBOLS * 3];
  int index = symbol - 'A';
  if (index < 0 || index >= MAX_SYMBOLS || first_set_size[index] == 0) {
    return "";
  }

  int pos = 0;
  for (int i = 0; i < first_set_size[index]; i++) {
    if (i > 0) {
      result[pos++] = ',';
    }
    if (first_sets[index][i] == '#') {
      result[pos++] = '#';
    } else if (!isalpha(first_sets[index][i])) {
      pos += sprintf(result + pos, "%d", first_sets[index][i]);
    } else {
      int j = 0;
      while (first_sets[index][i + j] != ',' &&
             first_sets[index][i + j] != '\0') {
        result[pos++] = first_sets[index][i + j];
        j++;
      }
      i += j - 1;
    }
  }
  result[pos] = '\0';
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
