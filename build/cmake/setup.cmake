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

# Optimization: we skip a number of checks that always pass when building under
# Linux.
#
# We probably could also do it when cross-compiling to Linux, but for now be
# conservative and only skip the checks in the most common case.
if(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
    set(wxSKIP_CHECK_IF_LINUX TRUE)
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

if(NOT WIN32 AND wxUSE_VISIBILITY)
    check_cxx_compiler_flag(-fvisibility=hidden HAVE_VISIBILITY)
else()
    set(HAVE_VISIBILITY 0)
endif()

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

# Variants of the functions above that check for availability only if not
# building under Linux, otherwise just defines HAVE_... as we assume that the
# symbols being checked for are always available under Linux.
if(wxSKIP_CHECK_IF_LINUX)
    function(wx_check_funcs_if_not_linux)
        foreach(func ${ARGN})
            string(TOUPPER ${func} func_upper)
            set(HAVE_${func_upper} 1 PARENT_SCOPE)
        endforeach()
    endfunction()

    function(wx_check_include_file_if_not_linux include var)
        set(${var} 1 PARENT_SCOPE)
    endfunction()

    function(wx_check_symbols_exist_if_not_linux header)
        foreach(func ${ARGN})
            string(TOUPPER "HAVE_${func}" var)
            set(${var} 1 PARENT_SCOPE)
        endforeach()
    endfunction()

    function(wx_check_c_source_compiles_if_not_linux code var)
        set(${var} 1 PARENT_SCOPE)
    endfunction()
else()
    function(wx_check_funcs_if_not_linux)
        wx_check_funcs(${ARGV})
    endfunction()

    function(wx_check_include_file_if_not_linux)
        check_include_file(${ARGV})
    endfunction()

    # This function takes the header as the first argument and the rest are
    # symbols to check for in that header. HAVE_XXX is defined for each symbol
    # if it exists.
    function(wx_check_symbols_exist_if_not_linux header)
        foreach(func ${ARGN})
            string(TOUPPER "HAVE_${func}" var)
            set(MSG_TYPE STATUS)
            if(DEFINED ${var})
                set(MSG_TYPE DEBUG)
            endif()
            message(${MSG_TYPE} "Looking for ${func} in ${header}")
            check_symbol_exists(${func} ${header} ${var})
            if(${var})
                message(${MSG_TYPE} "Looking for ${func} in ${header} - found")
                set(${var} 1 PARENT_SCOPE)
            else()
                message(${MSG_TYPE} "Looking for ${func} in ${header} - not found")
            endif()
        endforeach()
    endfunction()

    function(wx_check_c_source_compiles_if_not_linux code)
        wx_check_c_source_compiles("${code}" ${ARGN})
    endfunction()
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

if(wxSKIP_CHECK_IF_LINUX)
# Linux always uses socklen_t for getsockname and getsockopt, so we can skip
# the checks.
macro(wx_get_socket_param_type name code)
    set(${name} socklen_t)
endmacro()
else()
macro(wx_get_socket_param_type name code)
    # This test needs to be done in C++ mode since socket.cpp is C++ code and
    # pointer casts that are possible in C can fail in C++.
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
endif()

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
        wx_check_funcs_if_not_linux(sigaction)
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

    wx_check_funcs_if_not_linux(mkstemp)
    if(NOT HAVE_MKSTEMP)
        wx_check_funcs(mktemp)
    endif()

    # get the library function to use for wxGetDiskSpace(): prefer POSIX
    # statvfs() if it exists, but fall back to Linux/BSD-specific statfs() if
    # necessary.
    wx_check_c_source_compiles_if_not_linux("
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
    else()
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
        if(HAVE_STATFS)
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
        else()
            message(WARNING "wxGetDiskSpace() function won't work without statfs()")
        endif()
    endif()

    # check for fcntl() or at least flock() needed by Unix implementation of
    # wxSingleInstanceChecker
    if(wxUSE_SNGLINST_CHECKER)
        wx_check_funcs_if_not_linux(fcntl)
        if(NOT HAVE_FCNTL)
            wx_check_funcs_if_not_linux(flock)
            if(NOT HAVE_FLOCK)
                message(WARNING "wxSingleInstanceChecker not available")
                wx_option_force_value(wxUSE_SNGLINST_CHECKER OFF)
            endif()
        endif()
    endif()

    # look for a function to modify the environment
    wx_check_funcs_if_not_linux(setenv)
    if(HAVE_SETENV)
        wx_check_funcs_if_not_linux(unsetenv)
    else()
        wx_check_funcs_if_not_linux(putenv)
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
        wx_check_symbols_exist_if_not_linux(time.h nanosleep)

        if(NOT HAVE_NANOSLEEP)
            wx_check_symbols_exist_if_not_linux(unistd.h usleep)
            if(NOT HAVE_USLEEP)
                message(WARNING "wxSleep() function will not work")
            endif()
        endif()
    endif()

    # check for uname (POSIX) and gethostname (BSD)
    wx_check_symbols_exist_if_not_linux(sys/utsname.h uname)
    if(NOT HAVE_UNAME)
        wx_check_funcs_if_not_linux(gethostname)
    endif()

    cmake_push_check_state()
    list(APPEND CMAKE_REQUIRED_DEFINITIONS -D_REENTRANT)
    wx_check_funcs_if_not_linux(strtok_r)
    cmake_pop_check_state()

    # check for inet_addr and inet_aton (these may live either in libc, or in
    # libnsl or libresolv or libsocket)
    # TODO

    wx_check_funcs_if_not_linux(fdopen)

    if(wxBUILD_LARGEFILE_SUPPORT)
        wx_check_funcs_if_not_linux(fseeko)
    endif()

    if(wxUSE_TARSTREAM)
        wx_check_funcs_if_not_linux(sysconf)

        cmake_push_check_state()
        list(APPEND CMAKE_REQUIRED_DEFINITIONS -D_REENTRANT)
        wx_check_symbols_exist_if_not_linux(pwd.h getpwuid_r)
        wx_check_symbols_exist_if_not_linux(grp.h getgrgid_r)
        cmake_pop_check_state()
    endif()

    check_include_file(sys/soundcard.h HAVE_SYS_SOUNDCARD_H)

    set(HAVE_UNIX98_PRINTF ON)

    if(wxUSE_SOCKETS)
        # determine the type of third argument for getsockname
        wx_get_socket_param_type(WX_SOCKLEN_T "getsockname(0, 0, &len);")
        # Do this again for getsockopt as it may be different
        wx_get_socket_param_type(SOCKOPTLEN_T "getsockopt(0, 0, 0, 0, &len);")

        wx_check_c_source_compiles(
            [[struct hostent ret, *retp;
            char buf[1024];
            int buflen = 1024;
            int e;
            (void)gethostbyname_r("www.wxwidgets.org", &ret, buf, buflen, &retp, &e);]]
            HAVE_FUNC_GETHOSTBYNAME_R_6
            netdb.h
            )

        if(HAVE_FUNC_GETHOSTBYNAME_R_6)
            # Assume that we have getservbyname_r() with consistent signature,
            # this does seem to be the case in practice.
            set(HAVE_FUNC_GETSERVBYNAME_R_6 1)
        else()
            wx_check_c_source_compiles(
                [[struct hostent ret;
                char buf[1024];
                int buflen = 1024;
                int e;
                struct hostent *ret = gethostbyname_r("www.wxwidgets.org", &ret, buf, buflen, &e);]]
                HAVE_FUNC_GETHOSTBYNAME_R_5
                netdb.h
                )
            if(HAVE_FUNC_GETHOSTBYNAME_R_5)
                # See above.
                set(HAVE_FUNC_GETSERVBYNAME_R_5 1)
            endif()
        endif()

        wx_check_symbols_exist_if_not_linux(arpa/inet.h inet_aton inet_addr)
    endif(wxUSE_SOCKETS)

    if(wxUSE_JOYSTICK AND WXGTK)
        check_include_files("linux/joystick.h" HAVE_JOYSTICK_H)
        if(NOT HAVE_JOYSTICK_H)
            message(WARNING "wxJoystick is not available")
            wx_option_force_value(wxUSE_JOYSTICK OFF)
        endif()
    endif()
endif(UNIX)

if(CMAKE_USE_PTHREADS_INIT)
    wx_check_symbols_exist_if_not_linux(
        pthread.h
        pthread_attr_setstacksize
        pthread_cancel
        pthread_setconcurrency
        sched_yield
    )

    # Somehow CMake finds this function under macOS and iOS but it's not
    # actually available there.
    if(NOT APPLE)
        wx_check_symbols_exist_if_not_linux(pthread.h pthread_mutex_timedlock)
    endif()

    wx_check_funcs_if_not_linux(pthread_attr_getschedpolicy)
    if(HAVE_PTHREAD_ATTR_GETSCHEDPOLICY)
        wx_check_funcs_if_not_linux(pthread_attr_setschedparam)
        if(HAVE_PTHREAD_ATTR_SETSCHEDPARAM)
            wx_check_funcs_if_not_linux(sched_get_priority_max)
            if(HAVE_SCHED_GET_PRIORITY_MAX)
                set(HAVE_THREAD_PRIORITY_FUNCTIONS 1)
            else()
                message(WARNING "Setting thread priority will not work")
            endif()
        endif()
    endif()

    cmake_push_check_state(RESET)

    if(wxSKIP_CHECK_IF_LINUX)
        # Linux always has pthread_cleanup_push and pthread_cleanup_pop, so we
        # can skip the checks.
        set(wxHAVE_PTHREAD_CLEANUP 1)
        set(HAVE_PTHREAD_MUTEXATTR_T 1)
        set(HAVE_PTHREAD_MUTEXATTR_SETTYPE_DECL 1)
    else()
        set(CMAKE_REQUIRED_LIBRARIES ${CMAKE_THREAD_LIBS_INIT})
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
    endif()

    wx_check_cxx_source_compiles(
        "void foo(abi::__forced_unwind&);"
        HAVE_ABI_FORCEDUNWIND
        cxxabi.h)
    cmake_pop_check_state()
endif() # CMAKE_USE_PTHREADS_INIT

if(NOT WIN32)
    wx_check_symbols_exist_if_not_linux(time.h localtime_r gmtime_r)
endif()

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

# ---------------------------------------------------------------------------
# Checks for structures
# ---------------------------------------------------------------------------

if(NOT WIN32)

if(wxSKIP_CHECK_IF_LINUX)
    # struct passwd always has pw_gecos under Linux, so we can skip the check.
    set(HAVE_PW_GECOS 1)
else()
    check_struct_has_member("struct passwd" pw_gecos pwd.h HAVE_PW_GECOS)
endif()

endif(NOT WIN32)

# ---------------------------------------------------------------------------
# Check for functions
# ---------------------------------------------------------------------------

# Check various symbols from wchar.h.
cmake_push_check_state(RESET)
set(CMAKE_REQUIRED_INCLUDES "wchar.h")

wx_check_funcs_if_not_linux(
    wcsftime wprintf
    fputws wprintf vswprintf vswscanf
    wcsdup wcsnlen wcscasecmp wcsncasecmp
    wcsrctombs
    wcstoull
    wcslen
)

cmake_pop_check_state()

# Check various functions
wx_check_funcs_if_not_linux(
               snprintf vsnprintf strnlen strtoull
               setpriority
               gettimeofday
               vsscanf
               wcsrtombs
               )

if(NOT HAVE_GETTIMEOFDAY)
    wx_check_funcs(ftime)
endif()

# Check includes
if(NOT WIN32)
    wx_check_funcs_if_not_linux(fsync)
    wx_check_include_file_if_not_linux(langinfo.h HAVE_LANGINFO_H)
    wx_check_include_file_if_not_linux(sched.h HAVE_SCHED_H)
    wx_check_include_file_if_not_linux(unistd.h HAVE_UNISTD_H)
endif()

if(wxUSE_DATETIME)
    # check for timezone variable:
    #   - under POSIX systems we prefer using POSIX timezone and under Linux
    #     we don't even check for it as we know that it's available
    #   - but under Windows, at least with MinGW/clang, it results in
    #     deprecation warnings, so prefer using __timezone instead.
    #   - it doesn't exist at all under Darwin / Mac OS X which uses tm_gmtoff
    #     instead, so don't bother checking for it there at all
    if(wxSKIP_CHECK_IF_LINUX)
        set(WX_TIMEZONE timezone)
    elseif(APPLE)
        set(timezone_candidates)
    elseif(WIN32)
        set(timezone_candidates _timezone;__timezone;timezone)
    else()
        set(timezone_candidates timezone;_timezone;__timezone)
    endif()

    foreach(timezone_def ${timezone_candidates})
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

if(NOT WIN32)

wx_check_c_source_compiles_if_not_linux(
    "nl_langinfo(_NL_TIME_FIRST_WEEKDAY);"
    HAVE_NL_TIME_FIRST_WEEKDAY
    langinfo.h
    )

cmake_push_check_state(RESET)
set(CMAKE_REQUIRED_LIBRARIES ${CMAKE_DL_LIBS})
wx_check_symbols_exist_if_not_linux(dlfcn.h dlopen dladdr)
cmake_pop_check_state()
if(HAVE_DLOPEN)
    wx_check_symbols_exist_if_not_linux(link.h dl_iterate_phdr)
endif()

endif(NOT WIN32)

if(APPLE)
    set(wxUSE_EPOLL_DISPATCHER OFF)
    set(wxUSE_SELECT_DISPATCHER ON)
else()
    if(NOT WIN32)
        set(wxUSE_SELECT_DISPATCHER ON)
        wx_check_include_file_if_not_linux(sys/epoll.h wxUSE_EPOLL_DISPATCHER)
    endif()
endif()

if(NOT WIN32)

wx_check_include_file_if_not_linux(sys/select.h HAVE_SYS_SELECT_H)

if(wxUSE_FSWATCHER)
    wx_check_include_file_if_not_linux(sys/inotify.h wxHAS_INOTIFY)
    if(NOT wxHAS_INOTIFY)
        check_include_file(sys/event.h wxHAS_KQUEUE)
    endif()
endif()

if(wxUSE_XLOCALE)
    list(APPEND xlocale_headers locale.h stdlib.h)
    check_include_file(xlocale.h HAVE_XLOCALE_H)
    if(HAVE_XLOCALE_H)
        list(APPEND xlocale_headers xlocale.h)
    endif()
    wx_check_c_source_compiles_if_not_linux("
        locale_t t;
        strtod_l(NULL, NULL, t);
        strtol_l(NULL, NULL, 0, t);
        strtoul_l(NULL, NULL, 0, t);
"
        HAVE_LOCALE_T
        ${xlocale_headers}
        )
endif()

endif(NOT WIN32)

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
