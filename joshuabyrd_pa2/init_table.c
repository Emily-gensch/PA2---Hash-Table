#include "chash.h"

void init_table() {
    pthread_rwlock_init(&rwlock, NULL);
    pthread_mutex_init(&cv_mutex, NULL);
    pthread_cond_init(&insert_complete, NULL);
    head = NULL;
    pending_inserts = 0;
    lock_acquisitions = 0;
    lock_releases = 0;
}