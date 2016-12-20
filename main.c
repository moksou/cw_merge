#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
//#include <regex.h>

#include "globaldefs.h"
#include "wad.h"



int main(int argc, char* argv[])
{
    int wadnum = argc - 1;
    wad_t* wads[wadnum];

//    regex_t rxp_map;

//    regcomp(&rxp_map, "MAP[0-9]{2}|E[0-9]M[0-9]", REG_EXTENDED);

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

    for (int i = 0; i < wadnum - 1; i++)
        for (int j = 0; j < wads[i]->lumpCount; j++) {
            lump_t* op = &wads[i]->dir[j];

            l_load(wads[i], op);

            if (!t_exists(wads[wadnum - 1], op)) {
                t_push(wads[wadnum - 1], op);
            } else {
                if (t_ismap(op)) {
                    for (int m = j; m <= j + 10; m++, j++)
                        t_push(wads[wadnum - 1], &wads[i]->dir[m]);
                }
            }
            l_unload(op);
        }

    for (int i = 0; i < wads[wadnum - 1]->lumpCount; i++)
        printf("%4d ===%.8s\n", i, wads[wadnum - 1]->dir[i].name);

    w_mktable(wads[wadnum - 1]);

    for (int i = 0; i < wadnum; i++)
        w_close(wads[i]);
    return 0;
}
