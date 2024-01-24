#############################################################################
# Name:        build/cmake/setup.cmake
# Purpose:     Create and configure setup.h
# Author:      Tobias Taschner
# Created:     2016-09-22
# Copyright:   (c) 2016 wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

# Include modules required for setup.h generation
include(CheckCSourceCompiles)
include(CheckCXXCompilerFlag)
include(CheckCXXSourceCompiles)
include(CheckCXXSymbolExists)
include(CheckFunctionExists)
include(CheckIncludeFile)
include(CheckIncludeFileCXX)
include(CheckIncludeFiles)
include(CheckPrototypeDefinition)
include(CheckStructHasMember)
include(CheckSymbolExists)
include(CheckTypeSize)
include(CMakePushCheckState)
include(TestBigEndian)

# Add a definition to setup.h and append it to a list of defines for
# for compile checks
macro(wx_setup_definition def)
    set(${def} YES)
    list(APPEND CMAKE_REQUIRED_DEFINITIONS -D${def})
endmacro()

# Add define based on system name
string(TOUPPER ${CMAKE_SYSTEM_NAME} wxSYS_NAME)
wx_setup_definition(__${wxSYS_NAME}__)

if(WIN32)
    wx_setup_definition(__WIN32__)
endif()

if(CYGWIN)
    wx_setup_definition(__GNUWIN32__)
endif()

if(UNIX)
    wx_setup_definition(wxUSE_UNIX)
    wx_setup_definition(__UNIX__)
    list(APPEND CMAKE_REQUIRED_DEFINITIONS -D_FILE_OFFSET_BITS=64)
endif()

if(UNIX AND NOT APPLE)
    wx_setup_definition(_GNU_SOURCE)
endif()

if(APPLE)
    wx_setup_definition(__BSD__)
endif()

if(WXGTK)
    # Add GTK version definitions
    foreach(gtk_version 2.0 2.10 2.18 2.20 3.0 3.90.0)
        if(NOT wxTOOLKIT_VERSION VERSION_LESS gtk_version)
            if(gtk_version EQUAL 3.90.0)
                set(__WXGTK4__ ON)
            elseif(gtk_version EQUAL 3.0)
                set(__WXGTK3__ ON)
            else()
                string(REPLACE . "" gtk_version_dotless ${gtk_version})
                set(__WXGTK${gtk_version_dotless}__ ON)
            endif()
        endif()
    endforeach()
endif()

set(wxINSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

check_include_files("stdlib.h;stdarg.h;string.h;float.h" STDC_HEADERS)

if(wxBUILD_SHARED)
    if(wxUSE_VISIBILITY)
        check_cxx_compiler_flag(-fvisibility=hidden HAVE_VISIBILITY)
    endif()
endif() # wxBUILD_SHARED

if(MSVC)
    set(DISABLE_ALL_WARNINGS "/w")
else()
    set(DISABLE_ALL_WARNINGS "-w")
endif()

# wx_check_cxx_source_compiles(<code> <var> [headers...])
function(wx_check_cxx_source_compiles code res_var)
    set(src)
    foreach(header ${ARGN})
        if(header STREQUAL "DEFINITION")
            set(is_definition TRUE)
        elseif(is_definition)
            set(src "${src}${header}")
            set(is_definition FALSE)
        else()
            set(src "${src}#include <${header}>\n")
        endif()
    endforeach()
    set(src "${src}\n\nint main(int argc, char* argv[]) {\n ${code}\nreturn 0; }")
    # We're not interested in any warnings that can arise in the test, which is
    # especially important if -Werror is globally in effect.
    cmake_push_check_state()
    set(CMAKE_REQUIRED_FLAGS ${DISABLE_ALL_WARNINGS})
    check_cxx_source_compiles("${src}" ${res_var})
    cmake_pop_check_state()
endfunction()

# wx_check_c_source_compiles(<code> <var> [headers...])
function(wx_check_c_source_compiles code res_var)
    set(src)
    foreach(header ${ARGN})
        if(header STREQUAL "DEFINITION")
            set(is_definition TRUE)
        elseif(is_definition)
            set(src "${src}${header}")
            set(is_definition FALSE)
        else()
            set(src "${src}#include <${header}>\n")
        endif()
    endforeach()
    set(src "${src}\n\nint main(int argc, char* argv[]) {\n ${code}\nreturn 0; }")
    check_c_source_compiles("${src}" ${res_var})
endfunction()

# wx_check_funcs(<...>)
# define a HAVE_... for every function in list if available
function(wx_check_funcs)
    foreach(func ${ARGN})
        string(TOUPPER ${func} func_upper)
        check_function_exists(${func} HAVE_${func_upper})
    endforeach()
endfunction()

if(NOT MSVC)
    check_symbol_exists(va_copy stdarg.h HAVE_VA_COPY)
    if(NOT HAVE_VA_COPY)
        # try to understand how can we copy va_lists
        set(VA_LIST_IS_ARRAY YES)
    endif()
endif()

# Check for availability of GCC's atomic operations builtins.
wx_check_c_source_compiles("
    unsigned int value=0;
    /* wxAtomicInc doesn't use return value here */
    __sync_fetch_and_add(&value, 2);
    __sync_sub_and_fetch(&value, 1);
    /* but wxAtomicDec does, so mimic that: */
    volatile unsigned int r2 = __sync_sub_and_fetch(&value, 1);"
    HAVE_GCC_ATOMIC_BUILTINS
    )

macro(wx_get_socket_param_type name code)
    # This test needs to be done in C++ mode since gsocket.cpp now
    # is C++ code and pointer cast that are possible even without
    # warning in C still fail in C++.
    wx_check_cxx_source_compiles(
        "socklen_t len;
        ${code}"
        ${name}_IS_SOCKLEN_T
        sys/types.h sys/socket.h
        )
    if(${name}_IS_SOCKLEN_T)
        set(${name} socklen_t)
    else()
        wx_check_cxx_source_compiles(
            "size_t len;
            ${code}"
            ${name}_IS_SIZE_T
            sys/types.h sys/socket.h
            )
        if(${name}_IS_SIZE_T)
            set(${name} size_t)
        else()
            wx_check_cxx_source_compiles(
                "int len;
                ${code}"
                ${name}_IS_INT
                sys/types.h sys/socket.h
                )
            if(${name}_IS_INT)
                set(${name} int)
            else()
                message(ERROR "Couldn't find ${name} for this system")
            endif()
        endif()
    endif()
endmacro()

# the following tests are for Unix(like) systems only
if(UNIX)
    if(wxUSE_LIBICONV)
        set(HAVE_ICONV ON)
        set(ICONV_CONST " ")
        if(ICONV_SECOND_ARGUMENT_IS_CONST)
            set(ICONV_CONST "const")
        endif()
    endif()

    # check for POSIX signals if we need them
    if(wxUSE_ON_FATAL_EXCEPTION)
        wx_check_funcs(sigaction)
        if(NOT HAVE_SIGACTION)
            message(WARNING "No POSIX signal functions on this system, wxApp::OnFatalException will not be called")
            wx_option_force_value(wxUSE_ON_FATAL_EXCEPTION OFF)
        endif()
    endif()

    # backtrace() and backtrace_symbols() for wxStackWalker
    if(wxUSE_STACKWALKER)
        wx_check_cxx_source_compiles("
            void *trace[1];
            char **messages;
            backtrace(trace, 1);
            messages = backtrace_symbols(trace, 1);"
            wxHAVE_BACKTRACE
            execinfo.h)
        if(NOT wxHAVE_BACKTRACE)
            message(WARNING "backtrace() is not available, wxStackWalker will not be available")
            wx_option_force_value(wxUSE_STACKWALKER OFF)
        else()
            wx_check_cxx_source_compiles(
                "int rc;
                 __cxxabiv1::__cxa_demangle(\"foo\", 0, 0, &rc);"
                HAVE_CXA_DEMANGLE
                cxxabi.h)
        endif()
    endif()

    wx_check_funcs(mkstemp mktemp)

    # get the library function to use for wxGetDiskSpace(): it is statfs() under
    # Linux and *BSD and statvfs() under Solaris and NetBSD
    wx_check_c_source_compiles("
        return 0; }
        #if defined(__BSD__)
        #include <sys/param.h>
        #include <sys/mount.h>
        #else
        #include <sys/vfs.h>
        #endif

        int foo() {
        long l;
        struct statfs fs;
        statfs(\"/\", &fs);
        l = fs.f_bsize;
        l += fs.f_blocks;
        l += fs.f_bavail;"
        HAVE_STATFS)
    wx_check_c_source_compiles("
        return 0; }
        #include <sys/statvfs.h>

        int foo() {
        long l;
        struct statvfs fs;
        statvfs(\"/\", &fs);
        l = fs.f_bsize;
        l += fs.f_blocks;
        l += fs.f_bavail;"
        HAVE_STATVFS)
    if(HAVE_STATVFS)
      set(WX_STATFS_T "struct statvfs")
    elseif(HAVE_STATFS)
      set(WX_STATFS_T "struct statfs")
      wx_check_cxx_source_compiles("
            return 0; }
            #if defined(__BSD__)
            #include <sys/param.h>
            #include <sys/mount.h>
            #else
            #include <sys/vfs.h>
            #endif

            int foo() {
            struct statfs fs;
            statfs(\"/\", &fs);"
        HAVE_STATFS_DECL)
    endif()

    if(NOT HAVE_STATFS AND NOT HAVE_STATVFS)
        message(WARNING "wxGetDiskSpace() function won't work without statfs()")
    endif()

    # check for fcntl() or at least flock() needed by Unix implementation of
    # wxSingleInstanceChecker
    if(wxUSE_SNGLINST_CHECKER)
        wx_check_funcs(fcntl flock)
        if(NOT HAVE_FCNTL AND NOT HAVE_FLOCK)
            message(WARNING "wxSingleInstanceChecker not available")
            wx_option_force_value(wxUSE_SNGLINST_CHECKER OFF)
        endif()
    endif()

    # look for a function to modify the environment
    wx_check_funcs(setenv putenv)
    if(HAVE_SETENV)
        wx_check_funcs(unsetenv)
    endif()

    set(HAVE_SOME_SLEEP_FUNC FALSE)
    if(APPLE)
        # Mac OS X/Darwin has both nanosleep and usleep
        # but only usleep is defined in unistd.h
        set(HAVE_USLEEP TRUE)
        set(HAVE_SOME_SLEEP_FUNC TRUE)
    endif()

    if(NOT HAVE_SOME_SLEEP_FUNC)
        # try nanosleep() in libc and libposix4, if this fails - usleep()
        check_symbol_exists(nanosleep time.h HAVE_NANOSLEEP)

        if(NOT HAVE_NANOSLEEP)
            check_symbol_exists(usleep unistd.h HAVE_USLEEP)
            if(NOT HAVE_USLEEP)
                message(WARNING "wxSleep() function will not work")
            endif()
        endif()
    endif()

    # check for uname (POSIX) and gethostname (BSD)
    check_symbol_exists(uname sys/utsname.h HAVE_UNAME)
    if(HAVE_UNAME)
        wx_check_funcs(gethostname)
    endif()

    cmake_push_check_state()
    list(APPEND CMAKE_REQUIRED_DEFINITIONS -D_REENTRANT)
    wx_check_funcs(strtok_r)
    cmake_pop_check_state()

    # check for inet_addr and inet_aton (these may live either in libc, or in
    # libnsl or libresolv or libsocket)
    # TODO

    wx_check_funcs(fdopen)

    if(wxBUILD_LARGEFILE_SUPPORT)
        wx_check_funcs(fseeko)
    endif()

    if(wxUSE_TARSTREAM)
        wx_check_funcs(sysconf)

        cmake_push_check_state()
        list(APPEND CMAKE_REQUIRED_DEFINITIONS -D_REENTRANT)
        check_symbol_exists(getpwuid_r pwd.h HAVE_GETPWUID_R)
        check_symbol_exists(getgrgid_r grp.h HAVE_GETGRGID_R)
        cmake_pop_check_state()
    endif()

    check_include_file(sys/soundcard.h HAVE_SYS_SOUNDCARD_H)

    set(HAVE_UNIX98_PRINTF ON)

    if(wxUSE_SOCKETS)
        # determine the type of third argument for getsockname
        wx_get_socket_param_type(WX_SOCKLEN_T "getsockname(0, 0, &len);")
        # Do this again for getsockopt as it may be different
        wx_get_socket_param_type(SOCKOPTLEN_T "getsockopt(0, 0, 0, 0, &len);")

        check_symbol_exists(gethostbyname netdb.h HAVE_GETHOSTBYNAME)
        check_symbol_exists(gethostbyname_r netdb.h HAVE_GETHOSTBYNAME_R)
        if(HAVE_GETHOSTBYNAME_R)
            check_prototype_definition(gethostbyname_r
                "int gethostbyname_r(const char *name, struct hostent *hp, struct hostent_data *hdata)"
                "0"
                "netdb.h"
                HAVE_FUNC_GETHOSTBYNAME_R_3)

            check_prototype_definition(gethostbyname_r
                "struct hostent *gethostbyname_r(const char *name, struct hostent *hp, char *buf, size_t buflen, int *herr)"
                "NULL"
                "netdb.h"
                HAVE_FUNC_GETHOSTBYNAME_R_5)

            check_prototype_definition(gethostbyname_r
                "int gethostbyname_r(const char *name, struct hostent *hp, char *buf, size_t buflen, struct hostent **result, int *herr)"
                "0"
                "netdb.h"
                HAVE_FUNC_GETHOSTBYNAME_R_6)
        endif()

        check_symbol_exists(getservbyname netdb.h HAVE_GETSERVBYNAME)
        check_symbol_exists(inet_aton arpa/inet.h HAVE_INET_ATON)
        check_symbol_exists(inet_addr arpa/inet.h HAVE_INET_ADDR)
    endif(wxUSE_SOCKETS)

    if(wxUSE_JOYSTICK AND NOT APPLE)
        check_include_files("linux/joystick.h" HAVE_JOYSTICK_H)
        if(NOT HAVE_JOYSTICK_H)
            message(WARNING "wxJoystick is not available")
            wx_option_force_value(wxUSE_JOYSTICK OFF)
        endif()
    endif()
endif(UNIX)

if(CMAKE_USE_PTHREADS_INIT)
    cmake_push_check_state(RESET)
    set(CMAKE_REQUIRED_LIBRARIES pthread)
    wx_check_cxx_source_compiles("
        void *p;
        pthread_cleanup_push(ThreadCleanupFunc, p);
        pthread_cleanup_pop(0);"
        wxHAVE_PTHREAD_CLEANUP
        pthread.h
        DEFINITION
        "void ThreadCleanupFunc(void *p) { }\;"
        )
    wx_check_c_source_compiles(
        "pthread_mutexattr_t attr;
        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);"
        HAVE_PTHREAD_MUTEXATTR_T
        pthread.h
        )
    if(HAVE_PTHREAD_MUTEXATTR_T)
        # check if we already have the declaration we need, it is not
        # present in some systems' headers
        wx_check_c_source_compiles(
            "pthread_mutexattr_t attr;
            pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);"
            HAVE_PTHREAD_MUTEXATTR_SETTYPE_DECL
            pthread.h
            )
    else()
        # don't despair, there may be another way to do it
        wx_check_c_source_compiles(
            "pthread_mutex_t attr = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;"
            HAVE_PTHREAD_RECURSIVE_MUTEX_INITIALIZER
            pthread.h
            )
        if(NOT HAVE_PTHREAD_RECURSIVE_MUTEX_INITIALIZER)
            # this may break code working elsewhere, so at least warn about it
            message(WARNING "wxMutex won't be recursive on this platform")
        endif()
    endif()
    wx_check_cxx_source_compiles(
        "void foo(abi::__forced_unwind&);"
        HAVE_ABI_FORCEDUNWIND
        cxxabi.h)
    cmake_pop_check_state()
endif() # CMAKE_USE_PTHREADS_INIT

check_symbol_exists(localtime_r time.h HAVE_LOCALTIME_R)
check_symbol_exists(gmtime_r time.h HAVE_GMTIME_R)

# ---------------------------------------------------------------------------
# Checks for typedefs
# ---------------------------------------------------------------------------

# check what exactly size_t is on this machine - this is necessary to avoid
# ambiguous overloads in several places, notably wx/string.h and wx/array.h

# an obvious check like wx_check_cxx_source_compiles(struct Foo { ... };) doesn't work
# with egcs (at least) up to 1.1.1 as it allows you to compile duplicate
# methods in a local class (i.e. class inside a function) declaration
# without any objections!!
#
# hence the hack below: we must have Foo at global scope!
wx_check_cxx_source_compiles("return 0; }

            struct Foo { void foo(size_t); void foo(unsigned int); };

            int bar() {"
            wxSIZE_T_IS_NOT_UINT
            stddef.h)
if(wxSIZE_T_IS_NOT_UINT)
    wx_check_cxx_source_compiles("return 0; }

            struct Foo { void foo(size_t); void foo(unsigned long); };

            int bar() {"
            wxSIZE_T_IS_NOT_ULONG
            stddef.h)
    if(NOT wxSIZE_T_IS_NOT_ULONG)
        set(wxSIZE_T_IS_ULONG YES)
    endif()
else()
    set(wxSIZE_T_IS_UINT YES)
endif()

# check if wchar_t is separate type
wx_check_cxx_source_compiles("return 0; }

            struct Foo { void foo(wchar_t);
                         void foo(unsigned short);
                         void foo(unsigned int);
                         void foo(unsigned long); };

            int bar() {"
            wxWCHAR_T_IS_REAL_TYPE
            wchar.h)

# ---------------------------------------------------------------------------
# Checks for structures
# ---------------------------------------------------------------------------

check_struct_has_member("struct passwd" pw_gecos pwd.h HAVE_PW_GECOS)

# ---------------------------------------------------------------------------
# Check for functions
# ---------------------------------------------------------------------------

# Check various string symbols
foreach(func
    ftime
    wcsftime wprintf
    putws fputws wprintf vswprintf vswscanf
    wcsdup wcsnlen wcscasecmp wcsncasecmp
    wcsrctombs
    wcstoull
    wcslen
    )
    string(TOUPPER ${func} func_upper)
    check_symbol_exists(${func} wchar.h HAVE_${func_upper})
endforeach()

# Check various functions
wx_check_funcs(fsync
               snprintf vsnprintf strnlen strtoull
               setpriority
               gettimeofday
               )

if(MSVC)
    check_symbol_exists(vsscanf stdio.h HAVE_VSSCANF)
endif()

# Check includes
check_include_file(fcntl.h HAVE_FCNTL_H)
check_include_file(langinfo.h HAVE_LANGINFO_H)
check_include_file(sched.h HAVE_SCHED_H)
check_include_file(unistd.h HAVE_UNISTD_H)
check_include_file(wchar.h HAVE_WCHAR_H)
check_include_file(wcstr.h HAVE_WCSTR_H)

if(wxUSE_DATETIME)
    # check for timezone variable
    #   doesn't exist under Darwin / Mac OS X which uses tm_gmtoff instead
    foreach(timezone_def timezone _timezone __timezone)
        wx_check_cxx_source_compiles("
            int tz;
            tz = ${timezone_def};"
        wxTEST_TZ_${timezone_def}
        time.h
        )
        if(${wxTEST_TZ_${timezone_def}})
            set(WX_TIMEZONE ${timezone_def})
            break()
        endif()
    endforeach()

    check_struct_has_member("struct tm" tm_gmtoff time.h WX_GMTOFF_IN_TM)
endif()

cmake_push_check_state(RESET)
set(CMAKE_REQUIRED_LIBRARIES ${CMAKE_DL_LIBS})
check_symbol_exists(dlopen dlfcn.h HAVE_DLOPEN)
cmake_pop_check_state()
if(HAVE_DLOPEN)
    check_symbol_exists(dladdr dlfcn.h HAVE_DLADDR)
    check_symbol_exists(dl_iterate_phdr link.h HAVE_DL_ITERATE_PHDR)
endif()

if(APPLE)
    set(wxUSE_EPOLL_DISPATCHER OFF)
    set(wxUSE_SELECT_DISPATCHER ON)
else()
    if(NOT WIN32)
        set(wxUSE_SELECT_DISPATCHER ON)
    endif()
    check_include_file(sys/epoll.h wxUSE_EPOLL_DISPATCHER)
endif()
check_include_file(sys/select.h HAVE_SYS_SELECT_H)

if(wxUSE_FSWATCHER)
    check_include_file(sys/inotify.h wxHAS_INOTIFY)
    if(NOT wxHAS_INOTIFY)
        check_include_file(sys/event.h wxHAS_KQUEUE)
    endif()
endif()

if(wxUSE_XLOCALE)
    check_include_file(xlocale.h HAVE_XLOCALE_H)
    set(CMAKE_EXTRA_INCLUDE_FILES locale.h)
    if(HAVE_XLOCALE_H)
        list(APPEND CMAKE_EXTRA_INCLUDE_FILES xlocale.h)
    endif()
    check_type_size(locale_t LOCALE_T)
    set(CMAKE_EXTRA_INCLUDE_FILES)
endif()

# Check sizes of various types
set(SYSTYPES size_t wchar_t int long short)

foreach(SYSTYPE ${SYSTYPES})
    string(TOUPPER ${SYSTYPE} SYSTYPE_UPPER)
    check_type_size(${SYSTYPE} SIZEOF_${SYSTYPE_UPPER})
    if(NOT HAVE_SIZEOF_${SYSTYPE_UPPER})
        # Add a definition if it is not available
        set(${SYSTYPE} ON)
    endif()
endforeach()

check_type_size("long long" SIZEOF_LONG_LONG)
check_type_size(ssize_t SSIZE_T)

test_big_endian(WORDS_BIGENDIAN)

# For generators using build type, ensure that wxHAVE_CEF_DEBUG matches it.
if(wxUSE_WEBVIEW_CHROMIUM AND DEFINED CMAKE_BUILD_TYPE)
    string(TOUPPER ${CMAKE_BUILD_TYPE} build_type)
    if(${build_type} STREQUAL DEBUG)
        set(wxHAVE_CEF_DEBUG ON)
    endif()
endif()

configure_file(build/cmake/setup.h.in ${wxSETUP_HEADER_FILE})
if(DEFINED wxSETUP_HEADER_FILE_DEBUG)
    # The debug version may be configured with different values in the future
    configure_file(build/cmake/setup.h.in ${wxSETUP_HEADER_FILE_DEBUG})
endif()
