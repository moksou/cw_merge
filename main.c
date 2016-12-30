#include "globaldefs.h"
#include "wad.h"

texturex_t  tx;
pnames_t    pn;

int         mapnum = 0,
            musnum = 0;


void dst_createfile(FILE** f, char* name);
void dst_updheader(FILE** f, int32_t num, int32_t pos);
void dst_writedata(FILE** f, wadtable_t* t);
void dst_mktable(FILE** f, wadtable_t* t);
void printtable(wadtable_t* table);

int main(int argc, char* argv[])
{
    FILE        *f[argc - 2],
                *fdst;

    wadtable_t  wadsrc[argc - 2],
                dst;
    wadtable_t  maps,
                music,
                sprites,
                flats,
                misc;


//    myalloc((char*)&dst.dir, sizeof(lump_t));
    table_init(&dst);
    table_init(&maps);
    table_init(&music);
    table_init(&sprites);
    table_init(&flats);
    table_init(&misc);

    tx.numtextures = 0;
    tx.offset = NULL;
    tx.mtexture = NULL;

    pn.numpatches = 0;
    pn.names = NULL;

    for (int i = 0; i < argc - 2; i++) {
        if ((f[i] = fopen(argv[i + 1], "rb")) == NULL)
            return 1;
        t_gettable(&f[i], &wadsrc[i]);
    }

//    printtable(&wadsrc[1]);
//    printf("\n");

    for (int i = 0; i < argc - 2; i++)
        for (int j = 0; j < wadsrc[i].total; j++) {
            lump_t* op = &wadsrc[i].dir[j];
            switch (op->type) {
            case MAP:
                if (op->size == 0)
                    t_setmapnum(op, ++mapnum);
                t_push(&maps, op);
                break;
            case MUSIC:
                ++musnum;
                if (t_exists(&music, op))
                    t_setmusname(op, musnum);
                t_push(&music, op);
                break;
            case FLAT:
                if (!t_exists(&flats, op))
                    t_push(&flats, op);
                break;
            case SPRITE:
                if (!t_exists(&sprites, op))
                    t_push(&sprites, op);
                break;
            case TEXTUREx:
//                tx_merge(&tx, op);
                break;
            case PNAMES:
                pnames_merge(&pn, op);
                break;
            default:
                if (!t_exists(&misc, op) && !t_isflag(op))
                    t_push(&misc, op);
                break;
            }
        }

    dst_createfile(&fdst, argv[argc - 1]);


    t_cattable(&dst, &maps, 0);
    t_cattable(&dst, &music, 0);
    t_cattable(&dst, &sprites, SPRITE);
    t_cattable(&dst, &flats, FLAT);
    t_cattable(&dst, &misc, 0);

    table_close(&maps);
    table_close(&music);
    table_close(&sprites);
    table_close(&flats);
    table_close(&misc);

    dst_updheader(&fdst, 0, ftell(fdst));
    dst_writedata(&fdst, &dst);
    dst_mktable(&fdst, &dst);

//    printtable(&dst);

    for (int i = 0; i < argc - 2; i++) {
        free(wadsrc[i].dir);
        fclose(f[i]);
    };

    free(pn.names);

    fclose(fdst);
    table_close(&dst);

    return 0;
}

void dst_createfile(FILE** f, char* name)
{
    *f = fopen(name, "wb+");
    if (*f == NULL)
        return;
    dst_updheader(f, 0, 0);
}

void dst_updheader(FILE** f, int32_t num, int32_t pos)
{
    fseek(*f, 0, SEEK_SET);
    fwrite("PWAD", 4, 1, *f);
    fwrite(&num, 4, 1, *f);
    fwrite(&pos, 4, 1, *f);
    fseek(*f, 0, SEEK_END);
}

void dst_writedata(FILE** f, wadtable_t* t)
{
    if (t->total == 0)
        return;

    fseek(*f, 0, SEEK_END);
//    tx_write(f, t, &tx);
    pn_write(f, t, &pn);

    for (int i = 0; i < t->total; i++) {
        l_write(*f, &t->dir[i]);
    }
}

void dst_mktable(FILE** f, wadtable_t* t)
{
    int32_t pos = 0;

    if (t->total == 0)
        return;

    fseek(*f, 0, SEEK_END);
    pos = ftell(*f);
    for (int i = 0; i < t->total; i++) {
        fwrite(&t->dir[i], 16, 1, *f);
    }

    dst_updheader(f, t->total, pos);
}

void printtable(wadtable_t* table)
{
    if (table->dir == NULL || table->total == 0)
        return;
    for (int i = 0; i < table->total; i++)
        fprintf(stdout, "%4d %9d %9d %9.8s\n", table->dir[i].type, table->dir[i].pos, table->dir[i].size, table->dir[i].name);
}
