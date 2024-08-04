#ifndef clox_chunk_h
#define clox_chunk_h

#include "common.h"
#include "value.h"

typedef enum {
    OP_CONSTANT,
    OP_RETURN,  // <-- Istruzione per ritornare da una funzione.
} OpCode;

// Array dinamico per le istruzioni.
typedef struct {
    int count;
    int capacity;
    uint8_t* code; // <-- Visto che ogni istruzione in bytecode è lunga 1 byte.
    int* lines; // <-- Array parallelo alle istruzioni per tener conto del numero di linea.
    ValueArray constants; // <-- Array dinamico per i valori costanti.
} Chunk;

// Istruzioni fondamentali per gestire l'array dinamico.
void initChunk(Chunk* chunk);
void writeChunk(Chunk* chunk, uint8_t byte, int line);
void freeChunk(Chunk* chunk);
int addConstant(Chunk* chunk, Value value);

#endif