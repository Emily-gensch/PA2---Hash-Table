#ifndef CHASH_H
#define CHASH_H

#include <pthread.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>

typedef struct hash_struct
{
  uint32_t hash;
  char name[50];
  uint32_t salary;
  struct hash_struct *next;
} hashRecord;

// Global variables
extern pthread_rwlock_t rwlock;
extern pthread_mutex_t cv_mutex;
extern pthread_cond_t insert_complete;
extern int pending_inserts;
extern int lock_acquisitions;
extern int lock_releases;
extern hashRecord* head;

// Function declarations
void init_table();
void destroy_table();
bool insert(char* name, uint32_t salary, FILE* output);
bool delete(char* name, FILE* output);
hashRecord* search(char* name, FILE* output);
void print_table(FILE* output);
uint32_t jenkins_one_at_a_time_hash(char* key);
uint64_t get_current_time_in_micro();

#endif