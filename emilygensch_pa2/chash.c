#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <stdint.h>

#define MAX_COMMAND_LENGTH 100
#define MAX_NAME_LENGTH 50
#define MAX_ACTION_LENGTH 10

typedef struct hash_struct {
    uint32_t hash;
    char name[MAX_NAME_LENGTH];
    uint32_t salary;
    struct hash_struct *next;
} hashRecord;

hashRecord *head = NULL;
FILE *output_file;

// Custom flags for lock simulation
int write_lock_acquired = 0;
int read_lock_acquired = 0;

int lock_acquisitions = 0;
int lock_releases = 0;

// Function prototypes
uint32_t hash_key(const char *key);
void insert(const char *name, uint32_t salary);
void delete(const char *name);
void search(const char *name);
void log_action(const char *action, const char *name, uint32_t salary);
void log_lock(const char *action);
void process_commands(const char *filename);

// Get timestamp in microseconds
long get_timestamp() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000000 + tv.tv_usec;
}

// Jenkins's one-at-a-time hash function
uint32_t hash_key(const char *key) {
    uint32_t hash = 0;
    while (*key) {
        hash += *key++;
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);
    return hash;
}

// Simulate acquiring a write lock
void acquire_write_lock() {
    while (write_lock_acquired); // Busy-wait until lock is available
    write_lock_acquired = 1;
    lock_acquisitions++;
    log_lock("WRITE LOCK ACQUIRED");
}

// Simulate releasing a write lock
void release_write_lock() {
    write_lock_acquired = 0;
    lock_releases++;
    log_lock("WRITE LOCK RELEASED");
}

// Simulate acquiring a read lock
void acquire_read_lock() {
    while (read_lock_acquired); // Busy-wait until lock is available
    read_lock_acquired = 1;
    lock_acquisitions++;
    log_lock("READ LOCK ACQUIRED");
}

// Simulate releasing a read lock
void release_read_lock() {
    read_lock_acquired = 0;
    lock_releases++;
    log_lock("READ LOCK RELEASED");
}

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

// Delete function for the hash table
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

// Search function for the hash table
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

// Log actions to output file
void log_action(const char *action, const char *name, uint32_t salary) {
    fprintf(output_file, "%ld,%s,%s,%u\n", get_timestamp(), action, name ? name : "NULL", salary);
}

// Log lock acquisition/release events
void log_lock(const char *action) {
    fprintf(output_file, "%ld,%s\n", get_timestamp(), action);
}

// Read and process commands
void process_commands(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening command file");
        return;
    }

    output_file = fopen("output.txt", "w");
    if (!output_file) {
        perror("Error opening output file");
        fclose(file);
        return;
    }

    char line[MAX_COMMAND_LENGTH];

    while (fgets(line, sizeof(line), file)) {
        char action[MAX_ACTION_LENGTH], name[MAX_NAME_LENGTH];
        uint32_t salary = 0;

        if (sscanf(line, "%[^,],%[^,],%u", action, name, &salary) < 2) {
            continue; // Invalid line format, skip it
        }

        if (strcmp(action, "insert") == 0) {
            insert(name, salary);
        } else if (strcmp(action, "delete") == 0) {
            delete(name);
        } else if (strcmp(action, "search") == 0) {
            search(name);
        } else if (strcmp(action, "print") == 0) {
            hashRecord *current = head;
            while (current) {
                fprintf(output_file, "%u,%s,%u\n", current->hash, current->name, current->salary);
                current = current->next;
            }
        }
    }

    fclose(file);
    fprintf(output_file, "\nNumber of lock acquisitions: %d\n", lock_acquisitions);
    fprintf(output_file, "Number of lock releases: %d\n", lock_releases);
    fclose(output_file);
}

int main() {
    process_commands("commands.txt");
    return 0;
}

