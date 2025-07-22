#ifndef SYMBOLS_HASH_H
#define SYMBOLS_HASH_H

#include <stddef.h>

struct Symbol {
    const char* name;
    int value;
};

const struct Symbol* get_value(const char* str, size_t len);

#endif // SYMBOLS_HASH_H
