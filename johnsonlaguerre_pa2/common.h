#include <stdlib.h>
#include <semaphore.h>

sem_t mutex;

typedef struct hash_struct
{
  uint32_t hash; // 32-bit unsigned integer for the hash value produced by running the name text through the Jenkins one-at-a-time function.
  char name[50]; // Arbitrary string up to 50 characters long.
  uint32_t salary; // 32-bit unsigned integer to represent an annual salary.
  struct hash_struct *next; // pointer to the next node in the list.
} hash_struct;

typedef struct Line{
 uint8_t cmd[50];
 uint8_t name[50];
 size_t num;
} Line;

typedef struct InsertObj{
 uint8_t* key;
 size_t values;
} InsertObj;

hash_struct *hashlist;

uint32_t jenkins_one_at_a_time_hash(const uint8_t* key) {
  size_t i = 0;
  uint32_t hash = 0;
  while (key[i] != '\0') {
    hash += key[i++];
    hash += hash << 10;
    hash ^= hash >> 6;
  }
  hash += hash << 3;
  hash ^= hash >> 11;
  hash += hash << 15;
  return hash;
}

hash_struct* hash_struct_decl(){
	return (hash_struct*)(malloc(sizeof(hash_struct)));
}
