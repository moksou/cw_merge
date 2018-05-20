maptexture_t* tx_init(str8_t name);
int tx_isempty(maptexture_t* tx);
void tx_add(list_t* table, maptexture_t* tx, int dup);
maptexture_t *tx_find(list_t* table, str8_t txname);
list_t* textures_parse(char *buffer);
void tx_loadpnames(wadfile_t *archive);
void tx_loadtextures(wadfile_t *archive);
void tx_cleanup(list_t *table);
char *tx_getpatchname(array_t *pnames, mappatch_t *patch);
