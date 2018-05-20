#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <malloc.h>
#include <unistd.h>

#define ENTRY_SIZE 16

#define THING_BYTE_SIZE   10
#define LINEDEF_BYTE_SIZE 14
#define SIDEDEF_BYTE_SIZE 30
#define VERTEX_BYTE_SIZE   4

#define max(a, b) \
({ __typeof__ (a) _a = (a); \
   __typeof__ (b) _b = (b); \
 _a > _b ? _a : _b; })


#define min(a, b) \
({ __typeof__ (a) _a = (a); \
   __typeof__ (b) _b = (b); \
 _a < _b ? _a : _b; })

typedef char str8_t[8];

typedef struct list {
    void        *data;
    struct list *next;
} list_t;

typedef struct {
    int32_t     num;
    str8_t      *string;
} array_t;

typedef struct
{
    uint32_t     pos, size;
    str8_t       name;
    FILE         *f;
} lump_t;

typedef struct
{
    int32_t     num;
    lump_t      *lump;
} lumptable_t;


typedef struct
{
    int16_t      x, y,
                 angle,
                 doomednum,
                 flags;
} thing_t;

typedef struct
{
    int16_t      start_vertex,
                 end_vertex,
                 flags,
                 special,
                 sectortag,
                 front_sidedef,
                 back_sidedef;
} linedef_t;

typedef struct
{
    int16_t      x, y;
    str8_t       upper,
                 lower,
                 middle;
    int16_t      face_id;
} sidedef_t;

typedef struct {
    int16_t      originx,
                 originy,
                 patch,
                 stepdir,
                 colormap;
} mappatch_t;

typedef struct {
    str8_t       name;
    int32_t      masked;
    int16_t      width,
                 height;
    int32_t      columndirectory;
    int16_t      patchcount;
    mappatch_t   *patches;

} maptexture_t;

typedef struct {
    int16_t     floorheight,
                ceilingheight;
    str8_t      floortexture,
                ceilingtexture;
    int16_t     light,
                type,
                tag;
} mapsector_t;

typedef struct {
    int32_t        num;
    lumptable_t   *data;
    array_t       *textures,
                  *flats;
} doomlevel_t;

typedef struct {
    FILE         *f;
    char         signature[4];
    int32_t      numlumps,
                 offset,
                 totalmaps;
    lumptable_t  *table;
    doomlevel_t  *maps;
    list_t       *textures;
    array_t      *patches;
} wadfile_t;

#include "misc.h"
