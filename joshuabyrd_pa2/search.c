#include "chash.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

void search(char* name, FILE* output) {
    time_t timestamp;
    uint32_t hash = jenkins_one_at_a_time_hash(name);

    // Acquire read lock
    pthread_rwlock_rdlock(&rwlock);
    timestamp = get_current_time_in_micro();
    fprintf(output, "%ld: READ LOCK ACQUIRED\n", timestamp);
    lock_acquisitions++;
    hashRecord* current = head;
    hashRecord* result = NULL;

    // Search table
    while (current && current->hash <= hash) {
        if (current->hash == hash) {
            // Create a copy of the record
            result = malloc(sizeof(hashRecord));
            if (result) {
                memcpy(result, current, sizeof(hashRecord));
                result->next = NULL;
            }
            break;
        }
        current = current->next;
    }

    if (current) {
       timestamp = get_current_time_in_micro();
       fprintf(output, "%ld: SEARCH,%s\n", timestamp, name);
    }
    else {
        timestamp = get_current_time_in_micro();
        fprintf(output, "%ld: SEARCH: No record found\n", timestamp);
    }

    // Release read lock
    pthread_rwlock_unlock(&rwlock);
    timestamp = get_current_time_in_micro();
    fprintf(output, "%ld: READ LOCK RELEASED\n", timestamp);
    lock_releases++;
}
