// Delete function with timestamped log_action
void delete(const char *key) {
    struct timespec req = {0, 100000000};
    nanosleep(&req, NULL);

    unsigned long long acquire_time = current_timestamp();
    pthread_mutex_lock(&write_lock);
    log_lock("WRITE LOCK ACQUIRED", acquire_time);
    lock_acquisitions++;

    nanosleep(&req, NULL);  // Slowing down by 1 second
    uint32_t hash_val = jenkins_one_at_a_time_hash((const uint8_t*)key, strlen(key));
    Node *prev = NULL, *current = head;

    while (current && current->hash != hash_val) {
        prev = current;
        current = current->next;
    }

    unsigned long long action_time = current_timestamp();
    if (current) {
        if (prev == NULL) {
            head = current->next;
        } else {
            prev->next = current->next;
        }
        free(current->key);
        free(current);
        log_action("DELETE", key, 0, action_time);
    } else {
        log_action("DELETE (not found)", key, 0, action_time);
    }

    nanosleep(&req, NULL);  // Slowing down by 1 second
    unsigned long long release_time = current_timestamp();
    lock_releases++;
    pthread_mutex_unlock(&write_lock);
    log_lock("WRITE LOCK RELEASED", release_time);
}
