maptexture_t* tx_init(str8_t name);
int tx_isempty(maptexture_t* tx);
void tx_add(list_t* table, maptexture_t* tx, int dup);
maptexture_t *tx_find(list_t* table, str8_t txname);
