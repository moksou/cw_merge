#include "globaldefs.h"
#include "wad.h"


int main(int argc, char* argv[])
{
    int wadnum = argc - 1;
    wad_t* wads[wadnum];

    int mapnum = 0;

    if (argc < 3) {
        fprintf(stderr, "usage: %s source_wad_1 ... source_wad_n destination_wad\n", argv[0]);
        return 1;
    }
    for (int i = 0; i < wadnum - 1; i++)
        if ((wads[i] = w_open(argv[i + 1])) == NULL) {
            fprintf(stderr, "%s: couldn't open wad %s\n", argv[0], argv[i + 1]);
            return 1;
        }
    if ((wads[wadnum - 1] = w_create(argv[wadnum])) == NULL) {
        fprintf(stderr, "%s: couldn't create destination wad %s\n", argv[0], argv[wadnum]);
        return 1;
    }
//    printtable(wads[0]);

    for (int i = 0; i < wadnum - 1; i++)
        for (int j = 0; j < wads[i]->lumpCount; j++) {
            lump_t* op = &wads[i]->dir[j];

            if (t_ismap(op)) {
                m_setnum(op, ++mapnum);
                for (int maplump = j; maplump < j + BLOCKMAP; maplump++)
                    c_copy(wads[wadnum - 1], wads[i], &wads[i]->dir[maplump]);
                j += 11;
            } else if (!t_exists(wads[wadnum - 1], op)) {
                c_copy(wads[wadnum - 1], wads[i], op);
            }
        }
    w_mktable(wads[wadnum - 1]);
    printtable(wads[wadnum - 1]);

    for (int i = 0; i < wadnum; i++)
        w_close(wads[i]);
    return 0;
}

void printtable(wad_t* wadfile)
{
    for (int i = 0; i < wadfile->lumpCount; i++)
        fprintf(stdout, "%3d %9.8s\n", wadfile->dir[i].type, wadfile->dir[i].name);
}
