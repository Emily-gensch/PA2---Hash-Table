#include "chash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define MAX_LINE 256
#define MAX_THREADS 100

typedef struct {
    char command[MAX_LINE];
    FILE* output;
} threadArg;

void* process_command(void* arg) {
    threadArg* thread_arg = (threadArg*)arg;
    char command_line[MAX_LINE];
    strcpy(command_line, thread_arg->command);
    FILE* output = thread_arg->output;
    
    char* command = strtok(command_line, ",");
    char* param1 = strtok(NULL, ",");
    char* param2 = strtok(NULL, ",");
    
    time_t timestamp = time(NULL);
    
    if (strcmp(command, "insert") == 0) {
        uint32_t salary = atoi(param2);
        fprintf(output, "%ld,INSERT,%s,%u\n", timestamp, param1, salary);
        hash_table_insert(param1, salary);
    }
    else if (strcmp(command, "delete") == 0) {
        fprintf(output, "%ld,DELETE,%s\n", timestamp, param1);
        hash_table_delete(param1);
    }
    else if (strcmp(command, "search") == 0) {
        fprintf(output, "%ld,SEARCH,%s\n", timestamp, param1);
        hashRecord* record = hash_table_search(param1);
        if (record) {
            fprintf(output, "%u,%s,%u\n", record->hash, record->name, record->salary);
            free(record);
        } else {
            fprintf(output, "No Record Found\n");
        }
    }
    else if (strcmp(command, "print") == 0) {
        fprintf(output, "%ld,PRINT\n", timestamp);
        hash_table_print(output);
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
    
    hash_table_init();
    
    char line[MAX_LINE];
    int num_threads = 0;
    pthread_t threads[MAX_THREADS];
    
    // Read first line for thread count
    if (fgets(line, sizeof(line), command_file)) {
        char* token = strtok(line, ",");
        if (strcmp(token, "threads") == 0) {
            token = strtok(NULL, ",");
            num_threads = atoi(token);
        }
    }
    
    // Process commands
    int thread_index = 0;
    while (fgets(line, sizeof(line), command_file) && thread_index < num_threads) {
        // Remove newline
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
    fprintf(output_file, "\nNumber of lock acquisitions: %d\n", lock_acquisitions);
    fprintf(output_file, "Number of lock releases: %d\n", lock_releases);
    hash_table_print(output_file);
    
    // Cleanup
    hash_table_destroy();
    fclose(command_file);
    fclose(output_file);
    
    return 0;
}