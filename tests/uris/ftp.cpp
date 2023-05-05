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


#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif // WX_PRECOMP

#include <wx/protocol/ftp.h>

#include <memory>

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
        REQUIRE( ftp.ChDir(directory) );

        // test NLIST and LIST
        wxArrayString files;
        CHECK( ftp.GetFilesList(files) );

        wxArrayString dirs;
        CHECK( ftp.GetDirList(dirs) );

        CHECK( files.size() == dirs.size() );
    }

    SECTION("Download")
    {
        REQUIRE( ftp.ChDir(directory) );

        // test RETR
        std::unique_ptr<wxInputStream> in1(ftp.GetInputStream("bloordyblop"));
        CHECK( !in1 );

        std::unique_ptr<wxInputStream> in2(ftp.GetInputStream(valid_filename));
        CHECK( in2 );

        size_t size = in2->GetSize();
        std::vector<unsigned char> data(size);
        CHECK( in2->Read(&data[0], size).GetLastError() == wxSTREAM_NO_ERROR );
    }

    SECTION("FileSize")
    {
        REQUIRE( ftp.ChDir(directory) );

        REQUIRE( ftp.FileExists(valid_filename) );

        int size = ftp.GetFileSize(valid_filename);
        CHECK( size != -1 );
    }

    SECTION("Pwd")
    {
        CHECK( ftp.Pwd()  == "/" );

        REQUIRE( ftp.ChDir(directory) );
        CHECK( ftp.Pwd()  == directory );
    }

    SECTION("Misc")
    {
        CHECK( ftp.SendCommand(wxT("STAT")) == '2' );
        CHECK( ftp.SendCommand(wxT("HELP SITE")) == '2' );
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
        std::unique_ptr<wxOutputStream> out(ftp.GetOutputStream(file1));
        REQUIRE( out );
        CHECK( out->Write("First hello", 11).GetLastError() == wxSTREAM_NO_ERROR );

        // send a command to check the remote file
        REQUIRE( ftp.SendCommand(wxString(wxT("STAT ")) + file1) == '2' );
        CHECK( ftp.GetLastResult() == "11" );
    }
}
