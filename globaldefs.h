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


typedef struct list
{
    uint32_t      pos, size;
    str8_t       name;
    struct list     *next;
} lump_t;





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

typedef struct t {
    str8_t       name;
    int16_t      masked,
                 width,
                 height;
    int32_t      columndirectory;
    int          patchcount;
    mappatch_t   *patches;
    struct t     *next;

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
    int32_t       numtextures;
    int32_t*      offset;
    maptexture_t* mtexture;
} texturex_t;

typedef struct {
    int32_t        num;
    lump_t         data[10];
    maptexture_t  *textures;
    lump_t        *patches;
    lump_t        *flats;
} doomlevel_t;


typedef struct {
    FILE         *f;
    char         signature[4];
    int32_t      numlumps,
                 offset,
                 totalmaps;
    lump_t       *table;
    doomlevel_t  *maps;
} wadfile_t;

enum mapdatalumps {
    THINGS = 0,
    LINEDEFS,
    SIDEDEFS,
    VERTEXES,
    SEGS,
    SSECTORS,
    NODES,
    SECTORS,
    REJECT,
    BLOCKMAP
};

extern const str8_t mus_order[32];
extern const str8_t map_order[10];
extern const str8_t flatmarkers[];
