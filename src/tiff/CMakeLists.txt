# CMake build for libtiff
# Run "cmake" to generate the build files for your platform
#
# Copyright © 2015 Open Microscopy Environment / University of Dundee
# Written by Roger Leigh <rleigh@codelibre.net>
#
# Permission to use, copy, modify, distribute, and sell this software and
# its documentation for any purpose is hereby granted without fee, provided
# that (i) the above copyright notices and this permission notice appear in
# all copies of the software and related documentation, and (ii) the names of
# Sam Leffler and Silicon Graphics may not be used in any advertising or
# publicity relating to the software without the specific, prior written
# permission of Sam Leffler and Silicon Graphics.
#
# THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,
# EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
# WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
#
# IN NO EVENT SHALL SAM LEFFLER OR SILICON GRAPHICS BE LIABLE FOR
# ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND,
# OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
# WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF
# LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
# OF THIS SOFTWARE.

cmake_minimum_required(VERSION 2.8.9)

# Default policy is from 2.8.9
cmake_policy(VERSION 2.8.9)
# Set MacOSX @rpath usage globally.
if (POLICY CMP0020)
  cmake_policy(SET CMP0020 NEW)
endif(POLICY CMP0020)
if (POLICY CMP0042)
  cmake_policy(SET CMP0042 NEW)
endif(POLICY CMP0042)
# Use new variable expansion policy.
if (POLICY CMP0053)
  cmake_policy(SET CMP0053 NEW)
endif(POLICY CMP0053)
if (POLICY CMP0054)
  cmake_policy(SET CMP0054 NEW)
endif(POLICY CMP0054)

# Read version information from configure.ac.
FILE(READ "${CMAKE_CURRENT_SOURCE_DIR}/configure.ac" configure)
STRING(REGEX REPLACE ";" "\\\\;" configure "${configure}")
STRING(REGEX REPLACE "\n" ";" configure "${configure}")
foreach(line ${configure})
  foreach(var LIBTIFF_MAJOR_VERSION LIBTIFF_MINOR_VERSION LIBTIFF_MICRO_VERSION LIBTIFF_ALPHA_VERSION
          LIBTIFF_CURRENT LIBTIFF_REVISION LIBTIFF_AGE)
    if(NOT ${var})
      string(REGEX MATCH "^${var}=(.*)" ${var}_MATCH "${line}")
      if(${var}_MATCH)
        string(REGEX REPLACE "^${var}=(.*)" "\\1" ${var} "${line}")
      endif()
    endif()
  endforeach()
endforeach()

math(EXPR SO_MAJOR "${LIBTIFF_CURRENT} - ${LIBTIFF_AGE}")
set(SO_MINOR "${LIBTIFF_AGE}")
set(SO_REVISION "${LIBTIFF_REVISION}")

message(STATUS "Building tiff version ${LIBTIFF_MAJOR_VERSION}.${LIBTIFF_MINOR_VERSION}.${LIBTIFF_MICRO_VERSION}${LIBTIFF_ALPHA_VERSION}")
message(STATUS "libtiff library version ${SO_MAJOR}.${SO_MINOR}.${SO_REVISION}")

set(BUILD_SHARED_LIBS ON CACHE BOOL "Build shared libraries")

# Project version
project(tiff C)
set(VERSION "${LIBTIFF_MAJOR_VERSION}.${LIBTIFF_MINOR_VERSION}.${LIBTIFF_MICRO_VERSION}")
set(tiff_VERSION "${VERSION}")
set(tiff_VERSION_MAJOR "${LIBTIFF_MAJOR_VERSION}")
set(tiff_VERSION_MINOR "${LIBTIFF_MINOR_VERSION}")
set(tiff_VERSION_PATCH "${LIBTIFF_MICRO_VERSION}")

# the other tiff_VERSION_* variables are set automatically
set(tiff_VERSION_ALPHA "${LIBTIFF_ALPHA_VERSION}")
# Library version (unlike libtool's baroque scheme, WYSIWYG here)
set(SO_COMPATVERSION "${SO_MAJOR}")
set(SO_VERSION "${SO_MAJOR}.${SO_MINOR}.${SO_REVISION}")

# For autotools header compatibility
set(PACKAGE_NAME "LibTIFF Software")
set(PACKAGE_TARNAME "${PROJECT_NAME}")
set(PACKAGE_VERSION "${PROJECT_VERSION}${tiff_VERSION_ALPHA}")
set(PACKAGE_STRING "${PACKAGE_NAME} ${PACKAGE_VERSION}")
set(PACKAGE_BUGREPORT "tiff@lists.maptools.org")

include(GNUInstallDirs)
include(CheckCCompilerFlag)
include(CheckCSourceCompiles)
include(CheckIncludeFile)
include(CheckTypeSize)
include(CheckFunctionExists)
enable_testing()

macro(current_date var)
  if(UNIX)
    execute_process(COMMAND "date" +"%Y%m%d" OUTPUT_VARIABLE ${var})
  endif()
endmacro()

current_date(RELEASE_DATE)

macro(extra_dist)
  foreach(file ${ARGV})
    file(RELATIVE_PATH relfile "${PROJECT_SOURCE_DIR}"
         "${CMAKE_CURRENT_SOURCE_DIR}/${file}")
    list(APPEND EXTRA_DIST "${relfile}")
  endforeach()
  set(EXTRA_DIST "${EXTRA_DIST}" PARENT_SCOPE)
endmacro()

set(EXTRA_DIST
  HOWTO-RELEASE
  Makefile.vc
  SConstruct
  autogen.sh
  configure.com
  nmake.opt
  libtiff-4.pc.in)

# These are annoyingly verbose, produce false positives or don't work
# nicely with all supported compiler versions, so are disabled unless
# explicitly enabled.
option(extra-warnings "Enable extra compiler warnings" OFF)

# This will cause the compiler to fail when an error occurs.
option(fatal-warnings "Compiler warnings are errors" OFF)

# Check if the compiler supports each of the following additional
# flags, and enable them if supported.  This greatly improves the
# quality of the build by checking for a number of common problems,
# some of which are quite serious.
if(CMAKE_C_COMPILER_ID STREQUAL "GNU" OR
   CMAKE_C_COMPILER_ID MATCHES "Clang")
  set(test_flags
      -Wall
      -Winline
      -W
      -Wformat-security
      -Wpointer-arith
      -Wdisabled-optimization
      -Wno-unknown-pragmas
      -Wdeclaration-after-statement
      -fstrict-aliasing)
  if(extra-warnings)
    list(APPEND test_flags
        -Wfloat-equal
        -Wmissing-prototypes
        -Wunreachable-code)
  endif()
  if(fatal-warnings)
    list(APPEND test_flags
         -Werror)
  endif()
elseif(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
  set(test_flags)
  if(extra-warnings)
    list(APPEND test_flags
         /W4)
  else()
    list(APPEND test_flags
         /W3)
  endif()
  if (fatal-warnings)
    list(APPEND test_flags
         /WX)
  endif()
endif()

foreach(flag ${test_flags})
  string(REGEX REPLACE "[^A-Za-z0-9]" "_" flag_var "${flag}")
  set(test_c_flag "C_FLAG${flag_var}")
  CHECK_C_COMPILER_FLAG(${flag} "${test_c_flag}")
  if (${test_c_flag})
     set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${flag}")
  endif (${test_c_flag})
endforeach(flag ${test_flags})

if(MSVC)
    set(CMAKE_DEBUG_POSTFIX "d")
endif()

option(ld-version-script "Enable linker version script" ON)
# Check if LD supports linker scripts.
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/conftest.map" "VERS_1 {
        global: sym;
};

VERS_2 {
        global: sym;
} VERS_1;
")
set(CMAKE_REQUIRED_FLAGS_SAVE ${CMAKE_REQUIRED_FLAGS})
set(CMAKE_REQUIRED_FLAGS ${CMAKE_REQUIRED_FLAGS} "-Wl,--version-script=${CMAKE_CURRENT_BINARY_DIR}/conftest.map")
check_c_source_compiles("int main(void){return 0;}" HAVE_LD_VERSION_SCRIPT)
set(CMAKE_REQUIRED_FLAGS ${CMAKE_REQUIRED_FLAGS_SAVE})
file(REMOVE "${CMAKE_CURRENT_BINARY_DIR}/conftest.map")
if (ld-version-script AND HAVE_LD_VERSION_SCRIPT)
  set(HAVE_LD_VERSION_SCRIPT TRUE)
else()
  set(HAVE_LD_VERSION_SCRIPT FALSE)
endif()

# Find libm, if available
find_library(M_LIBRARY m)

check_include_file(assert.h    HAVE_ASSERT_H)
check_include_file(dlfcn.h     HAVE_DLFCN_H)
check_include_file(fcntl.h     HAVE_FCNTL_H)
check_include_file(inttypes.h  HAVE_INTTYPES_H)
check_include_file(io.h        HAVE_IO_H)
check_include_file(limits.h    HAVE_LIMITS_H)
check_include_file(malloc.h    HAVE_MALLOC_H)
check_include_file(memory.h    HAVE_MEMORY_H)
check_include_file(search.h    HAVE_SEARCH_H)
check_include_file(stdint.h    HAVE_STDINT_H)
check_include_file(string.h    HAVE_STRING_H)
check_include_file(strings.h   HAVE_STRINGS_H)
check_include_file(sys/time.h  HAVE_SYS_TIME_H)
check_include_file(sys/types.h HAVE_SYS_TYPES_H)
check_include_file(unistd.h    HAVE_UNISTD_H)

# Inspired from /usr/share/autoconf/autoconf/c.m4
foreach(inline_keyword "inline" "__inline__" "__inline")
  if(NOT DEFINED C_INLINE)
    set(CMAKE_REQUIRED_DEFINITIONS_SAVE ${CMAKE_REQUIRED_DEFINITIONS})
    set(CMAKE_REQUIRED_DEFINITIONS ${CMAKE_REQUIRED_DEFINITIONS}
        "-Dinline=${inline_keyword}")
    check_c_source_compiles("
        typedef int foo_t;
        static inline foo_t static_foo() {return 0;}
        foo_t foo(){return 0;}
        int main(int argc, char *argv[]) {return 0;}"
      C_HAS_${inline_keyword})
    set(CMAKE_REQUIRED_DEFINITIONS ${CMAKE_REQUIRED_DEFINITIONS_SAVE})
    if(C_HAS_${inline_keyword})
      set(C_INLINE TRUE)
      set(INLINE_KEYWORD "${inline_keyword}")
    endif()
 endif()
endforeach()
if(NOT DEFINED C_INLINE)
  set(INLINE_KEYWORD)
endif()

# off_t and size_t checks omitted; not clear they are used at all
# Are off_t and size_t checks strictly necessary?

# Check if sys/time.h and time.h allow use together
check_c_source_compiles("
#include <sys/time.h>
#include <time.h>
int main(void){return 0;}"
  TIME_WITH_SYS_TIME)

# Check if struct tm is in sys/time.h
check_c_source_compiles("
#include <sys/types.h>
#include <time.h>

int main(void){
  struct tm tm;
  int *p = &tm.tm_sec;
  return !p;
}"
  TM_IN_SYS_TIME)

# Check type sizes
# NOTE: Could be replaced with C99 <stdint.h>
check_type_size("signed short" SIZEOF_SIGNED_SHORT)
check_type_size("unsigned short" SIZEOF_UNSIGNED_SHORT)
check_type_size("signed int" SIZEOF_SIGNED_INT)
check_type_size("unsigned int" SIZEOF_UNSIGNED_INT)
check_type_size("signed long" SIZEOF_SIGNED_LONG)
check_type_size("unsigned long" SIZEOF_UNSIGNED_LONG)
check_type_size("signed long long" SIZEOF_SIGNED_LONG_LONG)
check_type_size("unsigned long long" SIZEOF_UNSIGNED_LONG_LONG)
check_type_size("unsigned char *" SIZEOF_UNSIGNED_CHAR_P)

set(CMAKE_EXTRA_INCLUDE_FILES_SAVE ${CMAKE_EXTRA_INCLUDE_FILES})
set(CMAKE_EXTRA_INCLUDE_FILES ${CMAKE_EXTRA_INCLUDE_FILES} "stddef.h")
check_type_size("size_t" SIZEOF_SIZE_T)
check_type_size("ptrdiff_t" SIZEOF_PTRDIFF_T)
set(CMAKE_EXTRA_INCLUDE_FILES ${CMAKE_EXTRA_INCLUDE_FILES_SAVE})

macro(report_values)
  foreach(val ${ARGV})
    message(STATUS "${val} set to ${${val}}")
  endforeach()
endmacro()

set(TIFF_INT8_T "signed char")
set(TIFF_UINT8_T "unsigned char")

set(TIFF_INT16_T "signed short")
set(TIFF_UINT16_T "unsigned short")

if(SIZEOF_SIGNED_INT EQUAL 4)
  set(TIFF_INT32_T "signed int")
  set(TIFF_INT32_FORMAT "%d")
elseif(SIZEOF_SIGNED_LONG EQUAL 4)
  set(TIFF_INT32_T "signed long")
  set(TIFF_INT32_FORMAT "%ld")
endif()

if(SIZEOF_UNSIGNED_INT EQUAL 4)
  set(TIFF_UINT32_T "unsigned int")
  set(TIFF_UINT32_FORMAT "%u")
elseif(SIZEOF_UNSIGNED_LONG EQUAL 4)
  set(TIFF_UINT32_T "unsigned long")
  set(TIFF_UINT32_FORMAT "%lu")
endif()

if(SIZEOF_SIGNED_LONG EQUAL 8)
  set(TIFF_INT64_T "signed long")
  set(TIFF_INT64_FORMAT "%ld")
elseif(SIZEOF_SIGNED_LONG_LONG EQUAL 8)
  set(TIFF_INT64_T "signed long long")
  if (MINGW)
    set(TIFF_INT64_FORMAT "%I64d")
  else()
    set(TIFF_INT64_FORMAT "%lld")
  endif()
endif()

if(SIZEOF_UNSIGNED_LONG EQUAL 8)
  set(TIFF_UINT64_T "unsigned long")
  set(TIFF_UINT64_FORMAT "%lu")
elseif(SIZEOF_UNSIGNED_LONG_LONG EQUAL 8)
  set(TIFF_UINT64_T "unsigned long long")
  if (MINGW)
    set(TIFF_UINT64_FORMAT "%I64u")
  else()
    set(TIFF_UINT64_FORMAT "%llu")
  endif()
endif()

if(SIZEOF_UNSIGNED_INT EQUAL SIZEOF_SIZE_T)
  set(TIFF_SIZE_T "unsigned int")
  set(TIFF_SIZE_FORMAT "%u")
elseif(SIZEOF_UNSIGNED_LONG EQUAL SIZEOF_SIZE_T)
  set(TIFF_SIZE_T "unsigned long")
  set(TIFF_SIZE_FORMAT "%lu")
elseif(SIZEOF_UNSIGNED_LONG_LONG EQUAL SIZEOF_SIZE_T)
  set(TIFF_SIZE_T "unsigned long")
  if (MINGW)
    set(TIFF_SIZE_FORMAT "%I64u")
  else()
    set(TIFF_SIZE_FORMAT "%llu")
  endif()
endif()

if(SIZEOF_SIGNED_INT EQUAL SIZEOF_UNSIGNED_CHAR_P)
  set(TIFF_SSIZE_T "signed int")
  set(TIFF_SSIZE_FORMAT "%d")
elseif(SIZEOF_SIGNED_LONG EQUAL SIZEOF_UNSIGNED_CHAR_P)
  set(TIFF_SSIZE_T "signed long")
  set(TIFF_SSIZE_FORMAT "%ld")
elseif(SIZEOF_SIGNED_LONG_LONG EQUAL SIZEOF_UNSIGNED_CHAR_P)
  set(TIFF_SSIZE_T "signed long long")
  if (MINGW)
    set(TIFF_SSIZE_FORMAT "%I64d")
  else()
    set(TIFF_SSIZE_FORMAT "%lld")
  endif()
endif()

if(NOT SIZEOF_PTRDIFF_T)
  set(TIFF_PTRDIFF_T "${TIFF_SSIZE_T}")
  set(TIFF_PTRDIFF_FORMAT "${SSIZE_FORMAT}")
else()
  set(TIFF_PTRDIFF_T "ptrdiff_t")
  set(TIFF_PTRDIFF_FORMAT "%ld")
endif()

#report_values(TIFF_INT8_T TIFF_INT8_FORMAT
#              TIFF_UINT8_T TIFF_UINT8_FORMAT
#              TIFF_INT16_T TIFF_INT16_FORMAT
#              TIFF_UINT16_T TIFF_UINT16_FORMAT
#              TIFF_INT32_T TIFF_INT32_FORMAT
#              TIFF_UINT32_T TIFF_UINT32_FORMAT
#              TIFF_INT64_T TIFF_INT64_FORMAT
#              TIFF_UINT64_T TIFF_UINT64_FORMAT
#              TIFF_SSIZE_T TIFF_SSIZE_FORMAT
#              TIFF_PTRDIFF_T TIFF_PTRDIFF_FORMAT)

# Nonstandard int types
check_type_size(INT8 int8)
set(HAVE_INT8 ${INT8})
check_type_size(INT16 int16)
set(HAVE_INT16 ${INT16})
check_type_size(INT32 int32)
set(HAVE_INT32 ${INT32})

# Check functions
set(CMAKE_REQUIRED_LIBRARIES_SAVE ${CMAKE_REQUIRED_LIBRARIES})
set(CMAKE_REQUIRED_LIBRARIES ${CMAKE_REQUIRED_LIBRARIES} ${M_LIBRARY})
check_function_exists(floor HAVE_FLOOR)
check_function_exists(pow   HAVE_POW)
check_function_exists(sqrt  HAVE_SQRT)
set(CMAKE_REQUIRED_LIBRARIES ${CMAKE_REQUIRED_LIBRARIES_SAVE})

check_function_exists(isascii    HAVE_ISASCII)
check_function_exists(memmove    HAVE_MEMMOVE)
check_function_exists(memset     HAVE_MEMSET)
check_function_exists(mmap       HAVE_MMAP)
check_function_exists(setmode    HAVE_SETMODE)
check_function_exists(strcasecmp HAVE_STRCASECMP)
check_function_exists(strchr     HAVE_STRCHR)
check_function_exists(strrchr    HAVE_STRRCHR)
check_function_exists(strstr     HAVE_STRSTR)
check_function_exists(strtol     HAVE_STRTOL)
check_function_exists(strtol     HAVE_STRTOUL)
check_function_exists(strtoull   HAVE_STRTOULL)
check_function_exists(getopt     HAVE_GETOPT)
check_function_exists(lfind      HAVE_LFIND)

# May be inlined, so check it compiles:
check_c_source_compiles("
#include <stdio.h>
int main(void) {
  char buf[10];
  snprintf(buf, 10, \"Test %d\", 1);
  return 0;
}"
  HAVE_SNPRINTF)

if(NOT HAVE_SNPRINTF)
  add_definitions(-DNEED_LIBPORT)
endif()

# CPU bit order
set(fillorder FILLORDER_MSB2LSB)
if(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "i.*86.*" OR
   CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "amd64.*" OR
   CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "x86_64.*")
  set(fillorder FILLORDER_LSB2MSB)
endif()
set(HOST_FILLORDER ${fillorder} CACHE STRING "Native CPU bit order")
mark_as_advanced(HOST_FILLORDER)

# CPU endianness
include(TestBigEndian)
test_big_endian(bigendian)
if (bigendian)
  set(bigendian ON)
else()
  set(bigendian OFF)
endif()
set(HOST_BIG_ENDIAN ${bigendian} CACHE STRING "Native CPU bit order")
mark_as_advanced(HOST_BIG_ENDIAN)
if (HOST_BIG_ENDIAN)
  set(HOST_BIG_ENDIAN 1)
else()
  set(HOST_BIG_ENDIAN 0)
endif()

# IEEE floating point
set(HAVE_IEEEFP 1 CACHE STRING "IEEE floating point is available")
mark_as_advanced(HAVE_IEEEFP)

report_values(CMAKE_HOST_SYSTEM_PROCESSOR HOST_FILLORDER
              HOST_BIG_ENDIAN HAVE_IEEEFP)

# Large file support
if (UNIX)
  # This might not catch every possibility catered for by
  # AC_SYS_LARGEFILE.
  add_definitions(-D_FILE_OFFSET_BITS=64)
  set(FILE_OFFSET_BITS 64)
endif()

# Documentation install directory (default to cmake project docdir)
set(LIBTIFF_DOCDIR "${CMAKE_INSTALL_FULL_DOCDIR}")

# Options to enable and disable internal codecs

option(ccitt "support for CCITT Group 3 & 4 algorithms" ON)
set(CCITT_SUPPORT ${ccitt})

option(packbits "support for Macintosh PackBits algorithm" ON)
set(PACKBITS_SUPPORT ${packbits})

option(lzw "support for LZW algorithm" ON)
set(LZW_SUPPORT ${lzw})

option(thunder "support for ThunderScan 4-bit RLE algorithm" ON)
set(THUNDER_SUPPORT ${thunder})

option(next "support for NeXT 2-bit RLE algorithm" ON)
set(NEXT_SUPPORT ${next})

option(logluv "support for LogLuv high dynamic range algorithm" ON)
set(LOGLUV_SUPPORT ${logluv})

# Option for Microsoft Document Imaging
option(mdi "support for Microsoft Document Imaging" ON)
set(MDI_SUPPORT ${mdi})

# ZLIB
option(zlib "use zlib (required for Deflate compression)" ON)
if (zlib)
  find_package(ZLIB)
endif()
set(ZLIB_SUPPORT 0)
if(ZLIB_FOUND)
  set(ZLIB_SUPPORT 1)
endif()
set(ZIP_SUPPORT ${ZLIB_SUPPORT})
# Option for Pixar log-format algorithm

# Pixar log format
option(pixarlog "support for Pixar log-format algorithm (requires Zlib)" ON)
set(PIXARLOG_SUPPORT FALSE)
if (ZLIB_SUPPORT)
  if(pixarlog)
    set(PIXARLOG_SUPPORT TRUE)
  endif()
endif()

# JPEG
option(jpeg "use libjpeg (required for JPEG compression)" ON)
if (jpeg)
  find_package(JPEG)
endif()
set(JPEG_SUPPORT FALSE)
if(JPEG_FOUND)
  set(JPEG_SUPPORT TRUE)
endif()

option(old-jpeg "support for Old JPEG compression (read-only)" ON)
set(OJPEG_SUPPORT FALSE)
if (JPEG_SUPPORT)
  if (old-jpeg)
    set(OJPEG_SUPPORT TRUE)
  endif()
endif()

# JBIG-KIT
option(jbig "use ISO JBIG compression (requires JBIT-KIT library)" ON)
if (jbig)
  set(JBIG_FOUND 0)
  find_path(JBIG_INCLUDE_DIR jbig.h)
  set(JBIG_NAMES ${JBIG_NAMES} jbig libjbig)
  find_library(JBIG_LIBRARY NAMES ${JBIG_NAMES})
  if (JBIG_INCLUDE_DIR AND JBIG_LIBRARY)
    set(JBIG_FOUND 1)
    set(JBIG_LIBRARIES ${JBIG_LIBRARY})
  endif()
endif()
set(JBIG_SUPPORT 0)
if(JBIG_FOUND)
  set(JBIG_FOUND TRUE)
  set(JBIG_SUPPORT 1)
else()
  set(JBIG_FOUND FALSE)
endif()

set(CMAKE_REQUIRED_LIBRARIES_SAVE ${CMAKE_REQUIRED_LIBRARIES})
set(CMAKE_REQUIRED_INCLUDES_SAVE ${CMAKE_REQUIRED_INCLUDES})
set(CMAKE_REQUIRED_INCLUDES ${CMAKE_REQUIRED_INCLUDES} ${JBIG_INCLUDE_DIR})
set(CMAKE_REQUIRED_LIBRARIES ${CMAKE_REQUIRED_LIBRARIES} ${JBIG_LIBRARY})
check_function_exists(jbg_newlen HAVE_JBG_NEWLEN)
set(CMAKE_REQUIRED_LIBRARIES ${CMAKE_REQUIRED_LIBRARIES_SAVE})
set(CMAKE_REQUIRED_INCLUDES ${CMAKE_REQUIRED_INCLUDES_SAVE})

# liblzma2
option(lzma "use liblzma (required for LZMA2 compression)" ON)
if (lzma)
  find_package(LibLZMA)
endif()
set(LZMA_SUPPORT 0)
if(LIBLZMA_FOUND)
  set(LZMA_SUPPORT 1)
endif()

# 8/12-bit jpeg mode
option(jpeg12 "enable libjpeg 8/12-bit dual mode (requires separate
12-bit libjpeg build)" ON)
set(JPEG12_INCLUDE_DIR JPEG12_INCLUDE_DIR-NOTFOUND CACHE PATH "Include directory for 12-bit libjpeg")
set(JPEG12_LIBRARY JPEG12_LIBRARY-NOTFOUND CACHE FILEPATH "12-bit libjpeg library")
set(JPEG12_FOUND FALSE)
if (JPEG12_INCLUDE_DIR AND JPEG12_LIBRARY)
  set(JPEG12_LIBRARIES ${JPEG12_LIBRARY})
  set(JPEG12_FOUND TRUE)
endif()
if (JPEG12_FOUND)
  set(JPEG_DUAL_MODE_8_12 1)
  set(LIBJPEG_12_PATH "${JPEG12_INCLUDE_DIR}/jpeglib.h")
endif()

# C++ support
option(cxx "Enable C++ stream API building (requires C++ compiler)" ON)
set(CXX_SUPPORT FALSE)
if (cxx)
  enable_language(CXX)
  set(CXX_SUPPORT TRUE)
endif()

# OpenGL and GLUT
find_package(OpenGL)
find_package(GLUT)
set(HAVE_OPENGL FALSE)
if(OPENGL_FOUND AND OPENGL_GLU_FOUND AND GLUT_FOUND)
  set(HAVE_OPENGL TRUE)
endif()
# Purely to satisfy the generated headers:
check_include_file(GL/gl.h HAVE_GL_GL_H)
check_include_file(GL/glu.h HAVE_GL_GLU_H)
check_include_file(GL/glut.h HAVE_GL_GLUT_H)
check_include_file(GLUT/glut.h HAVE_GLUT_GLUT_H)
check_include_file(OpenGL/gl.h HAVE_OPENGL_GL_H)
check_include_file(OpenGL/glu.h HAVE_OPENGL_GLU_H)

# Win32 IO
set(win32_io FALSE)
if(WIN32)
  set(win32_io TRUE)
endif()
set(USE_WIN32_FILEIO ${win32_io} CACHE BOOL "Use win32 IO system (Microsoft Windows only)")
if (USE_WIN32_FILEIO)
  set(USE_WIN32_FILEIO TRUE)
else()
  set(USE_WIN32_FILEIO FALSE)
endif()

# Orthogonal features

# Strip chopping
option(strip-chopping "strip chopping (whether or not to convert single-strip uncompressed images to mutiple strips of specified size to reduce memory usage)" ON)
set(TIFF_DEFAULT_STRIP_SIZE 8192 CACHE STRING "default size of the strip in bytes (when strip chopping is enabled)")

set(STRIPCHOP_DEFAULT)
if(strip-chopping)
  set(STRIPCHOP_DEFAULT TRUE)
  if(TIFF_DEFAULT_STRIP_SIZE)
    set(STRIP_SIZE_DEFAULT "${TIFF_DEFAULT_STRIP_SIZE}")
  endif()
endif()

# Defer loading of strip/tile offsets
option(defer-strile-load "enable deferred strip/tile offset/size loading (experimental)" OFF)
set(DEFER_STRILE_LOAD ${defer-strile-load})

# CHUNKY_STRIP_READ_SUPPORT
option(chunky-strip-read "enable reading large strips in chunks for TIFFReadScanline() (experimental)" OFF)
set(CHUNKY_STRIP_READ_SUPPORT ${chunky-strip-read})

# SUBIFD support
set(SUBIFD_SUPPORT 1)

# Default handling of ASSOCALPHA support.
option(extrasample-as-alpha "the RGBA interface will treat a fourth sample with no EXTRASAMPLE_ value as being ASSOCALPHA. Many packages produce RGBA files but don't mark the alpha properly" ON)
if(extrasample-as-alpha)
  set(DEFAULT_EXTRASAMPLE_AS_ALPHA 1)
endif()

# Default handling of YCbCr subsampling support.
# See Bug 168 in Bugzilla, and JPEGFixupTestSubsampling() for details.
option(check-ycbcr-subsampling "enable picking up YCbCr subsampling info from the JPEG data stream to support files lacking the tag" ON)
if (check-ycbcr-subsampling)
  set(CHECK_JPEG_YCBCR_SUBSAMPLING 1)
endif()

# Generate pkg-config file
set(prefix "${CMAKE_INSTALL_PREFIX}")
set(exec_prefix "${CMAKE_INSTALL_PREFIX}")
set(libdir "${CMAKE_INSTALL_FULL_LIBDIR}")
set(includedir "${CMAKE_INSTALL_FULL_INCLUDEDIR}")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/libtiff-4.pc.in
               ${CMAKE_CURRENT_BINARY_DIR}/libtiff-4.pc)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libtiff-4.pc
        DESTINATION "${CMAKE_INSTALL_FULL_LIBDIR}/pkgconfig")

# Includes used by libtiff (and tests)
if(ZLIB_INCLUDE_DIRS)
  list(APPEND TIFF_INCLUDES ${ZLIB_INCLUDE_DIRS})
endif()
if(JPEG_INCLUDE_DIR)
  list(APPEND TIFF_INCLUDES ${JPEG_INCLUDE_DIR})
endif()
if(JPEG12_INCLUDE_DIR)
  list(APPEND TIFF_INCLUDES ${JPEG12_INCLUDE_DIR})
endif()
if(JBIG_INCLUDE_DIR)
  list(APPEND TIFF_INCLUDES ${JBIG_INCLUDE_DIR})
endif()
if(LIBLZMA_INCLUDE_DIRS)
  list(APPEND TIFF_INCLUDES ${LIBLZMA_INCLUDE_DIRS})
endif()

# Libraries required by libtiff
set(TIFF_LIBRARY_DEPS)
if(M_LIBRARY)
  list(APPEND TIFF_LIBRARY_DEPS ${M_LIBRARY})
endif()
if(ZLIB_LIBRARIES)
  list(APPEND TIFF_LIBRARY_DEPS ${ZLIB_LIBRARIES})
endif()
if(JPEG_LIBRARIES)
  list(APPEND TIFF_LIBRARY_DEPS ${JPEG_LIBRARIES})
endif()
if(JPEG12_LIBRARIES)
  list(APPEND TIFF_LIBRARY_DEPS ${JPEG12_LIBRARIES})
endif()
if(JBIG_LIBRARIES)
  list(APPEND TIFF_LIBRARY_DEPS ${JBIG_LIBRARIES})
endif()
if(LIBLZMA_LIBRARIES)
  list(APPEND TIFF_LIBRARY_DEPS ${LIBLZMA_LIBRARIES})
endif()

#report_values(TIFF_INCLUDES TIFF_LIBRARY_DEPS)

# Process subdirectories
add_subdirectory(port)
add_subdirectory(libtiff)
add_subdirectory(tools)
add_subdirectory(test)
add_subdirectory(contrib)
add_subdirectory(build)
add_subdirectory(man)
add_subdirectory(html)

#message(STATUS "EXTRA_DIST: ${EXTRA_DIST}")

message(STATUS "")
message(STATUS "Libtiff is now configured for ${host}")
message(STATUS "")
message(STATUS "  Installation directory:             ${prefix}")
message(STATUS "  Documentation directory:            ${LIBTIFF_DOCDIR}")
message(STATUS "  C compiler:                         ${CMAKE_C_COMPILER}")
message(STATUS "  C++ compiler:                       ${CMAKE_CXX_COMPILER}")
message(STATUS "  Build shared libraries:             ${BUILD_SHARED_LIBS}")
message(STATUS "  Enable linker symbol versioning:    ${HAVE_LD_VERSION_SCRIPT}")
message(STATUS "  Support Microsoft Document Imaging: ${mdi}")
message(STATUS "  Use win32 IO:                       ${USE_WIN32_FILEIO}")
message(STATUS "")
message(STATUS " Support for internal codecs:")
message(STATUS "  CCITT Group 3 & 4 algorithms:       ${ccitt}")
message(STATUS "  Macintosh PackBits algorithm:       ${packbits}")
message(STATUS "  LZW algorithm:                      ${lzw}")
message(STATUS "  ThunderScan 4-bit RLE algorithm:    ${thunder}")
message(STATUS "  NeXT 2-bit RLE algorithm:           ${next}")
message(STATUS "  LogLuv high dynamic range encoding: ${logluv}")
message(STATUS "")
message(STATUS " Support for external codecs:")
message(STATUS "  ZLIB support:                       ${zlib} (requested) ${ZLIB_FOUND} (availability)")
message(STATUS "  Pixar log-format algorithm:         ${pixarlog} (requested) ${PIXARLOG_SUPPORT} (availability)")
message(STATUS "  JPEG support:                       ${jpeg} (requested) ${JPEG_FOUND} (availability)")
message(STATUS "  Old JPEG support:                   ${old-jpeg} (requested) ${JPEG_FOUND} (availability)")
message(STATUS "  JPEG 8/12 bit dual mode:            ${jpeg12} (requested) ${JPEG12_FOUND} (availability)")
message(STATUS "  ISO JBIG support:                   ${jbig} (requested) ${JBIG_FOUND} (availability)")
message(STATUS "  LZMA2 support:                      ${lzma} (requested) ${LIBLZMA_FOUND} (availability)")
message(STATUS "")
message(STATUS "  C++ support:                        ${cxx} (requested) ${CXX_SUPPORT} (availability)")
message(STATUS "")
# message(STATUS "  X Athena Widgets support:           ${HAVE_XAW}")
message(STATUS "  OpenGL support:                     ${HAVE_OPENGL}")
message(STATUS "")
