///////////////////////////////////////////////////////////////////////////////
// Name:        tests/file/desktopenv.cpp
// Purpose:     wxDesktopEnv unit test
// Author:      Igor Korot
// Created:     2015-11-17
// Copyright:   (C) wxWidgets team
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/app.h"
#include "wx/desktopenv.h"

#include <fstream>

#define DATABUFFER_SIZE     1024

class DesktopEnvTestCase
{
public:
    void MoveToTrash();
};

TEST_CASE_METHOD(DesktopEnvTestCase, "DesktopEnvTestCase::MoveToTrash")
{
    wxDesktopEnv env;
    char buf[DATABUFFER_SIZE];
    std::ofstream out( "ffileinstream.test", std::ofstream::out );
    wxString currentDir = wxGetCwd() + "/";

    // Init the data buffer.
    for( size_t i = 0; i < DATABUFFER_SIZE; i++ )
    {
        buf[i] = (i % 0xFF);
        // Save the data
        out << buf << std::endl;
    }
    out.close();
    CHECK( env.MoveFileToRecycleBin( currentDir + "ffileinstream.test" ) );
    wxMkdir( "TrashTest" );
    std::ofstream out1( "TrashTest/ffileinstream.test", std::ofstream::out );

    // Init the data buffer.
    for( size_t i = 0; i < DATABUFFER_SIZE; i++ )
    {
        buf[i] = (i % 0xFF);
        // Save the data
        out1 << buf << std::endl;
    }
    out1.close();
    CHECK( env.MoveFileToRecycleBin( currentDir + "TrashTest" ) );
}

