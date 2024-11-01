// Delete all TEMPCOMs.
// Delete all TEMPCOMs.
// Delete all TEMPCOMs.

#include <ctype.h>
#include <pthread.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct hash_struct
{
  uint32_t hash; // 32-bit unsigned integer for the hash value produced by running the name text through the Jenkins one-at-a-time function.
  char name[50]; // Arbitrary string up to 50 characters long.
  uint32_t salary; // 32-bit unsigned integer to represent an annual salary.
  struct hash_struct *next; // pointer to the next node in the list.
} hashRecord;

typedef struct Line{
 char cmd[50];
 char name[50];
 int num;
} Line;

typedef struct Lock{
 int acqs;
 int rels;
} Lock;

//Function prototypes
uint32_t jenkins_one_at_a_time_hash(const uint8_t* key, size_t length);

FILE *infile, *outfile;

int main(int argc, char** argv)
{
	char temp='.';
	char* inputfile = "commands.txt";
	char field1[50]="\0", field2[50]="\0", field3[50]="\0";
	int i=0, j=0, numthreads = 0, lctr = 0, ictr = 0, gctr = 0;
	hash_struct** hashlist = malloc(sizeof(hash_struct*) * 50);
	pthread_t** threads = NULL;
	Line lines[50];
	Line inserts[50];
	Line general[50];
	Lock lock;
	
	
	infile = fopen(inputfile, "r");
		
	if (infile == NULL)
	{
		printf("File \"%s\" was not found or could not be opened for reading. Closing program.\n", inputfile);
		return 0;
	}
	
	// Read input file.
	for (i = 0; (temp = fgetc(infile)) != EOF; i++)
	{
		// First field: "threads" or cmd.
		for (j = 0; (temp = fgetc(infile)) != ','; j++)
		{
			field1[j] = temp;
		}
		field1[j] = '\0';

		// Second field: numthreads or name.
		for (j = 0; (temp = fgetc(infile)) != ','; j++)
		{
			field2[j] = temp;
		}
		field2[j] = '\0';
		
		// Third field: num.
		for (j = 0; (temp = fgetc(infile)) != '\n'; j++)
		{
			field3[j] = temp;
		}
		field3[j] = '\0';
		
		// Decide where to store.
		if (strcmp("threads", strtemp) == 0){
			numthreads = atoi(field2);
			threads = malloc(sizeof(pthread_t*) * numthreads);
			lock.acqs = numthreads+1;
			lock.rels = lock.acqs;
		} else if (strcmp("insert", strtemp) == 0){
			strcpy(inserts[ictr].cmd, field1);
			strcpy(inserts[ictr].name, field2);
			inserts[ictr].num = atoi(field3);
			ictr++;
			
		} else {
			strcpy(general[gctr].cmd, field1);
			strcpy(general[gctr].name, field2);
			general[gctr].num = atoi(field3);
			gctr++;
			
		} 
	}
	
	// Run all INSERT commands.
	print("%llu: WAITING ON INSERTS\n", time(NULL));
	for (i = 0; i < ictr; i++){
		
	}
	
	// Run all SEARCH and DELETE commands.
	print("%llu: DELETE AWAKENED\n", time(NULL));
	for (i = 0; i < gctr; i++){
		// TEMPCOM: how to properly reference a thread address?
		// TEMPCOM: how to properly create a thread?
		Pthread_create(&(threads[i]), NULL, child, NULL);
	}	
	
	printf("Finished all threads.\n");
	printf("Number of lock acquisitions: %d\n", lock.acqs);
	printf("Number of lock releases: %d\n", lock.rels);
	print();
	
	fclose(infile);
	fclose(outfile);
	return 0;
}

//Function definitions
uint32_t jenkins_one_at_a_time_hash(const uint8_t* key, size_t length) {
  size_t i = 0;
  uint32_t hash = 0;
  while (i != length) {
    hash += key[i++];
    hash += hash << 10;
    hash ^= hash >> 6;
  }
  hash += hash << 3;
  hash ^= hash >> 11;
  hash += hash << 15;
  return hash;
}