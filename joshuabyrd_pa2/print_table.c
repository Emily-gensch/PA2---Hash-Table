#include "chash.h"

void print_table(FILE* output) {
    time_t timestamp;

    // Acquire read lock
    pthread_rwlock_rdlock(&rwlock);
    timestamp = get_current_time_in_micro();
    fprintf(output, "%ld: READ LOCK ACQUIRED\n", timestamp);
    lock_acquisitions++;

    // Print table
    hashRecord* current = head;
    while (current) {
        fprintf(output, "%u,%s,%u\n", current->hash, current->name, current->salary);
        current = current->next;
    }

    // Release read lock
    pthread_rwlock_unlock(&rwlock);
    timestamp = get_current_time_in_micro();
    fprintf(output, "%ld: READ LOCK RELEASED\n", timestamp);
    lock_releases++;
}
