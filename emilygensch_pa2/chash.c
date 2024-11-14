#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>
#include <stdint.h>
#include <unistd.h>

#define MAX_COMMAND_LENGTH 100
#define MAX_KEY_LENGTH 50
#define MAX_ACTION_LENGTH 10

// Node structure for the linked list in the hash table
typedef struct Node {
    char *key;
    int value;
    uint32_t hash;
    struct Node *next;
} Node;

Node *head = NULL;
FILE *output_file;

pthread_mutex_t write_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t read_lock = PTHREAD_MUTEX_INITIALIZER;

int lock_acquisitions = 0;
int lock_releases = 0;

// Function prototypes
uint32_t jenkins_one_at_a_time_hash(const uint8_t* key, size_t length);
void insert(const char *key, int value);
void delete(const char *key);
void search(const char *key);
void log_action(const char *action, const char *key, int value, unsigned long long timestamp);
void log_lock(const char *action, unsigned long long timestamp);
void process_commands(const char *filename);

// Get timestamp in microseconds (using your provided function)
long long current_timestamp() {
  struct timeval te;
  gettimeofday(&te, NULL); // get current time
  long long microseconds = (te.tv_sec * 1000000) + te.tv_usec; // calculate milliseconds
  return microseconds;
}

// Jenkins one-at-a-time hash function
uint32_t jenkins_one_at_a_time_hash(const uint8_t* key, size_t length) {
    size_t i = 0;
    uint32_t hash = 0;
    while (i != length) {
        hash += key[i++];
        hash += hash << 10;
        hash ^= hash >> 6;
    }
    hash += hash << 3;
    hash ^= hash >> 11;
    hash += hash << 15;
    return hash;
}

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


// Log actions to output file with specific timestamp
void log_action(const char *action, const char *key, int value, unsigned long long timestamp) {
    fprintf(output_file, "%llu,%s,%s,%d\n", timestamp, action, key ? key : "NULL", value);
}

// Log lock acquisition/release events with specific timestamps
void log_lock(const char *action, unsigned long long timestamp) {
    fprintf(output_file, "%llu,%s\n", timestamp, action);
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
    int insert_count = 0, other_count = 0;

    while (fgets(line, sizeof(line), file)) {
        char action[MAX_ACTION_LENGTH], key[MAX_KEY_LENGTH];
        int value = 0;
        
        if (sscanf(line, "%[^,],%[^,],%d", action, key, &value) < 2) {
            continue; // Invalid line format, skip it
        }

        if (strcmp(action, "insert") == 0) {
            insert(key, value);
            insert_count++;
        } else if (strcmp(action, "delete") == 0) {
            delete(key);
            other_count++;
        } else if (strcmp(action, "search") == 0) {
            search(key);
            other_count++;
        } else if (strcmp(action, "print") == 0) {
            // Print current contents to the output file
            Node *current = head;
            while (current) {
                fprintf(output_file, "%u,%s,%d\n", current->hash, current->key, current->value);
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
