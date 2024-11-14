// Log actions to output file
void log_action(const char *action, const char *name, uint32_t salary) {
    fprintf(output_file, "%ld,%s,%s,%u\n", get_timestamp(), action, name ? name : "NULL", salary);
}

// Log lock acquisition/release events
void log_lock(const char *action) {
    fprintf(output_file, "%ld,%s\n", get_timestamp(), action);
}
