#ifndef __uac_dcpr_h
#define __uac_dcpr_h

#include "declare.h"

INT  calc_dectabs(void);
void dcpr_comm(INT comm_size);
INT  read_wd(UINT maxwd, USHORT * code, UCHAR * wd, INT max_el);
void dcpr_init(void);
INT  dcpr_adds_blk(CHAR * buf, UINT len);
void dcpr_init_file(void);

#endif /* __uac_dcpr_h */

