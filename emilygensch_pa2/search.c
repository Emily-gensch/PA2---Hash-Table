// Search function with timestamped log_action
void search(const char *key) {
    struct timespec req = {0, 100000000};
    nanosleep(&req, NULL);

    unsigned long long acquire_time = current_timestamp();
    pthread_mutex_lock(&read_lock);
    log_lock("READ LOCK ACQUIRED", acquire_time);
    lock_acquisitions++;
    nanosleep(&req, NULL);
    
    uint32_t hash_val = jenkins_one_at_a_time_hash((const uint8_t*)key, strlen(key));
    Node *current = head;

    while (current && current->hash != hash_val) {
        current = current->next;
    }

    unsigned long long action_time = current_timestamp();
    if (current) {
        log_action("SEARCH", key, current->value, action_time);
    } else {
        log_action("SEARCH NOT FOUND", key, -1, action_time);
    }

    nanosleep(&req, NULL);  // Slowing down by 1 second
    unsigned long long release_time = current_timestamp();
    lock_releases++;
    pthread_mutex_unlock(&read_lock);
    log_lock("READ LOCK RELEASED", release_time);
}
