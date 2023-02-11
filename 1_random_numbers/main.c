#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define INPUT_FILE_NAME "input.txt"
#define OUTPUT_FILE_NAME "output.txt"

#define COMMAND_GET_C "get_c"
#define COMMAND_GET_A "get_a"
#define COMMAND_LGC "lgc"
#define COMMAND_TEST "test"

#define PARAM_CMAX "cmax"
#define PARAM_CMIN "cmin"
#define PARAM_M "m"
#define PARAM_A "a"
#define PARAM_X "x0"
#define PARAM_C "c"
#define PARAM_N "n"
#define PARAM_INP "inp"

FILE *input_file;
FILE *output_file;
FILE *input_file_test;

int is_num(const char *buff);

typedef enum {
    INVALID_PARAM,
    CMAX,
    CMIN,
    M,
    A,
    X0,
    C,
    N,
    INP
} param;

typedef enum {
    INVALID_COMMAND,
    GET_C,
    GET_A,
    LGC,
    TEST
} command;

void error_exit(char *message, int write_to_file) {
    if (write_to_file) {
        fprintf(output_file, "%s", message);
    } else {

        printf("\n%s\n", message);
    }
    exit(1);
}

param str_to_param(char *str) {
    if (strcmp(str, PARAM_A) == 0) return A;
    if (strcmp(str, PARAM_C) == 0) return C;
    if (strcmp(str, PARAM_CMAX) == 0) return CMAX;
    if (strcmp(str, PARAM_CMIN) == 0) return CMIN;
    if (strcmp(str, PARAM_M) == 0) return M;
    if (strcmp(str, PARAM_N) == 0) return N;
    if (strcmp(str, PARAM_X) == 0) return X0;
    if (strcmp(str, PARAM_INP) == 0) return INP;
    return INVALID_PARAM;
}

command str_to_command(char *str) {
    if (strcmp(str, COMMAND_GET_C) == 0) return GET_C;
    if (strcmp(str, COMMAND_GET_A) == 0) return GET_A;
    if (strcmp(str, COMMAND_LGC) == 0) return LGC;
    if (strcmp(str, COMMAND_TEST) == 0) return TEST;
    return INVALID_COMMAND;
}

int is_num(const char *buff) {
    for (int i = 0; buff[i] != '\0'; i++) {
        if (!isdigit(buff[i])) return 0;
    }
    return 1;
}

void parse_command(char *src, unsigned long *parsed_command) {
    char comm[5] = {0};
    int i = 0;

    while (src[i] != ' ') {
        comm[i] = src[i];
        i++;
    }

    parsed_command[0] = str_to_command(comm);
    if (!parsed_command[0]) error_exit("incorrect command", 1);

    char *buff = (char *) malloc(sizeof(char) * strlen(src));

    param current_param;

    for (int k = 0; src[i - 1] != '\0'; i++) {
        if (isalnum(src[i]) || src[i] == '.') {
            buff[k++] = src[i];
            continue;
        }
        if (src[i] == '=') {
            current_param = str_to_param(buff);
            if (!current_param) error_exit("incorrect command", 1);
            memset(buff, 0, sizeof(char) * k);
            k = 0;
            continue;
        }
        if ((src[i] == ' ' || src[i] == '\0')) {
            if (!strlen(buff)) continue;
            if (current_param == INP) {
                input_file_test = fopen(buff, "r");
                if (!input_file_test) error_exit("Can't open test file", 0);
            } else {
                if (!is_num(buff)) error_exit("incorrect command", 1);
                parsed_command[current_param] = atoll(buff);
            }

            memset(buff, 0, sizeof(char) * k);
            k = 0;
        }
    }

    free(buff);
}


char *readline() {
    int size = 100;
    int c;
    int c_count = 0;

    char *buff = (char *) malloc(sizeof(char) * size);

    while ((c = getc(input_file)) != EOF && c != '\n') {
        if (c_count >= size - 1) {
            size *= 2;
            buff = realloc(buff, sizeof(char) * size);
        }
        buff[c_count++] = (char) c;
    }

    buff[c_count] = '\0';

    return buff;
}

int is_coprime(unsigned long a, unsigned long b) {
    while (b != 0) {
        unsigned long t = b;
        b = a % b;
        a = t;
    }

    return a == 1;
}

void get_c(const unsigned long parameters[6]) {
    unsigned long cmin = parameters[CMIN];
    unsigned long cmax = parameters[CMAX];
    unsigned long m = parameters[M];

    for (unsigned long c = cmin; c < cmax; c++) {
        if (is_coprime(c, m)) {
            fprintf(output_file, "%lu ", c);
        }
    }
}

int is_prime(unsigned long n) {
    for (int i = 2; i < sqrt(n) + 1; i++) {
        if (n % i == 0 && i != n) return 0;
    }

    return 1;
}

int check_a(unsigned long a, unsigned long m) {
    if (m % 4 == 0 && a % 4 != 0) return 0;
    for (int i = 2; i < sqrt(m) + 1 && i < sqrt(a); i++) {
        if (is_prime(i)) {
            if (a % i != 0) return 0;
        }
        if (is_prime(m / i)) {
            if (a % (m / i) != 0) return 0;
        }
    }
    return 1;
}

void get_a(const unsigned long parameters[6]) {
    unsigned long m = parameters[M];

    for (unsigned long a = 0; a < m; a++) {
        if (check_a(a, m)) {
            fprintf(output_file, "%lu ", a + 1);
        }
    }
}

void lgc(const unsigned long parameters[8]) {
    unsigned long a = parameters[A];
    unsigned long x0 = parameters[X0];
    unsigned long c = parameters[C];
    unsigned long m = parameters[M];
    unsigned long n = parameters[N];

    if (n < 1 || a >= m || c >= m || x0 >= m) {
        error_exit("no solutions", 1);
    }

    unsigned long x = x0;
    for (int i = 0; i < n; i++) {
        x = (a * x + c) % m;
        fprintf(output_file, "%lu ", x);
    }
}

int main() {
    input_file = fopen(INPUT_FILE_NAME, "r");
    output_file = fopen(OUTPUT_FILE_NAME, "w");

    if (input_file == NULL) {
        error_exit("Input file not found", 0);
    }

    if (output_file == NULL) {
        error_exit("Can't open output file", 0);
    }

    char *line = readline();

    unsigned long parsed_command[8];
    parse_command(line, parsed_command);

    switch (parsed_command[0]) {
        case GET_C:
            get_c(parsed_command);
            break;
        case GET_A:
            get_a(parsed_command);
            break;
        case LGC:
            lgc(parsed_command);
            break;
        case TEST:
            break;
    }

    free(line);
    fclose(input_file);
    fclose(output_file);
    if (input_file_test != NULL) fclose(input_file_test);
    return 0;
}
