#include "globaldefs.h"
#include "wad.h"

const str8_t mus_order[32] = {
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

const str8_t map_order[10] = {
    "THINGS",
    "LINEDEFS",
    "SIDEDEFS",
    "VERTEXES",
    "SEGS",
    "SSECTORS",
    "NODES",
    "SECTORS",
    "REJECT",
    "BLOCKMAP"
};

const str8_t spritemarkers[8] = {
    "S_START",
    "S1_START",
    "S2_START",
    "SS_START",
    "S_END",
    "S1_END",
    "S2_END",
    "SS_END"
};

const str8_t flatmarkers[8] = {
    "F_START",
    "F1_START",
    "F2_START",
    "FF_START",
    "F_END",
    "F1_END",
    "F2_END",
    "FF_END"
};

void table_init(wadtable_t* table)
{
    table->total = 0;
    table->dir = NULL;
}

void table_close(wadtable_t* table)
{
    free(table->dir);
}

void t_gettable(FILE** f, wadtable_t* table)
{
    char         type[4];
    int          pos = 0;

    table->total = 0;
    table->dir = NULL;

    if (*f == NULL) {
        fprintf(stderr, "file not found\n");
        exit(-2);
    }

    fread(type, 4, 1, *f);

    if (strncmp(type, "PWAD", 4)) {
        fprintf(stderr, "error: wad is corrupt or not pwad\n");
        return;
    }
    fread(&table->total, 4, 1, *f);
    fread(&pos, 4, 1, *f);

    fseek(*f, pos, SEEK_SET);

    myalloc((lump_t*)&table->dir, sizeof(lump_t) * table->total);

    fseek(*f, pos, SEEK_SET);

    for (int i = 0; i < table->total; i++) {
        lump_t* op = &table->dir[i];
        fread(op, 16, 1, *f);
        if (i > 1 && op->pos != (op-1)->pos + (op-1)->size) {
            fprintf(stderr, "error: table corruption\n");
            free(table->dir);
            exit(-2);
        }
        op->ref = *f;
        op->type = UNDEFINED;
    }

    t_getmaps(table);
    t_getmusic(table);
    t_getrangelumps(table, FLAT);
    t_getrangelumps(table, SPRITE);
    t_gettextures(table);
}

void t_cattable(wadtable_t* dst, wadtable_t* src, int type)
{
    if (type) {
        lump_t start, end;

        start.pos = 0;
        start.size = 0;
        start.ref = NULL;
        start.type = MARKER;

        end.pos = 0;
        end.size = 0;
        end.ref = NULL;
        end.type = MARKER;

        switch (type) {
        case FLAT:
            strncpy(start.name, "FF_START", 8);
            strncpy(end.name, "FF_END", 8);
            break;
        case SPRITE:
            strncpy(start.name, "SS_START", 8);
            strncpy(end.name, "SS_END", 8);
            break;
        default:
            break;
        }
        if (src->total) {
            t_writetopos(src, &start, 0);
            t_push(src, &end);
        }
    }
    for (int i = 0; i < src->total; i++) {
        lump_t* op1 = &src->dir[i];
        if (!t_exists(dst, op1))
            t_push(dst, op1);
    }
}

void t_getmaps(wadtable_t* table)
{
    for (int i = 0; i < table->total; i++) {
        lump_t* op = &table->dir[i];

        if (name_exist(op->name, &map_order, 10) || (strncmp(op->name, "MAP", 3) == 0 && op->size == 0))
            op->type = MAP;
    }
}

void t_getmusic(wadtable_t* table)
{
    for (int i = 0; i < table->total; i++) {
        lump_t* op = &table->dir[i];

        if (name_exist(op->name, &mus_order, 32))
            op->type = MUSIC;
    }
}

void t_getrangelumps(wadtable_t* table, int apptype)
{
    char *markername = NULL,
          num = 0;

    switch (apptype) {
    case FLAT:
        markername = &flatmarkers; num = 8; break;
    case SPRITE:
        markername = &spritemarkers; num = 8; break;
    default:
        return;
    }

    for (int i = 0; i < table->total; i++) {
        lump_t* op = &table->dir[i];

        if (name_exist(op->name, markername, num) && strstr(op->name, "_START") != NULL) {
            for (int j = i + 1; !name_exist(table->dir[j].name, &markername, num) && strstr(table->dir[j].name, "_END") == NULL; j++) {
                table->dir[j].type = apptype;
                table->dir[j + 1].type = MARKER;
            }
        }
    }
}

void t_gettextures(wadtable_t* table)
{
    for (int i = 0; i < table->total; i++) {
        lump_t* op = &table->dir[i];
        if (strncmp(op->name, "TEXTURE1", 8) == 0
                || strncmp(op->name, "TEXTURE2", 8) == 0) {
            op->type = TEXTUREx;
        }
        if (strncmp(op->name, "PNAMES", 8) == 0) {
            op->type = PNAMES;
        }
    }
}

void t_writetopos(wadtable_t* table, lump_t* lump, int32_t pos)
{
    int a = table->total;
    if (pos < 0 || pos > table->total) {
        fprintf(stderr, "out of range\n");
        return;
    }
    table->dir = realloc(table->dir, sizeof(lump_t) * (table->total + 1));
    for (int i = table->total; i > pos; i--)
        table->dir[i] = table->dir[i - 1];
    table->dir[pos] = *lump;
    table->total++;

}

void t_push(wadtable_t* table, lump_t* lump)
{
    t_writetopos(table, lump, table->total);
}

int t_exists(wadtable_t* table, lump_t* lump)
{
    if (table->dir == NULL)
        return 0;
    for (int i = 0; i < table->total; i++) {
        if (t_isequal(&table->dir[i], lump))
            return 1;
    }
    return 0;
}

int t_isflag(lump_t* l)
{
    if (name_exist(l->name, &flatmarkers, 8)) {
        if (strstr(l->name, "_START") != NULL)
            return 1;
        else if (strstr(l->name, "_END") != NULL)
            return 2;
    }
    return 0;
}

void t_initlump(lump_t* l, str8_t name, int32_t pos, int32_t size)
{
    strncpy(l->name, name, 8);
    l->pos = pos;
    l->size = size;
    l->ref = NULL;
    l->type = UNDEFINED;
}

void t_setmapnum(lump_t* l, int num)
{
    sprintf(l->name, "MAP%.2d", num);
}

void t_setmusname(lump_t* entry, int32_t num)
{
    sprintf(entry->name, "%.8s", mus_order[num - 1]);
}

int t_isequal(lump_t* l1, lump_t* l2)
{
    char *buf1 = NULL,
         *buf2 = NULL;

    if (strncmp(l1->name, l2->name, 8) == 0
             && l1->size == l2->size) {
        buf1 = l_load(l1);
        buf2 = l_load(l2);
        if (buf1 != NULL && buf2 != NULL && memcmp(&buf1, &buf2, l1->size) == 0) {
            l_free(buf1);
            l_free(buf2);
            return 1;
        } else {
            l_free(buf1);
            l_free(buf2);
            return 2;
        }
    }
    return 0;

}

char* l_load(lump_t* l)
{
    char* buffer;
    if (l->ref == NULL) {
        return NULL;
    }
    if ((buffer = malloc(l->size)) == NULL)
        return NULL;
    fseek(l->ref, l->pos, SEEK_SET);
    fread(buffer, l->size, 1, l->ref);
    return buffer;
}

void l_write(FILE* fp, lump_t* l)
{
    if (l->ref == NULL)
        return;
    char* buffer = l_load(l);
    fseek(fp, 0, SEEK_END);
    l->pos = ftell(fp);
    fwrite(buffer, l->size, 1, fp);
    l_free(buffer);
}

void l_free(char* buffer)
{
    free(buffer);
}
