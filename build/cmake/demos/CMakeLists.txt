#############################################################################
# Name:        build/cmake/demos/CMakeLists.txt
# Purpose:     CMake script for demos
# Author:      Tobias Taschner
# Created:     2016-10-21
# Copyright:   (c) 2016 wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

wx_add_demo(bombs
    bombs.cpp
    bombs.h
    bombs1.cpp
    game.cpp
    game.h
    )

wx_add_demo(forty
    canvas.cpp
    canvas.h
    card.cpp
    card.h
    forty.cpp
    forty.h
    game.cpp
    game.h
    pile.cpp
    pile.h
    playerdg.cpp
    playerdg.h
    scoredg.cpp
    scoredg.h
    scorefil.cpp
    scorefil.h
    DATA
        about.htm
    LIBRARIES
        wxhtml wxxml
    )

wx_add_demo(fractal
    fractal.cpp
    )

wx_add_demo(life
    dialogs.cpp
    dialogs.h
    game.cpp
    game.h
    life.cpp
    life.h
    reader.cpp
    reader.h
    DATA
        breeder.lif
    )

wx_add_demo(poem
    wxpoem.cpp
    wxpoem.h
    DATA
        wxpoem.txt wxpoem.dat wxpoem.idx
    LIBRARIES
        wxhtml
    NAME
        wxpoem
    )
