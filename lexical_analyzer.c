/**
 * @author  Mukund Shah
 * @note Lab 4: Implement a lexical analyzer (for a subset of C).
 * @file lexical_analyzer.c
 */

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LEXEME_LEN 100
#define MAX_OUTPUT_LEN 10000

typedef enum {
  TOKEN_KEYWORD,
  TOKEN_IDENTIFIER,
  TOKEN_NUMBER,
  TOKEN_OPERATOR,
  TOKEN_UNKNOWN
} TokenType;

const char *keywords[] = {"int",  "float", "char", "if",
                          "else", "while", "for",  "return"};
const int num_keywords = sizeof(keywords) / sizeof(keywords[0]);

bool is_keyword(const char *str) {
  for (int i = 0; i < num_keywords; i++) {
    if (strcmp(str, keywords[i]) == 0) {
      return true;
    }
  }
  return false;
}

TokenType get_token_type(const char *lexeme) {
  if (is_keyword(lexeme)) {
    return TOKEN_KEYWORD;
  } else if (isalpha(lexeme[0]) || lexeme[0] == '_') {
    return TOKEN_IDENTIFIER;
  } else if (isdigit(lexeme[0])) {
    return TOKEN_NUMBER;
  } else if (strchr("+-*/=<>(){}[];", lexeme[0]) != NULL) {
    return TOKEN_OPERATOR;
  }
  return TOKEN_UNKNOWN;
}

const char *token_type_to_string(TokenType type) {
  switch (type) {
  case TOKEN_KEYWORD:
    return "KEYWORD";
  case TOKEN_IDENTIFIER:
    return "IDENTIFIER";
  case TOKEN_NUMBER:
    return "NUMBER";
  case TOKEN_OPERATOR:
    return "OPERATOR";
  default:
    return "UNKNOWN";
  }
}

char *lexical_analyzer(const char *input) {
  char *output = malloc(MAX_OUTPUT_LEN);
  if (output == NULL) {
    return NULL;
  }
  output[0] = '\0';

  char lexeme[MAX_LEXEME_LEN];
  int lexeme_index = 0;
  int input_len = strlen(input);

  for (int i = 0; i <= input_len; i++) {
    char c = input[i];

    if (isalnum(c) || c == '_' || (lexeme_index > 0 && c == '.')) {
      if (lexeme_index < MAX_LEXEME_LEN - 1) {
        lexeme[lexeme_index++] = c;
      }

      // Force token break for long identifiers
      if (lexeme_index == MAX_LEXEME_LEN - 1 || i == input_len - 1) {
        lexeme[lexeme_index] = '\0';
        TokenType type = get_token_type(lexeme);
        snprintf(output + strlen(output), MAX_OUTPUT_LEN - strlen(output),
                 "Token: %-10s Type: %s\n", lexeme, token_type_to_string(type));
        lexeme_index = 0;
      }
    } else {
      if (lexeme_index > 0) {
        lexeme[lexeme_index] = '\0';
        TokenType type = get_token_type(lexeme);
        snprintf(output + strlen(output), MAX_OUTPUT_LEN - strlen(output),
                 "Token: %-10s Type: %s\n", lexeme, token_type_to_string(type));
        lexeme_index = 0;
      }

      if (!isspace(c) && c != '\0') {
        lexeme[0] = c;
        lexeme[1] = '\0';
        TokenType type = get_token_type(lexeme);
        snprintf(output + strlen(output), MAX_OUTPUT_LEN - strlen(output),
                 "Token: %-10s Type: %s\n", lexeme, token_type_to_string(type));
      }
    }
  }

  return output;
}

// Function to be exposed for the shared library
const char *analyze_input(const char *input) { return lexical_analyzer(input); }

int main() {
  const char *input = "int main() { return 0; }";
  const char *output = analyze_input(input);
  printf("%s", output);
  free((void *)output);
  return 0;
}
