/* Protocol defined to connect between PC and mobile */

/* status or cmd define */
#define OKAY            5
#define FAIL            6
#define BUSY            7
#define CONF            8
#define CNTL            9
#define XCHG            10
#define SYNC            11
#define UNKOWN          -1

#define CMD_OKAY        "OKAY"
#define CMD_FAIL        "FAIL"
#define CMD_BUSY        "BUSY"
#define CMD_CONF        "CONF"
#define CMD_CNTL        "CNTL"
#define CMD_XCHG        "XCHG"
#define CMD_SYNC        "SYNC"

#define MSG_SIZE        8
#define CMD_SIZE        4 

#define MSG2INT(x)              ((*(x)<<24) | (*((x)+1)<<16) | (*((x)+2)<<8) | *((x)+3))
#define INT2MSG(s, i)           do { \
                                        *(s+0) = (i>>0x18)&0xff; \
                                        *(s+1) = (i>>0x10)&0xff; \
                                        *(s+2) = (i>>0x08)&0xff; \
                                        *(s+3) = (i>>0x00)&0xff; \
                                } while (0)

/*

message define :

msg     | <-- status or cmd --> | <-- length of arg below --> | <-- arg... --> |
bytes   | <--       4       --> | <--          4          --> | <--   x    --> |

*/
