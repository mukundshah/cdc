/**
 * @author  Mukund Shah
 * @note Lab 3: Test whether or not the given string is a valid identifier.
 * @file identifier_checker.c
 */

#include <stdbool.h>
#include <stdio.h>

typedef enum { START, VALID, INVALID } State;

State transition(State current_state, char c) {
  switch (current_state) {
  case START:
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_')
      return VALID;
    else
      return INVALID;
  case VALID:
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
        (c >= '0' && c <= '9') || c == '_')
      return VALID;
    else
      return INVALID;
  case INVALID:
    return INVALID;
  }
  return INVALID;
}

bool is_valid_identifier(const char *str) {
  State state = START;

  for (int i = 0; str[i] != '\0'; i++) {
    state = transition(state, str[i]);
    if (state == INVALID)
      return false;
  }

  return state == VALID;
}

// Function to be exposed for the shared library
int check_identifier(const char *str) {
  return is_valid_identifier(str) ? 1 : 0;
}

int main() {
  printf("%d\n", check_identifier("abc"));  // 1
  printf("%d\n", check_identifier("123"));  // 0
  printf("%d\n", check_identifier("_abc")); // 1
  printf("%d\n", check_identifier("a b"));  // 0
  return 0;
}
