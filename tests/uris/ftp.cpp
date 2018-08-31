///////////////////////////////////////////////////////////////////////////////
// Name:        tests/uris/ftp.cpp
// Purpose:     wxFTP unit test
// Author:      Francesco Montorsi (extracted from console sample)
// Created:     2010-05-23
// Copyright:   (c) 2010 wxWidgets team
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif // WX_PRECOMP

#include <wx/protocol/ftp.h>

// For this to run, the following environment variables need to be defined:
//
//  - WX_FTP_TEST_HOST: the host to use for testing (e.g. ftp.example.com)
//  - WX_FTP_TEST_DIR: the directory in which to perform most of the tests
//  - WX_FTP_TEST_FILE: name of an existing file in this directory
//
// Optionally, WX_FTP_TEST_USER and WX_FTP_TEST_PASS may also be defined,
// otherwise anonymous FTP is used.
TEST_CASE("FTP", "[net][.]")
{
    wxString hostname,
             directory,
             valid_filename;
    if ( !wxGetEnv("WX_FTP_TEST_HOST", &hostname) ||
            !wxGetEnv("WX_FTP_TEST_DIR", &directory) ||
                !wxGetEnv("WX_FTP_TEST_FILE", &valid_filename) )
    {
        WARN("Skipping FTPTestCase because required WX_FTP_TEST_XXX "
             "environment variables are not defined.");
        return;
    }

    const wxString user = wxGetenv("WX_FTP_TEST_USER");
    const wxString password = wxGetenv("WX_FTP_TEST_PASS");

    class SocketInit
    {
    public:
        SocketInit() { wxSocketBase::Initialize(); }
        ~SocketInit() { wxSocketBase::Shutdown(); }
    } socketInit;

    // wxFTP cannot be a static variable as its ctor needs to access
    // wxWidgets internals after it has been initialized
    wxFTP ftp;

    if ( !user.empty() )
    {
        ftp.SetUser(user);
        ftp.SetPassword(password);
    }

    REQUIRE( ftp.Connect(hostname) );

    SECTION("List")
    {
        // test CWD
        CPPUNIT_ASSERT( ftp.ChDir(directory) );

        // test NLIST and LIST
        wxArrayString files;
        CPPUNIT_ASSERT( ftp.GetFilesList(files) );
        CPPUNIT_ASSERT( ftp.GetDirList(files) );

        CPPUNIT_ASSERT( ftp.ChDir(wxT("..")) );
    }

    SECTION("Download")
    {
        CPPUNIT_ASSERT( ftp.ChDir(directory) );

        // test RETR
        wxInputStream *in1 = ftp.GetInputStream("bloordyblop");
        CPPUNIT_ASSERT( in1 == NULL );
        delete in1;

        wxInputStream *in2 = ftp.GetInputStream(valid_filename);
        CPPUNIT_ASSERT( in2 != NULL );

        size_t size = in2->GetSize();
        wxChar *data = new wxChar[size];
        CPPUNIT_ASSERT( in2->Read(data, size).GetLastError() == wxSTREAM_NO_ERROR );

        delete [] data;
        delete in2;
    }

    SECTION("FileSize")
    {
        CPPUNIT_ASSERT( ftp.ChDir(directory) );

        CPPUNIT_ASSERT( ftp.FileExists(valid_filename) );

        int size = ftp.GetFileSize(valid_filename);
        CPPUNIT_ASSERT( size != -1 );
    }

    SECTION("Pwd")
    {
        CPPUNIT_ASSERT_EQUAL( "/", ftp.Pwd() );

        CPPUNIT_ASSERT( ftp.ChDir(directory) );
        CPPUNIT_ASSERT_EQUAL( directory, ftp.Pwd() );
    }

    SECTION("Misc")
    {
        CPPUNIT_ASSERT( ftp.SendCommand(wxT("STAT")) == '2' );
        CPPUNIT_ASSERT( ftp.SendCommand(wxT("HELP SITE")) == '2' );
    }

    SECTION("Upload")
    {
        if ( user.empty() )
        {
            WARN("Skipping upload test when using anonymous FTP.");
            return;
        }

        // upload a file
        static const wxChar *file1 = wxT("test1");
        wxOutputStream *out = ftp.GetOutputStream(file1);
        CPPUNIT_ASSERT( out != NULL );
        CPPUNIT_ASSERT( out->Write("First hello", 11).GetLastError() == wxSTREAM_NO_ERROR );
        delete out;

        // send a command to check the remote file
        CPPUNIT_ASSERT( ftp.SendCommand(wxString(wxT("STAT ")) + file1) == '2' );
        CPPUNIT_ASSERT( ftp.GetLastResult() == "11" );
    }
}
