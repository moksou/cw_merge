#include "globaldefs.h"
#include "wad.h"
#include "lump.h"
#include "textures.h"
#include "misc.h"

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

static void wad_loadmaps();
static void wad_loadlumps();
static void wad_printmaps();
static void wad_getmapflats(doomlevel_t *map);
static void wad_getmaptextures(doomlevel_t *map);

wadfile_t* wad;

wadfile_t *wad_create()
{
    wadfile_t *target = calloc(1, sizeof(wadfile_t));

    target->f = NULL;
    strncpy(target->signature, "PWAD", 4);
    target->numlumps = 0;
    target->offset = 0;
    target->totalmaps = 0;
    return target;
}

wadfile_t *wad_load(char* filename)
{
    wad = calloc(1, sizeof(wadfile_t));
    wad->totalmaps = 0;
    wad->maps = NULL;
    wad->table = NULL;
    wad->textures = NULL;
    //wad->textures->next = NULL;

    wad->f = fopen(filename, "rb");
    if (wad->f == NULL) {
        fprintf(stderr, "Couldn't open file %s\n", filename);
        wad_cleanup(wad);
        return NULL;
    }

    fread(wad->signature, 4, 1, wad->f);
    if (strncmp(wad->signature, "IWAD", 4)
          && strncmp(wad->signature, "PWAD", 4)) {
        fprintf(stderr, "File %s is not IWAD or PWAD\n", filename);
        wad_cleanup(wad);
        return NULL;
    }

    fread(&wad->numlumps, 4, 1, wad->f);
    if (wad->numlumps == 0) {
        fprintf(stderr, "File %s is empty WAD\n", filename);
        wad_cleanup(wad);
        return NULL;
    }

    fread(&wad->offset, 4, 1, wad->f);
    if (wad->offset == 0) {
        fprintf(stderr, "No pointer to WAD table in file %s", filename);
        wad_cleanup(wad);
        return NULL;
    }
    
    wad_loadlumps();
    wad_loadmaps();

    for (int i = 0; i < wad->totalmaps; i++) {
        wad_getmapflats(&wad->maps[i]);
        wad_getmaptextures(&wad->maps[i]);
    }

    wad_printmaps();

    printf("Loaded file %s, %.4s with %d lumps\n", filename, wad->signature, wad->numlumps);
    return wad;

}

static void wad_loadlumps()
{
    wad->table = list_init(sizeof(lump_t));
    fseek(wad->f, wad->offset, SEEK_SET);

    for (int i = 0; i < wad->numlumps; i++) {
        lump_t tmp;
        fread(&tmp, ENTRY_SIZE, 1, wad->f);
        list_add(wad->table, &tmp, sizeof(lump_t), 0);
    }


    //for (list_t *current = wad->table; current != NULL; current = current->next) {
    //    lump_t* lump = current->data;
    //    printf("%p %9d %9d %8.8s %p\n", current, lump->pos, lump->size, lump->name, current->next);
    //}
}


static void wad_loadmaps()
{
    list_t *current;
    lump_t *entry;
    
    for (current = wad->table; current != NULL; current = current->next) {
        entry = current->data;
        if (strncmp(entry->name, "MAP", 3) == 0) {
            list_t* rover;
            lump_t *tmp;
            doomlevel_t *mapdata = calloc(1, sizeof(doomlevel_t));

            rover = current->next;
            tmp = rover->data;
            for (int i = 0; i < 10; i++) {
                if (strncmp(tmp->name, "THINGS", 6) == 0)
                    goto LUMP_ADD;
                else if (strncmp(tmp->name, "LINEDEFS", 8) == 0)
                    goto LUMP_ADD;
                else if (strncmp(tmp->name, "SIDEDEFS", 8) == 0)
                    goto LUMP_ADD;
                else if (strncmp(tmp->name, "VERTEXES", 8) == 0)
                    goto LUMP_ADD;
                else if (strncmp(tmp->name, "SEGS", 4) == 0)
                    goto LUMP_ADD;
                else if (strncmp(tmp->name, "SSECTORS", 8) == 0)
                    goto LUMP_ADD;
                else if (strncmp(tmp->name, "NODES", 5) == 0)
                    goto LUMP_ADD;
                else if (strncmp(tmp->name, "SECTORS", 7) == 0)
                    goto LUMP_ADD;
                else if (strncmp(tmp->name, "REJECT", 6) == 0)
                    goto LUMP_ADD;
                else if (strncmp(tmp->name, "BLOCKMAP", 8) == 0)
                    goto LUMP_ADD;
                else
                    goto FAIL;

                LUMP_ADD:
                    memcpy(&mapdata->data[i], tmp, sizeof(lump_t));
                    rover = rover->next;
                    tmp = rover->data;
                    continue;

                FAIL:
                    free(mapdata);
                    mapdata = NULL;
                    fprintf(stderr, "Lump %.8s in position %4d is not a map\n", entry->name, entry->pos);
                    break;
            }
            if (mapdata != NULL) {
                wad->totalmaps++;
                mapdata->num = wad->totalmaps;
                wad->maps = realloc(wad->maps, sizeof(doomlevel_t) * wad->totalmaps);
                wad->maps[wad->totalmaps - 1] = *mapdata;
                fprintf(stderr, "Rename %.8s to MAP%.2d\n", entry->name, mapdata->num);
                free(mapdata);
            }
        }
    }
}

void wad_getmapflats(doomlevel_t *map)
{
    mapsector_t* sector;

    fseek(wad->f, map->data[SECTORS].pos, SEEK_SET);

    sector = calloc(1, map->data[SECTORS].size);
    fread(sector, map->data[SECTORS].size, 1, wad->f);
    
    map->flats = list_init();
    
    for (int i = 0; i < (map->data[SECTORS].size / sizeof(mapsector_t)); i++) {
        lump_t *floor = lump_find(wad->table, sector[i].floortexture);
        if (floor == NULL) {
            floor = lump_init(sector[i].floortexture);
            lump_add(map->flats, floor, 1);
            free(floor);
        } else {
            lump_add(map->flats, floor, 1);
        }
        lump_t *ceiling = lump_find(wad->table, sector[i].ceilingtexture);
        if (ceiling == NULL) {
            ceiling = lump_init(sector[i].ceilingtexture);
            lump_add(map->flats, ceiling, 1);
            free(ceiling);
        } else {
            lump_add(map->flats, ceiling, 1);
        }
    }
    free(sector);
}

void wad_getmaptextures(doomlevel_t *map)
{
    sidedef_t* sidedef;
    maptexture_t* tmp;

    fseek(wad->f, map->data[SIDEDEFS].pos, SEEK_SET);

    sidedef = calloc(1, map->data[SIDEDEFS].size);
    fread(sidedef, map->data[SIDEDEFS].size, 1, wad->f);
    
    map->textures = list_init();
    
    for (int i = 0; i < (map->data[SIDEDEFS].size / sizeof(sidedef_t)); i++) {
        tmp = tx_find(wad->textures, sidedef[i].lower);
        if (tmp == NULL) {
            tmp = tx_init(sidedef[i].lower);
            //printf("%.8s\n", tmp->name);
            tx_add(map->textures, tmp, 1);
            free(tmp);
        } else {
            tx_add(map->textures, tmp, 1);
        }
        tmp = tx_find(wad->textures, sidedef[i].middle);
        if (tmp == NULL) {
            tmp = tx_init(sidedef[i].middle);
            //printf("%.8s\n", tmp->name);
            tx_add(map->textures, tmp, 1);
            free(tmp);
        } else {
            tx_add(map->textures, tmp, 1);
        }
        tmp = tx_find(wad->textures, sidedef[i].upper);
        if (tmp == NULL) {
            tmp = tx_init(sidedef[i].upper);
            //printf("%.8s\n", tmp->name);
            tx_add(map->textures, tmp, 1);
            free(tmp);
        } else {
            tx_add(map->textures, tmp, 1);
        }
    }
    free(sidedef);
}

static void wad_printmaps()
{
    for (int i = 0; i < wad->totalmaps; i++) {
        fprintf(stderr, "MAP%.2d\n", wad->maps[i].num);
        for (int j = 0; j < 10; j++)
            fprintf(stderr, "%9.8s at pos %8d\n", wad->maps[i].data[j].name, wad->maps[i].data[j].pos);
        for (list_t* current = wad->maps[i].textures; current != NULL; current = current->next) {
            maptexture_t *entry = current->data;
            fprintf(stderr, "Texture: %.8s\n", entry->name);
        }
        for (list_t* current = wad->maps[i].flats; current != NULL; current = current->next) {
            lump_t *entry = current->data;
            fprintf(stderr, "Flat: %9.8s at pos %9.8d\n", entry->name, entry->pos);
        }
    }
}

void wad_cleanup(wadfile_t *archive)
{
    if (archive == NULL)
        return;
    

    if (archive->maps) {
        for (int i = 0; i < wad->totalmaps; i++) {
            list_cleanup(archive->maps[i].textures);
            list_cleanup(archive->maps[i].flats);
        }
        free(archive->maps);
    }
    list_cleanup(wad->table);

    if (archive->f)
        fclose(archive->f);

    free(archive);
    archive = NULL;
}
