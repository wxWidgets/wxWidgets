#############################################################################
# Name:        build/cmake/tests/base/CMakeLists.txt
# Purpose:     CMake file for base test
# Author:      Tobias Taschner
# Created:     2016-10-31
# Copyright:   (c) 2016 wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

set(TEST_SRC
    test.cpp
    any/anytest.cpp
    archive/archivetest.cpp
    archive/ziptest.cpp
    archive/tartest.cpp
    arrays/arrays.cpp
    base64/base64.cpp
    cmdline/cmdlinetest.cpp
    config/fileconf.cpp
    config/regconf.cpp
    datetime/datetimetest.cpp
    events/evthandler.cpp
    events/evtlooptest.cpp
    events/evtsource.cpp
    events/stopwatch.cpp
    events/timertest.cpp
    exec/exec.cpp
    file/dir.cpp
    file/filefn.cpp
    file/filetest.cpp
    filekind/filekind.cpp
    filename/filenametest.cpp
    filesys/filesystest.cpp
    fontmap/fontmaptest.cpp
    formatconverter/formatconvertertest.cpp
    fswatcher/fswatchertest.cpp
    hashes/hashes.cpp
    interactive/output.cpp
    interactive/input.cpp
    intl/intltest.cpp
    lists/lists.cpp
    log/logtest.cpp
    longlong/longlongtest.cpp
    mbconv/convautotest.cpp
    mbconv/mbconvtest.cpp
    misc/dynamiclib.cpp
    misc/environ.cpp
    misc/metatest.cpp
    misc/misctests.cpp
    misc/module.cpp
    misc/pathlist.cpp
    misc/typeinfotest.cpp
    net/ipc.cpp
    net/socket.cpp
    net/webrequest.cpp
    regex/regextest.cpp
    regex/wxregextest.cpp
    scopeguard/scopeguardtest.cpp
    strings/iostream.cpp
    strings/numformatter.cpp
    strings/strings.cpp
    strings/stdstrings.cpp
    strings/tokenizer.cpp
    strings/unichar.cpp
    strings/unicode.cpp
    strings/vararg.cpp
    strings/crt.cpp
    strings/vsnprintf.cpp
    strings/hexconv.cpp
    streams/datastreamtest.cpp
    streams/ffilestream.cpp
    streams/fileback.cpp
    streams/filestream.cpp
    streams/iostreams.cpp
    streams/largefile.cpp
    streams/lzmastream.cpp
    streams/memstream.cpp
    streams/socketstream.cpp
    streams/sstream.cpp
    streams/stdstream.cpp
    streams/tempfile.cpp
    streams/textstreamtest.cpp
    streams/zlibstream.cpp
    textfile/textfiletest.cpp
    thread/atomic.cpp
    thread/misc.cpp
    thread/queue.cpp
    thread/tls.cpp
    uris/ftp.cpp
    uris/uris.cpp
    uris/url.cpp
    weakref/evtconnection.cpp
    weakref/weakref.cpp
    xlocale/xlocale.cpp

    testprec.h
    testableframe.h
    testdate.h
    testfile.h
    archive/archivetest.h
    streams/bstream.h
    )

if(wxUSE_XML)
    list(APPEND TEST_SRC xml/xmltest.cpp)
endif()

set(TEST_DATA
    intl/en_GB/internat.mo
    intl/en_GB/internat.po
    intl/fr/internat.mo
    intl/fr/internat.po
    intl/ja/internat.mo
    intl/ja/internat.po
    horse.bmp
    horse.png
    horse.xpm
    testdata.conf
    )

wx_add_test(test_base CONSOLE ${TEST_SRC}
    DATA ${TEST_DATA}
    )
if(wxUSE_SOCKETS)
    wx_exe_link_libraries(test_base wxnet)
endif()
if(wxUSE_XML)
    wx_exe_link_libraries(test_base wxxml)
endif()
