#include "globaldefs.h"

lump_t* lump_init(str8_t name)
{
    lump_t *lump;
    if (!name)
        return NULL;
    lump = calloc(1, sizeof(lump_t));
    lump->pos = 0;
    lump->size = 0;
    strncpy(lump->name, name, 8);
    lump->next = NULL;
    return lump;
}

int lump_isempty(lump_t* lump)
{
    if (lump->pos == 0
          && lump->size == 0
          && strncmp(lump->name, "", 8) == 0)
        return 1;
    return 0;
}

void lump_addtotable(lump_t* table, lump_t* lump, int16_t dup)
{
    lump_t* current;
        
    current = table;
    if (lump_isempty(current)) {
        memcpy(current, lump, sizeof(lump_t));
        current->next = NULL;
        return;
    }
    for (current = table; current != NULL; current = current->next) {
        if (dup && strncmp(current->name, lump->name, 8) == 0) {
            //fprintf(stderr, "Error: lump %.8s exists in table\n", lump->name);
            return;
        }
        if (current->next == NULL) {
            current->next = calloc(1, sizeof(lump_t));
            memcpy(current->next, lump, sizeof(lump_t));
            current->next->next = NULL;
            return;
        }
    }
}

lump_t *lump_find(lump_t* table, str8_t lumpname)
{
    for (lump_t* current = table; current != NULL; current = current->next)
        if (strncmp(lumpname, current->name, 8) == 0)
            return current;
    return NULL;
}

int lump_deletefromtable(lump_t* table, lump_t* lump)
{
    lump_t* current;

    for (current = table; current->next != NULL; current = current->next) {
        lump_t* target = current->next;
        if (memcmp(target, lump, sizeof(lump_t)) == 0) {
            current->next = target->next;
            free(target);
            return 0;
        }
    }
    return 1;
}
