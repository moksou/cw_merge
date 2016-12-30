#include "globaldefs.h"

int pnames_merge(pnames_t* pn, lump_t* l)
{
    pnames_t src;
    src.numpatches = 0;
    myalloc((str8_t*)&src.names, l->size - 4);
    fseek(l->ref, l->pos, SEEK_SET);

    fread(&src.numpatches, 4, 1, l->ref);
    fread(src.names, l->size - 4, 1, l->ref);
    for (int i = 0; i < src.numpatches; i++) {
        if (!name_exist(&src.names[i], pn->names, pn->numpatches)) {
            pn->names = realloc(pn->names, (pn->numpatches + 1) * sizeof(str8_t));
            memcpy(pn->names[pn->numpatches], &src.names[i], 8);
            pn->numpatches++;
        }
    }
    free(src.names);
    return 0;
}

//int tx_merge(texturex_t* tx, lump_t* l)
//{
//    texturex_t src;
//
//    src.mtexture = NULL;
//
//    fseek(l->ref, l->pos, SEEK_SET);
//    fread(&src.numtextures, 4, 1, l->ref);
//
//    myalloc((int32_t*)&src.offset, sizeof(int32_t) * src.numtextures);
//    fread(src.offset, sizeof(int32_t), src.numtextures, l->ref);
//
//    for (int i = 0; i < src.numtextures; i++) {
//        src.mtexture = realloc(src.mtexture, sizeof(maptexture_t) * (i + 1));
//        fseek(l->ref, l->pos + src.offset[i], SEEK_SET);
//        fread(&src.mtexture[i], 8 + 4 + 2 + 2 + 4 + 2, 1, l->ref);
//        myalloc((mappatch_t*)&src.mtexture[i].patches, src.mtexture[i].patchcount * 10);
//        fread(src.mtexture[i].patches, 10, src.mtexture[i].patchcount, l->ref);
////        printf("%.8s\n", src.mtexture[i].name);
//    }
//
//    for (int i = 0; i < src.numtextures; i++) {
//        maptexture_t* op = &src.mtexture[i];
//        if (!tx_exist(tx, op)) {
//            tx->offset = realloc(tx->offset, 4 * (tx->numtextures + 1));
//            tx->mtexture = realloc(tx->mtexture, sizeof(maptexture_t) * (tx->numtextures + 1));
//
//            tx->offset[tx->numtextures] = tx->offset[tx->numtextures - 1] + sizeof(mappatch_t) * op->patchcount + sizeof(maptexture_t);
//
//            tx->mtexture[tx->numtextures] = *op;
//            tx->mtexture[tx->numtextures].patches = op->patches;
//
//            tx->numtextures++;
//        }
//    }
//
//    return 0;
//}

int tx_exist(texturex_t* table, maptexture_t* t)
{
    for (int i = 0; i < table->numtextures; i++) {
        maptexture_t* op = &table->mtexture[i];
        if (strncmp(op->name, t->name, 8) == 0
                && op->width == t->width
                && op->height == t->height
                && op->masked == t->masked
                && op->columndirectory == t->columndirectory
                && op->patchcount == t->patchcount)
            return 1;
    }
    return 0;
}

void pn_write(FILE** f, wadtable_t* dst, pnames_t* pn)
{
    lump_t op;
    int32_t size = sizeof(str8_t) * pn->numpatches + 4;
    char* buf;

    strncpy(op.name, "PNAMES", 8);
    op.pos = ftell(*f);
    op.size = size;
    op.ref = NULL;
    op.type = UNDEFINED;
//    t_initlump(op, "PNAMES", 0, size);
    myalloc((char*)&buf, size);
    t_writetopos(dst, &op, 0);
    memcpy(&buf[0], &pn->numpatches, 4);
    memcpy(&buf[4], pn->names, size - 4);
    fwrite(buf, size, 1, *f);
    free(buf);
}
