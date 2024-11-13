void search(const char *name) {
    acquire_read_lock();

    uint32_t hash_val = hash_key(name);
    hashRecord *current = head;

    while (current && current->hash != hash_val) {
        current = current->next;
    }

    if (current && strcmp(current->name, name) == 0) {
        log_action("SEARCH", name, current->salary);
    } else {
        log_action("SEARCH NOT FOUND", name, -1);
    }

    release_read_lock();
}
