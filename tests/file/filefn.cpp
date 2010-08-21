///////////////////////////////////////////////////////////////////////////////
// Name:        tests/file/filefn.cpp
// Purpose:     generic file functions unit test
// Author:      Francesco Montorsi (extracted from console sample)
// Created:     2010-06-13
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

#if wxUSE_FILE

#include "wx/ffile.h"
#include "wx/filefn.h"

#include "testfile.h"

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class FileFunctionsTestCase : public CppUnit::TestCase
{
public:
    FileFunctionsTestCase() { }

private:
    CPPUNIT_TEST_SUITE( FileFunctionsTestCase );
        CPPUNIT_TEST( CopyFile );
    CPPUNIT_TEST_SUITE_END();

    void CopyFile();

    wxDECLARE_NO_COPY_CLASS(FileFunctionsTestCase);
};

// ----------------------------------------------------------------------------
// CppUnit macros
// ----------------------------------------------------------------------------

CPPUNIT_TEST_SUITE_REGISTRATION( FileFunctionsTestCase );
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( FileFunctionsTestCase, "FileFunctionsTestCase" );

// ----------------------------------------------------------------------------
// tests implementation
// ----------------------------------------------------------------------------

void FileFunctionsTestCase::CopyFile()
{
    static const wxChar *filename1 = wxT("horse.bmp");
    static const wxChar *filename2 = wxT("test_copy");
    
    CPPUNIT_ASSERT( wxCopyFile(filename1, filename2) );

    // verify that the two files have the same contents!
    wxFFile f1(filename1, wxT("rb")),
            f2(filename2, wxT("rb"));

    CPPUNIT_ASSERT( f1.IsOpened() && f2.IsOpened() );
    
    wxString s1, s2;
    CPPUNIT_ASSERT( f1.ReadAll(&s1) && f2.ReadAll(&s2) );
    CPPUNIT_ASSERT( (s1.length() == s2.length()) &&
                    (memcmp(s1.c_str(), s2.c_str(), s1.length()) == 0) );

    CPPUNIT_ASSERT( f1.Close() && f2.Close() );
    CPPUNIT_ASSERT( wxRemoveFile(filename2) );
}


/*
    TODO: other file functions to test:

bool wxFileExists(const wxString& filename);

bool wxDirExists(const wxString& pathName);

bool wxIsAbsolutePath(const wxString& filename);

wxChar* wxFileNameFromPath(wxChar *path);
wxString wxFileNameFromPath(const wxString& path);

wxString wxPathOnly(const wxString& path);

wxString wxFindFirstFile(const wxString& spec, int flags = wxFILE);
wxString wxFindNextFile();

bool wxIsWild(const wxString& pattern);

bool wxMatchWild(const wxString& pattern,  const wxString& text, bool dot_special = true);

bool wxConcatFiles(const wxString& file1, const wxString& file2, const wxString& file3);

bool wxRemoveFile(const wxString& file);

bool wxRenameFile(const wxString& file1, const wxString& file2, bool overwrite = true);

wxString wxGetCwd();

bool wxSetWorkingDirectory(const wxString& d);

bool wxMkdir(const wxString& dir, int perm = wxS_DIR_DEFAULT);

bool wxRmdir(const wxString& dir, int flags = 0);

wxFileKind wxGetFileKind(int fd);
wxFileKind wxGetFileKind(FILE *fp);

bool wxIsWritable(const wxString &path);
bool wxIsReadable(const wxString &path);
bool wxIsExecutable(const wxString &path);
*/

#endif // wxUSE_FILE
