///////////////////////////////////////////////////////////////////////////////
// Name:        tests/file/filefn.cpp
// Purpose:     generic file functions unit test
// Author:      Francesco Montorsi (extracted from console sample)
// Created:     2010-06-13
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
#include "wx/textfile.h"
#include "wx/filesys.h"

#include "testfile.h"

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class FileFunctionsTestCase : public CppUnit::TestCase
{
public:
    FileFunctionsTestCase() { }
    void setUp();
    void tearDown();

private:
    CPPUNIT_TEST_SUITE( FileFunctionsTestCase );
        CPPUNIT_TEST( GetTempFolder );
        CPPUNIT_TEST( CopyFile );
        CPPUNIT_TEST( CreateFile );
        CPPUNIT_TEST( FileExists );
        CPPUNIT_TEST( FindFile );
        CPPUNIT_TEST( FindFileNext );
        CPPUNIT_TEST( RemoveFile );
        CPPUNIT_TEST( RenameFile );
        CPPUNIT_TEST( ConcatenateFiles );
        CPPUNIT_TEST( GetCwd );
    CPPUNIT_TEST_SUITE_END();

    void GetTempFolder();
    void CopyFile();
    void CreateFile();
    void FileExists();
    void FindFile();
    void FindFileNext();
    void RemoveFile();
    void RenameFile();
    void ConcatenateFiles();
    void GetCwd();

    // Helper methods
    void DoCreateFile(const wxString& filePath);
    void DoFileExists(const wxString& filePath);
    void DoFindFile(const wxString& filePath);
    void DoRemoveFile(const wxString& filePath);
    void DoRenameFile(const wxString& oldFilePath,
                      const wxString& newFilePath,
                      bool overwrite,
                      bool withNew);
    void DoConcatFile(const wxString& filePath1,
                      const wxString& filePath2,
                      const wxString& destFilePath);

    wxString m_fileNameASCII;
    wxString m_fileNameNonASCII;
    wxString m_fileNameWork;

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

void FileFunctionsTestCase::setUp()
{
    // Initialize local data

    wxFileName fn1(wxFileName::GetTempDir(), wxT("wx_file_mask.txt"));
    m_fileNameASCII = fn1.GetFullPath();

    // This file name is 'wx_file_mask.txt' in Russian.
    wxFileName fn2(wxFileName::GetTempDir(),
      wxT("wx_\u043C\u0430\u0441\u043A\u0430_\u0444\u0430\u0439\u043B\u0430.txt"));
    m_fileNameNonASCII = fn2.GetFullPath();

    wxFileName fn3(wxFileName::GetTempDir(), wxT("wx_test_copy"));
    m_fileNameWork = fn3.GetFullPath();
}

void FileFunctionsTestCase::tearDown()
{
    // Remove all remaining temporary files
    if ( wxFileExists(m_fileNameASCII) )
    {
        wxRemoveFile(m_fileNameASCII);
    }
    if ( wxFileExists(m_fileNameNonASCII) )
    {
        wxRemoveFile(m_fileNameNonASCII);
    }
    if ( wxFileExists(m_fileNameWork) )
    {
        wxRemoveFile(m_fileNameWork);
    }
}

void FileFunctionsTestCase::GetTempFolder()
{
    // Verify that obtained temporary folder is not empty.
    wxString tmpDir = wxFileName::GetTempDir();

    CPPUNIT_ASSERT( !tmpDir.IsEmpty() );
}

void FileFunctionsTestCase::CopyFile()
{
    const wxString filename1(wxT("horse.bmp"));
    const wxString& filename2 = m_fileNameWork;

    const wxString msg = wxString::Format(wxT("File 1: %s  File 2:%s"),
                                          filename1.c_str(), filename2.c_str());
    const char *pUnitMsg = (const char*)msg.mb_str(wxConvUTF8);

    CPPUNIT_ASSERT_MESSAGE( pUnitMsg, wxCopyFile(filename1, filename2) );

    // verify that the two files have the same contents!
    wxFFile f1(filename1, wxT("rb")),
            f2(filename2, wxT("rb"));

    CPPUNIT_ASSERT_MESSAGE( pUnitMsg, f1.IsOpened() && f2.IsOpened() );

    wxString s1, s2;
    CPPUNIT_ASSERT_MESSAGE( pUnitMsg, f1.ReadAll(&s1) && f2.ReadAll(&s2) );
    CPPUNIT_ASSERT_MESSAGE( pUnitMsg, (s1.length() == s2.length()) &&
                    (memcmp(s1.c_str(), s2.c_str(), s1.length()) == 0) );

    CPPUNIT_ASSERT_MESSAGE( pUnitMsg, f1.Close() && f2.Close() );
    CPPUNIT_ASSERT_MESSAGE( pUnitMsg, wxRemoveFile(filename2) );
}

void FileFunctionsTestCase::CreateFile()
{
    // Create file name containing ASCII characters only.
    DoCreateFile(m_fileNameASCII);
    // Create file name containing non-ASCII characters.
    DoCreateFile(m_fileNameNonASCII);
}

void FileFunctionsTestCase::DoCreateFile(const wxString& filePath)
{
    const wxString msg = wxString::Format(wxT("File: %s"),
                                          filePath.c_str());
    const char *pUnitMsg = (const char*)msg.mb_str(wxConvUTF8);

    // Create temporary file.
    wxTextFile file;
    CPPUNIT_ASSERT_MESSAGE( pUnitMsg, file.Create(filePath) );
    CPPUNIT_ASSERT_MESSAGE( pUnitMsg, file.Close() );

    wxRemoveFile(filePath);
}

void FileFunctionsTestCase::FileExists()
{
    CPPUNIT_ASSERT( wxFileExists(wxT("horse.png")) );
    CPPUNIT_ASSERT( !wxFileExists(wxT("horse.___")) );

    // Check file name containing ASCII characters only.
    DoFileExists(m_fileNameASCII);
    // Check file name containing non-ASCII characters.
    DoFileExists(m_fileNameNonASCII);
}

void FileFunctionsTestCase::DoFileExists(const wxString& filePath)
{
    const wxString msg = wxString::Format(wxT("File: %s"),
                                          filePath.c_str());
    const char *pUnitMsg = (const char*)msg.mb_str(wxConvUTF8);

    // Create temporary file.
    wxTextFile file;
    CPPUNIT_ASSERT_MESSAGE( pUnitMsg, file.Create(filePath) );
    CPPUNIT_ASSERT_MESSAGE( pUnitMsg, file.Close() );

    // Verify that file exists with 2 methods.
    CPPUNIT_ASSERT_MESSAGE( pUnitMsg, file.Exists() );
    CPPUNIT_ASSERT_MESSAGE( pUnitMsg, wxFileExists(filePath) );

    wxRemoveFile(filePath);
}

void FileFunctionsTestCase::FindFile()
{
    // Find file name containing ASCII characters only.
    DoFindFile(m_fileNameASCII);
    // Find file name containing non-ASCII characters.
    DoFindFile(m_fileNameNonASCII);
}

void FileFunctionsTestCase::DoFindFile(const wxString& filePath)
{
    const wxString msg = wxString::Format(wxT("File: %s"),
                                          filePath.c_str());
    const char *pUnitMsg = (const char*)msg.mb_str(wxConvUTF8);

    // Create temporary file.
    wxTextFile file;
    CPPUNIT_ASSERT_MESSAGE( pUnitMsg, file.Create(filePath) );
    CPPUNIT_ASSERT_MESSAGE( pUnitMsg, file.Close() );

    // Check if file can be found (method 1).
    wxString foundFile = wxFindFirstFile(filePath, wxFILE);
    CPPUNIT_ASSERT_MESSAGE( pUnitMsg, foundFile == filePath );

    // Check if file can be found (method 2).
    wxFileSystem fs;
    wxString furl = fs.FindFirst(filePath, wxFILE);
    wxFileName fname = wxFileSystem::URLToFileName(furl);
    foundFile = fname.GetFullPath();
    CPPUNIT_ASSERT_MESSAGE( pUnitMsg, foundFile == filePath );

    wxRemoveFile(filePath);
}

void FileFunctionsTestCase::FindFileNext()
{
    // Construct file name containing ASCII characters only.
    const wxString fileMask(wxT("horse.*"));

    // Check using method 1.
    wxString foundFile1 = wxFindFirstFile(fileMask, wxFILE);
    wxString foundFile2 = wxFindNextFile();
    wxFileName fn1(foundFile1);
    wxFileName fn2(foundFile2);
    // Full names must be different.
    CPPUNIT_ASSERT( foundFile1 != foundFile2 );
    // Base names must be the same.
    CPPUNIT_ASSERT( fn1.GetName() == fn2.GetName() );

    // Check using method 2.
    wxFileSystem fs;
    wxString furl = fs.FindFirst(fileMask, wxFILE);
    fn1 = wxFileSystem::URLToFileName(furl);
    foundFile1 = fn1.GetFullPath();
    furl = fs.FindNext();
    fn2 = wxFileSystem::URLToFileName(furl);
    foundFile2 = fn2.GetFullPath();
    // Full names must be different.
    CPPUNIT_ASSERT( fn1.GetFullPath() != fn2.GetFullPath() );
    // Base names must be the same.
    CPPUNIT_ASSERT( fn1.GetName() == fn2.GetName() );
}

void FileFunctionsTestCase::RemoveFile()
{
    // Create & remove file with name containing ASCII characters only.
    DoRemoveFile(m_fileNameASCII);
    // Create & remove file with name containing non-ASCII characters.
    DoRemoveFile(m_fileNameNonASCII);
}

void FileFunctionsTestCase::DoRemoveFile(const wxString& filePath)
{
    const wxString msg = wxString::Format(wxT("File: %s"),
                                          filePath.c_str());
    const char *pUnitMsg = (const char*)msg.mb_str(wxConvUTF8);

    // Create temporary file.
    wxTextFile file;
    CPPUNIT_ASSERT_MESSAGE( pUnitMsg, file.Create(filePath) );
    CPPUNIT_ASSERT_MESSAGE( pUnitMsg, file.Close() );

    CPPUNIT_ASSERT_MESSAGE( pUnitMsg, file.Exists() );
    CPPUNIT_ASSERT_MESSAGE( pUnitMsg, wxRemoveFile(filePath) );
    CPPUNIT_ASSERT_MESSAGE( pUnitMsg, !file.Exists() );
}

void FileFunctionsTestCase::RenameFile()
{
    // Verify renaming file with/without overwriting
    // when new file already exist/don't exist.
    DoRenameFile(m_fileNameASCII, m_fileNameNonASCII, false, false);
    DoRenameFile(m_fileNameASCII, m_fileNameNonASCII, false, true);
    DoRenameFile(m_fileNameASCII, m_fileNameNonASCII, true, false);
    DoRenameFile(m_fileNameASCII, m_fileNameNonASCII, true, true);
    DoRenameFile(m_fileNameNonASCII, m_fileNameASCII, false, false);
    DoRenameFile(m_fileNameNonASCII, m_fileNameASCII, false, true);
    DoRenameFile(m_fileNameNonASCII, m_fileNameASCII, true, false);
    DoRenameFile(m_fileNameNonASCII, m_fileNameASCII, true, true);
}

void
FileFunctionsTestCase::DoRenameFile(const wxString& oldFilePath,
                                    const wxString& newFilePath,
                                    bool overwrite,
                                    bool withNew)
{
    const wxString msg = wxString::Format(wxT("File 1: %s  File 2: %s"),
                                          oldFilePath.c_str(), newFilePath.c_str());
    const char *pUnitMsg = (const char*)msg.mb_str(wxConvUTF8);

    // Create temporary source file.
    wxTextFile file;
    CPPUNIT_ASSERT_MESSAGE( pUnitMsg, file.Create(oldFilePath) );
    CPPUNIT_ASSERT_MESSAGE( pUnitMsg, file.Close() );

    if ( withNew )
    {
        // Create destination file to test overwriting.
        wxTextFile file2;
        CPPUNIT_ASSERT_MESSAGE( pUnitMsg, file2.Create(newFilePath) );
        CPPUNIT_ASSERT_MESSAGE( pUnitMsg, file2.Close() );

        CPPUNIT_ASSERT_MESSAGE( pUnitMsg, wxFileExists(newFilePath) );
    }
    else
    {
        // Remove destination file
        if ( wxFileExists(newFilePath) )
        {
            wxRemoveFile(newFilePath);
        }

        CPPUNIT_ASSERT_MESSAGE( pUnitMsg, !wxFileExists(newFilePath) );
    }

    CPPUNIT_ASSERT_MESSAGE( pUnitMsg, wxFileExists(oldFilePath) );
    CPPUNIT_ASSERT_MESSAGE( pUnitMsg, wxFileExists(oldFilePath) );
    CPPUNIT_ASSERT_MESSAGE( pUnitMsg, wxFileExists(oldFilePath) );
    bool shouldFail = !overwrite && withNew;
    if ( shouldFail )
    {
        CPPUNIT_ASSERT_MESSAGE( pUnitMsg, !wxRenameFile(oldFilePath, newFilePath, overwrite));
        // Verify that file has not been renamed.
        CPPUNIT_ASSERT_MESSAGE( pUnitMsg, wxFileExists(oldFilePath) );
        CPPUNIT_ASSERT_MESSAGE( pUnitMsg, wxFileExists(newFilePath) );

        // Cleanup.
        wxRemoveFile(oldFilePath);
    }
    else
    {
        CPPUNIT_ASSERT_MESSAGE( pUnitMsg, wxRenameFile(oldFilePath, newFilePath, overwrite) );
        // Verify that file has been renamed.
        CPPUNIT_ASSERT_MESSAGE( pUnitMsg, !wxFileExists(oldFilePath) );
        CPPUNIT_ASSERT_MESSAGE( pUnitMsg, wxFileExists(newFilePath) );
    }

    // Cleanup.
    wxRemoveFile(newFilePath);
}

void FileFunctionsTestCase::ConcatenateFiles()
{
    DoConcatFile(m_fileNameASCII, m_fileNameNonASCII, m_fileNameWork);
    DoConcatFile(m_fileNameNonASCII, m_fileNameASCII, m_fileNameWork);
}

void FileFunctionsTestCase::DoConcatFile(const wxString& filePath1,
                                         const wxString& filePath2,
                                         const wxString& destFilePath)
{
    const wxString msg = wxString::Format(wxT("File 1: %s  File 2: %s  File 3: %s"),
                                  filePath1.c_str(), filePath2.c_str(), destFilePath.c_str());
    const char *pUnitMsg = (const char*)msg.mb_str(wxConvUTF8);

    // Prepare source data
    wxFFile f1(filePath1, wxT("wb")),
            f2(filePath2, wxT("wb"));
    CPPUNIT_ASSERT_MESSAGE( pUnitMsg, f1.IsOpened() && f2.IsOpened() );

    wxString s1(wxT("1234567890"));
    wxString s2(wxT("abcdefghij"));
    CPPUNIT_ASSERT_MESSAGE( pUnitMsg, f1.Write(s1) );
    CPPUNIT_ASSERT_MESSAGE( pUnitMsg, f2.Write(s2) );

    CPPUNIT_ASSERT_MESSAGE( pUnitMsg, f1.Close() && f2.Close() );

    // Concatenate source files
    CPPUNIT_ASSERT_MESSAGE( pUnitMsg, wxConcatFiles(filePath1, filePath2, destFilePath) );

    // Verify content of destination file
    CPPUNIT_ASSERT_MESSAGE( pUnitMsg, wxFileExists(destFilePath) );
    wxString sSrc = s1 + s2;
    wxString s3;
    wxFFile f3(destFilePath, wxT("rb"));
    CPPUNIT_ASSERT_MESSAGE( pUnitMsg, f3.ReadAll(&s3) );
    CPPUNIT_ASSERT_MESSAGE( pUnitMsg, (sSrc.length() == s3.length()) &&
                    (memcmp(sSrc.c_str(), s3.c_str(), sSrc.length()) == 0) );
    CPPUNIT_ASSERT_MESSAGE( pUnitMsg, f3.Close() );

    CPPUNIT_ASSERT_MESSAGE( pUnitMsg, wxRemoveFile(filePath1) );
    CPPUNIT_ASSERT_MESSAGE( pUnitMsg, wxRemoveFile(filePath2) );
    CPPUNIT_ASSERT_MESSAGE( pUnitMsg, wxRemoveFile(destFilePath) );
}

void FileFunctionsTestCase::GetCwd()
{
    // Verify that obtained working directory is not empty.
    wxString cwd = wxGetCwd();

    CPPUNIT_ASSERT( !cwd.IsEmpty() );
}

/*
    TODO: other file functions to test:

bool wxDirExists(const wxString& pathName);

bool wxIsAbsolutePath(const wxString& filename);

wxChar* wxFileNameFromPath(wxChar *path);
wxString wxFileNameFromPath(const wxString& path);

wxString wxPathOnly(const wxString& path);

bool wxIsWild(const wxString& pattern);

bool wxMatchWild(const wxString& pattern,  const wxString& text, bool dot_special = true);

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
