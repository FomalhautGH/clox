#ifndef clox_vm_h
#define clox_vm_h

#include "chunk.h"
#include "table.h"
#include "value.h"

#define STACK_MAX 256


typedef struct {
    Chunk* chunk; // <-- Chunk di codice da eseguire.
    uint8_t* ip; // <-- "Instruction Pointer" indica l'istruzione corrente.

    Value stack[STACK_MAX]; // <-- Stack per tenere conto dei valori.
    Value* stackTop; // <-- Puntatore al primo elemento libero della stack. (Il Top)

    Table strings;

    Obj* objects; // Linked list di tutti gli oggetti per poterli deallocare senza memory leaks.
} VM;

extern VM vm;

typedef enum {
    INTERPRET_OK,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR
} InterpretResult;

void initVM();
void freeVM();
InterpretResult interpret(const char* source);

void push(Value value);
Value pop();

#endif