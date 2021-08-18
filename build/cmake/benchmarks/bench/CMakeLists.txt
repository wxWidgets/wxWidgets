#############################################################################
# Name:        build/cmake/benchmarks/bench/CMakeLists.txt
# Purpose:     CMake file for benchmarks
# Author:      Maarten Bent
# Created:     2021-02-07
# Copyright:   (c) 2021 wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

set(BENCH_SRC
    bench.cpp
    bench.h
    datetime.cpp
    htmlparser/htmlpars.cpp
    htmlparser/htmlpars.h
    htmlparser/htmltag.cpp
    htmlparser/htmltag.h
    ipcclient.cpp
    log.cpp
    mbconv.cpp
    printfbench.cpp
    strings.cpp
    tls.cpp
    )

set(BENCH_DATA
    htmltest.html
    )

wx_add_benchmark(bench CONSOLE ${BENCH_SRC} DATA ${BENCH_DATA})

if(wxUSE_SOCKETS)
    wx_exe_link_libraries(bench wxnet)
endif()
