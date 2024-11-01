#include <stdio.h>
#include <stdlib.h>

// This function searches for a key-data pair in the hash table and returns the value, if it exists. To search for a key-data pair, the function first computes the hash value of the key acquires a reader lock. Then, it searches the linked list for the key. If the key is found, it returns the value. Otherwise, it returns NULL. Finally, it releases the read lock and returns. The caller should then print the record or "No Record Found" if the return is NULL.
void search(key){
	
}
