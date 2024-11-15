#include "chash.h"
#include <stdlib.h>

void destroy_table() {
    hashRecord* current = head;
    while (current) {
        hashRecord* next = current->next;
        free(current);
        current = next;
    }
}
