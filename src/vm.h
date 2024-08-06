#ifndef clox_vm_h
#define clox_vm_h

#include "chunk.h"
#include "value.h"

#define STACK_MAX 256

typedef struct {
    Chunk* chunk; // <-- Chunk di codice da eseguire.
    uint8_t* ip; // <-- "Instruction Pointer" indica l'istruzione corrente.

    Value stack[STACK_MAX]; // <-- Stack per tenere conto dei valori.
    Value* stackTop; // <-- Puntatore al primo elemento libero della stack. (Il Top)
} VM;

typedef enum {
    INTERPRET_OK,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR
} InterpretResult;

void initVM();
void freeVM();
InterpretResult interpret(Chunk* chunk);

void push(Value value);
Value pop();

#endif