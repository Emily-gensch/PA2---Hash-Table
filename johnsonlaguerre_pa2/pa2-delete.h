#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "common.h"

// This function deletes a key-data pair from the hash table, if it exists. To delete a key-data pair, the function first computes the hash value of the key and obtains a writer lock. Then it searches the linked list for the key. If the key is found, it removes the node from the list and frees the memory. Otherwise, it does nothing. Finally, it releases the write lock and returns.
void delete(uint8_t* key){
	hash_struct *prev = NULL, *temp = hashlist;
	uint32_t hash = jenkins_one_at_a_time_hash(key);
	
	printf("%llu,DELETE,%s\n", time(NULL), temp->name);
	
	// write lock get.
	// sem_wait(&mutex);
	
	printf("%llu: WRITE LOCK ACQUIRED\n", time(NULL));
	
	for (temp = hashlist; temp != NULL; prev = temp, temp = temp->next){
		if (hash == temp->hash){
			if (prev != NULL){
				// Rewire.
				prev->next = temp->next;
			} else if (prev == NULL){
				// New head of list.
				hashlist = temp->next;
			}
			
			free(temp);
		}
	}
	
	// write lock rel.
	// sem_post(&mutex);

	printf("%llu: WRITE LOCK RELEASED\n", time(NULL));
	
	return;
}
