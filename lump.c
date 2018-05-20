#include "globaldefs.h"

lumptable_t* lt_init()
{
    lumptable_t *lt = calloc(1, sizeof(lumptable_t));
    lt->num = 0;
    lt->lump = NULL;
    return lt;
}

void lt_add(lumptable_t *lt, lump_t* lump, uint32_t index, size_t dup)
{
    if (index > lt->num)
        return;

    if (dup) {
        for (size_t i = 0; i < lt->num; i++) {
            if (memcmp(&lt->lump[index], lump, sizeof(lump_t)) == 0)
                return;
        }
    }

    lt->lump = realloc(lt->lump, sizeof(lump_t) * (lt->num + 1));
    memmove(&lt->lump[index + 1], &lt->lump[index], sizeof(lump_t) * (lt->num - index));

    lt->lump[index] = *lump;
    lt->num++;
}

void lt_push(lumptable_t *lt, lump_t *lump, int dup)
{
    lt_add(lt, lump, lt->num, dup);
}

lump_t *lt_findbyname(lumptable_t *lt, str8_t name)
{
    for (int i = 0; i < lt->num; i++)
        if (strncmp(lt->lump[i].name, name, 8) == 0)
            return &lt->lump[i];
    return NULL;
}

int lt_findindex(lumptable_t *lt, str8_t name)
{
    for (int i = 0; i < lt->num; i++)
        if (strncmp(lt->lump[i].name, name, 8) == 0)
            return i;
    return -1;
}

void lt_delete(lumptable_t *lt, uint32_t index)
{
    if (index > lt->num)
        return;
    
    memmove(&lt->lump[index], &lt->lump[index + 1], sizeof(lump_t) * (lt->num - index - 1));

    lt->lump = realloc(lt->lump, sizeof(lump_t) * (lt->num - 1));
    lt->num--;
}

void lt_cleanup(lumptable_t *lt)
{
    free(lt->lump);
    free(lt);
}

/*
void lump_add(list_t *table, lump_t *lump, int dup)
{
    list_add(table, lump, sizeof(lump_t), dup);
}

void lump_delete(list_t *table, lump_t *lump)
{
    list_delete(table, lump, sizeof(lump_t));
}

void lump_move(list_t *dst, list_t *src, lump_t *lump)
{
    lump_add(dst, lump, 0);
    lump_delete(src, lump);
}


lump_t *lump_find(list_t* table, str8_t lumpname)
{
    for (list_t* current = table; current != NULL; current = current->next) {
        lump_t *needle = current->data;
        if (strncmp(needle->name, lumpname, 8) == 0)
            return needle;
    }
    return NULL;
}
*/
