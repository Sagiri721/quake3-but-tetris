/**
 * @file        packets.h
 * @brief       Net packet with input data
 */

#include "../input.h"
#include "packet_payloads.h"
#include <stddef.h>

// thanks jdh, i hate it https://gist.github.com/jdah/1ae0048faa2c627f7f5cb1b68f7a2c02

// List of all packet types in this macro
// We seperate lower from upper case for consistent enum and 
// struct naming conventions thats purely aesthetics
/**
 * Each entry contains:
 *  - Uppercase name (for enums)
 *  - Lowercase name (for structs)
 *  - Packet id
 *  - Extra args for payload
 */ 
#define PACKET_TYPES_ITER(_F, ...)                          \
    _F(NONE,            none,           0, __VA_ARGS__)     \
    _F(CONNECT,         connect,        1, __VA_ARGS__)     \
    _F(DISCONNECT,      disconnect,     2, __VA_ARGS__)

#define DECL_TYPES_ENUM_MEMBER(uc, lc, i, ...)  \
    PACKET_TYPE_##uc = i,

// The enum with all dynamic packet types
typedef enum {
    PACKET_TYPES_ITER(DECL_TYPES_ENUM_MEMBER)
} packet_type;

// Now this makes a big ass union using each packet type using the type bellow as a descriminator
// For later identification
typedef struct {
    union 
    {
#define PACKET_TYPES_UNION_MEMBER(uc, lc, ...) lc##_t lc;
        PACKET_TYPES_ITER(PACKET_TYPES_UNION_MEMBER)
    };
    
    packet_type type;
} packet_types_t;

typedef enum field_type {
    FIELD_TYPE_INT,
    FIELD_TYPE_FLOAT,
    FIELD_TYPE_STR,
} field_type_t;


// Convert primitive type to enum type
#define TYPE_TO_FIELD_TYPE(x) _Generic(*((x*) NULL),    \
    int: FIELD_TYPE_INT,                                \
    float: FIELD_TYPE_FLOAT,                            \
    const char*: FIELD_TYPE_STR                         \
)

// Now this is some funny reflection overhead struct thing
// It stores the name of an argument, its data type, its size and offset on the struct
typedef struct field_desc
{
    field_type_t type;
    const char* name;
    int offset, size;
} field_desc_t;

typedef struct type_desc
{
    field_desc_t fields[16];
} type_desc_t;

// then we can use the above macros to autogenerate type info (!) which can be
// used for all sorts of stuff, including serialization!

#define TYPE_OF_FIELD(parent, field) __typeof__(((parent*) NULL)->field)

// Auto generate field description
#define DO_FIELD_DESC(fname, pname)                             \
    {                                                           \
        .type = TYPE_TO_FIELD_TYPE(TYPE_OF_FIELD(pname, fname)),\
        .name = #fname,                                         \
        .size = sizeof(TYPE_OF_FIELD(pname, fname)),            \
        .offset = offsetof(pname, fname),                       \
    },

#define DO_TYPE_DESC(uc, lc, i, ...)                            \
    [i] = { .fields = {uc##_FIELDS(DO_FIELD_DESC, lc##_t)} },

// This array will now have all type descriptions automatically generated for all
// packet types:
/**
 * types[PACKET_TYPE_CONNECT]
    fields[0] = {
        type   = FIELD_TYPE_STR
        name   = "username"
        size   = 8
        offset = 0
    }
 */
const type_desc_t types[] = {
    PACKET_TYPES_ITER(DO_TYPE_DESC)
};

/**
 * What's sent to the server, the time the event was registered 
 * and what event it was
 */
typedef struct {
    // Client identification
    unsigned int id; 
    int time;
    input_event_type input;
} input_packet;