#############################################################################
# Name:        build/cmake/samples/html.cmake
# Purpose:     CMake script to build html samples
# Author:      Tobias Taschner
# Created:     2016-10-22
# Copyright:   (c) 2016 wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

set(wxSAMPLE_FOLDER html)
set(wxSAMPLE_SUBDIR html/)

wx_add_sample(about DATA data/about.htm data/logo.png LIBRARIES wxhtml)
wx_list_add_prefix(HELP_DATA_FILES helpfiles/
    Index.hhk
    another.hhc
    another.hhp
    another.htm
    book1.htm
    book2.htm
    contents.hhc
    main.htm
    page2-b.htm
    testing.hhp
    )
wx_add_sample(help DATA ${HELP_DATA_FILES} LIBRARIES wxhtml NAME htmlhelp DEPENDS wxUSE_HELP)
wx_add_sample(helpview DATA test.zip LIBRARIES wxhtml DEPENDS wxUSE_HELP)
#TODO: htmlctrl sample uses outdated definitions
#wx_add_sample(htmlctrl LIBRARIES wxhtml)
wx_add_sample(printing DATA logo6.gif test.htm LIBRARIES wxhtml NAME htmlprinting
    DEPENDS wxUSE_PRINTING_ARCHITECTURE)
wx_add_sample(test
    DATA
        imagemap.png pic.png pic2.bmp i18n.gif
        imagemap.htm tables.htm test.htm listtest.htm 8859_2.htm cp1250.htm
        regres.htm foo.png subsup.html
    LIBRARIES wxnet wxhtml NAME htmltest)
wx_add_sample(virtual DATA start.htm LIBRARIES wxhtml)
wx_add_sample(widget DATA start.htm LIBRARIES wxhtml)
wx_add_sample(zip DATA pages.zip start.htm LIBRARIES wxhtml DEPENDS wxUSE_FS_ZIP)

set(wxSAMPLE_SUBDIR)
set(wxSAMPLE_FOLDER)
