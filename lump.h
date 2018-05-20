lumptable_t* lt_init();
void lt_add(lumptable_t *lt, lump_t* lump, uint32_t index, size_t dup);
void lt_push(lumptable_t *lt, lump_t* lump, size_t dup);
lump_t *lt_findbyname(lumptable_t *lt, str8_t name);
int lt_findindex(lumptable_t *lt, str8_t name);
void lt_delete(lumptable_t *lt, uint32_t index);
void lt_cleanup(lumptable_t *lt);

void lump_add(list_t* table, lump_t* lump, int16_t dup);
void lump_delete(list_t *table, lump_t *lump);
lump_t *lump_find(list_t* table, str8_t lumpname);
void lump_deletefromtable(lump_t* table, lump_t* lump);
