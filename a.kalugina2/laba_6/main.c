﻿#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

typedef struct line_info {
    int indentation;
    int length;
} line_info;

typedef struct vector {
    line_info* data;
    int capacity;
    int count;
} vector;

int descriptor;
vector table;

vector vector_init() {
    vector v;
    v.data = (line_info*)malloc(sizeof(line_info));
    v.capacity = 1;
    v.count = 0;
    return v;
}

void vector_add(vector* v, line_info* data) {
    if (v != NULL) {
        if (v->capacity == v->count) {
            v->capacity *= 2;
            line_info* check = (line_info*)realloc(v->data, v->capacity * sizeof(line_info));
            if (check != NULL)
                v->data = check;
            else
                return;
        }
        v->data[v->count++] = *data;
    }
}

void vector_destroy(vector* v) {
    if (v != NULL)
        free(v->data);
}

void build_table() {
    table = vector_init();
    char symbol;
    int indentation = 0;
    int length = 0;
    while (1) {
        long f = read(descriptor, &symbol, 1);
        if (symbol == '\n' || f == 0) {
            line_info data = { indentation, length };
            vector_add(&table, &data);
            indentation += length + 1;
            length = 0;
            if (f == 0)
                break;
        }
        else
            length++;
    }
}

void print_line(long line) {
    lseek(descriptor, table.data[line].indentation, SEEK_SET);
    char str[table.data[line].length];
    read(descriptor, str, table.data[line].length);
    str[table.data[line].length] = 0;
    printf("%s\n", str);
}

void sig_handler(int _) {
    printf("\n5 seconds is left!\n");
    printf("The content of the file:\n");
    int line = 0;
    for (; line < table.count; line++)
        print_line(line);
    exit(0);
}

int main() {
    descriptor = open("input.txt", O_RDONLY);
    if (descriptor == -1) {
        perror("");
        exit(1);
    }
    build_table();
    signal(SIGALRM, sig_handler);
    while (1) {
        printf("Enter the number of a line: ");
        char digits[32];
        alarm(5);
        fgets(digits, 32, stdin);
        long number = strtol(digits, NULL, 10);
        if (number == 0)
            break;
        if (0 < number && number <= table.count)
            print_line(number - 1);
    }
    vector_destroy(&table);
    close(descriptor);
    return 0;
}