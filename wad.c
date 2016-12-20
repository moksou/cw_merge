#include "globaldefs.h"
#include "wad.h"

#include <regex.h>

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

    fseek(wadfile->fd, dirPos, SEEK_SET);
    for (int i = 0; i < wadfile->lumpCount; i++) {
        fread(&wadfile->dir[i], 16, 1, wadfile->fd);
        wadfile->dir[i].buffer = NULL;
    }
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
    for (int i = 0; i < wadfile->lumpCount; i++) {
        fwrite(&wadfile->dir[i].pos, 4, 1, wadfile->fd);
        fwrite(&wadfile->dir[i].size, 4, 1, wadfile->fd);
        fwrite(&wadfile->dir[i].name, 8, 1, wadfile->fd);
        fseek(wadfile->fd, 0, SEEK_END);
    }
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

    fseek(wadfile->fd, 0, SEEK_END);
    wadfile->dir[wadfile->lumpCount].pos = ftell(wadfile->fd);
    l_write(wadfile, &wadfile->dir[wadfile->lumpCount]);
    fseek(wadfile->fd, 0, SEEK_END);

    wadfile->lumpCount++;
    dirPos = ftell(wadfile->fd);
    w_updateheader(wadfile, dirPos);

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
    for (int i = 0; i < wadfile->lumpCount; i++)
        if (t_compare(entry, &wadfile->dir[i]) == 0)
            return 1;
    return 0;
}

int t_ismap(lump_t* entry)
{
    regex_t rxp;

    regcomp(&rxp, "MAP[0-9]{2}|E[0-9]M[0-9]", REG_EXTENDED);
    if (regexec(&rxp, entry->name, (size_t) 0, NULL, 0) == REG_NOERROR) {
        regfree(&rxp);
        return 1;
    }
    regfree(&rxp);
    return 0;
}

void t_rename(lump_t* entry, const char* newname)
{
    strncpy(entry->name, newname, 8);
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

int l_compare(lump_t* left, lump_t* right)
{
    if (left->buffer == NULL || right->buffer == NULL)
        return 0;
    for (int i = 0; i < left->size; i++)
        if (left->buffer[i] != right->buffer[i])
            return 1;
    return 0;
}

int l_write(wad_t* wadfile, lump_t* target)
{
    return fwrite(target->buffer, target->size, 1, wadfile->fd);
}

void l_unload(lump_t* lump)
{
    if (lump->size == 0)
        return;
    free(lump->buffer);
    lump->buffer = NULL;
}
