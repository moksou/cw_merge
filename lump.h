lump_t* lump_init(str8_t name);
void lump_addtotable(lump_t* table, lump_t* lump, int16_t dup);
lump_t *lump_find(lump_t* table, str8_t lumpname);
void lump_deletefromtable(lump_t* table, lump_t* lump);
