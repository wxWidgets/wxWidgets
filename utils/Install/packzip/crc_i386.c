/* crc_i386.c -- Microsoft 32-bit C/C++ adaptation of crc_i386.asm
 * Created by Rodney Brown from crc_i386.asm, modified by Chr. Spieler.
 * Last revised: 22-Mai-1998
 *
 * Original coded (in crc_i386.asm) and put into the public domain
 * by Paul Kienitz and Christian Spieler.
 *
 * Revised 06-Oct-96, Scott Field (sfield@microsoft.com)
 *   fixed to assemble with masm by not using .model directive which makes
 *   assumptions about segment alignment.  Also,
 *   avoid using loop, and j[e]cxz where possible.  Use mov + inc, rather
 *   than lodsb, and other misc. changes resulting in the following performance
 *   increases:
 *
 *      unrolled loops                NO_UNROLLED_LOOPS
 *      *8    >8      <8              *8      >8      <8
 *
 *      +54%  +42%    +35%            +82%    +52%    +25%
 *
 *   first item in each table is input buffer length, even multiple of 8
 *   second item in each table is input buffer length, > 8
 *   third item in each table is input buffer length, < 8
 *
 * Revised 02-Apr-97, Chr. Spieler, based on Rodney Brown (rdb@cmutual.com.au)
 *   Incorporated Rodney Brown's 32-bit-reads optimization as found in the
 *   UNIX AS source crc_i386.S. This new code can be disabled by defining
 *   the macro symbol NO_32_BIT_LOADS.
 *
 * Revised 12-Oct-97, Chr. Spieler, based on Rodney Brown (rdb@cmutual.com.au)
 *   Incorporated Rodney Brown's additional tweaks for 32-bit-optimized CPUs
 *   (like the Pentium Pro, Pentium II, and probably some Pentium clones).
 *   This optimization is controlled by the macro symbol __686 and is disabled
 *   by default. (This default is based on the assumption that most users
 *   do not yet work on a Pentium Pro or Pentium II machine ...)
 *
 * Revised 16-Nov-97, Chr. Spieler: Made code compatible with Borland C++
 *   32-bit, removed unneeded kludge for potentially unknown movzx mnemonic,
 *   confirmed correct working with MS VC++ (32-bit).
 *
 * Revised 22-Mai-98, Peter Kunath, Chr. Spieler : The 16-Nov-97 revision broke
 *   MSVC 5.0. Inside preprocessor macros, each instruction is enclosed in its
 *   own __asm {...} construct.  For MSVC, a "#pragma warning" was added to
 *   shut up the "no return value" warning message.
 *
 * FLAT memory model assumed.
 *
 * The loop unrolling can be disabled by defining the macro NO_UNROLLED_LOOPS.
 * This results in shorter code at the expense of reduced performance.
 *
 */

#include "zip.h"

#ifndef USE_ZLIB

#ifndef ZCONST
#  define ZCONST const
#endif

/* Select wether the following inline-assember code is supported. */
#if (defined(_MSC_VER) && _MSC_VER >= 700)
#if (defined(_M_IX86) && _M_IX86 >= 300)
#  define MSC_INLINE_ASM_32BIT_SUPPORT
   /* Disable warning for no return value, typical of asm functions */
#  pragma warning( disable : 4035 )
#endif
#endif

#if (defined(__BORLANDC__) && __BORLANDC__ >= 452)
#  define MSC_INLINE_ASM_32BIT_SUPPORT
#endif

#ifdef MSC_INLINE_ASM_32BIT_SUPPORT
/* This code is intended for Microsoft C/C++ (32-bit) compatible compilers. */

/*
 * These two (three) macros make up the loop body of the CRC32 cruncher.
 * registers modified:
 *   eax  : crc value "c"
 *   esi  : pointer to next data byte (or dword) "buf++"
 * registers read:
 *   edi  : pointer to base of crc_table array
 * scratch registers:
 *   ebx  : index into crc_table array
 *          (requires upper three bytes = 0 when __686 is undefined)
 */
#ifndef __686
#define Do_CRC { \
  __asm { mov   bl, al }; \
  __asm { shr   eax, 8 }; \
  __asm { xor   eax, [edi+ebx*4] }; }
#else /* __686 */
#define Do_CRC { \
  __asm { movzx ebx, al }; \
  __asm { shr   eax, 8  }; \
  __asm { xor   eax, [edi+ebx*4] }; }
#endif /* ?__686 */

#define Do_CRC_byte { \
  __asm { xor   al, byte ptr [esi] }; \
  __asm { inc   esi }; \
  Do_CRC; }

#ifndef NO_32_BIT_LOADS
#define Do_CRC_dword { \
  __asm { xor   eax, dword ptr [esi] }; \
  __asm { add   esi, 4 }; \
  Do_CRC; \
  Do_CRC; \
  Do_CRC; \
  Do_CRC; }
#endif /* !NO_32_BIT_LOADS */

/* ========================================================================= */
ulg crc32(crc, buf, len)
    ulg crc;                    /* crc shift register */
    ZCONST uch *buf;            /* pointer to bytes to pump through */
    extent len;                 /* number of bytes in buf[] */
/* Run a set of bytes through the crc shift register.  If buf is a NULL
   pointer, then initialize the crc shift register contents instead.
   Return the current crc in either case. */
{
    __asm {
                push    edx
                push    ecx

                mov     esi,buf         ;/* 2nd arg: uch *buf              */
                sub     eax,eax         ;/*> if (!buf)                     */
                test    esi,esi         ;/*>   return 0;                   */
                jz      fine            ;/*> else {                        */

                call    get_crc_table
                mov     edi,eax
                mov     eax,crc         ;/* 1st arg: ulg crc               */
#ifndef __686
                sub     ebx,ebx         ;/* ebx=0; => bl usable as a dword */
#endif
                mov     ecx,len         ;/* 3rd arg: extent len            */
                not     eax             ;/*>   c = ~crc;                   */

#ifndef NO_UNROLLED_LOOPS
#  ifndef NO_32_BIT_LOADS
                test    ecx,ecx
                je      bail
align_loop:
                test    esi,3           ;/* align buf pointer on next      */
                jz      aligned_now     ;/*  dword boundary                */
    }
                Do_CRC_byte             ;
    __asm {
                dec     ecx
                jnz     align_loop
aligned_now:
#  endif /* !NO_32_BIT_LOADS */
                mov     edx,ecx         ;/* save len in edx  */
                and     edx,000000007H  ;/* edx = len % 8    */
                shr     ecx,3           ;/* ecx = len / 8    */
                jz      No_Eights
; align loop head at start of 486 internal cache line !!
                align   16
Next_Eight:
    }
#  ifndef NO_32_BIT_LOADS
                Do_CRC_dword ;
                Do_CRC_dword ;
#  else /* NO_32_BIT_LOADS */
                Do_CRC_byte ;
                Do_CRC_byte ;
                Do_CRC_byte ;
                Do_CRC_byte ;
                Do_CRC_byte ;
                Do_CRC_byte ;
                Do_CRC_byte ;
                Do_CRC_byte ;
#  endif /* ?NO_32_BIT_LOADS */
    __asm {
                dec     ecx
                jnz     Next_Eight
No_Eights:
                mov     ecx,edx

#endif /* NO_UNROLLED_LOOPS */
#ifndef NO_JECXZ_SUPPORT
                jecxz   bail            ;/*>  if (len)                     */
#else
                test    ecx,ecx         ;/*>  if (len)                     */
                jz      bail
#endif
; align loop head at start of 486 internal cache line !!
                align   16
loupe:                                  ;/*>    do { */
    }
                Do_CRC_byte             ;/*       c = CRC32(c, *buf++);    */
    __asm {
                dec     ecx             ;/*>    } while (--len);           */
                jnz     loupe

bail:                                   ;/*> }                             */
                not     eax             ;/*> return ~c;                    */
fine:
                pop     ecx
                pop     edx
    }
#ifdef NEED_RETURN
    return _EAX;
#endif
}
#endif /* MSC_INLINE_ASM_32BIT_SUPPORT */
#if (defined(_MSC_VER) && _MSC_VER >= 700)
#if (defined(_M_IX86) && _M_IX86 >= 300)
   /* Reenable missing return value warning */
#  pragma warning( default : 4035 )
#endif
#endif
#endif /* !USE_ZLIB */
