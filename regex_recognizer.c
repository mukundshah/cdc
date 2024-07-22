/**
 * @author  Mukund Shah
 * @note Lab 2: Recognize strings under the patterns a*, a*b+, abb using a DFA.
 */

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define MAX_STATES 10
#define MAX_ALPHABET 26

typedef struct {
  int transitions[MAX_STATES][MAX_ALPHABET];
  bool is_final[MAX_STATES];
  int num_states;
} DFA;

void init_dfa(DFA *dfa) {
  memset(dfa->transitions, -1, sizeof(dfa->transitions));
  memset(dfa->is_final, false, sizeof(dfa->is_final));
  dfa->num_states = 0;
}

void add_transition(DFA *dfa, int from, int to, char symbol) {
  dfa->transitions[from][symbol - 'a'] = to;
  if (from >= dfa->num_states)
    dfa->num_states = from + 1;
  if (to >= dfa->num_states)
    dfa->num_states = to + 1;
}

void set_final(DFA *dfa, int state) { dfa->is_final[state] = true; }

bool run_dfa(DFA *dfa, const char *input) {
  int current_state = 0;
  for (int i = 0; input[i] != '\0'; i++) {
    if (input[i] < 'a' || input[i] > 'z')
      return false;
    current_state = dfa->transitions[current_state][input[i] - 'a'];
    if (current_state == -1)
      return false;
  }
  return dfa->is_final[current_state];
}

void setup_dfa_a_star(DFA *dfa) {
  add_transition(dfa, 0, 0, 'a');
  set_final(dfa, 0);
}

void setup_dfa_a_star_b_plus(DFA *dfa) {
  add_transition(dfa, 0, 0, 'a');
  add_transition(dfa, 0, 1, 'b');
  add_transition(dfa, 1, 1, 'b');
  set_final(dfa, 1);
}

void setup_dfa_abb(DFA *dfa) {
  add_transition(dfa, 0, 1, 'a');
  add_transition(dfa, 1, 2, 'b');
  add_transition(dfa, 2, 3, 'b');
  set_final(dfa, 3);
}

bool recognize_pattern(const char *input, const char *pattern) {
  DFA dfa;
  init_dfa(&dfa);

  if (strcmp(pattern, "a*") == 0) {
    setup_dfa_a_star(&dfa);
  } else if (strcmp(pattern, "a*b+") == 0) {
    setup_dfa_a_star_b_plus(&dfa);
  } else if (strcmp(pattern, "abb") == 0) {
    setup_dfa_abb(&dfa);
  } else {
    printf("Unsupported pattern: %s\n", pattern);
    return false;
  }

  return run_dfa(&dfa, input);
}

// Function to be exposed for the shared library
int recognize_pattern_wrapper(const char *input, const char *pattern) {
  return recognize_pattern(input, pattern) ? 1 : 0;
}
