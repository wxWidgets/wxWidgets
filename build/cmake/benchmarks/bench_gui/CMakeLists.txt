#############################################################################
# Name:        build/cmake/benchmarks/bench_gui/CMakeLists.txt
# Purpose:     CMake file for benchmarks
# Author:      Maarten Bent
# Created:     2021-02-07
# Copyright:   (c) 2021 wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

set(BENCH_GUI_SRC
    bench.cpp
    bench.h
    display.cpp
    image.cpp
    )

set(IMAGE_DATA
    ../../samples/image/horse.bmp:horse.bmp
    ../../samples/image/horse.jpg:horse.jpg
    ../../samples/image/horse.png:horse.png
    ../../samples/image/horse.tif:horse.tif
    )

wx_add_benchmark(bench_gui CONSOLE_GUI ${BENCH_GUI_SRC} DATA ${IMAGE_DATA})
