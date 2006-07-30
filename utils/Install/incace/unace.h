#ifndef __unace_h
#define __unace_h


#include "declare.h"


//--------- functions

INT read_adds_blk(CHAR * buffer, INT len);


//--------- buffers: increase sizes when possible to speed up the program

#define size_rdb  1024
#define size_wrb  2048
#define size_buf  1024

#define size_headrdb (sizeof(head)-20) // (some bytes less esp. for Amiga)


//--------- (de-)compressor constants

#define maxdic      22
#define maxwd_mn    11
#define maxwd_lg    11
#define maxwd_svwd   7
#define maxlength  259
#define maxdis2    255
#define maxdis3   8191
#define maxcode   (255+4+maxdic)
#define svwd_cnt    15
#define max_cd_mn (256+4+(maxdic+1)-1)
#define max_cd_lg (256-1)

//--------- flags

#define ERR_MEM      1
#define ERR_FILES    2
#define ERR_FOUND    3
#define ERR_FULL     4
#define ERR_OPEN     5
#define ERR_READ     6
#define ERR_WRITE    7
#define ERR_CLINE    8
#define ERR_CRC      9
#define ERR_OTHER   10
#define ERR_USER   255

//--------- system things

#define flush fflush(stdout);

#define in(v1,v2,v3) ((((long)v1)>=(v2)) && (((long)v1)<=(v3)))

#define delay_len 500

//-------- file creation

#define fileexists(name) (!access(name, 0))


//-------- eof

#endif /* __unace_h */

