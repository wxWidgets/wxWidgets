/* ------------------------------------------------------------------------ */
/*                                                                          */
/*      CRC-calculation routines.                                           */
/*                                                                          */
/* ------------------------------------------------------------------------ */

#include "uac_crc.h"


ULONG crctable[256];
ULONG rd_crc=0;


void make_crctable(void)   // initializes CRC table
{
	ULONG r,
	i,
	j;

	for (i = 0; i <= 255; i++)
	{
		for (r = i, j = 8; j; j--)
			r = (r & 1) ? (r >> 1) ^ CRCPOLY : (r >> 1);
		crctable[i] = r;
	}
}

// Updates crc from addr till addr+len-1
//
ULONG getcrc(ULONG crc, UCHAR * addr, INT len)
{
	while (len--)
		crc = crctable[(unsigned char) crc ^ (*addr++)] ^ (crc >> 8);
	return (crc);
}

