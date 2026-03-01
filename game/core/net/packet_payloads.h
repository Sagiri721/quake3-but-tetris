/**
 * @file        packet_payloads.h
 * @brief       Definr payloads for each packet
 */

// Packet payloads
typedef struct connect {
#define CONNECT_FIELDS(_F, ...)     \
    _F(username, __VA_ARGS__)
    const char* username; 
} connect_t;

typedef struct disconnect {
#define DISCONNECT_FIELDS(_F, ...)  \
    _F(username, __VA_ARGS__)
    const char* username; 
} disconnect_t;

typedef struct none {
#define NONE_FIELDS(_F, ...)
} none_t;
