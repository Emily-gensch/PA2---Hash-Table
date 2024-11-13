// Insert function for the hash table
void insert(const char *name, uint32_t salary) {
    acquire_write_lock();

    uint32_t hash_val = hash_key(name);
    printf("DEBUG: Hash for '%s' is %u\n", name, hash_val); // Debug statement

    hashRecord *prev = NULL, *current = head;

    while (current && current->hash < hash_val) {
        prev = current;
        current = current->next;
    }

    if (current && current->hash == hash_val && strcmp(current->name, name) == 0) {
        current->salary = salary;
    } else {
        hashRecord *new_record = (hashRecord *)malloc(sizeof(hashRecord));
        new_record->hash = hash_val;
        strncpy(new_record->name, name, sizeof(new_record->name) - 1);
        new_record->name[sizeof(new_record->name) - 1] = '\0'; // Ensure null-termination
        new_record->salary = salary;
        new_record->next = current;

        if (prev == NULL) {
            head = new_record;
        } else {
            prev->next = new_record;
        }
    }
    log_action("INSERT", name, salary);

    release_write_lock();
}
