#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <malloc.h>

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

typedef struct
{
    int32_t      pos, size;
    str8_t       name;
    int8_t       type;
    void*        ref;
} lump_t;

typedef struct {
    int32_t      total;
    lump_t*      dir;
} wadtable_t;

typedef struct {
    int32_t      numpatches;
    str8_t*      names;
} pnames_t;

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
    int16_t      masked,
                 width,
                 height;
    int32_t      columndirectory;
    int          patchcount;
    mappatch_t*  patches;

} maptexture_t;

typedef struct {
    int32_t       numtextures;
    int32_t*      offset;
    maptexture_t* mtexture;
} texturex_t;

enum types {
    UNDEFINED = 0,
    MARKER,
    MAP,
    MUSIC,
    SPRITE,
    FLAT,
    TEXTUREx,
    PNAMES,
    TYPESNUM
};

extern const str8_t mus_order[32];
extern const str8_t map_order[10];
extern const str8_t flatmarkers[];
