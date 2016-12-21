#include "globaldefs.h"
#include "wad.h"


/* wad functions */

wad_t* w_open(const char* path)
{
    wad_t* wadfile;
    int32_t dirPos;

    if ((wadfile = (wad_t*)malloc(sizeof(wad_t))) == NULL) {
        fprintf(stderr, "%s: malloc error", __func__);
        return NULL;
    }
    wadfile->dir = NULL;

    if ((wadfile->fd = fopen(path, "rb")) == NULL)
        return NULL;

    fread(&wadfile->type, 4, 1, wadfile->fd);
    if (strncmp(wadfile->type, "PWAD", 4)) {
        fprintf(stderr, "%s: file is corrupted or not pwad\n", path);
        free(wadfile);
        return NULL;
    }

    fread(&wadfile->lumpCount, 4, 1, wadfile->fd);
    fread(&dirPos, 4, 1, wadfile->fd);

    if ((wadfile->dir = (lump_t*)malloc(sizeof(lump_t) * wadfile->lumpCount)) == NULL) {
        fprintf(stderr, "%s: malloc error\n", __func__);
        free(wadfile);
        return NULL;
    }

    w_loadtable(wadfile, dirPos);

    return wadfile;
}

void w_close(wad_t* wadfile)
{
    fclose(wadfile->fd);
    free(wadfile->dir);
    free(wadfile);
    wadfile->fd = NULL;
    wadfile->dir = NULL;
    wadfile = NULL;
}

wad_t* w_create(const char* path)
{
    wad_t* wadfile;

    if ((wadfile = (wad_t*)malloc(sizeof(wad_t))) == NULL ||
            (wadfile->dir = (lump_t*)malloc(sizeof(lump_t))) == NULL)
        return NULL;
    if ((wadfile->fd = fopen(path, "wb+")) == NULL)
        return NULL;
    strncpy(wadfile->type, "PWAD", 4);
    wadfile->lumpCount = 0;
    w_updateheader(wadfile, 0x0000);
    return wadfile;
}

int w_loadtable(wad_t* wadfile, int32_t dirPos)
{
    fseek(wadfile->fd, dirPos, SEEK_SET);
    for (int i = 0; i < wadfile->lumpCount; i++) {
        lump_t* entry = &wadfile->dir[i];

        fread(entry, ENTRY_SIZE, 1, wadfile->fd);
        entry->buffer = NULL;
        entry->type = 0;
    }

    for (int i = 0; i < wadfile->lumpCount; i++) {
        lump_t* entry = &wadfile->dir[i];

        if (t_ismap(entry)) {
            entry->type = MARKER;
            for (int j = ++i, apptype = THINGS; j < i + 10; j++, apptype++)
                wadfile->dir[j].type = apptype;
            i += 11;
        } else if (t_isflag(entry)) {
            entry->type = MARKER;
            int apptype = t_flagtype(entry);

            for (int j = ++i; t_isflag(&wadfile->dir[j]) != 2; j++, i++) {
                wadfile->dir[j].type = apptype;
                wadfile->dir[j + 1].type = MARKER;
            }
        } else if (!strncmp(entry->name, "TEXTURE", 7)) {
            entry->type = TEXTUREx;
        } else if (!strncmp(entry->name, "PNAMES", 8)) {
            entry->type = PNAMES;
        } else if (entry->size == 0) {
            entry->type = MARKER;
        }
    }
}

int w_updateheader(wad_t* wadfile, int32_t dirPos)
{
    fseek(wadfile->fd, 0, SEEK_SET);
    fwrite(wadfile->type, 4, 1, wadfile->fd);
    fwrite(&wadfile->lumpCount, 4, 1, wadfile->fd);
    fwrite(&dirPos, 4, 1, wadfile->fd);
    return 0;
}

int w_mktable(wad_t* wadfile)
{
    fseek(wadfile->fd, 0, SEEK_END);
    for (int i = 0; i < wadfile->lumpCount; i++)
        fwrite(&wadfile->dir[i], ENTRY_SIZE, 1, wadfile->fd);
    return 0;
}

/* wad table functions */


int t_getindex(wad_t* wadfile, char name[8])
{
    for (int i = 0; i < wadfile->lumpCount; i++)
        if (strncmp(name, wadfile->dir[i].name, 8) == 0)
            return i;
    return 0;
}

int t_push(wad_t* wadfile, lump_t* entry)
{
    int dirPos;

    if ((wadfile->dir = realloc(wadfile->dir, sizeof(lump_t) * (wadfile->lumpCount + 1))) == NULL) {
        fprintf(stderr, "%s: malloc error\n", __func__);
        return 1;
    }
    wadfile->dir[wadfile->lumpCount] = *entry;

    wadfile->lumpCount++;
    w_updateheader(wadfile, dirPos);

    return 0;
}

int c_copy(wad_t* dst, wad_t* src, lump_t* entry)
{
    lump_t newentry;

    newentry = *entry;

    fseek(dst->fd, 0, SEEK_END);
    newentry.pos = ftell(dst->fd);
    if (l_load(src, entry) == 0 &&
            l_write(dst, entry) != 0) {
        t_push(dst, &newentry);
        l_unload(entry);
        return 1;
    }
    return 0;
}

int t_compare(lump_t* left, lump_t* right)
{
    if (left->size == right->size &&
            strncmp(left->name, right->name, 8) == 0) {
        return l_compare(left, right);
    }
    return 1;
}

int t_exists(wad_t* wadfile, lump_t* entry)
{
    if (entry->size == 0)
        return 1;
    for (int i = 0; i < wadfile->lumpCount; i++)
        if (t_compare(entry, &wadfile->dir[i]) == 0)
            return 1;
    return 0;
}

int t_ismap(lump_t* entry)
{
    if (strncmp(entry->name, "MAP", 3) == 0 ||
            (entry->name[0] == 'E' && entry->name[2] == 'M'))
        return 1;
    return 0;
}

int t_isflag(lump_t* entry)
{
    if (strstr(entry->name, "_START") != NULL)
        return 1;
    if (strstr(entry->name, "_END") != NULL)
        return 2;
    return 0;
}

int t_flagtype(lump_t* entry)
{
    switch (entry->name[0]) {
    case 'P':
        return PATCH;
    case 'F':
        return FLAT;
    case 'S':
        return SPRITE;
    }
    return UNDEFINED;
}

void t_rename(lump_t* entry, const char* newname)
{
    strncpy(entry->name, newname, 8);
}

//int t_swap(wad_t* wadfile, lump_t* target)
//{
//    for (int i = 0; i < wadfile->lumpCount; i++) {
//        if ()
//    }
//}

/* lump low-level functions */

int l_load(wad_t* wadfile, lump_t* lump)
{
    fseek(wadfile->fd, lump->pos, SEEK_SET);
    if (lump->size == 0)
        return 0;
    if ((lump->buffer = (char*)malloc(sizeof(char) * lump->size)) == NULL)
        return 1;
    if (fread(lump->buffer, lump->size, 1, wadfile->fd) == 0)
        return 2;
    return 0;
}

int l_compare(lump_t* left, lump_t* right)
{
    if (left->buffer == NULL || right->buffer == NULL)
        return 0;
    for (int i = 0; i < left->size; i++)
        if (left->buffer[i] != right->buffer[i])
            return 1;
    return 0;
}

int m_getnum(lump_t* entry)
{
    if (strncmp(entry->name, "MAP", 3) == 0)
        return strtol(&entry->name[3], &entry->name[7], 10);

}

int m_setnum(lump_t* entry, int32_t num)
{
    if (strncmp(entry->name, "MAP", 3) == 0)
        sprintf(entry->name, "MAP%0.2d", num);
}

int l_write(wad_t* wadfile, lump_t* target)
{
    if (target->size)
        return fwrite(target->buffer, target->size, 1, wadfile->fd);
    else
        return 1;
}

void l_unload(lump_t* lump)
{
    if (lump->size == 0)
        return;
    free(lump->buffer);
    lump->buffer = NULL;
}

//char* l_getusedtextures(lump_t* target)
//{
//    sidedef_t* src = (sidedef_t*)target->buffer;
//    int amount = src / 30;
//    char* tlist;
//
//    if ((tlist = (sidedef_t*)malloc(sizeof(sidedef_t))) == NULL)
//        return NULL;
//    for (int i )
//
//}
