#include "chash.h"
#include <string.h>

uint32_t jenkins_one_at_a_time_hash(char* key) {
    size_t length = strlen(key);
    size_t i = 0;
    uint32_t hash = 0;

    while (i != length) {
        hash += key[i++];
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);

    return hash;
}
