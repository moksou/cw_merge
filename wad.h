wadfile_t *wad_load(char* filename);
static void wad_loadmaps();
static void wad_loadlumps();
void wad_getmapflats(doomlevel_t *map);
static void wad_printmaps();
void wad_cleanup(wadfile_t *archive);
