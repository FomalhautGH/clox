#include <stdlib.h>
#include <string.h>

#include "memory.h"
#include "object.h"
#include "table.h"
#include "value.h"

#define TABLE_MAX_LOAD 0.75

static Entry* findEntry(Entry* entries, int capacity, ObjString* key) {
    uint32_t index = key->hash % capacity;
    Entry* tombstone = NULL;

    for (;;) {
        Entry* entry = &entries[index];

        if (entry->key == NULL) {
            if (IS_NIL(entry->value)) {
                // Empty entry.
                // In questo modo se ci stava una tombstone ritorniamo quella
                // per inserire ad esempio.
                return tombstone != NULL ? tombstone : entry;
            } else {
                // We found a tombstone.
                if (tombstone == NULL) tombstone = entry;
            }
        } else if (entry->key == key) {
            // We found the key.
            return entry;
        }

        index = (index + 1) % capacity;
    }
}

// Non si può usare il realloc perchè visto che scegliamo gli slots
// delle entries con i moduli se la capacity cambia anche il modulo cambia
// e se il modulo cambia ci saranno delle collissioni.
// Viene più comodo ricostruire la table dall'inizio.
static void adjustCapacity(Table* table, int capacity) {
    Entry* entries = ALLOCATE(Entry, capacity);
    for (int i = 0; i < capacity; ++i) {
        entries[i].key = NULL;
        entries[i].value = NIL_VAL;
    }

    // Ricalcoliamo il count per non tenere conto delle tombstones.
    table->count = 0;
    for (int i = 0; i < table->capacity; ++i) {
        Entry* entry = &table->entries[i];
        if (entry->key == NULL) continue;

        Entry* dest = findEntry(entries, capacity, entry->key);
        dest->key = entry->key;
        dest->value = entry->value;
        table->count++;
    }

    FREE_ARRAY(Entry, table->entries, table->capacity);

    table->entries = entries;
    table->capacity = capacity;
}

void initTable(Table* table) {
    table->count = 0;
    table->capacity = 0;
    table->entries = NULL;
}

// Aggiunge la coppia key & value alla HashTable e se viene già trovata la sovrascrive.
// Ritorna true se è stata aggiunta e false se è stata sovrascritta.
bool tableSet(Table* table, ObjString* key, Value value) {
    // Non aumentiamo l'array quando è completamente piendo ma in percentuale a TABLE_MAX_LOAD.
    if (table->count + 1 > table->capacity * TABLE_MAX_LOAD) {
        int capacity = GROW_CAPACITY(table->capacity);
        adjustCapacity(table, capacity);
    }

    Entry* entry = findEntry(table->entries, table->capacity, key);
    bool isNewKey = entry->key == NULL;
    if (isNewKey) table->count++;

    entry->key = key;
    entry->value = value;
    return isNewKey;
}

// Dalla key ottiene il value, se l'elemento con quella key non c'è ritorna false
// altrimenti ritorna true e il valore lo mette nel puntatore.
bool tableGet(Table* table, ObjString* key, Value* value) {
    if (table->count == 0) return false;

    Entry* entry = findEntry(table->entries, table->capacity, key);
    if (entry->key == NULL) return false;

    *value = entry->value;
    return true;
}

// Rimuove elementi dalla HashTable tramite la key utlizzando la tecnica delle tombstone.
// Ritorna true se l'ha eliminata altrimenti false.
bool tableDelete(Table* table, ObjString* key) {
    if (table->count == 0) return false;

    // Find the entry.
    Entry* entry = findEntry(table->entries, table->capacity, key);
    if (entry->key == NULL) return false;

    // Place a tombstone in the entry.
    entry->key = NULL;
    entry->value = BOOL_VAL(true);
    return true;
}

ObjString* tableFindString(Table* table, const char* chars, int length, uint32_t hash) {
    if (table->count == 0) return NULL;

    uint32_t index = hash % table->capacity;

    for (;;) {
        Entry* entry = &table->entries[index];
        if (entry->key == NULL) {
            // Stop if we find an empty non-tombstone entry.
            if (IS_NIL(entry->value)) return NULL;
        } else if (
            entry->key->length == length &&
            entry->key->hash == hash &&
            memcmp(entry->key->chars, chars, length) == 0
        ) {
            // We found it.
            return entry->key;
        }

        index = (index + 1) % table->capacity;
    }
}

void tableAddAll(Table* from, Table* to) {
    for (int i = 0; i < from->capacity; i++) {
        Entry* entry = &from->entries[i];
        if (entry->key != NULL) {
            tableSet(to, entry->key, entry->value);
        }
    }
}

void freeTable(Table* table) {
    FREE_ARRAY(Entry, table->entries, table->capacity);
    initTable(table);
}