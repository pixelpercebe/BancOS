
#ifndef OS_TASK_TASK_MAP_H
#define OS_TASK_TASK_MAP_H
#include <cstddef>


typedef struct ht ht;


typedef struct {
    const char* key;  // sys task hash
    void* value;      // task control timer struct pointer

    // Don't use these fields directly.
    ht* _table;       // reference to hash table being iterated
    size_t _index;    // current index into ht._entries
} hti;

hti ht_iterator(ht* table);
int ht_iterator_has_next(hti* iterator);
void ht_iterator_next(hti* iterator);

#endif // OS_TASK_TASK_MAP_H