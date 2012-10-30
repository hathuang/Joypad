/* Protocol defined to connect between PC and mobile */

/* status or cmd define */
#define OKAY    5
#define FAIL    6
#define BUSY    7
#define CONF    8
#define CNTL    9
#define XCHG    10
#define SYNC    11


/*

message define :

msg     | <-- status or cmd --> | <-- length of arg below --> | <-- arg... --> |
bytes   | <--       4       --> | <--          4          --> | <--   x    --> |

*/
