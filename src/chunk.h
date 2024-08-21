#ifndef clox_chunk_h
#define clox_chunk_h

#include "common.h"
#include "value.h"

typedef enum {
    // Binary operations
    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_EQUAL,
    OP_GREATER,
    OP_LESS,

    // Special values.
    OP_NIL,
    OP_TRUE,
    OP_FALSE,

    // Boolean operation
    OP_NOT,

    OP_CONSTANT,
    OP_NEGATE,  // <-- Negazione.

    OP_POP, // <-- Poppa valore dallo stack dei valori.
    OP_PRINT, // <-- Stampa.
    OP_RETURN,  // <-- Istruzione per ritornare da una funzione.

    OP_GET_GLOBAL, // <-- Ottenere variabile globale.
    OP_SET_GLOBAL, // <-- Settare variabile globale.
    OP_GET_LOCAL, // <-- Ottenere variabile locale.
    OP_SET_LOCAL, // <-- Settare variabile locale.

    // Control Flow.
    OP_JUMP_IF_FALSE,
    OP_JUMP,    
    OP_LOOP,

    OP_DEFINE_GLOBAL // <-- Dichiara variabile globale.
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