#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define MAX_INPUT_LEN 3000
#define MAX_QUEUE_SIZE 1000
#define ALPHABET_LEN 2000

#ifdef USE_DOUBLE
#define num_type double
#define NUM_FORMAT "%g"
#else
#define num_type long long int
#define NUM_FORMAT "%lld"
#endif

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
    VARIABLE = 'x',
    CONST = -1,
    UNARY_MINUS = 64
} operator;


// If operation is unary, only left value is used
struct operation {
    struct operation *left;
    struct operation *right;
    num_type value;
    operator op;
};

typedef struct {
    char c;
    int value;
} variable;

variable variables[ALPHABET_LEN];

typedef struct operation operation;

struct operation_linked {
    struct operation_linked *previous;
    operation *op;
};

operation *operation_tree;

typedef struct operation_linked operation_linked;

int malloc_count;
int calloc_count;
int free_count;

void load_prf(char *input);

void load_pst(char *input);

void *mmalloc(size_t size) {
    malloc_count++;
    return malloc(size);
}

void *mcalloc(size_t size, int count) {
    calloc_count++;
    return calloc(size, count);
}

void mfree(void *ptr) {
    free_count++;
    free(ptr);
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
    if (old == BRACES_OPEN && new != BRACES_CLOSED) return 0;
    return operator_weight(new) <= operator_weight(old);
}


int is_operator(char c) {
    switch (c) {
        case PLUS:
        case MINUS:
        case PROD:
        case POW:
        case FACT:
        case DIV:
        case BRACES_OPEN:
        case BRACES_CLOSED:
        case MOD:
        case UNARY_MINUS:
            return 1;
        default:
            return 0;
    }
}

num_type fact(num_type x) {
    if (x == 0) return 1;
    return (x * fact(x - 1));
}

void free_operation(operation *op) {
    if (op == NULL) return;
    if (op->left != NULL) {
        free_operation(op->left);
        op->left = NULL;
    }
    if (op->right != NULL) {
        free_operation(op->right);
        op->right = NULL;
    }
    mfree(op);
    op = NULL;
}

void free_operations_list(operation_linked *last_operation) {
    if (last_operation->previous != NULL) free_operations_list(last_operation->previous);
    mfree(last_operation);
}

operation *queue_to_tree(char *queue[MAX_QUEUE_SIZE], int queue_size, int is_reversed) {
    if (is_reversed) {
        for (int i = 0; i < queue_size / 2; i++) {
            char *temp = queue[i];
            queue[i] = queue[queue_size - i - 1];
            queue[queue_size - i - 1] = temp;
        }
    }


    operation *result = NULL;

    if (queue_size == 1) {
        result = mmalloc(sizeof(operation));
        result->op = CONST;
        result->left = NULL;
        result->right = NULL;
        result->value = atof(queue[0]);
        return result;
    }

    operation_linked *operation_list = (operation_linked *) mmalloc(sizeof(operation_linked));

    operation_linked *last_operation = operation_list;

    operation_list->previous = NULL;

    for (int i = 0; i < queue_size; i++) {
        if (isdigit(queue[i][0]) || isalpha(queue[i][0])) {
            operation_linked *const_op = mmalloc(sizeof(operation_linked));
            const_op->op = mmalloc(sizeof(operation));
            const_op->op->op = CONST;
            const_op->op->left = NULL;
            const_op->op->right = NULL;
            const_op->op->value = atof(queue[i]);
            operation_linked *last = last_operation->previous;
            last_operation->previous = const_op;
            last_operation->previous->previous = last;
            queue[i] = NULL;
            continue;
        }

        operation *new_op = (operation *) mmalloc(sizeof(operation));
        result = new_op;
        operator op = (operator) queue[i][0];
        last_operation->op = new_op;

        switch (op) {
            case PLUS:
            case MINUS:
            case PROD:
            case DIV:
            case POW:
            case MOD: {
                new_op->left = (operation *) mmalloc(sizeof(operation));
                new_op->right = (operation *) mmalloc(sizeof(operation));

                int j = i;
                while (--j >= 0 && (queue[j] != NULL && !isalnum(queue[j][0])));

                int is_previous_null = queue[j] == NULL;

                if (j >= 0) {
                    if (queue[j] != NULL) {
                        if (isdigit(queue[j][0])) {
                            new_op->right->value = atof(queue[j]);
                            new_op->right->op = CONST;
                        } else {
                            new_op->right->value = queue[j][0];
                            new_op->right->op = VARIABLE;
                        }

                        queue[j] = NULL;
                    } else {
                        if (last_operation->previous == NULL || last_operation->previous->op == NULL)
                            incorrect_input_exit();
                        mfree(new_op->right);
                        new_op->right = last_operation->previous->op;
                        operation_linked *prev_operation_ptr = last_operation->previous;
                        last_operation->previous = last_operation->previous->previous;
                        mfree(prev_operation_ptr);
                    }

                    while (--j >= 0) {
                        if (is_previous_null) {
                            if (queue[j] != NULL && isalnum(queue[j][0])) break;
                        } else {
                            if (queue[j] == NULL || isalnum(queue[j][0])) break;
                        }
                    }

                    if (j >= 0 && queue[j] != NULL) {
                        if (isdigit(queue[j][0])) {
                            new_op->left->value = atof(queue[j]);
                            new_op->left->op = CONST;
                        } else {
                            new_op->left->value = queue[j][0];
                            new_op->left->op = VARIABLE;
                        }

                        queue[j] = NULL;
                    } else {
                        if (last_operation->previous == NULL) incorrect_input_exit();
                        if (last_operation->previous->op == NULL) incorrect_input_exit();
                        mfree(new_op->left);
                        if (is_previous_null) {
                            if (last_operation->previous == NULL) incorrect_input_exit();
                            new_op->left = last_operation->previous->op;
                            operation_linked *prev_operation_ptr = last_operation->previous;
                            last_operation->previous = last_operation->previous->previous;
                            mfree(prev_operation_ptr);
                        } else {
                            new_op->left = last_operation->previous->op;
                            operation_linked *prev_operation_ptr = last_operation->previous;
                            last_operation->previous = last_operation->previous->previous;
                            mfree(prev_operation_ptr);
                        }
                    }
                } else {
                    if (last_operation->previous == NULL || last_operation->previous->op == NULL ||
                        last_operation->previous->previous == NULL || last_operation->previous->previous->op == NULL)
                        incorrect_input_exit();
                    new_op->right = last_operation->previous->op;
                    new_op->left = last_operation->previous->previous->op;
                    operation_linked *previous_ptr = last_operation->previous;
                    last_operation->previous = last_operation->previous->previous->previous;
                    mfree(previous_ptr);
                }

                break;
            }
            case UNARY_MINUS:
            case FACT: {
                operation *left = (operation *) mmalloc(sizeof(operation));
                new_op->left = left;
                new_op->right = NULL;
                int j = i;
                while (--j >= 0 && (queue[j] != NULL && !isalnum(queue[j][0])));

                if (j >= 0 && queue[j] != NULL) {
                    if (isdigit(queue[j][0])) {
                        left->value = atof(queue[j]);
                        left->op = CONST;
                        left->left = NULL;
                        left->right = NULL;
                    } else {
                        left->value = queue[j][0];
                        left->op = VARIABLE;
                    }
                } else {
                    if (last_operation->previous == NULL) incorrect_input_exit();
                    if (last_operation->previous->op == NULL) incorrect_input_exit();
                    mfree(left);

                    left = last_operation->previous->op;
                    operation_linked *prev_ptr = last_operation->previous;
                    last_operation->previous = last_operation->previous->previous;
                    mfree(prev_ptr);

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
        last_operation = (operation_linked *) mmalloc(sizeof(operation_linked));
        last_operation->previous = last_operation_ptr;
    }

    free_operations_list(last_operation);
    return result;
}

void parse(char **output_queue, char *input, int *queue_size) {
    int input_len = strlen(input) + 1;
    operator operator_stack[20];
    int operator_stack_size = 0;
    for (int i = 0; i < MAX_QUEUE_SIZE; i++) {
        output_queue[i] = (char *) mcalloc(sizeof(char), 101);
    }
    *queue_size = 0;

    char current_number[100] = "";
    int current_number_size = 0;

    for (int i = 0; i < input_len; i++) {
        if (isdigit(input[i]) || input[i] == '.' || isalpha(input[i])) {
            current_number[current_number_size++] = input[i];
            current_number[current_number_size] = '\0';
            continue;
        }

        if (current_number_size) {
            strcpy(output_queue[(*queue_size)++], current_number);
            current_number_size = 0;
        }

        if (input[i] == ' ') continue;

        if (is_operator(input[i])) {
            if (input[i] == (operator) MINUS && (i == 0 || input[i - 1] == (operator) BRACES_OPEN)) {
                input[i] = (char) UNARY_MINUS;
            }
            while (operator_stack_size &&
                   (should_replace((operator) input[i], operator_stack[operator_stack_size - 1]))) {
                operator_stack[operator_stack_size] = 0;
                operator op = operator_stack[--operator_stack_size];
                if (op != BRACES_OPEN && op != BRACES_CLOSED)
                    output_queue[(*queue_size)++][0] = (char) op;
                else break;
            }

            if ((operator) input[i] != BRACES_CLOSED) operator_stack[operator_stack_size++] = (operator) input[i];
        }

    }

    if (operator_stack_size) {
        while (operator_stack_size) {
            operator op = operator_stack[--operator_stack_size];
            if (op != BRACES_OPEN && op != BRACES_CLOSED)
                output_queue[(*queue_size)++][0] = (char) op;
        }
    }
}

void zero_division_error() {
    printf("Zero division error!\n");
    exit(0);
}

num_type calculate(operation *op) {
    switch (op->op) {
        case PLUS:
            return calculate(op->left) + calculate(op->right);
        case MINUS:
            return calculate(op->left) - calculate(op->right);
        case PROD:
            return calculate(op->left) * calculate(op->right);
        case DIV: {
            num_type denominator = calculate(op->left);
            num_type numerator = calculate(op->right);
            if (numerator == 0) zero_division_error();
            return denominator / numerator;
        }
        case POW:
            return pow(calculate(op->left), calculate(op->right));
        case FACT:
            return fact(calculate(op->left));
        case MOD: {
            int denominator = (int) calculate(op->left);
            int numerator = (int) calculate(op->right);
            if (numerator == 0) zero_division_error();
            return denominator % numerator;
        }
        case CONST:
            return op->value;
        case UNARY_MINUS:
            return -(calculate(op->left));
        case VARIABLE:
            return (num_type) variables[(int) op->value].value;
    }
}

void print_prf(operation *op) {
    switch (op->op) {
        case VARIABLE:
            printf("%c ", (int) op->value);
            break;
        case CONST:
            printf(NUM_FORMAT" ", op->value);
            break;
        default:
            printf("%c ", op->op);
            break;
    }
    if (op->left != NULL) print_prf(op->left);
    if (op->right != NULL) print_prf(op->right);
}

void print_pst(operation *op) {
    if (op->left != NULL) print_pst(op->left);
    if (op->right != NULL) print_pst(op->right);
    switch (op->op) {
        case VARIABLE:
            printf("%c ", (int) op->value);
            break;
        case CONST:
            printf(NUM_FORMAT" ", op->value);
            break;
        default:
            printf("%c ", op->op);
            break;
    }
}

void validate_input(char *input) {
    int input_len = strlen(input) + 1;
    int operators_count = 0;
    int space_count = 0;

    for (int i = 0; i < input_len; i++) {
        if (input[i] == '\n') {
            input[i] = '\0';
            break;
        }
        if (input[i] == '\0') break;
        if (!isdigit(input[i]) && !isalpha(input[i])) {
            if (!is_operator(input[i]) && input[i] != ' ' && input[i] != '.') {
                incorrect_input_exit();
            }
            if (input[i] == ' ') {
                space_count++;
                continue;
            }
            operators_count++;
        }
    }
}

void get_commands(char *filename) {
    FILE *file = fopen(filename, "r");

    char buf[MAX_INPUT_LEN];
    while (fgets(buf, MAX_INPUT_LEN, file) != NULL) {
        char *input = strchr(buf, ' ');
        if (input) {
            input[0] = '\0';
            input++;
        }
        char *command = buf;
        if (command[strlen(command) - 1] == '\n') command[strlen(command) - 1] = '\0';
        if (strcmp(command, "parse") == 0) {
            validate_input(input);


            char *output_queue[MAX_QUEUE_SIZE];
            int queue_size;

            parse(output_queue, input, &queue_size);
            operation_tree = queue_to_tree(output_queue, queue_size, 0);

            for (int i = 0; i < MAX_QUEUE_SIZE; i++) {
                mfree(output_queue[i]);
            }
        } else if (strcmp(command, "save_prf") == 0) {
            printf("\n");
            if (!operation_tree) {
                printf("not_loaded\n");
                exit(1);
            }
            print_prf(operation_tree);
            printf("\n");
        } else if (strcmp(command, "save_pst") == 0) {
            printf("\n");
            if (!operation_tree) {
                printf("not_loaded\n");
                exit(1);
            }
            print_pst(operation_tree);
            printf("\n");
        } else if (strcmp(command, "eval") == 0) {
            if (input) {
                char *token = strtok(input, ",");

                while (token != NULL) {
                    char var_name = token[0];
                    int value = atoi(token + 2);

                    variables[var_name].value = value;
                    token = strtok(NULL, ",");
                }
            }
            printf("\n"NUM_FORMAT"\n", calculate(operation_tree));
        } else if (strcmp(command, "load_prf") == 0) {
            load_prf(input);
        } else if (strcmp(command, "load_pst") == 0) {
            load_pst(input);
        }
    }
}

void simple_parse(char **output_queue, char *input, int *queue_size) {
    int input_len = strlen(input) + 1;
    operator operator_stack[20];
    int operator_stack_size = 0;
    for (int i = 0; i < MAX_QUEUE_SIZE; i++) {
        output_queue[i] = (char *) mcalloc(sizeof(char), 101);
    }

    *queue_size = 0;

    char current_number[100] = "";
    int current_number_size = 0;


    for (int i = 0; i < input_len; i++) {
        if (isdigit(input[i]) || input[i] == '.' || isalpha(input[i])) {
            current_number[current_number_size++] = input[i];
            current_number[current_number_size] = '\0';
            continue;
        }

        if (current_number_size) {
            strcpy(output_queue[(*queue_size)++], current_number);
            current_number_size = 0;
        }

        if (input[i] == ' ') continue;

        if (is_operator(input[i])) {
            output_queue[(*queue_size)++][0] = input[i];
        }

        if ((operator) input[i] != BRACES_CLOSED) operator_stack[operator_stack_size++] = (operator) input[i];
        else operator_stack_size--;
    }
}

void load_prf(char *input) {
    validate_input(input);

    char *output_queue[MAX_QUEUE_SIZE];
    int queue_size;

    simple_parse(output_queue, input, &queue_size);
    operation_tree = queue_to_tree(output_queue, queue_size, 11);

    for (int i = 0; i < MAX_QUEUE_SIZE; i++) {
        mfree(output_queue[i]);
    }
}

void load_pst(char *input) {
    validate_input(input);

    char *output_queue[MAX_QUEUE_SIZE];
    int queue_size;

    simple_parse(output_queue, input, &queue_size);
    operation_tree = queue_to_tree(output_queue, queue_size, 0);

    for (int i = 0; i < MAX_QUEUE_SIZE; i++) {
        mfree(output_queue[i]);
    }
}

int main() {
    malloc_count = 0;
    calloc_count = 0;
    free_count = 0;

    get_commands("../input.txt");

//    free_operation(operation_tree);

    printf("\n---------------------------\n");
    printf("malloc: %d\ncalloc: %d\nfree: %d\nmalloc+calloc-free: %d\n", malloc_count, calloc_count, free_count,
           malloc_count + calloc_count - free_count);

    return 0;
}

