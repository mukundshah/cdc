/**
 * @author  Mukund Shah
 * @note Lab 1: Test whether or not the given string is a valid comment.
 * @file comment_identifier.c
 */

#include <stdio.h>
#include <string.h>

typedef enum {
  INITIAL,
  SLASH,
  SINGLE_LINE_COMMENT,
  MULTI_LINE_COMMENT_START,
  MULTI_LINE_COMMENT,
  MULTI_LINE_COMMENT_END,
  DOCUMENTATION_COMMENT_START,
  DOCUMENTATION_COMMENT,
  DOCUMENTATION_COMMENT_END,
  SINGLE_LINE_COMMENT_COMPLETE,
  MULTI_LINE_COMMENT_COMPLETE,
  DOCUMENTATION_COMMENT_COMPLETE,
  INVALID
} State;

State identify_comment(const char *str) {
  State state = INITIAL;
  int i = 0;

  while (str[i] != '\0') {
    char c = str[i];

    switch (state) {
    case INITIAL:
      if (c == '/')
        state = SLASH;
      else if (c != ' ' && c != '\t')
        return INVALID;
      break;
    case SLASH:
      if (c == '/')
        state = SINGLE_LINE_COMMENT;
      else if (c == '*')
        state = MULTI_LINE_COMMENT_START;
      else
        return INVALID;
      break;
    case SINGLE_LINE_COMMENT:
      if (c == '\n')
        return SINGLE_LINE_COMMENT_COMPLETE;
      break;
    case MULTI_LINE_COMMENT_START:
      if (c == '*')
        state = DOCUMENTATION_COMMENT_START;
      else
        state = MULTI_LINE_COMMENT;
      break;
    case MULTI_LINE_COMMENT:
      if (c == '*')
        state = MULTI_LINE_COMMENT_END;
      break;
    case MULTI_LINE_COMMENT_END:
      if (c == '/')
        return MULTI_LINE_COMMENT_COMPLETE;
      else if (c != '*')
        state = MULTI_LINE_COMMENT;
      break;
    case DOCUMENTATION_COMMENT_START:
      if (c == '*')
        state = DOCUMENTATION_COMMENT_END;
      else
        state = DOCUMENTATION_COMMENT;
      break;
    case DOCUMENTATION_COMMENT:
      if (c == '*')
        state = DOCUMENTATION_COMMENT_END;
      break;
    case DOCUMENTATION_COMMENT_END:
      if (c == '/')
        return DOCUMENTATION_COMMENT_COMPLETE;
      else if (c != '*')
        state = DOCUMENTATION_COMMENT;
      break;
    default:
      return INVALID;
    }
    i++;
  }

  if (state == SINGLE_LINE_COMMENT)
    return SINGLE_LINE_COMMENT_COMPLETE;
  return INVALID;
}

const char *state_to_string(State state) {
  switch (state) {
  case SINGLE_LINE_COMMENT_COMPLETE:
    return "Single Line Comment";
  case MULTI_LINE_COMMENT_COMPLETE:
    return "Multi Line Comment";
  case DOCUMENTATION_COMMENT_COMPLETE:
    return "Documentation Comment";
  default:
    return "Not a valid comment";
  }
}

int identify_comment_wrapper(const char *str) {
  return (int)identify_comment(str);
}

int main() {
  char str[100];

  printf("Enter a string: ");
  scanf("%[^\n]", str);

  State state = identify_comment(str);
  printf("%s is a %s\n", str, state_to_string(state));

  return 0;
}
