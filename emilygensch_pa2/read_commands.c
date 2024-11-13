// Read and process commands
void process_commands(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening command file");
        return;
    }

    output_file = fopen("output.txt", "w");
    if (!output_file) {
        perror("Error opening output file");
        fclose(file);
        return;
    }

    char line[MAX_COMMAND_LENGTH];

    while (fgets(line, sizeof(line), file)) {
        char action[MAX_ACTION_LENGTH], name[MAX_NAME_LENGTH];
        uint32_t salary = 0;

        if (sscanf(line, "%[^,],%[^,],%u", action, name, &salary) < 2) {
            continue; // Invalid line format, skip it
        }

        if (strcmp(action, "insert") == 0) {
            insert(name, salary);
        } else if (strcmp(action, "delete") == 0) {
            delete(name);
        } else if (strcmp(action, "search") == 0) {
            search(name);
        } else if (strcmp(action, "print") == 0) {
            hashRecord *current = head;
            while (current) {
                fprintf(output_file, "%u,%s,%u\n", current->hash, current->name, current->salary);
                current = current->next;
            }
        }
    }

    fclose(file);
    fprintf(output_file, "\nNumber of lock acquisitions: %d\n", lock_acquisitions);
    fprintf(output_file, "Number of lock releases: %d\n", lock_releases);
    fclose(output_file);
}
