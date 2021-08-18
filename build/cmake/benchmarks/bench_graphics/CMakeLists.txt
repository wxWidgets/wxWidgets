#############################################################################
# Name:        build/cmake/benchmarks/bench_graphics/CMakeLists.txt
# Purpose:     CMake file for benchmarks
# Author:      Maarten Bent
# Created:     2021-02-07
# Copyright:   (c) 2021 wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

set(BENCH_GRAPGICS_SRC
    graphics.cpp
    )

wx_add_benchmark(bench_graphics CONSOLE_GUI ${BENCH_GRAPGICS_SRC})

if(wxUSE_OPENGL)
    wx_exe_link_libraries(bench_graphics wxgl)
endif()
