#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void print(){
	time_t seconds;
	seconds = time(NULL);
	
	print("%llu: READ LOCK ACQUIRED\n", time(NULL));
	
	for (int i = 0; i < database->size; i++){
		printf("%llu, %s, %d\n", hashlist[i]., line[i]->name, line[i]->salary);
		
	}
		
	print("%llu: READ LOCK RELEASED\n", time(NULL));
}
