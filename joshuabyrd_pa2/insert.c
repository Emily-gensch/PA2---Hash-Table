#include "chash.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

void insert(char* name, uint32_t salary, FILE* output) {
    time_t timestamp;
    uint32_t hash = jenkins_one_at_a_time_hash(name);

    // Increment pending inserts
    pthread_mutex_lock(&cv_mutex);
    pending_inserts++;
    pthread_mutex_unlock(&cv_mutex);

    // Acquire write lock
    pthread_rwlock_wrlock(&rwlock);
    timestamp = get_current_time_in_micro();
    fprintf(output, "%lld: WRITE LOCK ACQUIRED\n", timestamp);
    lock_acquisitions++;

    // Find insertion point (maintain sorted order by hash)
    hashRecord* current = head;
    hashRecord* prev = NULL;
    while (current && current->hash < hash) {
        prev = current;
        current = current->next;
    }

    // Update existing record
    if (current && current->hash == hash) {
        current->salary = salary;
    } else {
        // Create new record
        hashRecord* new_record = malloc(sizeof(hashRecord));
        if (new_record) {
            new_record->hash = hash;
            strncpy(new_record->name, name, 49);
            new_record->name[49] = '\0';
            new_record->salary = salary;

            // Insert into list
            if (prev) {
                new_record->next = prev->next;
                prev->next = new_record;
            } else {
                new_record->next = head;
                head = new_record;
            }
            timestamp = get_current_time_in_micro();
            fprintf(output, "%lld: INSERT,%u,%s,%u\n", timestamp, hash, name, salary);
        }
    }

    // Release write lock
    pthread_rwlock_unlock(&rwlock);
    timestamp = get_current_time_in_micro();
    fprintf(output, "%lld: WRITE LOCK RELEASED\n", timestamp);
    lock_releases++;

    // Decrement pending inserts and signal if complete
    pthread_mutex_lock(&cv_mutex);
    pending_inserts--;
    if (pending_inserts == 0) {
        pthread_cond_broadcast(&insert_complete);
    }
    pthread_mutex_unlock(&cv_mutex);

    // return success;
}
