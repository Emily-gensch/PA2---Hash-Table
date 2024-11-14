// Insert function for the hash table with added delay
void insert(const char *key, int value) {
    struct timespec req = {0, 100000000};  // 0.1 second delay
    nanosleep(&req, NULL);  // Slowing down by 1 second

    unsigned long long acquire_time = current_timestamp();
    pthread_mutex_lock(&write_lock);
    log_lock("WRITE LOCK ACQUIRED", acquire_time);
    lock_acquisitions++;

    nanosleep(&req, NULL);  // Slowing down by 1 second
    // Use the Jenkins hash function for the key
    uint32_t hash_val = jenkins_one_at_a_time_hash((const uint8_t*)key, strlen(key));
    Node *prev = NULL, *current = head;

    while (current && current->hash < hash_val) {
        prev = current;
        current = current->next;
    }

    if (current && current->hash == hash_val) {
        current->value = value;
    } else {
        Node *new_node = (Node *)malloc(sizeof(Node));
        new_node->key = strdup(key);
        new_node->value = value;
        new_node->hash = hash_val;
        new_node->next = current;
        if (prev == NULL) {
            head = new_node;
        } else {
            prev->next = new_node;
        }
    }
    unsigned long long action_time = current_timestamp();
    log_action("INSERT", key, value, action_time);

    nanosleep(&req, NULL);  // Slowing down by 1 second
    unsigned long long release_time = current_timestamp();
    lock_releases++;
    pthread_mutex_unlock(&write_lock);
    log_lock("WRITE LOCK RELEASED", release_time);
}
