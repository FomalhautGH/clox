#include <stdio.h>

#include "common.h"
#include "debug.h"
#include "vm.h"

VM vm; 

static InterpretResult run() {
    #define READ_BYTE() (*vm.ip++) // <-- Macro che ritorna il puntatore alla istruzione corrente e lo consuma.
    #define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])

    // Macro per operazioni binarie, il do_while serve per fare in modo che gli statements
    // siano tutti nello stesso scope onde evitare problemi.
    // Si poteva potenzialmente usare uno scope vuoto ma poteva dare altri tipi di problemi.
    #define BINARY_OP(op) \
        do { \
            double b = pop(); \
            double a = pop(); \
            push(a op b); \
        } while (false) 

    for (;;) {
        // Debugging infos
        #ifdef DEBUG_TRACE_EXECUTION
            printf("          ");

            for (Value* slot = vm.stack; slot < vm.stackTop; slot++) {
                printf("[ ");
                printValue(*slot);
                printf(" ]");
            }

            printf("\n");
            disassembleInstruction(vm.chunk, (int)(vm.ip - vm.chunk->code));
        #endif

        uint8_t instruction;

        switch (instruction = READ_BYTE()) {
            case OP_RETURN: {
                printValue(pop());
                printf("\n");

                return INTERPRET_OK;
            }

            case OP_CONSTANT: {
                Value constant = READ_CONSTANT();

                push(constant);

                break;
            }

            case OP_NEGATE: push(-pop()); break;

            // Binary operations.
            case OP_ADD:      BINARY_OP(+); break;
            case OP_SUBTRACT: BINARY_OP(-); break;
            case OP_MULTIPLY: BINARY_OP(*); break;
            case OP_DIVIDE:   BINARY_OP(/); break;
        }
    }

    #undef READ_BYTE
    #undef BINARY_OP
    #undef READ_CONSTANT
}

static void resetStack() {
    vm.stackTop = vm.stack;
}

void initVM() {
    resetStack();
}

void freeVM() {

}

void push(Value value) {
    *vm.stackTop = value;
    vm.stackTop++;
}

Value pop() {
    vm.stackTop--;
    return *vm.stackTop;
}

InterpretResult interpret(Chunk* chunk) {
    vm.chunk = chunk;
    vm.ip = vm.chunk->code;

    return run();
}