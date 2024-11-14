#include "chash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>
#include <time.h>

#define MAX_LINE 256
#define MAX_THREADS 100

typedef struct {
    char command[MAX_LINE];
    FILE* output;
} threadArg;

// Global locks/variables
pthread_rwlock_t rwlock;
pthread_mutex_t cv_mutex;
pthread_cond_t insert_complete;
int pending_inserts = 0;
int lock_acquisitions = 0;
int lock_releases = 0;
hashRecord* head = NULL;

// Util function to get time in milliseconds
uint64_t get_current_time_in_micro() {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    // If I don't divide by 1000, the least significant digit always becomes the same digit for whatever reason
    return ((uint64_t)(ts.tv_sec) * (uint64_t)1000000000 + (uint64_t)ts.tv_nsec) / 1000;
}

// Each thread runs processs_command
void* process_command(void* arg) {
    // Get command and output file
    threadArg* thread_arg = (threadArg*)arg;
    char command_line[MAX_LINE];
    strcpy(command_line, thread_arg->command);
    FILE* output = thread_arg->output;
    
    char* command = strtok(command_line, ",");
    char* name = strtok(NULL, ",");
    char* salary_string = strtok(NULL, ",");
    
    if (strcmp(command, "insert") == 0) {
        uint32_t salary = atoi(salary_string);
        insert(name, salary, output);
    }
    else if (strcmp(command, "delete") == 0) {
        delete(name, output);
    }
    else if (strcmp(command, "search") == 0) {
        search(name, output); 
    }
    else if (strcmp(command, "print") == 0) {
        print_table(output);
    }
    
    free(thread_arg);
    return NULL;
}

int main() {
    FILE* command_file = fopen("commands.txt", "r");
    FILE* output_file = fopen("output.txt", "w");
    
    if (!command_file || !output_file) {
        perror("Error opening files");
        return 1;
    }
    
    init_table();
    
    char line[MAX_LINE];
    int num_threads = 0;
    pthread_t threads[MAX_THREADS];
    
    // Get number of threads
    if (fgets(line, sizeof(line), command_file)) {
        char* token = strtok(line, ",");
        if (strcmp(token, "threads") == 0) {
            token = strtok(NULL, ",");
            num_threads = atoi(token);
        }
    }

    fprintf(output_file, "Running %d threads\n", num_threads);
    
    // Process commands
    int thread_index = 0;
    while (fgets(line, sizeof(line), command_file) && thread_index < num_threads) {
        line[strcspn(line, "\n")] = 0;
        
        threadArg* arg = malloc(sizeof(threadArg));
        strcpy(arg->command, line);
        arg->output = output_file;
        
        pthread_create(&threads[thread_index], NULL, process_command, arg);
        thread_index++;
    }
    
    // Wait for all threads to complete
    for (int i = 0; i < thread_index; i++) {
        pthread_join(threads[i], NULL);
    }
    
    // Print to output_file
    fprintf(output_file, "Finished all threads.\n");
    fprintf(output_file, "Number of lock acquisitions: %d\n", lock_acquisitions);
    fprintf(output_file, "Number of lock releases: %d\n", lock_releases);
    print_table(output_file);
    
    // Cleanup
    destroy_table();
    fclose(command_file);
    fclose(output_file);
    
    return 0;
}