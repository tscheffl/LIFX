#include <unistd.h>
#define CHECK(sts,msg) if((sts) == -1) {perror(msg);exit(-1);}

#define PORT 56700

/*Struct for LIFX Frame-Header*/
#pragma pack(push, 1)
typedef struct {
    /* frame */
    uint16_t size;
    uint16_t protocol:12;
    uint8_t  addressable:1;
    uint8_t  tagged:1;
    uint8_t  origin:2;
    uint32_t source;
    /* frame address */
    uint8_t  target[8];
    uint8_t  reserved[6];
    uint8_t  res:6;
    uint8_t  ack_required:1;
    uint8_t  res_required:1;
    uint8_t  sequence;
    /* protocol header */
    uint64_t resa:64;
    uint16_t type;
    uint16_t resb:16;
    /* variable length payload follows */
    
} lx_protocol_header_t;
#pragma pack(pop)


/*Data for different LIFX Payloads*/
/*SetColor - 102*/
#pragma pack(push, 1)
typedef struct {
    uint8_t  reserved;
    struct {
        uint16_t hue;
        uint16_t saturation;
        uint16_t brightness;
        uint16_t kelvin;
    };
    uint32_t duration;
} lx_color_t;
#pragma pack(pop)

/*SetPower - 21*/
uint16_t lx_powerlevel;	  
