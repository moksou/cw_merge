#include "globaldefs.h"
#include "wad.h"


int main(int argc, char* argv[])
{
    int wadnum = argc - 1;
    wad_t* wads[wadnum];

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
            l_load(wads[i], op);

            if (t_ismap(op)) {
                for (int m = j++; m < j + 10; m++) {
                    l_load(wads[i], &wads[i]->dir[m]);
                    t_push(wads[wadnum - 1], &wads[i]->dir[m]);
                    l_unload(&wads[i]->dir[m]);
                }
                j += 11;
            } else {
                t_push(wads[wadnum - 1], &wads[i]->dir[j]);
                l_unload(op);
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
