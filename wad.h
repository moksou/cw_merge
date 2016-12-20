wad_t*      w_open(const char*);
void        w_close(wad_t*);
wad_t*      w_create(const char* path);
int         w_updateheader(wad_t* wadfile, int32_t dirPos);
int         w_mktable(wad_t* wadfile);

int         t_getindex(wad_t* wadfile, char name[8]);
int         t_push(wad_t* wadfile, lump_t* entry);
int         t_compare(lump_t* left, lump_t* right);
int         t_exists(wad_t* wadfile, lump_t* entry);
int         t_ismap(lump_t* entry);
void        t_rename(lump_t* entry, const char* newname);

int         l_load(wad_t*, lump_t*);
int         l_compare(lump_t* left, lump_t* right);
int         l_write(wad_t* wadfile, lump_t* target);
void        l_unload(lump_t*);
