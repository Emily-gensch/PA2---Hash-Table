# AI Attribution

## Explanation

The only time our group used AI/ChatGPT for this assignment was for some help with processing the lines from the commands.txt file. One of us only used a single prompt (listed below) and we ended up using some of the tricks that ChatGPT used (plus some looking up other stuff) to finally manage to process commands.txt. Everything else was coded without AI assistance.

## Prompt Used

I'm writing a program in C and I need to process this file called commands.txt. Each line will have three tokens separated by commas. The program first needs to read the number of threads that it will create, which is written in commands.txt as threads,5,0 where 5 is the number of threads (can be any number) and 0 is just there so that there are three tokens.

Afterwards, there will be a list of commands (either insert, delete, or search) written as command,name,salary, and I want to store these char\* values in the variables with names command, name, and salary, respectively. Can you help me do this? Here is an example of a commands.txt file for your help:

threads,10,0
insert,Richard Garriot,40000
insert,Sid Meier,50000
insert,Shigeru Miyamoto,51000
delete,Sid Meier,0
insert,Hideo Kojima,45000
insert,Gabe Newell,49000
insert,Roberta Williams,45900
delete,Richard Garriot,0
insert,Carol Shaw,41000
search,Sid Meier,0

## ChatGPT Result

Certainly! Here's a C program snippet to process the commands.txt file as you described. The program reads the number of threads first and then parses each subsequent command line into the command, name, and salary variables.

Code
c
Copy code
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function to trim newline or extra spaces from a string
void trim_newline(char *str) {
char *pos;
if ((pos = strchr(str, '\n')) != NULL) {
\*pos = '\0';
}
}

int main() {
FILE \*file = fopen("commands.txt", "r");
if (file == NULL) {
perror("Error opening file");
return EXIT_FAILURE;
}

    char line[256];
    int thread_count = 0;

    // Read the first line for thread count
    if (fgets(line, sizeof(line), file)) {
        trim_newline(line);
        char *token = strtok(line, ",");
        if (token && strcmp(token, "threads") == 0) {
            token = strtok(NULL, ",");
            if (token) {
                thread_count = atoi(token);
                printf("Number of threads: %d\n", thread_count);
            }
        }
    }

    // Process the remaining lines
    while (fgets(line, sizeof(line), file)) {
        trim_newline(line);

        char *command = strtok(line, ",");
        char *name = strtok(NULL, ",");
        char *salary = strtok(NULL, ",");

        if (command && name && salary) {
            printf("Command: %s, Name: %s, Salary: %s\n", command, name, salary);

            // Additional processing can be done here based on the command type
            if (strcmp(command, "insert") == 0) {
                printf("Inserting %s with salary %s\n", name, salary);
            } else if (strcmp(command, "delete") == 0) {
                printf("Deleting %s\n", name);
            } else if (strcmp(command, "search") == 0) {
                printf("Searching for %s\n", name);
            } else {
                printf("Unknown command: %s\n", command);
            }
        } else {
            printf("Malformed line: %s\n", line);
        }
    }

    fclose(file);
    return EXIT_SUCCESS;

}
Explanation
Read the File:

The program uses fopen to open the commands.txt file and fgets to read each line.
Extract Tokens:

The first line is parsed to retrieve the number of threads.
Subsequent lines are parsed using strtok to split the line into command, name, and salary.
Handle Commands:

Based on the command value (insert, delete, search), the program logs actions.
Trimming:

A helper function trim_newline removes any trailing newline characters from the lines read by fgets.
Dynamic Processing:

The extracted values are stored in variables and printed. Further logic can be added to actually process these commands as needed.
Example Output
For the provided commands.txt:

yaml
Copy code
Number of threads: 10
Command: insert, Name: Richard Garriot, Salary: 40000
Inserting Richard Garriot with salary 40000
Command: insert, Name: Sid Meier, Salary: 50000
Inserting Sid Meier with salary 50000
Command: insert, Name: Shigeru Miyamoto, Salary: 51000
Inserting Shigeru Miyamoto with salary 51000
Command: delete, Name: Sid Meier, Salary: 0
Deleting Sid Meier
Command: insert, Name: Hideo Kojima, Salary: 45000
Inserting Hideo Kojima with salary 45000
...
This program is a skeleton to parse and process commands. You can integrate threading or data structure logic (e.g., linked lists, hash tables) for actual command execution.
