#ifndef __uac_crc_h
#define __uac_crc_h


#include "declare.h"

#define CRC_MASK 0xFFFFFFFFL
#define CRCPOLY  0xEDB88320L


extern ULONG crctable[256];
extern ULONG rd_crc;


ULONG getcrc(ULONG crc, UCHAR * addr, INT len);
void  make_crctable(void);


#endif /* __uac_crc_h */

