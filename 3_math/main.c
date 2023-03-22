#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define MAX_INPUT_LEN 3000
typedef enum {
    PLUS = 43,
    MINUS = 45,
    PROD = 42,
    POW = 94,
    FACT = 33,
    BRACES_OPEN = 40,
    BRACES_CLOSED = 41,
    MOD = 37
} operator;


int ctoi(char c) {
    return (int) (c - 48);
}

char itoc(int i) {
    return (char) (i + 48);
}

int operator_weight(operator op) {
    if (op == PLUS || op == MINUS) return 1;
    if (op == PROD || op == MOD) return 2;
    if (op == POW) return 3;
    if (op == FACT) return 4;
    if (op == BRACES_OPEN) return 5;
    return 0;
}

void incorrect_input_exit() {
    printf("Некорректный ввод\n");
    exit(1);
}

int should_replace(operator new, operator old) {
    if (old == BRACES_OPEN) return 0;
    if (operator_weight(new) == operator_weight(old)) {
        return 1;
    }
    return (operator_weight(new) < operator_weight(old));
}


int is_operator(char c) {
    return c == PLUS || c == MINUS || c == PROD || c == POW || c == FACT || c == BRACES_OPEN || c == BRACES_CLOSED;
}


int main() {
    char input[MAX_INPUT_LEN * 100 + 1];

    fgets(input, 10000, stdin);

    int input_len = strlen(input) + 1;
    int operators_count = 0;
    int space_count = 0;

    for (int i = 0; i < input_len; i++) {
        if (input[i] == '\n') {
            input[i] = '\0';
            break;
        }
        if (input[i] == '\0') break;
        if (!isdigit(input[i])) {
            if (!is_operator(input[i]) && input[i] != ' ') {
                incorrect_input_exit();
            }
            if (input[i] == ' ') {
                space_count++;
                continue;
            }
            operators_count++;
        }
    }

    operator operator_stack[20];
    int operator_stack_size = 0;

    char *output_queue[30];
    for (int i = 0; i < 30; i++) {
        output_queue[i] = (char *) malloc(sizeof(char) * 101);
    }
    int queue_size = 0;

    char current_number[100] = {};
    int current_number_size = 0;

    for (int i = 0; i < input_len; i++) {
        if (isdigit(input[i])) {
            current_number[current_number_size++] = input[i];
            current_number[current_number_size] = '\0';
            continue;
        }

        if (current_number_size) {
            strcpy(output_queue[queue_size++], current_number);
            current_number_size = 0;
        }

        if (input[i] == ' ') continue;

        if (is_operator(input[i])) {
            while (operator_stack_size && (should_replace((operator) input[i], operator_stack[operator_stack_size - 1]))) {
                operator op = operator_stack[--operator_stack_size];
                if (op != BRACES_OPEN && op != BRACES_CLOSED)
                    output_queue[queue_size++][0] = (char) op;
            }

            operator_stack[operator_stack_size++] = (operator) input[i];
        }

    }

    if (operator_stack_size) {
        while (operator_stack_size) {
            operator op = operator_stack[--operator_stack_size];
            if (op != BRACES_OPEN && op != BRACES_CLOSED)
                output_queue[queue_size++][0] = (char) op;
        }
    }

    for (int i = 0; i < queue_size; i++) {
        printf("%s ", output_queue[i]);
    }

    return 0;
}

