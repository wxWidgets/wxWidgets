#   pragma warning(disable:4001)    /* non standard extension used: single line comment */
#include "wx/setup.h"
#include <math.h>

#if wxUSE_APPLE_IEEE

/*
 * C O N V E R T   T O	 I E E E   E X T E N D E D
 */

/* Copyright (C) 1988-1991 Apple Computer, Inc.
 * All rights reserved.
 *
 * Machine-independent I/O routines for IEEE floating-point numbers.
 *
 * NaN's and infinities are converted to HUGE_VAL or HUGE, which
 * happens to be infinity on IEEE machines.	 Unfortunately, it is
 * impossible to preserve NaN's in a machine-independent way.
 * Infinities are, however, preserved on IEEE machines.
 *
 * These routines have been tested on the following machines:
 *    Apple Macintosh, MPW 3.1 C compiler
 *    Apple Macintosh, THINK C compiler
 *    Silicon Graphics IRIS, MIPS compiler
 *    Cray X/MP and Y/MP
 *    Digital Equipment VAX
 *
 *
 * Implemented by Malcolm Slaney and Ken Turkowski.
 *
 * Malcolm Slaney contributions during 1988-1990 include big- and little-
 * endian file I/O, conversion to and from Motorola's extended 80-bit
 * floating-point format, and conversions to and from IEEE single-
 * precision floating-point format.
 *
 * In 1991, Ken Turkowski implemented the conversions to and from
 * IEEE double-precision format, added more precision to the extended
 * conversions, and accommodated conversions involving +/- infinity,
 * NaN's, and denormalized numbers.
 */

#ifndef HUGE_VAL
#define HUGE_VAL HUGE
#endif /*HUGE_VAL*/

#define FloatToUnsigned(f) ((unsigned long) (((long) (f - 2147483648.0)) + 2147483647L) + 1)

void ConvertToIeeeExtended(double num, unsigned char *bytes)
{
	int				sign;
	int				expon;
	double			fMant, fsMant;
	unsigned long	hiMant, loMant;

	if (num < 0) {
		sign = 0x8000;
		num *= -1;
	} else {
		sign = 0;
	}

	if (num == 0) {
		expon = 0; hiMant = 0; loMant = 0;
	}
	else {
		fMant = frexp(num, &expon);
		if ((expon > 16384) || !(fMant < 1)) {	  /* Infinity or NaN */
			expon = sign|0x7FFF; hiMant = 0; loMant = 0; /* infinity */
		}
		else {	  /* Finite */
			expon += 16382;
			if (expon < 0) {	/* denormalized */
				fMant = ldexp(fMant, expon);
				expon = 0;
			}
			expon |= sign;
			fMant = ldexp(fMant, 32);
			fsMant = floor(fMant);
			hiMant = FloatToUnsigned(fsMant);
			fMant = ldexp(fMant - fsMant, 32);
			fsMant = floor(fMant);
			loMant = FloatToUnsigned(fsMant);
		}
	}

    /* disable the warning about 'possible loss of data' & 'conversion between diff types' */
    #ifdef _MSC_VER
        #pragma warning(disable: 4244)
        #pragma warning(disable: 4135)
    #endif /* Visual C++ */

	bytes[0] = (expon >> 8) & 0xff;
	bytes[1] = expon & 0xff;
	bytes[2] = (unsigned char) ((hiMant >> 24) & 0xff);
	bytes[3] = (unsigned char) ((hiMant >> 16) & 0xff);
	bytes[4] = (unsigned char) ((hiMant >> 8) & 0xff);
	bytes[5] = (unsigned char) (hiMant & 0xff);
	bytes[6] = (unsigned char) ((loMant >> 24) & 0xff);
	bytes[7] = (unsigned char) ((loMant >> 16) & 0xff);
	bytes[8] = (unsigned char) ((loMant >> 8) & 0xff);
	bytes[9] = (unsigned char) (loMant & 0xff);

    #ifdef _MSC_VER
        #pragma warning(default: 4244)
        #pragma warning(default: 4135)
    #endif /* Visual C++ */
}

/*
 * C O N V E R T   F R O M	 I E E E   E X T E N D E D
 */

/*
 * Copyright (C) 1988-1991 Apple Computer, Inc.
 * All rights reserved.
 *
 * Machine-independent I/O routines for IEEE floating-point numbers.
 *
 * NaN's and infinities are converted to HUGE_VAL or HUGE, which
 * happens to be infinity on IEEE machines.	 Unfortunately, it is
 * impossible to preserve NaN's in a machine-independent way.
 * Infinities are, however, preserved on IEEE machines.
 *
 * These routines have been tested on the following machines:
 *	  Apple Macintosh, MPW 3.1 C compiler
 *	  Apple Macintosh, THINK C compiler
 *	  Silicon Graphics IRIS, MIPS compiler
 *	  Cray X/MP and Y/MP
 *	  Digital Equipment VAX
 *
 *
 * Implemented by Malcolm Slaney and Ken Turkowski.
 *
 * Malcolm Slaney contributions during 1988-1990 include big- and little-
 * endian file I/O, conversion to and from Motorola's extended 80-bit
 * floating-point format, and conversions to and from IEEE single-
 * precision floating-point format.
 *
 * In 1991, Ken Turkowski implemented the conversions to and from
 * IEEE double-precision format, added more precision to the extended
 * conversions, and accommodated conversions involving +/- infinity,
 * NaN's, and denormalized numbers.
 */

#ifndef HUGE_VAL
# define HUGE_VAL HUGE
#endif /*HUGE_VAL*/

# define UnsignedToFloat(u) (((double) ((long) (u - 2147483647L - 1))) + 2147483648.0)

/****************************************************************
 * Extended precision IEEE floating-point conversion routine.
 ****************************************************************/

double ConvertFromIeeeExtended(const unsigned char *bytes)
{
	double			f;
	int				expon;
	unsigned long	hiMant, loMant;

	expon = ((bytes[0] & 0x7F) << 8) | (bytes[1] & 0xFF);
	hiMant = ((unsigned long)(bytes[2] & 0xFF) << 24)
		| ((unsigned long) (bytes[3] & 0xFF) << 16)
		| ((unsigned long) (bytes[4] & 0xFF) << 8)
		| ((unsigned long) (bytes[5] & 0xFF));
	loMant = ((unsigned long) (bytes[6] & 0xFF) << 24)
		| ((unsigned long) (bytes[7] & 0xFF) << 16)
		| ((unsigned long) (bytes[8] & 0xFF) << 8)
		| ((unsigned long) (bytes[9] & 0xFF));

	if (expon == 0 && hiMant == 0 && loMant == 0) {
		f = 0;
	}
	else {
		if (expon == 0x7FFF) {	  /* Infinity or NaN */
			f = HUGE_VAL;
		}
		else {
			expon -= 16383;
			f  = ldexp(UnsignedToFloat(hiMant), expon-=31);
			f += ldexp(UnsignedToFloat(loMant), expon-=32);
		}
	}

	if (bytes[0] & 0x80)
		return -f;
	else
		return f;
}

#endif /* wxUSE_APPLE_IEEE */
