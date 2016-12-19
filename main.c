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

    wad_t* reswad;
//    regex_t rxp_map;

//    regcomp(&rxp_map, "MAP[0-9]{2}|E[0-9]M[0-9]", REG_EXTENDED);

    if (argc < 3) {
        fprintf(stderr, "usage: %s [wad 1] [wad 2]\n", argv[0]);
        return 1;
    }
    for (int i = 0; i < wadnum; i++)
        if ((wads[i] = w_open(argv[i + 1])) == NULL)
            return 1;
    if ((reswad = w_create("/home/moksou/tmp/res.wad")) == NULL)
        return 1;
//    if((reswad = w_open("/tmp/wad1.wad")) == NULL)
//       return 2;
//    for (int i = 0; i < wads[0]->lumpCount; i++)
//        printf("%4d ---%.8s\n", i, wads[0]->dir[i].name);
//    for (int i = 0; i < wads[1]->lumpCount; i++)
//        printf("%4d +++%.8s\n", i, wads[1]->dir[i].name);
//    if (l_load(wads[1], &wads[0]->dir[13]) != 0)
//        return 6;
    for (int i = 0; i < argc - 1; i++)
        for (int j = 0; j < wads[i]->lumpCount; j++) {
            lump_t* op = &wads[i]->dir[j];

            l_load(wads[i], op);

            if (!t_exists(reswad, op)) {
                t_push(reswad, op);
            } else {
                if (t_ismap(op)) {
                    for (int m = j; m <= j + 10; m++, j++)
                        t_push(reswad, &wads[i]->dir[m]);
                }
            }
            l_unload(op);
        }

    for (int i = 0; i < reswad->lumpCount; i++)
        printf("%4d ===%.8s\n", i, reswad->dir[i].name);

    for (int i = 0; i < wadnum; i++)
        w_close(wads[i]);
    w_mktable(reswad);
    w_close(reswad);
    return 0;
}
