#ifndef __acestruc_h
#define __acestruc_h

#include "declare.h"
#include "portable.h"

#define acesign_len           7
#define bytes_before_acesign  7
#define acever               10

struct tech
{
   UCHAR TYPE;
   UCHAR QUAL;
   USHORT PARM;
};

typedef struct tacehead
{
   USHORT HEAD_CRC;
   USHORT HEAD_SIZE;
   UCHAR  HEAD_TYPE;
   USHORT HEAD_FLAGS;
   ULONG  ADDSIZE;
   UCHAR  other[2048];

}  thead;

typedef struct tacemhead
{
   USHORT HEAD_CRC;
   USHORT HEAD_SIZE;
   UCHAR  HEAD_TYPE;
   USHORT HEAD_FLAGS;

   UCHAR  ACESIGN[acesign_len];
   UCHAR  VER_MOD;
   UCHAR  VER_CR;
   UCHAR  HOST_CR;
   UCHAR  VOL_NUM;
   ULONG  TIME_CR;
   USHORT RES1;
   USHORT RES2;
   ULONG  RES;
   UCHAR  AV_SIZE;
   UCHAR  AV[256];
   USHORT COMM_SIZE;
   UCHAR  COMM[2048];

}  tmhead;

#ifndef PATH_MAX
  #define PATH_MAX   512
#endif

typedef struct tacefhead
{
   USHORT HEAD_CRC;
   USHORT HEAD_SIZE;
   UCHAR  HEAD_TYPE;
   USHORT HEAD_FLAGS;

   ULONG  PSIZE;
   ULONG  SIZE;
   ULONG  FTIME;
   ULONG  ATTR;
   ULONG  CRC32;
   struct tech TECH;
   USHORT RESERVED;
   USHORT FNAME_SIZE;
   UCHAR  FNAME[PATH_MAX];
   USHORT COMM_SIZE;
   UCHAR  COMM[2048];

}  tfhead;

#define mhead    (*t_mhead)
#define fhead    (*t_fhead)
#define rhead    (*t_rhead)
#define tmpmhead (*t_tmpmhead)
#define tmpfhead (*t_tmpfhead)
#define tmprhead (*t_tmprhead)

#define mhead_size h_mhead_size(mhead)
#define fhead_size h_fhead_size(fhead)
#define rhead_size ((INT)&rhead.REC_CRC-(INT)&head.HEAD_TYPE+sizeof(rhead.REC_CRC))

#define h_mhead_size(phead) ((INT)mhead.AV-(INT)&head.HEAD_TYPE+        \
                    (phead).AV_SIZE+                                    \
                    ((phead).HEAD_FLAGS&ACE_COMM?                       \
                      sizeof((phead).COMM_SIZE)+H_MCOMM_SIZE(phead):0))


#define h_fhead_size(phead) ((INT)fhead.FNAME-(INT)&head.HEAD_TYPE+     \
                    (phead).FNAME_SIZE+                                 \
                    ((phead).HEAD_FLAGS&ACE_COMM?                       \
                      sizeof((phead).COMM_SIZE)+H_FCOMM_SIZE(phead):0))


//main-comment-definitions
#define H_MCOMM_SIZE(head) (BUF2WORD((UCHAR*)&(head).AV+(head).AV_SIZE))
#define MCOMM_SIZE         H_MCOMM_SIZE(mhead)
#define H_MCOMM(head)      ((UCHAR*)&(head).AV+(head).AV_SIZE+sizeof((head).COMM_SIZE))
#define MCOMM              H_MCOMM(mhead)


//file-comment-definitions
#define H_FCOMM_SIZE(head) (BUF2WORD((UCHAR*)&(head).FNAME+(head).FNAME_SIZE))
#define FCOMM_SIZE         H_FCOMM_SIZE(fhead)
#define H_FCOMM(head)      ((UCHAR*)&(head).FNAME+(head).FNAME_SIZE+sizeof((head).COMM_SIZE))
#define FCOMM              H_FCOMM(fhead)

//
#define calc_head_crc getcrc16(CRC_MASK, (CHAR*)&head.HEAD_TYPE, head.HEAD_SIZE)
#define GET_ADDSIZE   ((head.HEAD_FLAGS & ACE_ADDSIZE) ? head.ADDSIZE : 0)

//archive-header-flags
#define ACE_LIM256   1024
#define ACE_MULT_VOL 2048
#define ACE_AV       4096
#define ACE_RECOV    8192
#define ACE_LOCK    16384
#define ACE_SOLID   32768

//file-header-flags
#define ACE_ADDSIZE  1
#define ACE_PASSW    16384
#define ACE_SP_BEF   4096
#define ACE_SP_AFTER 8192
#define ACE_COMM     2

//block types
#define MAIN_BLK 0
#define FILE_BLK 1
#define REC_BLK  2

//known compression types
#define TYPE_STORE 0
#define TYPE_LZW1  1

//structure for archive handling
struct tadat
{
   INT   sol,
         vol,
         vol_num;
   ULONG time_cr;
};


#endif /* __acestruc_h */
