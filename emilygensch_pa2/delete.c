void delete(const char *name) {
    log_lock("WAITING ON INSERTS");
    acquire_write_lock();

    uint32_t hash_val = hash_key(name);
    hashRecord *prev = NULL, *current = head;

    while (current && current->hash != hash_val) {
        prev = current;
        current = current->next;
    }

    if (current && strcmp(current->name, name) == 0) {
        if (prev == NULL) {
            head = current->next;
        } else {
            prev->next = current->next;
        }
        free(current);
        log_action("DELETE", name, 0);
    } else {
        log_action("DELETE (not found)", name, 0);
    }

    release_write_lock();
    log_lock("DELETE AWAKENED");
}
