#include "globaldefs.h"
#include "misc.h"
#include "wad.h"
#include "lump.h"
#include "textures.h"

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

static void wad_loadlumps();
static void wad_loadmaps();
static void wad_printmaps();
void wad_getmapgraphics(doomlevel_t *map);

wadfile_t* wad;

wadfile_t *wad_create()
{
    wadfile_t *target = calloc(1, sizeof(wadfile_t));

    target->f = NULL;
    memcpy(target->signature, "PWAD", 4);
    target->numlumps = 0;
    target->offset = 0;
    return target;
}

wadfile_t *wad_load(char* filename)
{
    wad = calloc(1, sizeof(wadfile_t));
    wad->totalmaps = 0;
    wad->maps = NULL;
    wad->table = lt_init();
    wad->textures = NULL;

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
    for (int i = 0; i < wad->totalmaps; i++)
        wad_getmapgraphics(&wad->maps[i]);
    tx_loadpnames(wad);
    tx_loadtextures(wad);
    wad_printmaps();
    
    for (int i = 0; i < wad->table->num; i++)
        printf("%4.4d %4.4d %.8s\n", wad->table->lump[i].pos,
                                     wad->table->lump[i].size,
                                     wad->table->lump[i].name);

    printf("Loaded file %s, %.4s with %d lumps\n", filename, wad->signature, wad->numlumps);
    return wad;

}

static void wad_loadlumps()
{
    lump_t entry;

    fseek(wad->f, wad->offset, SEEK_SET);

    for (int i = 0; i < wad->numlumps; i++) {
        fread(&entry, ENTRY_SIZE, 1, wad->f);
        entry.f = wad->f;
        
        lt_push(wad->table, &entry, 0);
    }

    entry.pos = 0;
    entry.size = 0;
    strncpy(entry.name, "TEST", 8);

}

static void wad_loadmaps()
{
    wad->maps = calloc(1, sizeof(doomlevel_t));

    for (uint32_t i = 0; i < wad->table->num; i++) {
        //printf("lump: %.8s\n", wad->table->lump[i].name);
        if (strncmp(wad->table->lump[i].name, "MAP", 3) == 0) {
            //printf("found map %.8s\n", wad->table->lump[i].name);
            int j;
            doomlevel_t *map = calloc(1, sizeof(doomlevel_t));
            map->data = lt_init();

            for (j = 1; j <= 10; j++) 
                if (strncmp(wad->table->lump[i + j].name, map_order[j - 1], 8) != 0)
                    break;

            if (j < 10)
                continue; /* lump is not a map */

            for (j = 1; j <= 10; j++) {
                //printf("catch lump %.8s\n", wad->table->lump[i + 1].name);
                lt_push(map->data, &wad->table->lump[i + 1], 0);
                lt_delete(wad->table, i + 1);
            }
            wad->totalmaps++;
            wad->maps = realloc(wad->maps, sizeof(doomlevel_t) * wad->totalmaps);
            wad->maps[wad->totalmaps - 1] = *map;
            free(map);
            lt_delete(wad->table, i);
        }
    }
}



void wad_getmapgraphics(doomlevel_t *map)
{
    sidedef_t *sidedef = calloc(1, sizeof(sidedef_t) * map->data->lump[2].size);
    mapsector_t *sector = calloc(1, sizeof(mapsector_t) * map->data->lump[7].size);

    fseek(wad->f, map->data->lump[2].pos, SEEK_SET);
    fread(sidedef, map->data->lump[2].size, 1, wad->f);

    fseek(wad->f, map->data->lump[7].pos, SEEK_SET);
    fread(sector, map->data->lump[7].size, 1, wad->f);
    
    map->textures = array_init();
    map->flats = array_init();
    
    for (int i = 0; i < (map->data->lump[2].size / sizeof(sidedef_t)); i++) {
        array_add(map->textures, sidedef[i].lower, 0, 1);
        array_add(map->textures, sidedef[i].middle, 0, 1);
        array_add(map->textures, sidedef[i].upper, 0, 1);
    }
    
    for (int i = 0; i < (map->data->lump[7].size / sizeof(mapsector_t)); i++) {
        array_add(map->flats, sector[i].floortexture, 0, 1);
        array_add(map->flats, sector[i].ceilingtexture, 0, 1);
    }

    free(sidedef);
    free(sector);
}

static void wad_printmaps()
{
    if (0) {
    for (int i = 0; i < wad->totalmaps; i++) {
        lumptable_t *entry = wad->maps[i].data;
        for (int j = 0; j < 10; j++) {
            printf("MAP %d: %4.4d %4.4d %.8s\n", i, entry->lump[j].pos, entry->lump[j].size, entry->lump[j].name);
        }
        printf("Textures:\n");
        for (int k = 0; k < wad->maps[i].textures->num; k++) {
            printf("%.8s ", wad->maps[i].textures->string[k]);
        }
        printf("\n");
        printf("Flats\n");
        for (int k = 0; k < wad->maps[i].flats->num; k++) {
            printf("%.8s ", wad->maps[i].flats->string[k]);
        }
        printf("\n");
    }
    }
    if (0) {
    for (int i = 0; i < wad->maps->flats->num; i++)
        printf("FLAT %.8s\n", wad->maps->flats->string[i]);
    }
    if (0) {
    for (list_t *current = wad->textures; current != NULL; current = current->next) {
        maptexture_t *entry = current->data;
        printf("%.8s:", entry->name);
        for (int i = 0 ; i < entry->patchcount; i++) {
            printf("%.8s ", tx_getpatchname(wad->patches, &entry->patches[i]));
        }
        printf("\n");
    }
    }
    if (0) {
    for (int i = 0; i < wad->patches->num; i++) {
        printf("%.8s\n", wad->patches->string[i]);
    }
    }
}

void wad_cleanup(wadfile_t *archive)
{
    if (archive == NULL)
        return;
    
    if (wad->textures)
        tx_cleanup(archive->textures);
    if (archive->maps) {
        for (int i = 0; i < wad->totalmaps; i++) {
            lt_cleanup(archive->maps[i].data);
            array_cleanup(archive->maps[i].textures);
            array_cleanup(archive->maps[i].flats);
        }
        free(archive->maps);
    }

    array_cleanup(wad->patches);
    lt_cleanup(wad->table);

    if (archive->f)
        fclose(archive->f);

    free(archive);
    archive = NULL;
}
