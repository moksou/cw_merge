wad_t*      w_open(const char*);
void        w_close(wad_t*);
int w_updateheader(wad_t* wadfile, int32_t dirPos);
int w_mktable(wad_t* wadfile);

int t_push(wad_t* wadfile, lump_t* entry);
int t_compare(lump_t* left, lump_t* right);

int l_compare(lump_t* left, lump_t* right);
int l_load(wad_t*, lump_t*);
void l_unload(lump_t*);
