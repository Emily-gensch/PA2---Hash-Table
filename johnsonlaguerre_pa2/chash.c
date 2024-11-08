#include <ctype.h>
#include <pthread.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "pa2-delete.h"
#include "pa2-print.h"
#include "pa2-search.h"
#include "pa2-insert.h"

FILE *infile, *outfile;

int main(void)
{
	uint8_t temp='.';
	char* inputfile = "commands.txt";
	uint8_t field1[50]="\0", field2[50]="\0", field3[50]="\0";
	Line inserts[50], general[50];
	InsertObj *iobj = malloc(sizeof(iobj));
	int i=0, j=0, numthreads = 0, listsize = 0, lctr = 0, ictr = 0, gctr = 0;
	hash_struct *hashlist = hash_struct_decl();
	// hashlist = hash_struct_decl();

	infile = fopen(inputfile, "r");
		
	if (infile == NULL)
	{
		printf("File \"%s\" was not found or could not be opened for reading. Closing program.\n", inputfile);
		return 0;
	}
	
	// Read input file.
	for (i = 0; (temp = fgetc(infile)) != EOF && isascii(temp); i++)
	{
		ungetc(temp, infile);
		
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
		for (j = 0; (temp = fgetc(infile)) != '\n' && temp != EOF && isascii(temp); j++)
		{
			field3[j] = temp;
		}
		field3[j] = '\0';
		
		// Decide where to store.
		if (strcmp("insert", field1) == 0){
			strcpy(inserts[ictr].cmd, field1);
			strcpy(inserts[ictr].name, field2);
			inserts[ictr].num = atoi(field3);
			ictr++;
			
		} else if (strcmp("threads", field1) == 0){
			numthreads = atoi(field2);
			
		} else {
			strcpy(general[gctr].cmd, field1);
			strcpy(general[gctr].name, field2);
			general[gctr].num = atoi(field3);
			gctr++;
			
		} 
	}
	
	pthread_t threads[numthreads];
	sem_init(&mutex, 0, 1);
	printf("Running %d threads\n", numthreads);
	
	// Run all INSERT commands.
	printf("%llu: WAITING ON INSERTS\n", time(NULL));
	for (i = 0; i < ictr; i++){
		iobj->key = malloc(sizeof(uint8_t) * (strlen(inserts[i].name)+1));
		strcpy(iobj->key, inserts[i].name);
		iobj->values = inserts[i].num;

		pthread_create(&threads[i], NULL, (void *)insert,iobj);
		pthread_join(threads[i], NULL);
	}
	
	for (j = 0; j < i; j++){
		pthread_join(threads[j], NULL);
	}
	
	// Run all SEARCH and DELETE commands.
	printf("%llu: DELETE AWAKENED\n", time(NULL));
	for (i = i, j=0; i < numthreads; i++, j++){
		if (strcmp(general[j].name, "delete") == 0){
			pthread_create(&threads[i], NULL, (void *)delete, general[j].name);
		} else {
			pthread_create(&threads[i], NULL, (void *)search, general[j].name);
		}
		pthread_join(threads[i], NULL);
	}	
	
	for (j = ictr; j < numthreads; j++){
		pthread_join(threads[j], NULL);
	}
	
	// for (i = 0; i < numthreads; i++){
		// pthread_join(threads[i], NULL);
	// }
	
	printf("Finished all threads.\n");
	printf("Number of lock acquisitions: %d\n", numthreads);
	printf("Number of lock releases: %d\n", numthreads);
	print();
	
	pthread_t *ptemp;
	for (i = 0, ptemp = threads; i < numthreads && ptemp != NULL; i+=1, ptemp = (ptemp+i)){
		free(ptemp);
	}
	
	hash_struct *htemp;
	for (i = 0, htemp = hashlist; htemp != NULL; i+=1, htemp = (htemp+i)){
		free(htemp);
	}
	
	fclose(infile);
	fclose(outfile);
	return 0;
}

