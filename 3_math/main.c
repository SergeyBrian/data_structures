#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define MAX_INPUT_LEN 3000
typedef enum {
    PLUS = (int) '+',
    MINUS = (int) '-',
    PROD = (int) '*',
    DIV = (int) '/',
    POW = (int) '^',
    FACT = (int) '!',
    BRACES_OPEN = (int) '(',
    BRACES_CLOSED = (int) ')',
    MOD = (int) '%',
    CONST = -1,
    VARIABLE = -2,
    UNARY_MINUS = 64
} operator;


// If operation is unary, only left value is used
struct operation {
    struct operation *left;
    struct operation *right;
    double value;
    operator op;
};

typedef struct operation operation;

struct operation_linked {
    struct operation_linked *previous;
    operation *op;
};

typedef struct operation_linked operation_linked;


int ctoi(char c) {
    return (int) (c - 48);
}

char itoc(int i) {
    return (char) (i + 48);
}

int operator_weight(operator op) {
    if (op == PLUS || op == MINUS) return 1;
    if (op == PROD || op == MOD || op == DIV) return 2;
    if (op == POW) return 3;
    if (op == FACT) return 4;
    if (op == UNARY_MINUS) return 5;
    if (op == BRACES_OPEN) return 6;
    return 0;
}

void incorrect_input_exit() {
    printf("Некорректный ввод\n");
    exit(1);
}

int should_replace(operator new, operator old) {
    if (old == BRACES_OPEN) return 0;
    return operator_weight(new) <= operator_weight(old);
}


int is_operator(char c) {
    return c == PLUS || c == MINUS || c == PROD || c == POW || c == FACT || c == DIV || c == BRACES_OPEN ||
           c == BRACES_CLOSED;
}

double fact(double x) {
    if (x == 0) return 1;
    return (x * fact(x - 1));
}

void free_operation(operation *op) {
//    if (op == NULL) return;
//    if (op->left != NULL) {
//        free_operation(op->left);
//        op->left = NULL;
//    }
//    if (op->right != NULL) {
//        free_operation(op->right);
//        op->right = NULL;
//    }
//    op = NULL;
}

void free_operations_list(operation_linked *last_operation) {
    if (last_operation->previous != NULL) free_operations_list(last_operation->previous);
    free_operation(last_operation->op);
    free(last_operation);
}

operation *queue_to_tree(char *queue[30], int queue_size) {
    operation *result = NULL;
    operation_linked *operation_list = (operation_linked *) malloc(sizeof(operation_linked));

    operation_linked *last_operation = operation_list;

    operation_list->previous = NULL;

    for (int i = 0; i < queue_size; i++) {
        if (isdigit(queue[i][0])) {
            continue;
        }

        operation *new_op = (operation *) malloc(sizeof(operation));
        result = new_op;
        last_operation->op = new_op;
        operator op = (operator) queue[i][0];

        switch (op) {
            case PLUS:
            case MINUS:
            case PROD:
            case DIV:
            case POW:
            case MOD: {
                operation *left = (operation *) malloc(sizeof(operation));
                operation *right = (operation *) malloc(sizeof(operation));
                new_op->left = left;
                new_op->right = right;

                int j = i;
                while (--j >= 0 && (queue[j] != NULL && !isdigit(queue[j][0])));

                if (j >= 0) {
                    if (queue[j] != NULL) {
                        right->value = atof(queue[j]);
                        right->op = CONST;

                        queue[j] = NULL;
                    } else {
                        if (last_operation->previous == NULL || last_operation->previous->op == NULL)
                            incorrect_input_exit();
                        new_op->right = last_operation->previous->op;
                        operation_linked *previous_ptr = last_operation->previous;
                        last_operation->previous = last_operation->previous->previous;
                    }

                    while (--j >= 0 && (queue[j] == NULL || !isdigit(queue[j][0])));

                    if (j >= 0 && queue[j] != NULL) {
                        left->value = atof(queue[j]);
                        left->op = CONST;
                        queue[j] = NULL;
                    } else {
                        if (last_operation->previous == NULL) incorrect_input_exit();
                        if (last_operation->previous->op == NULL) incorrect_input_exit();
                        free(left);
                        left = last_operation->previous->op;
                        operation_linked *previous_ptr = last_operation->previous;
                        last_operation->previous = last_operation->previous->previous;
                        free(previous_ptr);
                        new_op->left = right;
                        new_op->right = left;
                    }
                } else {
                    if (last_operation->previous == NULL || last_operation->previous->op == NULL ||
                        last_operation->previous->previous == NULL || last_operation->previous->previous->op == NULL)
                        incorrect_input_exit();
                    new_op->right = last_operation->previous->op;
                    new_op->left = last_operation->previous->previous->op;
                    operation_linked *previous_ptr = last_operation->previous;
                    last_operation->previous = last_operation->previous->previous->previous;
//                    free(previous_ptr->previous->previous);
//                    free(previous_ptr->previous);
                }

                break;
            }
            case UNARY_MINUS:
            case FACT: {
                operation *left = (operation *) malloc(sizeof(operation));
                new_op->left = left;
                new_op->right = NULL;
                int j = i;
                while (--j >= 0 && (queue[j] != NULL && !isdigit(queue[j][0])));

                if (j >= 0 && queue[j] != NULL) {
                    left->value = atof(queue[j]);
                    left->op = CONST;
                    left->left = NULL;
                    left->right = NULL;
                } else {
                    if (last_operation->previous == NULL) incorrect_input_exit();
                    if (last_operation->previous->op == NULL) incorrect_input_exit();
                    free(left);

                    left = last_operation->previous->op;
                    operation_linked *previous_ptr = last_operation->previous;
                    last_operation->previous = last_operation->previous->previous;
                    free(previous_ptr);

                    new_op->left = left;
                }

                new_op->op = op;

                if (j >= 0) queue[j] = NULL;

                break;
            }
            default:
                incorrect_input_exit();
                break;
        }

        new_op->op = op;

        operation_linked *last_operation_ptr = last_operation;
        last_operation = (operation_linked *) malloc(sizeof(operation_linked));
        last_operation->previous = last_operation_ptr;
    }

    free_operations_list(last_operation);
    return result;
}

double calculate(operation *op) {
    switch (op->op) {
        case PLUS:
            return calculate(op->left) + calculate(op->right);
        case MINUS:
            return calculate(op->left) - calculate(op->right);
        case PROD:
            return calculate(op->left) * calculate(op->right);
        case DIV:
            return calculate(op->left) / calculate(op->right);
        case POW:
            return pow(calculate(op->left), calculate(op->right));
        case FACT:
            return fact(calculate(op->left));
        case MOD:
            return (double) ((int) calculate(op->left) % (int) calculate(op->right));
        case CONST:
            return op->value;
        case UNARY_MINUS:
            return -(calculate(op->left));
    }
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
        output_queue[i] = (char *) calloc(sizeof(char), 101);
    }
    int queue_size = 0;

    char current_number[100] = "";
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
            if (input[i] == (operator) MINUS && (i == 0 || input[i - 1] == (operator) BRACES_OPEN)) {
                input[i] = (char) UNARY_MINUS;
            }
            while (operator_stack_size &&
                   (should_replace((operator) input[i], operator_stack[operator_stack_size - 1]))) {
                operator op = operator_stack[--operator_stack_size];
                if (op != BRACES_OPEN && op != BRACES_CLOSED)
                    output_queue[queue_size++][0] = (char) op;
            }

            if ((operator) input[i] != BRACES_CLOSED) operator_stack[operator_stack_size++] = (operator) input[i];
            else operator_stack_size--;
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
    operation *operation_tree = queue_to_tree(output_queue, queue_size);
    printf("\n%g\n", calculate(operation_tree));

    return 0;
}

