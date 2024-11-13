// Simulate acquiring a write lock
void acquire_write_lock() {
    while (write_lock_acquired); // Busy-wait until lock is available
    write_lock_acquired = 1;
    lock_acquisitions++;
    log_lock("WRITE LOCK ACQUIRED");
}

// Simulate releasing a write lock
void release_write_lock() {
    write_lock_acquired = 0;
    lock_releases++;
    log_lock("WRITE LOCK RELEASED");
}

// Simulate acquiring a read lock
void acquire_read_lock() {
    while (read_lock_acquired); // Busy-wait until lock is available
    read_lock_acquired = 1;
    lock_acquisitions++;
    log_lock("READ LOCK ACQUIRED");
}

// Simulate releasing a read lock
void release_read_lock() {
    read_lock_acquired = 0;
    lock_releases++;
    log_lock("READ LOCK RELEASED");
}
