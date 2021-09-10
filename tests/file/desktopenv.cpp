///////////////////////////////////////////////////////////////////////////////
// Name:        tests/file/desktopenv.cpp
// Purpose:     wxDesktopEnv unit test
// Author:      Igor Korot
// Created:     2021-09-05
// Copyright:   (c) 2019 wxWidgets development team
// License:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#include <fstream>
#include "wx/app.h"
#include "wx/filefn.h"
#include "wx/file.h"
#include "wx/desktopenv.h"

class DesktopEnvTestCase
{
public:
    void MoveToTrash();
};

TEST_CASE_METHOD(DesktopEnvTestCase, "DesktopEnvTestCase::MoveToTrash")
{
#if defined(__WXMSW__) || defined(__WXGTK__) || ( defined(__WXOSX__) && !( wxOSX_USE_IPHONE ) )
    std::ofstream out( "ffileinstream.test", std::ofstream::out );
    wxString currentDir = wxGetCwd() + "/";
    out << "test file to be moved to trash" << std::endl;
    out.close();
    int res = wxDesktopEnv::MoveToRecycleBin( currentDir + "ffileinstream.test" );
    CHECK( res );
    if( res )
        CHECK( !( wxFile::Exists( currentDir + "ffileinstream.test") ) );
    wxMkdir( "TrashTest" );
    std::ofstream out1( "TrashTest/ffileinstream.test", std::ofstream::out );
    out1 << "test file to be moved to trash with the directory" << std::endl;
    out1.close();
    CHECK( wxDesktopEnv::MoveToRecycleBin( currentDir + "TrashTest" ) );
    // also trying non-existing file
    CHECK( !wxDesktopEnv::MoveToRecycleBin( currentDir + "abc" ) );
#endif
}
