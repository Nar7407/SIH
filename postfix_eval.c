#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX 100

int stack[MAX];
int top = -1;

void push(int val) {
    top++;
    stack[top] = val;
}

int pop() {
    int val = stack[top];
    top--;
    return val;
}

int power(int base, int exp) {
    int result = 1;
    for (int i = 0; i < exp; i++) {
        result = result * base;
    }
    return result;
}

int isOperator(char c) {
    if (c == '+' || c == '-' || c == '*' || c == '/' || c == '%' || c == '^')
        return 1;
    return 0;
}

int applyOp(int a, int b, char op) {
    switch (op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/': return a / b;
        case '%': return a % b;
        case '^': return power(a, b);
    }
    return 0;
}

int evaluate(char *exp) {
    for (int i = 0; exp[i] != '\0'; i++) {
        char c = exp[i];

        if (c == ' ' || c == '\n')
            continue;

        if (isdigit(c)) {
            push(c - '0');
        } else if (isOperator(c)) {
            if (top < 1) {
                printf("\nError: Invalid postfix expression!\n");
                exit(1);
            }
            int b = pop();
            int a = pop();
            if ((c == '/' || c == '%') && b == 0) {
                printf("\nError: Division by zero!\n");
                exit(1);
            }
            push(applyOp(a, b, c));
        } else {
            printf("\nError: Invalid character '%c'\n", c);
            exit(1);
        }
    }

    if (top != 0) {
        printf("\nError: Invalid postfix expression!\n");
        exit(1);
    }
    return pop();
}

int main() {
    char exp[MAX];

    printf("===== POSTFIX EXPRESSION EVALUATOR =====\n");
    printf("Operands are single digits (0-9)\n");
    printf("Example: 23*54*+9-\n\n");
    printf("Enter postfix expression: ");
    fgets(exp, sizeof(exp), stdin);

    printf("\nResult = %d\n", evaluate(exp));
    return 0;
}
