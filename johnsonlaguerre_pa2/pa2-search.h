#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// This function searches for a key-data pair in the hash table and returns the value, if it exists. To search for a key-data pair, the function first computes the hash value of the key acquires a reader lock. Then, it searches the linked list for the key. If the key is found, it returns the value. Otherwise, it returns NULL. Finally, it releases the read lock and returns. The caller should then print the record or "No Record Found" if the return is NULL.
uint32_t* search(uint8_t* key){
	hash_struct *temp = hashlist;
	uint32_t hash = jenkins_one_at_a_time_hash(key);
	
	printf("%llu,SEARCH: ", time(NULL));
	
	// // read lock get.
	// sem_wait(&mutex);

	// printf("%llu: READ LOCK ACQUIRED\n", time(NULL));
	
	for (temp = hashlist; temp != NULL; temp = temp->next){
		// // printf("%p\n", temp);
		
		if (hash == temp->hash){
			
			// printf("%llu: READ LOCK RELEASED\n", time(NULL));
			// read lock rel.
			sem_post(&mutex);
			
			printf("%llu,%s,%d\n", temp->hash, temp->name, temp->salary);
			return &(temp->salary);
		}
	}
	
	// // read lock rel.
	// sem_post(&mutex);
	
	// printf("%llu: READ LOCK RELEASED\n", time(NULL));

	printf("NO RECORD FOUND\n", time(NULL));
	return NULL;
}
