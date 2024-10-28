#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdint.h>
#include <time.h>
#include <sys/time.h>

// Define hash structure
typedef struct hash_struct {
	uint32_t hash;
	char name[50];
	uint32_t salary;
	struct hash_struct *next;
} hashRecord;

// Global variables
hashRecord *head = NULL;
pthread_rwlock_t rwlock;
FILE *output_file;
int lock_acquisitions = 0;
int lock_releases = 0;

// Function prototypes
void *execute_command(void *arg);
uint32_t jenkins_one_at_a_time_hash(const char *key);
void insert(char *name, uint32_t salary);
void delete(char *name);
void search(char *name);
void init_rwlock(pthread_rwlock_t *lock);

// Hash function
uint32_t jenkins_one_at_a_time_hash(const char *key) {
	size_t i = 0;
	uint32_t hash = 0;
	while (key[i] != '\0') {
		hash += key[i++];
		hash += (hash << 10);
		hash ^= (hash >> 6);
	}
	hash += (hash << 3);
	hash ^= (hash >> 11);
	hash += (hash << 15);
	return hash;
}

// Insert function with write lock
void insert(char *name, uint32_t salary) {
	uint32_t hash = jenkins_one_at_a_time_hash(name);
	fprintf(output_file, "%ld: WRITE LOCK ACQUIRED\n", time(NULL));
	lock_acquisitions++;

	pthread_rwlock_wrlock(&rwlock);

	hashRecord *current = head;
	while (current) {
		if (current->hash == hash && strcmp(current->name, name) == 0) {
			current->salary = salary;
			fprintf(output_file, "%ld,INSERT,%s,%u\n", time(NULL), name, salary);
			pthread_rwlock_unlock(&rwlock);
			fprintf(output_file, "%ld: WRITE LOCK RELEASED\n", time(NULL));
			lock_releases++;
			return;
		}
		current = current->next;
	}

	hashRecord *newRecord = malloc(sizeof(hashRecord));
	newRecord->hash = hash;
	strncpy(newRecord->name, name, 50);
	newRecord->salary = salary;
	newRecord->next = head;
	head = newRecord;

	fprintf(output_file, "%ld: INSERT,%s,%u\n", time(NULL), name, salary);
	pthread_rwlock_unlock(&rwlock);
	fprintf(output_file, "%ld: WRITE LOCK RELEASED\n", time(NULL));
	lock_releases++;
}

// Delete function with write lock
void delete(char *name) {
	uint32_t hash = jenkins_one_at_a_time_hash(name);
	fprintf(output_file, "%ld: WRITE LOCK ACQUIRED\n", time(NULL));
	lock_acquisitions++;

	pthread_rwlock_wrlock(&rwlock);

	hashRecord *current = head, *prev = NULL;
	while (current) {
		if (current->hash == hash && strcmp(current->name, name) == 0) {
			if (prev) prev->next = current->next;
			else head = current->next;
			free(current);
			fprintf(output_file, "%ld: DELETE,%s\n", time(NULL), name);
			pthread_rwlock_unlock(&rwlock);
			fprintf(output_file, "%ld: WRITE LOCK RELEASED\n", time(NULL));
			lock_releases++;
			return;
		}
		prev = current;
		current = current->next;
	}

	pthread_rwlock_unlock(&rwlock);
	fprintf(output_file, "%ld: WRITE LOCK RELEASED\n", time(NULL));
	lock_releases++;
}

// Search function with read lock
void search(char *name) {
	uint32_t hash = jenkins_one_at_a_time_hash(name);
	fprintf(output_file, "%ld: READ LOCK ACQUIRED\n", time(NULL));
	lock_acquisitions++;

	pthread_rwlock_rdlock(&rwlock);

	hashRecord *current = head;
	while (current) {
		if (current->hash == hash && strcmp(current->name, name) == 0) {
			fprintf(output_file, "%ld,SEARCH,%s,%u\n", time(NULL), current->name, current->salary);
			pthread_rwlock_unlock(&rwlock);
			fprintf(output_file, "%ld: READ LOCK RELEASED\n", time(NULL));
			lock_releases++;
			return;
		}
		current = current->next;
	}

	fprintf(output_file, "%ld: SEARCH: NOT FOUND NOT FOUND\n", time(NULL));
	pthread_rwlock_unlock(&rwlock);
	fprintf(output_file, "%ld: READ LOCK RELEASED\n", time(NULL));
	lock_releases++;
}

// Initialize the read-write lock
void init_rwlock(pthread_rwlock_t *lock) {
	pthread_rwlock_init(lock, NULL);
}

// Command execution function
void *execute_command(void *arg) {
	char *command = (char *)arg;
	char operation[10], name[50];
	uint32_t salary;

	sscanf(command, "%[^,],%49[^,],%u", operation, name, &salary);

	if (strcmp(operation, "insert") == 0) {
		insert(name, salary);
	} else if (strcmp(operation, "delete") == 0) {
		delete(name);
	} else if (strcmp(operation, "search") == 0) {
		search(name);
	}

	free(command);
	return NULL;
}

int main() {
	init_rwlock(&rwlock);

	FILE *file = fopen("commands.txt", "r");
	if (!file) {
		perror("Failed to open commands.txt");
		return 1;
	}

	output_file = fopen("output.txt", "w");
	if (!output_file) {
		perror("Failed to open output.txt");
		fclose(file);
		return 1;
	}

	char line[100];
	pthread_t *threads;
	int num_threads = 0;

	if (fgets(line, sizeof(line), file)) {
		sscanf(line, "threads,%d", &num_threads);
	}

	fprintf(output_file, "Running %d threads\n", num_threads);

	threads = malloc(num_threads * sizeof(pthread_t));
	int thread_idx = 0;

	while (fgets(line, sizeof(line), file) && thread_idx < num_threads) {
		char *command = strdup(line);
		pthread_create(&threads[thread_idx++], NULL, execute_command, command);
	}

	for (int i = 0; i < thread_idx; i++) {
		pthread_join(threads[i], NULL);
	}

	fprintf(output_file, "Finished all threads.\n");
	fprintf(output_file, "Number of lock acquisitions: %d\n", lock_acquisitions);
	fprintf(output_file, "Number of lock releases: %d\n", lock_releases);

	hashRecord *current = head;
	fprintf(output_file, "%ld: READ LOCK ACQUIRED\n", time(NULL));
	while (current) {
		fprintf(output_file, "%u,%s,%u\n", current->hash, current->name, current->salary);
		current = current->next;
	}
	fprintf(output_file, "%ld: READ LOCK RELEASED\n", time(NULL));

	fclose(file);
	fclose(output_file);
	free(threads);
	return 0;
}
