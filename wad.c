#include "globaldefs.h"
#include "wad.h"

extern const char* mus_order[] = {
    "D_RUNNIN",
    "D_STALKS",
    "D_COUNTD",
    "D_BETWEE",
    "D_DOOM",
    "D_THE_DA",
    "D_SHAWN",
    "D_DDTBLU",
    "D_IN_CIT",
    "D_DEAD",
    "D_STLKS2",
    "D_THEDA2",
    "D_DOOM2",
    "D_DDTBL2",
    "D_RUNNI2",
    "D_DEAD2",
    "D_STLKS3",
    "D_ROMERO",
    "D_SHAWN2",
    "D_MESSAG",
    "D_COUNT2",
    "D_DDTBL3",
    "D_AMPIE",
    "D_THEDA3",
    "D_ADRIAN",
    "D_MESSG2",
    "D_ROMER2",
    "D_TENSE",
    "D_SHAWN3",
    "D_OPENIN",
    "D_EVIL",
    "D_ULTIMA"
};


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
            i += BLOCKMAP;
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
    return 0;
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

int t_push(wad_t* wadfile, lump_t* entry)
{
    int dirPos;

    if ((wadfile->dir = realloc(wadfile->dir, sizeof(lump_t) * (wadfile->lumpCount + 1))) == NULL) {
        fprintf(stderr, "%s: malloc error\n", __func__);
        return 1;
    }
    wadfile->dir[wadfile->lumpCount] = *entry;
    wadfile->lumpCount++;
    fseek(wadfile->fd, 0, SEEK_END);
    dirPos = ftell(wadfile->fd);
    w_updateheader(wadfile, dirPos);

    return 0;
}

int t_compare(lump_t* left, lump_t* right)
{
    if (left->size == right->size &&
            strncmp(left->name, right->name, 8) == 0) {
        return 0;
    }
    return 1;
}

int t_exists(wad_t* wadfile, lump_t* entry)
{
    if (entry->size == 0)
        return 0;
    for (int i = 0; i < wadfile->lumpCount; i++)
        if (t_compare(entry, &wadfile->dir[i]) == 0)
            return 1;
    return 0;
}

int t_ismap(lump_t* entry)
{
    if (strncmp(entry->name, "MAP", 3) == 0 ||
            (entry->name[0] == 'E' && entry->name[2] == 'M'))
        return strtol(&entry->name[3], &entry->name[7], 10);
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

int t_ismus(lump_t* entry)
{
    for (int i = 0; i < 32; i++)
        if (strncmp(entry->name, mus_order[i], 8) == 0)
            return i + 1;
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

int m_setnum(lump_t* entry, int32_t num)
{
    if (strncmp(entry->name, "MAP", 3) == 0) {
        sprintf(entry->name, "MAP%.2d", num);
        return 0;
    }
    return 1;
}

void m_setmus(lump_t* entry, int32_t num)
{
    sprintf(entry->name, "%.8s", mus_order[num - 1]);
}


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

//int l_compare(lump_t* left, lump_t* right)
//{
//    if (left->buffer == NULL || right->buffer == NULL)
//        return 0;
//    for (int i = 0; i < left->size; i++)
//        if (left->buffer[i] != right->buffer[i])
//            return 1;
//    return 0;
//}

int l_copy(wad_t* dst, wad_t* src, lump_t* entry)
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
