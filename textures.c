#include "globaldefs.h"

maptexture_t* tx_init(str8_t name)
{
    maptexture_t *tx;

    if (!name) {
        return NULL;
    }
    tx = calloc(1, sizeof(maptexture_t));
    strncpy(tx->name, name, 8);
    tx->masked = tx->width = tx->height = tx->columndirectory = tx->patchcount = 0;
    tx->patches = NULL;
    return tx;
}

int tx_isempty(maptexture_t* tx)
{
    if (strncmp(tx->name, "", 8) == 0)
        return 1;
    return 0;
}

void tx_add(list_t* table, maptexture_t* tx, int dup)
{
    list_add(table, tx, sizeof(maptexture_t), 1);
}

maptexture_t *tx_find(list_t* table, str8_t txname)
{
    for (list_t* current = table; current != NULL; current = current->next) {
        maptexture_t *needle = current->data;
        if (strncmp(needle->name, txname, 8) == 0)
            return needle;
    }
    return NULL;
}
