#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void print(){
	hash_struct *temp = hashlist;
	
	// read lock get.
	sem_wait(&mutex);
	
	printf("%llu: READ LOCK ACQUIRED\n", time(NULL));
	
	for (temp = hashlist; temp != NULL; temp = temp->next){
		printf("%llu,%s,%d\n", temp->hash, temp->name, temp->salary);
	}
		
	// read lock rel.
	sem_post(&mutex);
	
	printf("%llu: READ LOCK RELEASED\n", time(NULL));
}
