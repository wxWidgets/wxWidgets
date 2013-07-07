######################################################################
#
# Open Watcom 1.5 multiplatform target options file.
#
# Author        : Michael Greene, July 2006
#
######################################################################

STATICLIB  = libexpat.lib

# only for OS/2 and NT builds
DLLNAME    = expat.dll
IMPORTLIB  = iexpat.lib

# path to wget if not in path - only needed for test suite download

#WGET = D:\BLDTOOLS\

# Uncomment to compile multi-thread

MULTITHREAD = 1

# Uncomment to compile using runtime library - default static

#CLIBLNK = 1

# valid defines:
#   XML_LARGE_SIZE      (supported - see main EXPAT readme for info)
#   XML_UNICODE_WCHAR_T (not supported)
#   XML_UNICODE         (not supported)
#   XML_MIN_SIZE        (compiles with but not tested)
#   XML_DTD - XML_NS    (enabled always - see watcomconfig.h)

#DEFS = -DXML_LARGE_SIZE

#                              [Processor options]
# -3r 386 register calling conventions  -5r Pentium register calling conv.
# -3s 386 stack calling conventions     -5s Pentium stack calling conventions
# -4r 486 register calling conventions  -6r Pentium Pro register call conven.
# -4s 486 stack calling conventions     -6s Pentium Pro stack call conven.
#                      [Floating-point processor options]
# -fpc calls to floating-point library  -fp2 generate 287 floating-point code
# -fpd enable Pentium FDIV check        -fp3 generate 387 floating-point code
# -fpi   inline 80x87 with emulation    -fp5 optimize f-p for Pentium
# -fpi87 inline 80x87                   -fp6 optimize f-p for Pentium Pro
# -fpr use old floating-point conventions

MACHINE = -6s -fp6

#                            [Optimization options]
# -oa relax alias checking              -ol+ ol with loop unrolling
# -ob branch prediction                 -om generate inline math functions
# -oc disable call/ret optimization     -on numerically unstable floating-point
# -od disable optimizations             -oo continue compile when low on memory
# -oe[=num] expand functions inline     -op improve floating-point consistency
# Press any key to continue:
# -of[+] generate traceable stack frames-or re-order instructions to avoid stalls
# -oh enable repeated optimizations     -os optimize for space
# -oi inline intrinsic functions        -ot optimize for time
# -oi+ (C++) oi with max inlining depth -ou ensure unique addresses for functions
# -ok control flow entry/exit seq.      -ox maximum optimization (-obmiler -s)
# -ol perform loop optimizations

OPTIMIZATION = -otexan

#                              [Debugging options]
# -d0 no debugging information          -d2t (C++) d2 but without type names
# -d1{+} line number debugging info.    -d3 debug info with unref'd type names
# -d2 full symbolic debugging info.     -d3i (C++) d3 + inlines as COMDATs
# -d2i (C++) d2 + inlines as COMDATs    -d3s (C++) d3 + inlines as statics
# -d2s (C++) d2 + inlines as statics

DEBUGOPTS = -d3

# I used this for testing. If LINKLIB is expat.lib (default) then
# all executables are linked static. If LINKLIB is iexpat.lib then
# executables will use the expat dll.

#TARGLIB = $(IMPORTLIB)

TARGLIB = $(STATICLIB)

