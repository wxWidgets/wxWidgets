/*
 * crc_i386.S, optimized CRC calculation function for Zip and UnZip, not
 * copyrighted by Paul Kienitz and Christian Spieler.  Last revised 12 Oct 97.
 *
 * GRR 961110:  incorporated Scott Field optimizations from win32/crc_i386.asm
 *              => overall 6% speedup in "unzip -tq" on 9MB zipfile (486-66)
 *
 * SPC 970402:  revised for Rodney Brown's optimizations (32-bit-wide
 *              aligned reads for most of the data from buffer), can be
 *              disabled by defining the macro NO_32_BIT_LOADS
 *
 * SPC 971012:  added Rodney Brown's additional tweaks for 32-bit-optimized
 *              CPUs (like the Pentium Pro, Pentium II, and probably some
 *              Pentium clones). This optimization is controlled by the
 *              preprocessor switch "__686" and is disabled by default.
 *              (This default is based on the assumption that most users
 *              do not yet work on a Pentium Pro or Pentium II machine ...)
 *
 * FLAT memory model assumed.  Calling interface:
 *   - args are pushed onto the stack from right to left,
 *   - return value is given in the EAX register,
 *   - all other registers (with exception of EFLAGS) are preserved. (With
 *     GNU C 2.7.x, %edx and %ecx are `scratch' registers, but preserving
 *     them nevertheless adds only 4 single byte instructions.)
 *
 * This source generates the function
 * ulg crc32(ulg crc, ZCONST uch *buf, ulg len).
 *
 * The loop unrolling can be disabled by defining the macro NO_UNROLLED_LOOPS.
 * This results in shorter code at the expense of reduced performance.
 */

/* This file is NOT used in conjunction with zlib. */
#ifndef USE_ZLIB

/* Preprocess with -DNO_UNDERLINE if your C compiler does not prefix
 * external symbols with an underline character '_'.
 */
#if defined(NO_UNDERLINE) || defined(__ELF__)
#  define _crc32            crc32
#  define _get_crc_table    get_crc_table
#endif
/* Use 16-byte alignment if your assembler supports it. Warning: gas
 * uses a log(x) parameter (.align 4 means 16-byte alignment). On SVR4
 * the parameter is a number of bytes.
 */
#ifndef ALIGNMENT
#  define ALIGNMENT .align 4,0x90
#endif

#if defined(i386) || defined(_i386) || defined(_I386) || defined(__i386)

/* This version is for 386 Unix, OS/2, MSDOS in 32 bit mode (gcc & gas).
 * Warning: it uses the AT&T syntax: mov source,dest
 * This file is only optional. If you want to use the C version,
 * remove -DASM_CRC from CFLAGS in Makefile and set OBJA to an empty string.
 */

                .file   "crc_i386.S"

#if defined(NO_STD_STACKFRAME) && defined(USE_STD_STACKFRAME)
#  undef USE_STACKFRAME
#else
   /* The default is to use standard stack frame entry, because it
    * results in smaller code!
    */
#  ifndef USE_STD_STACKFRAME
#    define USE_STD_STACKFRAME
#  endif
#endif

#ifdef USE_STD_STACKFRAME
#  define _STD_ENTRY    pushl   %ebp ; movl   %esp,%ebp
#  define arg1  8(%ebp)
#  define arg2  12(%ebp)
#  define arg3  16(%ebp)
#  define _STD_LEAVE    popl    %ebp
#else /* !USE_STD_STACKFRAME */
#  define _STD_ENTRY
#  define arg1  24(%esp)
#  define arg2  28(%esp)
#  define arg3  32(%esp)
#  define _STD_LEAVE
#endif /* ?USE_STD_STACKFRAME */

/*
 * These two (three) macros make up the loop body of the CRC32 cruncher.
 * registers modified:
 *   eax  : crc value "c"
 *   esi  : pointer to next data byte (or lword) "buf++"
 * registers read:
 *   edi  : pointer to base of crc_table array
 * scratch registers:
 *   ebx  : index into crc_table array
 *          (requires upper three bytes = 0 when __686 is undefined)
 */
#ifndef __686   /* optimize for 386, 486, Pentium */
#define Do_CRC          /* c = (c >> 8) ^ table[c & 0xFF] */\
                movb    %al, %bl                ;/* tmp = c & 0xFF  */\
                shrl    $8, %eax                ;/* c = (c >> 8)    */\
                xorl    (%edi, %ebx, 4), %eax   ;/* c ^= table[tmp] */
#else   /* __686 : optimize for Pentium Pro and compatible CPUs */
#define Do_CRC          /* c = (c >> 8) ^ table[c & 0xFF] */\
                movzbl  %al, %ebx               ;/* tmp = c & 0xFF  */\
                shrl    $8, %eax                ;/* c = (c >> 8)    */\
                xorl    (%edi, %ebx, 4), %eax   ;/* c ^=table[tmp]  */
#endif  /* ?__686 */

#define Do_CRC_byte     /* c = (c >> 8) ^ table[(c^*buf++)&0xFF] */\
                xorb    (%esi), %al     ;/* c ^= *buf  */\
                incl    %esi            ;/* buf++      */\
                Do_CRC

#ifndef  NO_32_BIT_LOADS
#define Do_CRC_lword \
                xorl    (%esi), %eax    ;/* c ^= *(ulg *)buf */\
                addl    $4, %esi        ;/* ((ulg *)buf)++   */\
                Do_CRC \
                Do_CRC \
                Do_CRC \
                Do_CRC
#endif  /* !NO_32_BIT_LOADS */


                .text

                .globl  _crc32

_crc32:                         /* ulg crc32(ulg crc, uch *buf, extent len) */
                _STD_ENTRY
                pushl   %edi
                pushl   %esi
                pushl   %ebx
                pushl   %edx
                pushl   %ecx

                movl    arg2, %esi           /* 2nd arg: uch *buf            */
                subl    %eax, %eax           /* > if (!buf)                  */
                testl   %esi, %esi           /* >   return 0;                */
                jz      .L_fine              /* > else {                     */
                call    _get_crc_table
                movl    %eax, %edi
                movl    arg1, %eax           /* 1st arg: ulg crc             */
#ifndef __686
                subl    %ebx, %ebx           /* ebx=0; bl usable as dword    */
#endif
                movl    arg3, %ecx           /* 3rd arg: extent len          */
                notl    %eax                 /* >   c = ~crc;                */

#ifndef  NO_UNROLLED_LOOPS
#  ifndef  NO_32_BIT_LOADS
                testl   %ecx, %ecx
                jz      .L_bail
                /* Assert now have positive length */
.L_align_loop:
                testl   $3, %esi        /* Align buf on lword boundary */
                jz      .L_aligned_now
                Do_CRC_byte
                decl    %ecx
                jnz     .L_align_loop
.L_aligned_now:
#  endif  /* !NO_32_BIT_LOADS */
                movl    %ecx, %edx           /* save len in edx          */
                andl    $7, %edx             /* edx = len % 8            */
                shrl    $3, %ecx             /* ecx = len / 8            */
                jz      .L_No_Eights
/*  align loop head at start of 486 internal cache line !! */
                ALIGNMENT
.L_Next_Eight:
#  ifndef NO_32_BIT_LOADS
                 /* Do_CRC_lword */
                 xorl    (%esi), %eax    ;/* c ^= *(ulg *)buf */
                 addl    $4, %esi        ;/* ((ulg *)buf)++   */
                 Do_CRC
                 Do_CRC
                 Do_CRC
                 Do_CRC
                 /* Do_CRC_lword */
                 xorl    (%esi), %eax    ;/* c ^= *(ulg *)buf */
                 addl    $4, %esi        ;/* ((ulg *)buf)++   */
                 Do_CRC
                 Do_CRC
                 Do_CRC
                 Do_CRC
#  else   /* NO_32_BIT_LOADS */
                 Do_CRC_byte
                 Do_CRC_byte
                 Do_CRC_byte
                 Do_CRC_byte
                 Do_CRC_byte
                 Do_CRC_byte
                 Do_CRC_byte
                 Do_CRC_byte
#  endif  /* ?NO_32_BIT_LOADS */
                decl    %ecx
                jnz     .L_Next_Eight

.L_No_Eights:
                movl    %edx, %ecx
#endif /* NO_UNROLLED_LOOPS */

#ifndef NO_JECXZ_SUPPORT
                jecxz   .L_bail              /* > if (len)                   */
#else
                testl   %ecx, %ecx           /* > if (len)                   */
                jz      .L_bail
#endif
/* align loop head at start of 486 internal cache line !! */
                ALIGNMENT
.L_loupe:                                    /* >   do {                     */
                 Do_CRC_byte                 /*       c = CRC32(c, *buf++);  */
                decl    %ecx                 /* >   } while (--len);         */
                jnz     .L_loupe

.L_bail:                                     /* > }                          */
                notl    %eax                 /* > return ~c;                 */
.L_fine:
                popl    %ecx
                popl    %edx
                popl    %ebx
                popl    %esi
                popl    %edi
                _STD_LEAVE
                ret

#else
 error: this asm version is for 386 only
#endif /* i386 || _i386 || _I386 || __i386 */

#endif /* !USE_ZLIB */
