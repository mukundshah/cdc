/**
 * @author  Mukund Shah
 * @note Lab 8: Implement Intermediate Code Generation for a given expression.
 * @file icg.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function prototypes
void findopr();
void explore();
void fleft(int);
void fright(int);

int i = 1, j = 0, no = 0, tmpch = 90;
char str[100], left[15], right[15];

// Struct to hold operator positions and their types
struct exp {
  int pos;
  char op;
} k[15];

// Function to find operators in the expression
void findopr() {
  // Loop through the string to find the ':' operator
  for (i = 0; str[i] != '\0'; i++)
    if (str[i] == ':') {
      k[j].pos = i;
      k[j++].op = ':';
    }

  // Loop through the string to find the '/' operator
  for (i = 0; str[i] != '\0'; i++)
    if (str[i] == '/') {
      k[j].pos = i;
      k[j++].op = '/';
    }

  // Loop through the string to find the '*' operator
  for (i = 0; str[i] != '\0'; i++)
    if (str[i] == '*') {
      k[j].pos = i;
      k[j++].op = '*';
    }

  // Loop through the string to find the '+' operator
  for (i = 0; str[i] != '\0'; i++)
    if (str[i] == '+') {
      k[j].pos = i;
      k[j++].op = '+';
    }

  // Loop through the string to find the '-' operator
  for (i = 0; str[i] != '\0'; i++)
    if (str[i] == '-') {
      k[j].pos = i;
      k[j++].op = '-';
    }
}

// Function to explore the expression and generate intermediate code
void explore() {
  i = 1;

  // Loop through all found operators
  while (k[i].op != '\0') {
    fleft(k[i].pos);  // Find the left operand
    fright(k[i].pos); // Find the right operand

    str[k[i].pos] = tmpch--; // Replace operator with a temporary variable

    printf("\t%c := %s%c%s\t\t", str[k[i].pos], left, k[i].op, right);
    printf("\n");

    i++;
  }

  fright(-1); // Process the remaining part of the expression

  if (no == 0) {
    fleft(strlen(str)); // Find the final left operand
    printf("\t%s := %s", right, left);
    exit(0);
  }

  printf("\t%s := %c", right, str[k[--i].pos]);
}

// Function to find the left operand of an operator
void fleft(int x) {
  int w = 0, flag = 0;

  x--;

  // Traverse left until another operator or end of string is encountered
  while (x != -1 && str[x] != '+' && str[x] != '*' && str[x] != '=' &&
         str[x] != '\0' && str[x] != '-' && str[x] != '/' && str[x] != ':') {
    if (str[x] != '$' && flag == 0) {
      left[w++] = str[x];
      left[w] = '\0';
      str[x] = '$';
      flag = 1;
    }
    x--;
  }
}

// Function to find the right operand of an operator
void fright(int x) {
  int w = 0, flag = 0;

  x++;

  // Traverse right until another operator or end of string is encountered
  while (x != -1 && str[x] != '+' && str[x] != '*' && str[x] != '\0' &&
         str[x] != '=' && str[x] != ':' && str[x] != '-' && str[x] != '/') {
    if (str[x] != '$' && flag == 0) {
      right[w++] = str[x];
      right[w] = '\0';
      str[x] = '$';
      flag = 1;
    }
    x++;
  }
}

void main() {
  printf("\t\tINTERMEDIATE CODE GENERATION\n\n");

  printf("Enter the Expression :");
  scanf("%s", str);

  printf("The intermediate code:\n");
  findopr(); // Find all operators in the expression
  explore(); // Generate intermediate code based on the operators found
}
