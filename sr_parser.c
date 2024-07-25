/**
 * @author  Mukund Shah
 * @note Lab 7: Implement a Shift-Reduce Parser for a given grammar.
 * @file sr_parser.c
 */

#include <stdio.h>
#include <string.h>

typedef struct {
  char lhs[10];
  char rhs[10];
} Production;

int main() {
  char input[20], stack[50], temp[50], ch[2], *token1, *token2, *substring;
  int i, j, stack_length, substring_length, stack_top, rule_count = 0;

  Production rules[10];

  stack[0] = '\0';

  // User input for the number of production rules
  printf("\nEnter the number of production rules: ");
  scanf("%d", &rule_count);

  // User input for each production rule in the form 'left->right'
  printf("\nEnter the production rules (in the form 'left->right'): \n");
  for (i = 0; i < rule_count; i++) {
    scanf("%s", temp);
    token1 = strtok(temp, "->");
    token2 = strtok(NULL, "->");
    strcpy(rules[i].lhs, token1);
    strcpy(rules[i].rhs, token2);
  }

  // User input for the input string
  printf("\nEnter the input string: ");
  scanf("%s", input);

  i = 0;
  while (1) {
    // If there are more characters in the input string, add the next character
    // to the stack
    if (i < strlen(input)) {
      ch[0] = input[i];
      ch[1] = '\0';
      i++;
      strcat(stack, ch);
      printf("%s\t", stack);
      for (int k = i; k < strlen(input); k++) {
        printf("%c", input[k]);
      }
      printf("\tShift %s\n", ch);
    }

    // Iterate through the production rules
    for (j = 0; j < rule_count; j++) {
      // Check if the right-hand side of the production rule matches a substring
      // in the stack
      substring = strstr(stack, rules[j].rhs);
      if (substring != NULL) {
        // Replace the matched substring with the left-hand side of the
        // production rule
        stack_length = strlen(stack);
        substring_length = strlen(substring);
        stack_top = stack_length - substring_length;
        stack[stack_top] = '\0';
        strcat(stack, rules[j].lhs);
        printf("%s\t", stack);
        for (int k = i; k < strlen(input); k++) {
          printf("%c", input[k]);
        }
        printf("\tReduce %s->%s\n", rules[j].lhs, rules[j].rhs);
        j = -1; // Restart the loop to ensure immediate reduction of the newly
                // derived production rule
      }
    }

    // Check if the stack contains only the start symbol and if the entire input
    // string has been processed
    if (strcmp(stack, rules[0].lhs) == 0 && i == strlen(input)) {
      printf("\nAccepted");
      break;
    }

    // Check if the entire input string has been processed but the stack doesn't
    // match the start symbol
    if (i == strlen(input)) {
      printf("\nNot Accepted");
      break;
    }
  }

  return 0;
}
