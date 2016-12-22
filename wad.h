wad_t*      w_open(const char*);
void        w_close(wad_t*);
wad_t*      w_create(const char* path);
int         w_loadtable(wad_t* wadfile, int32_t dirPos);
int         w_updateheader(wad_t* wadfile, int32_t dirPos);
int         w_mktable(wad_t* wadfile);

int         t_getindex(wad_t* wadfile, char name[8]);
int         t_push(wad_t* wadfile, lump_t* entry);
int         t_compare(lump_t* left, lump_t* right);
int         t_exists(wad_t* wadfile, lump_t* entry);
int         t_ismap(lump_t* entry);
int         t_isflag(lump_t* entry);
int         t_ismus(lump_t* entry);
int         t_flagtype(lump_t* entry);

int         m_setnum(lump_t* entry, int32_t num);
void        m_setmus(lump_t* entry, int32_t num);

int         l_load(wad_t*, lump_t*);
int         l_copy(wad_t*, wad_t*, lump_t*);
int         l_write(wad_t* wadfile, lump_t* target);
void        l_unload(lump_t*);
