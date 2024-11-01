#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// This function inserts a new key-data pair into the hash table, or updates the value of an existing key. To insert a key-data pair, the function first computes the hash value of the given name (the key).  Then, it acquires the write lock that protects the list and searches the linked list for the hash. If the hash is found, it updates the value. Otherwise, it creates a new node and inserts it in the list. Finally, it releases the write lock and returns.
void insert(key, values){
	time_t seconds;
	seconds = time(NULL);
}
