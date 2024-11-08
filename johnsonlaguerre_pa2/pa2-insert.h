#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// This function inserts a new key-data pair into the hash table, or updates the value of an existing key. To insert a key-data pair, the function first computes the hash value of the given name (the key).  Then, it acquires the write lock that protects the list and searches the linked list for the hash. If the hash is found, it updates the value. Otherwise, it creates a new node and inserts it in the list. Finally, it releases the write lock and returns.
void insert(uint8_t* key, size_t values){
	uint32_t hash = jenkins_one_at_a_time_hash(key);
	hash_struct *temp = hashlist, *node = malloc(sizeof(hash_struct));
	
	node->hash = hash;
	strcpy(node->name, key);
	node->salary = values;
	node->next = NULL;
	
	printf("%llu,INSERT,%s,%d\n", time(NULL), temp->name, temp->salary);
	
	// // write lock get.
	// sem_wait(&mutex);
	
	printf("%llu: WRITE LOCK ACQUIRED\n", time(NULL));
	
	// printf("%llu: READ LOCK ACQUIRED\n", time(NULL));
	uint32_t *res = search(key);
	// printf("%llu: READ LOCK RELEASED\n", time(NULL));
	
	// printf("%p\n", res);
	// printf("%d\n", res);

	// Insert.
	if (res == NULL || *res == 0){
		// This is the first insertion.
		if (temp == NULL){
			hashlist = node; 
		}
		
		// This is not the first insertion.
		// Go to end of list.
		else {
			for (temp = hashlist; temp != NULL; temp = temp->next){
				if (temp->next == NULL){
					temp->next = node; 
				}
			}
		}
	}
	
	// Update.
	else {
		for (temp = hashlist; temp != NULL; temp = temp->next){
			if (hash == temp->hash){
				temp->salary = values;
			}
		}
	}
	
	// write lock rel.
	// sem_post(&mutex);
	
	printf("%llu: WRITE LOCK RELEASED\n", time(NULL));
	
	return;
}
