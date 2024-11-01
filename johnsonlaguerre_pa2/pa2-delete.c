#include <stdio.h>
#include <stdlib.h>

// This function deletes a key-data pair from the hash table, if it exists. To delete a key-data pair, the function first computes the hash value of the key and obtains a writer lock. Then it searches the linked list for the key. If the key is found, it removes the node from the list and frees the memory. Otherwise, it does nothing. Finally, it releases the write lock and returns.
void delete(key){
	
}
