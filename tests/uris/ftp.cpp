///////////////////////////////////////////////////////////////////////////////
// Name:        tests/uris/ftp.cpp
// Purpose:     wxFTP unit test
// Author:      Francesco Montorsi (extracted from console sample)
// Created:     2010-05-23
// RCS-ID:      $Id$
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

#define FTP_ANONYMOUS

#ifdef FTP_ANONYMOUS
    static const char *hostname = "ftp.wxwidgets.org";
    static const char *directory = "/pub/2.8.11";
    static const char *invalid_filename = "a_file_which_does_not_exist";
    static const char *valid_filename = "MD5SUM";
        // NOTE: choose a small file or otherwise the FTPTestCase::Download()
        //       function will take (a lot of) time to complete!
#else
    static const char *hostname = "localhost";
    static const char *user = "guest";
    static const char *password = "";
    static const char *directory = "/etc";
    static const char *invalid_filename = "issue";
    static const char *valid_filename = "hosts";
#endif

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class FTPTestCase : public CppUnit::TestCase
{
public:
    FTPTestCase() {}

    virtual void setUp();
    virtual void tearDown();

private:
    CPPUNIT_TEST_SUITE( FTPTestCase );
        CPPUNIT_TEST( List );
        CPPUNIT_TEST( Download );
        CPPUNIT_TEST( FileSize );
        CPPUNIT_TEST( Misc );
#ifndef FTP_ANONYMOUS
        CPPUNIT_TEST( Upload );
#endif
    CPPUNIT_TEST_SUITE_END();

    void List();
    void Download();
    void FileSize();
    void Misc();
    void Upload();

    wxFTP *m_ftp;

    DECLARE_NO_COPY_CLASS(FTPTestCase)
};

// NOTE: we do not run FTPTestCase suite by default because buildslaves typically
//       do not have FTP connectivity enabled by default...
//CPPUNIT_TEST_SUITE_REGISTRATION( FTPTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( FTPTestCase, "FTPTestCase" );


void FTPTestCase::setUp()
{
    wxSocketBase::Initialize();

    // wxFTP cannot be a static variable as its ctor needs to access
    // wxWidgets internals after it has been initialized
    m_ftp = new wxFTP;

#ifndef FTP_ANONYMOUS
    m_ftp->SetUser(user);
    m_ftp->SetPassword(password);
#endif // FTP_ANONYMOUS/!FTP_ANONYMOUS
}

void FTPTestCase::tearDown()
{    
    delete m_ftp;

    wxSocketBase::Shutdown();
}

void FTPTestCase::List()
{
    CPPUNIT_ASSERT( m_ftp->Connect(hostname) );

    // test CWD
    CPPUNIT_ASSERT( m_ftp->ChDir(directory) );

    // test NLIST and LIST
    wxArrayString files;
    CPPUNIT_ASSERT( m_ftp->GetFilesList(files) );
    CPPUNIT_ASSERT( m_ftp->GetDirList(files) );
    
    CPPUNIT_ASSERT( m_ftp->ChDir(wxT("..")) );
}

void FTPTestCase::Download()
{
    CPPUNIT_ASSERT( m_ftp->Connect(hostname) );
    CPPUNIT_ASSERT( m_ftp->ChDir(directory) );

    // test RETR
    wxInputStream *in1 = m_ftp->GetInputStream(invalid_filename);
    CPPUNIT_ASSERT( in1 == NULL );
    delete in1;

    wxInputStream *in2 = m_ftp->GetInputStream(valid_filename);
    CPPUNIT_ASSERT( in2 != NULL );
    
    size_t size = in2->GetSize();
    wxChar *data = new wxChar[size];
    CPPUNIT_ASSERT( in2->Read(data, size).GetLastError() == wxSTREAM_NO_ERROR );
    
    delete [] data;
    delete in2;
}

void FTPTestCase::FileSize()
{
    CPPUNIT_ASSERT( m_ftp->Connect(hostname) );

    CPPUNIT_ASSERT( m_ftp->ChDir(directory) );
    
    CPPUNIT_ASSERT( m_ftp->FileExists(valid_filename) );

    int size = m_ftp->GetFileSize(valid_filename);
    CPPUNIT_ASSERT( size != -1 );
}

void FTPTestCase::Misc()
{
    CPPUNIT_ASSERT( m_ftp->Connect(hostname) );

    CPPUNIT_ASSERT( m_ftp->SendCommand(wxT("STAT")) == '2' );
    CPPUNIT_ASSERT( m_ftp->SendCommand(wxT("HELP SITE")) == '2' );
}

#ifndef FTP_ANONYMOUS
void FTPTestCase::Upload()
{
    CPPUNIT_ASSERT( m_ftp->Connect(hostname) );

    // upload a file
    static const wxChar *file1 = wxT("test1");
    wxOutputStream *out = m_ftp->GetOutputStream(file1);
    CPPUNIT_ASSERT( out != NULL );
    CPPUNIT_ASSERT( out->Write("First hello", 11).GetLastError() == wxSTREAM_NO_ERROR );
    delete out;

    // send a command to check the remote file
    CPPUNIT_ASSERT( m_ftp->SendCommand(wxString(wxT("STAT ")) + file1) == '2' );
    CPPUNIT_ASSERT( m_ftp->GetLastResult() == "11" );
}
#endif


