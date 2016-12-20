#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <malloc.h>

#define LPOS_LENGTH  4
#define LSIZE_LENGTH 4
#define LNAME_LENGTH 8

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



typedef struct
{
    int32_t      pos, size;
    char         name[8];
    char*        buffer;
    int          type;
} lump_t;

typedef struct
{
    FILE*        fd;
    char         type[4];
    int32_t      lumpCount;
    lump_t*      dir;
} wad_t;


typedef struct
{
    char         *things,
                 *linedefs,
                 *sidedefs,
                 *vertexes,
                 *segs,
                 *ssectors,
                 *nodes,
                 *sectors,
                 *reject,
                 *blockmap;
} map_t;

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
    char         upper[8],
                 lower[8],
                 middle[8];
    int16_t      face_id;
} sidedef_t;

enum {
    UNDEFINED,
    MARKER,
    THINGS,
    LINEDEFS,
    SIDEDEFS,
    VERTEXES,
    SEGS,
    SSECTORS,
    NODES,
    SECTORS,
    REJECT,
    BLOCKMAP,
    TEXTUREx,
    PNAMES,
    PATCH,
    FLAT,
    SPRITE
};
