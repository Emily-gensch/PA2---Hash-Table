#include "chash.h"
#include <stdlib.h>
#include <time.h>

bool delete(char* name, FILE* output) {
    uint64_t timestamp;

    // Wait for all inserts to complete
    pthread_mutex_lock(&cv_mutex);
    while (pending_inserts > 0) {
        timestamp = get_current_time_in_micro();
        fprintf(output, "%lu: WAITING ON INSERTS\n", timestamp);
        pthread_cond_wait(&insert_complete, &cv_mutex);
    }
    pthread_mutex_unlock(&cv_mutex);

    uint32_t hash = jenkins_one_at_a_time_hash(name);

    // Acquire write lock
    pthread_rwlock_wrlock(&rwlock);
    timestamp = get_current_time_in_micro();
    fprintf(output, "%lu: WRITE LOCK ACQUIRED\n", timestamp);
    lock_acquisitions++;

    hashRecord* current = head;
    hashRecord* prev = NULL;
    bool success = false;

    // Find and delete record
    while (current && current->hash <= hash) {
        if (current->hash == hash) {
            if (prev) {
                prev->next = current->next;
            } else {
                head = current->next;
            }
            free(current);
            success = true;
            break;
        }
        prev = current;
        current = current->next;
    }

    // Release write lock
    pthread_rwlock_unlock(&rwlock);
    timestamp = get_current_time_in_micro();
    fprintf(output, "%lu: WRITE LOCK RELEASED\n", timestamp);
    lock_releases++;

    return success;
}
