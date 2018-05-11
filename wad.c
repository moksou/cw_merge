#include "globaldefs.h"
#include "wad.h"
#include "lump.h"

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

wadfile_t* wad;

wadfile_t *wad_load(char* filename)
{
    wad = calloc(1, sizeof(wadfile_t));
    wad->totalmaps = 0;
    wad->maps = NULL;
    wad->f = fopen(filename, "rb");
    if (wad->f == NULL) {
        fprintf(stderr, "Couldn't open file %s\n", filename);
        return NULL;
    }

    fread(wad->signature, 4, 1, wad->f);
    if (strncmp(wad->signature, "IWAD", 4)
          &&strncmp(wad->signature, "PWAD", 4)) {
        fprintf(stderr, "File %s is not IWAD or PWAD\n", filename);
        fclose(wad->f);
        return NULL;
    }

    fread(&wad->numlumps, 4, 1, wad->f);
    fread(&wad->offset, 4, 1, wad->f);
    
    wad_loadlumps();
    wad_loadmaps();

    for (int i = 0; i < wad->totalmaps; i++)
        wad_getmapflats(&wad->maps[i]);

    wad_printmaps();

    //for (lump_t* current = wad->table; current != NULL; current = current->next) 
    //    printf("%d %d %.8s\n", current->pos, current->size, current->name);

    printf("Loaded file %s, %.4s with %d lumps\n", filename, wad->signature, wad->numlumps);
    return wad;

}

static void wad_loadlumps()
{
    wad->table = calloc(1, sizeof(lump_t));

    fseek(wad->f, wad->offset, SEEK_SET);

    for (int i = 0; i < wad->numlumps; i++) {
        lump_t* tmp = calloc(1, sizeof(lump_t));
        fread(tmp, ENTRY_SIZE, 1, wad->f);
        lump_addtotable(wad->table, tmp, 0);
        free(tmp);
    }

    //for (int i = 0; i < wad->numlumps; i++) {
    //    printf("-%d %d %d %.8s %d\n", current, current->pos, current->size, current->name, current->next);
    //    current = current->next;
    //}
}

static void wad_loadmaps()
{
    lump_t *current;
    
    for (current = wad->table; current != NULL; current = current->next) {
        if (strncmp(current->name, "MAP", 3) == 0) {
            lump_t *tmp;
            doomlevel_t *mapdata = calloc(1, sizeof(doomlevel_t));

            tmp = current->next;
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
                    mapdata->data[i] = *tmp;
                    mapdata->data[i].next = NULL;
                    tmp = tmp->next;
                    continue;

                FAIL:
                    free(mapdata);
                    mapdata = NULL;
                    fprintf(stderr, "Lump %.8s in position %4d is not a map\n", current->name, current->pos);
                    break;
            }
            if (mapdata != NULL) {
                wad->totalmaps++;
                mapdata->num = wad->totalmaps;
                wad->maps = realloc(wad->maps, sizeof(doomlevel_t) * wad->totalmaps);
                wad->maps[wad->totalmaps - 1] = *mapdata;
                fprintf(stderr, "Rename %.8s to MAP%.2d\n", current->name, mapdata->num);
                free(mapdata);
            }
        }
    }
}

void wad_getmapflats(doomlevel_t *map)
{
    mapsector_t* buffer;

    fseek(wad->f, map->data[SECTORS].pos, SEEK_SET);

    buffer = calloc(1, map->data[SECTORS].size);
    fread(buffer, map->data[SECTORS].size, 1, wad->f);
    
    map->flats = calloc(1, sizeof(lump_t));
    
    for (int i = 0; i < (map->data[SECTORS].size / sizeof(mapsector_t)); i++) {
        /* floors */
        lump_t* tmp = lump_find(wad->table, buffer[i].floortexture);
        if (tmp == NULL) {
            tmp = lump_init(buffer[i].floortexture);
            lump_addtotable(map->flats, tmp, 1);
            free(tmp);
        } else {
            lump_addtotable(map->flats, tmp, 1);
        }

        /* ceilings */
        tmp = lump_find(wad->table, buffer[i].ceilingtexture);
        if (tmp == NULL) {
            tmp = lump_init(buffer[i].ceilingtexture);
            lump_addtotable(map->flats, tmp, 1);
            free(tmp);
        } else {
            lump_addtotable(map->flats, tmp, 1);
        }
    }
    free(buffer);
}

static void wad_printmaps()
{
    for (int i = 0; i < wad->totalmaps; i++) {
        fprintf(stderr, "MAP%.2d\n", wad->maps[i].num);
        for (int j = 0; j < 10; j++)
            fprintf(stderr, "%9.8s at pos %8d\n", wad->maps[i].data[j].name, wad->maps[i].data[j].pos);
        for (lump_t* current = wad->maps[i].flats; current != NULL; current = current->next) {
            fprintf(stderr, "Flat: %.8s\n", current->name);
        }
    }
}

void wad_cleanup(wadfile_t *archive)
{
    lump_t *current, *tmp;

    //printf("cleanup\n");

    for (int i = 0; i < archive->totalmaps; i++) {
        for (current = archive->maps[i].flats; current != NULL;) {
            tmp = current;
            current = current->next;
            free(tmp);
        }
    }
    free(archive->maps);

    for (current = archive->table; current != NULL;) {
        tmp = current;
        current = current->next;
        free(tmp);
    }
    fclose(archive->f);
    free(archive);
    archive = NULL;
}
