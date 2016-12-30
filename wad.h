void table_init(wadtable_t* table);
void table_close(wadtable_t* table);

void t_gettable(FILE** f, wadtable_t* table);
void t_cattable(wadtable_t* dst, wadtable_t* src, int type);
void t_getmaps(wadtable_t* table);
void t_getmusic(wadtable_t* table);
void t_getrangelumps(wadtable_t* table, int apptype);
void t_gettextures(wadtable_t* table);

void t_writetopos(wadtable_t* table, lump_t* lump, int32_t pos);
void t_push(wadtable_t* table, lump_t* lump);

int t_exists(wadtable_t* table, lump_t* lump);
int t_isflag(lump_t* l);
void t_initlump(lump_t* l, str8_t name, int32_t pos, int32_t size);

void t_setmapnum(lump_t* l, int num);
void t_setmusname(lump_t* entry, int32_t num);
int t_isequal(lump_t* l1, lump_t* l2);

char* l_load(lump_t* l);
void l_write(FILE* fp, lump_t* l);
void l_free(char* buffer);
