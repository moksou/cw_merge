#include "globaldefs.h"
#include "lump.h"

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

void tx_loadpnames(wadfile_t *archive)
{
    lump_t *pnames = lt_findbyname(archive->table, "PNAMES");

    if (pnames == NULL)
        return;

    archive->patches = array_init();

    fseek(archive->f, pnames->pos, SEEK_SET);
    fread(&archive->patches->num, 4, 1, archive->f);

    archive->patches->string = calloc(1, sizeof(str8_t) * archive->patches->num);
    fread(archive->patches->string, sizeof(str8_t) * archive->patches->num, 1, archive->f);

    lt_delete(archive->table, lt_findindex(archive->table, "PNAMES"));

}


void tx_loadtextures(wadfile_t *archive)
{
    uint32_t num = 0;

    lump_t *txlump = lt_findbyname(archive->table, "TEXTURE1");
    if (txlump == NULL) {
        printf("no texture lump\n");
        return;
    }

    archive->textures = list_init();

    fseek(archive->f, txlump->pos, SEEK_SET);
    fread(&num, 4, 1, archive->f);
    for (int i = 0; i < num; i++) {
        uint32_t offset;

        maptexture_t* tx = calloc(1, sizeof(maptexture_t));

        fseek(archive->f, txlump->pos + 4 + 4 * i, SEEK_SET);
        fread(&offset, 4, 1, archive->f);

        fseek(archive->f, txlump->pos + offset, SEEK_SET);
        fread(tx, 22, 1, archive->f);

        tx->patches = calloc(1, sizeof(mappatch_t) * tx->patchcount);
        fread(tx->patches, sizeof(mappatch_t) * tx->patchcount, 1, archive->f);

        tx_add(archive->textures, tx, 0);
        free(tx);
    }
}

void tx_cleanup(list_t *table)
{
    list_t *current = table;
    while (current != NULL) {
        list_t *tmp = current;
        maptexture_t *entry = tmp->data;

        current = current->next;
        free(entry->patches);
        free(entry);
        free(tmp);
    }
}
        

char *tx_getpatchname(array_t *pnames, mappatch_t *patch)
{
    return pnames->string[patch->patch];
}
