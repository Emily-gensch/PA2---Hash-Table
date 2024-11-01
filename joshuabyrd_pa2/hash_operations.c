#include "chash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Global variables
pthread_rwlock_t rwlock;
pthread_mutex_t cv_mutex;
pthread_cond_t insert_complete;
int pending_inserts = 0;
int lock_acquisitions = 0;
int lock_releases = 0;
hashRecord* head = NULL;

uint32_t jenkins_one_at_a_time_hash(char* key) {
    size_t length = strlen(key);
    size_t i = 0;
    uint32_t hash = 0;
    while (i != length) {
        hash += key[i++];
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);
    return hash;
}

void hash_table_init() {
    pthread_rwlock_init(&rwlock, NULL);
    pthread_mutex_init(&cv_mutex, NULL);
    pthread_cond_init(&insert_complete, NULL);
    head = NULL;
    pending_inserts = 0;
    lock_acquisitions = 0;
    lock_releases = 0;
}

void hash_table_destroy() {
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

bool hash_table_insert(char* name, uint32_t salary) {
    uint32_t hash = jenkins_one_at_a_time_hash(name);
    
    // Increment pending inserts
    pthread_mutex_lock(&cv_mutex);
    pending_inserts++;
    pthread_mutex_unlock(&cv_mutex);
    
    // Acquire write lock
    pthread_rwlock_wrlock(&rwlock);
    lock_acquisitions++;
    
    bool success = false;
    hashRecord* current = head;
    hashRecord* prev = NULL;
    
    // Find insertion point (maintain sorted order by hash)
    while (current && current->hash < hash) {
        prev = current;
        current = current->next;
    }
    
    // Update existing record
    if (current && current->hash == hash) {
        current->salary = salary;
        success = true;
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
            success = true;
        }
    }
    
    // Release write lock
    pthread_rwlock_unlock(&rwlock);
    lock_releases++;
    
    // Decrement pending inserts and signal if complete
    pthread_mutex_lock(&cv_mutex);
    pending_inserts--;
    if (pending_inserts == 0) {
        pthread_cond_broadcast(&insert_complete);
    }
    pthread_mutex_unlock(&cv_mutex);
    
    return success;
}

bool hash_table_delete(char* name) {
    // Wait for all inserts to complete
    pthread_mutex_lock(&cv_mutex);
    while (pending_inserts > 0) {
        pthread_cond_wait(&insert_complete, &cv_mutex);
    }
    pthread_mutex_unlock(&cv_mutex);
    
    uint32_t hash = jenkins_one_at_a_time_hash(name);
    
    // Acquire write lock
    pthread_rwlock_wrlock(&rwlock);
    lock_acquisitions++;
    
    hashRecord* current = head;
    hashRecord* prev = NULL;
    bool success = false;
    
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
    lock_releases++;
    
    return success;
}

hashRecord* hash_table_search(char* name) {
    uint32_t hash = jenkins_one_at_a_time_hash(name);
    
    // Acquire read lock
    pthread_rwlock_rdlock(&rwlock);
    lock_acquisitions++;
    
    hashRecord* current = head;
    hashRecord* result = NULL;
    
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
    
    // Release read lock
    pthread_rwlock_unlock(&rwlock);
    lock_releases++;
    
    return result;
}

void hash_table_print(FILE* output) {
    // Acquire read lock
    pthread_rwlock_rdlock(&rwlock);
    lock_acquisitions++;
    
    hashRecord* current = head;
    
    while (current) {
        fprintf(output, "%u,%s,%u\n", 
                current->hash,
                current->name,
                current->salary);
        current = current->next;
    }
    
    // Release read lock
    pthread_rwlock_unlock(&rwlock);
    lock_releases++;
}