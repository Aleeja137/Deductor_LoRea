#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct nlist { /* table entry: */
    struct nlist *next; /* next entry in chain */
    char *name; /* defined name */
    int defn; /* replacement value */
};

typedef struct {
    struct nlist **hashtab; /* pointer table */
    unsigned size;          /* size of the hash table */
} Dictionary;

/* Create a new dictionary */
Dictionary *create_dictionary(unsigned size) {
    Dictionary *dict = malloc(sizeof(Dictionary));
    dict->hashtab = calloc(size, sizeof(struct nlist *));
    dict->size = size;
    return dict;
}

/* Free the dictionary */
void free_dictionary(Dictionary *dict) {
    struct nlist *current, *temp;
    for (unsigned i = 0; i < dict->size; i++) {
        current = dict->hashtab[i];
        while (current != NULL) {
            temp = current->next;
            free(current->name);
            free(current);
            current = temp;
        }
    }
    free(dict->hashtab);
    free(dict);
}

/* hash: form hash value for string s */
unsigned hash(Dictionary *dict, char *s) {
    unsigned hashval;
    for (hashval = 0; *s != '\0'; s++)
        hashval = *s + 31 * hashval;
    return hashval % dict->size;
}

/* lookup: look for s in dictionary */
struct nlist *lookup(Dictionary *dict, char *s) {
    struct nlist *np;
    for (np = dict->hashtab[hash(dict, s)]; np != NULL; np = np->next)
        if (strcmp(s, np->name) == 0)
            return np; /* found */
    return NULL; /* not found */
}

/* install: put (name, defn) in dictionary */
struct nlist *install(Dictionary *dict, char *name, int defn) {
    struct nlist *np;
    unsigned hashval;
    if ((np = lookup(dict, name)) == NULL) {
        np = (struct nlist *)malloc(sizeof(*np));
        if (np == NULL || (np->name = strdup(name)) == NULL)
            return NULL;
        hashval = hash(dict, name);
        np->next = dict->hashtab[hashval];
        dict->hashtab[hashval] = np;
    }
    np->defn = defn;
    return np;
}

/* clear: clear all entries in the dictionary */
void clear(Dictionary *dict) {
    struct nlist *current, *temp;
    for (unsigned i = 0; i < dict->size; i++) {
        current = dict->hashtab[i];
        while (current != NULL) {
            temp = current->next;
            free(current->name);
            free(current);
            current = temp;
        }
        dict->hashtab[i] = NULL;
    }
}