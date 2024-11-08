#include "chash.h"
#include <stdlib.h>

void destroy_table() {
    hashRecord* current = head;
    while (current) {
        hashRecord* next = current->next;
        free(current);
        current = next;
    }
    pthread_rwlock_destroy(&rwlock);
    pthread_mutex_destroy(&cv_mutex);
    pthread_cond_destroy(&insert_complete);
}
