; crc_i386.asm, optimized CRC calculation function for Zip and UnZip, not
; copyrighted by Paul Kienitz and Christian Spieler.  Last revised 25 Mar 98.
;
; Revised 06-Oct-96, Scott Field (sfield@microsoft.com)
;   fixed to assemble with masm by not using .model directive which makes
;   assumptions about segment alignment.  Also,
;   avoid using loop, and j[e]cxz where possible.  Use mov + inc, rather
;   than lodsb, and other misc. changes resulting in the following performance
;   increases:
;
;      unrolled loops                NO_UNROLLED_LOOPS
;      *8    >8      <8              *8      >8      <8
;
;      +54%  +42%    +35%            +82%    +52%    +25%
;
;   first item in each table is input buffer length, even multiple of 8
;   second item in each table is input buffer length, > 8
;   third item in each table is input buffer length, < 8
;
; Revised 02-Apr-97, Chr. Spieler, based on Rodney Brown (rdb@cmutual.com.au)
;   Incorporated Rodney Brown's 32-bit-reads optimization as found in the
;   UNIX AS source crc_i386.S. This new code can be disabled by defining
;   the macro symbol NO_32_BIT_LOADS.
;
; Revised 12-Oct-97, Chr. Spieler, based on Rodney Brown (rdb@cmutual.com.au)
;   Incorporated Rodney Brown's additional tweaks for 32-bit-optimized CPUs
;   (like the Pentium Pro, Pentium II, and probably some Pentium clones).
;   This optimization is controlled by the macro symbol __686 and is disabled
;   by default. (This default is based on the assumption that most users
;   do not yet work on a Pentium Pro or Pentium II machine ...)
;
; FLAT memory model assumed.
;
; The loop unrolling can be disabled by defining the macro NO_UNROLLED_LOOPS.
; This results in shorter code at the expense of reduced performance.
;
; Revised 25-Mar-98, Cosmin Truta (cosmint@cs.ubbcluj.ro)
;   Working without .model directive caused tasm32 version 5.0 to produce
;   bad object code. The optimized alignments can be optionally disabled
;   by defining NO_ALIGN, thus allowing to use .model flat. There is no need
;   to define this macro if using other version of tasm.
;
;==============================================================================
;
; Do NOT assemble this source if external crc32 routine from zlib gets used.
;
    IFNDEF USE_ZLIB
;
        .386p
        name    crc_i386

    IFDEF NO_ALIGN
        .model flat
    ENDIF

extrn   _get_crc_table:near    ; ZCONST ulg near *get_crc_table(void);

;
    IFNDEF NO_STD_STACKFRAME
        ; Use a `standard' stack frame setup on routine entry and exit.
        ; Actually, this option is set as default, because it results
        ; in smaller code !!
STD_ENTRY       MACRO
                push    ebp
                mov     ebp,esp
        ENDM

        Arg1    EQU     08H[ebp]
        Arg2    EQU     0CH[ebp]
        Arg3    EQU     10H[ebp]

STD_LEAVE       MACRO
                pop     ebp
        ENDM

    ELSE  ; NO_STD_STACKFRAME

STD_ENTRY       MACRO
        ENDM

        Arg1    EQU     18H[esp]
        Arg2    EQU     1CH[esp]
        Arg3    EQU     20H[esp]

STD_LEAVE       MACRO
        ENDM

    ENDIF ; ?NO_STD_STACKFRAME

; These two (three) macros make up the loop body of the CRC32 cruncher.
; registers modified:
;   eax  : crc value "c"
;   esi  : pointer to next data byte (or dword) "buf++"
; registers read:
;   edi  : pointer to base of crc_table array
; scratch registers:
;   ebx  : index into crc_table array
;          (requires upper three bytes = 0 when __686 is undefined)
    IFNDEF  __686 ; optimize for 386, 486, Pentium
Do_CRC  MACRO
                mov     bl,al                ; tmp = c & 0xFF
                shr     eax,8                ; c = (c >> 8)
                xor     eax,[edi+ebx*4]      ;  ^ table[tmp]
        ENDM
    ELSE ; __686 : optimize for Pentium Pro, Pentium II and compatible CPUs
Do_CRC  MACRO
                movzx   ebx,al               ; tmp = c & 0xFF
                shr     eax,8                ; c = (c >> 8)
                xor     eax,[edi+ebx*4]      ;  ^ table[tmp]
        ENDM
    ENDIF ; ?__686
Do_CRC_byte     MACRO
                xor     al, byte ptr [esi]   ; c ^= *buf
                inc     esi                  ; buf++
                Do_CRC                       ; c = (c >> 8) ^ table[c & 0xFF]
        ENDM
    IFNDEF  NO_32_BIT_LOADS
Do_CRC_dword    MACRO
                xor     eax, dword ptr [esi] ; c ^= *(ulg *)buf
                add     esi, 4               ; ((ulg *)buf)++
                Do_CRC
                Do_CRC
                Do_CRC
                Do_CRC
        ENDM
    ENDIF ; !NO_32_BIT_LOADS

    IFNDEF NO_ALIGN
_TEXT   segment use32 para public 'CODE'
    ELSE
_TEXT   segment use32
    ENDIF
        assume  CS: _TEXT

        public  _crc32
_crc32          proc    near  ; ulg crc32(ulg crc, ZCONST uch *buf, extent len)
                STD_ENTRY
                push    edi
                push    esi
                push    ebx
                push    edx
                push    ecx

                mov     esi,Arg2             ; 2nd arg: uch *buf
                sub     eax,eax              ;> if (!buf)
                test    esi,esi              ;>   return 0;
                jz      fine                 ;> else {

                call    _get_crc_table
                mov     edi,eax
                mov     eax,Arg1             ; 1st arg: ulg crc
    IFNDEF __686
                sub     ebx,ebx              ; ebx=0; make bl usable as a dword
    ENDIF
                mov     ecx,Arg3             ; 3rd arg: extent len
                not     eax                  ;>   c = ~crc;

    IFNDEF  NO_UNROLLED_LOOPS
    IFNDEF  NO_32_BIT_LOADS
                test    ecx,ecx
                je      bail
align_loop:
                test    esi,3                ; align buf pointer on next
                jz      SHORT aligned_now    ;  dword boundary
                Do_CRC_byte
                dec     ecx
                jnz     align_loop
aligned_now:
    ENDIF ; !NO_32_BIT_LOADS
                mov     edx,ecx              ; save len in edx
                and     edx,000000007H       ; edx = len % 8
                shr     ecx,3                ; ecx = len / 8
                jz      SHORT No_Eights
    IFNDEF NO_ALIGN
; align loop head at start of 486 internal cache line !!
                align   16
    ENDIF
Next_Eight:
    IFNDEF  NO_32_BIT_LOADS
                Do_CRC_dword
                Do_CRC_dword
    ELSE ; NO_32_BIT_LOADS
                Do_CRC_byte
                Do_CRC_byte
                Do_CRC_byte
                Do_CRC_byte
                Do_CRC_byte
                Do_CRC_byte
                Do_CRC_byte
                Do_CRC_byte
    ENDIF ; ?NO_32_BIT_LOADS
                dec     ecx
                jnz     Next_Eight
No_Eights:
                mov     ecx,edx

    ENDIF ; NO_UNROLLED_LOOPS
    IFNDEF  NO_JECXZ_SUPPORT
                jecxz   bail                 ;>   if (len)
    ELSE
                test    ecx,ecx              ;>   if (len)
                jz      SHORT bail
    ENDIF
    IFNDEF NO_ALIGN
; align loop head at start of 486 internal cache line !!
                align   16
    ENDIF
loupe:                                       ;>     do {
                Do_CRC_byte                  ;        c = CRC32(c, *buf++);
                dec     ecx                  ;>     } while (--len);
                jnz     loupe

bail:                                        ;> }
                not     eax                  ;> return ~c;
fine:
                pop     ecx
                pop     edx
                pop     ebx
                pop     esi
                pop     edi
                STD_LEAVE
                ret
_crc32          endp

_TEXT   ends
;
    ENDIF ; !USE_ZLIB
;
end
