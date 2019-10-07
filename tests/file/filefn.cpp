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
    void setUp() wxOVERRIDE;
    void tearDown() wxOVERRIDE;

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
        CPPUNIT_TEST( FileEof );
        CPPUNIT_TEST( FileError );
        CPPUNIT_TEST( DirExists );
        CPPUNIT_TEST( IsAbsolutePath );
        CPPUNIT_TEST( PathOnly );
        CPPUNIT_TEST( Mkdir );
        CPPUNIT_TEST( Rmdir );
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
    void FileEof();
    void FileError();
    void DirExists();
    void IsAbsolutePath();
    void PathOnly();
    void Mkdir();
    void Rmdir();

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
    const wxString filename1(wxS("horse.xpm"));
    const wxString& filename2 = m_fileNameWork;

    const std::string msg =
        wxString::Format("File 1: %s  File 2:%s", filename1, filename2)
            .ToStdString();

    CPPUNIT_ASSERT_MESSAGE( msg, wxCopyFile(filename1, filename2) );

    // verify that the two files have the same contents!
    wxFFile f1(filename1, wxT("rb")),
            f2(filename2, wxT("rb"));

    CPPUNIT_ASSERT_MESSAGE( msg, f1.IsOpened() );
    CPPUNIT_ASSERT_MESSAGE( msg, f2.IsOpened() );

    wxString s1, s2;
    CPPUNIT_ASSERT_MESSAGE( msg, f1.ReadAll(&s1) );
    CPPUNIT_ASSERT_MESSAGE( msg, f2.ReadAll(&s2) );
    CPPUNIT_ASSERT_MESSAGE( msg, s1 == s2 );

    CPPUNIT_ASSERT_MESSAGE( msg, f1.Close() );
    CPPUNIT_ASSERT_MESSAGE( msg, f2.Close() );
    CPPUNIT_ASSERT_MESSAGE( msg, wxRemoveFile(filename2) );
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
    const std::string msg = wxString::Format("File: %s", filePath).ToStdString();

    // Create temporary file.
    wxTextFile file;
    CPPUNIT_ASSERT_MESSAGE( msg, file.Create(filePath) );
    CPPUNIT_ASSERT_MESSAGE( msg, file.Close() );

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
    const std::string msg = wxString::Format("File: %s", filePath).ToStdString();

    // Create temporary file.
    wxTextFile file;
    CPPUNIT_ASSERT_MESSAGE( msg, file.Create(filePath) );
    CPPUNIT_ASSERT_MESSAGE( msg, file.Close() );

    // Verify that file exists with 2 methods.
    CPPUNIT_ASSERT_MESSAGE( msg, file.Exists() );
    CPPUNIT_ASSERT_MESSAGE( msg, wxFileExists(filePath) );

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
    const std::string msg = wxString::Format("File: %s", filePath).ToStdString();

    // Create temporary file.
    wxTextFile file;
    CPPUNIT_ASSERT_MESSAGE( msg, file.Create(filePath) );
    CPPUNIT_ASSERT_MESSAGE( msg, file.Close() );

    // Check if file can be found (method 1).
    wxString foundFile = wxFindFirstFile(filePath, wxFILE);
    CPPUNIT_ASSERT_MESSAGE( msg, foundFile == filePath );

    // Check if file can be found (method 2).
    wxFileSystem fs;
    wxString furl = fs.FindFirst(filePath, wxFILE);
    wxFileName fname = wxFileName::URLToFileName(furl);
    foundFile = fname.GetFullPath();
    CPPUNIT_ASSERT_MESSAGE( msg, foundFile == filePath );

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
    fn1 = wxFileName::URLToFileName(furl);
    foundFile1 = fn1.GetFullPath();
    furl = fs.FindNext();
    fn2 = wxFileName::URLToFileName(furl);
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
    const std::string msg = wxString::Format("File: %s", filePath).ToStdString();

    // Create temporary file.
    wxTextFile file;
    CPPUNIT_ASSERT_MESSAGE( msg, file.Create(filePath) );
    CPPUNIT_ASSERT_MESSAGE( msg, file.Close() );

    CPPUNIT_ASSERT_MESSAGE( msg, file.Exists() );
    CPPUNIT_ASSERT_MESSAGE( msg, wxRemoveFile(filePath) );
    CPPUNIT_ASSERT_MESSAGE( msg, !file.Exists() );
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
    const std::string msg =
        wxString::Format(wxT("File 1: %s  File 2:%s"), oldFilePath, newFilePath)
            .ToStdString();

    // Create temporary source file.
    wxTextFile file;
    CPPUNIT_ASSERT_MESSAGE( msg, file.Create(oldFilePath) );
    CPPUNIT_ASSERT_MESSAGE( msg, file.Close() );

    if ( withNew )
    {
        // Create destination file to test overwriting.
        wxTextFile file2;
        CPPUNIT_ASSERT_MESSAGE( msg, file2.Create(newFilePath) );
        CPPUNIT_ASSERT_MESSAGE( msg, file2.Close() );

        CPPUNIT_ASSERT_MESSAGE( msg, wxFileExists(newFilePath) );
    }
    else
    {
        // Remove destination file
        if ( wxFileExists(newFilePath) )
        {
            wxRemoveFile(newFilePath);
        }

        CPPUNIT_ASSERT_MESSAGE( msg, !wxFileExists(newFilePath) );
    }

    CPPUNIT_ASSERT_MESSAGE( msg, wxFileExists(oldFilePath) );
    CPPUNIT_ASSERT_MESSAGE( msg, wxFileExists(oldFilePath) );
    CPPUNIT_ASSERT_MESSAGE( msg, wxFileExists(oldFilePath) );
    bool shouldFail = !overwrite && withNew;
    if ( shouldFail )
    {
        CPPUNIT_ASSERT_MESSAGE( msg, !wxRenameFile(oldFilePath, newFilePath, overwrite));
        // Verify that file has not been renamed.
        CPPUNIT_ASSERT_MESSAGE( msg, wxFileExists(oldFilePath) );
        CPPUNIT_ASSERT_MESSAGE( msg, wxFileExists(newFilePath) );

        // Cleanup.
        wxRemoveFile(oldFilePath);
    }
    else
    {
        CPPUNIT_ASSERT_MESSAGE( msg, wxRenameFile(oldFilePath, newFilePath, overwrite) );
        // Verify that file has been renamed.
        CPPUNIT_ASSERT_MESSAGE( msg, !wxFileExists(oldFilePath) );
        CPPUNIT_ASSERT_MESSAGE( msg, wxFileExists(newFilePath) );
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
    const std::string msg =
        wxString::Format("File 1: %s  File 2:%s  File 3: %s",
                         filePath1, filePath2, destFilePath)
            .ToStdString();

    // Prepare source data
    wxFFile f1(filePath1, wxT("wb")),
            f2(filePath2, wxT("wb"));
    CPPUNIT_ASSERT_MESSAGE( msg, f1.IsOpened() );
    CPPUNIT_ASSERT_MESSAGE( msg, f2.IsOpened() );

    wxString s1(wxT("1234567890"));
    wxString s2(wxT("abcdefghij"));
    CPPUNIT_ASSERT_MESSAGE( msg, f1.Write(s1) );
    CPPUNIT_ASSERT_MESSAGE( msg, f2.Write(s2) );

    CPPUNIT_ASSERT_MESSAGE( msg, f1.Close() );
    CPPUNIT_ASSERT_MESSAGE( msg, f2.Close() );

    // Concatenate source files
    CPPUNIT_ASSERT_MESSAGE( msg, wxConcatFiles(filePath1, filePath2, destFilePath) );

    // Verify content of destination file
    CPPUNIT_ASSERT_MESSAGE( msg, wxFileExists(destFilePath) );
    wxString sSrc = s1 + s2;
    wxString s3;
    wxFFile f3(destFilePath, wxT("rb"));
    CPPUNIT_ASSERT_MESSAGE( msg, f3.ReadAll(&s3) );
    CPPUNIT_ASSERT_MESSAGE( msg, sSrc.length() == s3.length() );
    CPPUNIT_ASSERT_MESSAGE( msg, memcmp(sSrc.c_str(), s3.c_str(), sSrc.length()) == 0 );
    CPPUNIT_ASSERT_MESSAGE( msg, f3.Close() );

    CPPUNIT_ASSERT_MESSAGE( msg, wxRemoveFile(filePath1) );
    CPPUNIT_ASSERT_MESSAGE( msg, wxRemoveFile(filePath2) );
    CPPUNIT_ASSERT_MESSAGE( msg, wxRemoveFile(destFilePath) );
}

void FileFunctionsTestCase::GetCwd()
{
    // Verify that obtained working directory is not empty.
    wxString cwd = wxGetCwd();

    CPPUNIT_ASSERT( !cwd.IsEmpty() );
}

void FileFunctionsTestCase::FileEof()
{
    const wxString filename(wxT("horse.bmp"));
    const std::string msg = wxString::Format("File: %s", filename).ToStdString();

    wxFFile file(filename, wxT("r"));
    // wxFFile::Eof must be false at start
    CPPUNIT_ASSERT_MESSAGE( msg, !file.Eof() );
    CPPUNIT_ASSERT_MESSAGE( msg, file.SeekEnd() );
    // wxFFile::Eof returns true only after attempt to read last byte
    char array[1];
    CPPUNIT_ASSERT_MESSAGE( msg, file.Read(array, 1) == 0 );
    CPPUNIT_ASSERT_MESSAGE( msg, file.Eof() );

    CPPUNIT_ASSERT_MESSAGE( msg, file.Close() );
    // wxFFile::Eof after close should not cause crash but fail instead
    bool failed = true;
    try
    {
    file.Eof();
        failed = false;
    }
    catch (...)
    {
    }
    CPPUNIT_ASSERT_MESSAGE( msg, failed );
}

void FileFunctionsTestCase::FileError()
{
    const wxString filename(wxT("horse.bmp"));
    const std::string msg = wxString::Format("File: %s", filename).ToStdString();

    wxFFile file(filename, wxT("r"));
    // wxFFile::Error must be false at start assuming file "horse.bmp" exists.
    CPPUNIT_ASSERT_MESSAGE( msg, !file.Error() );
    // Attempt to write to file opened in readonly mode should cause error
    CPPUNIT_ASSERT_MESSAGE( msg, !file.Write(filename) );
    CPPUNIT_ASSERT_MESSAGE( msg, file.Error() );

    CPPUNIT_ASSERT_MESSAGE( msg, file.Close() );
    // wxFFile::Error after close should not cause crash but fail instead
    bool failed = true;
    try
    {
        file.Error();
        failed = false;
    }
    catch (...)
    {
    }
    CPPUNIT_ASSERT_MESSAGE( msg, failed );
}


void FileFunctionsTestCase::DirExists()
{
    wxString cwd = wxGetCwd();
    const std::string msg = wxString::Format("CWD: %s", cwd).ToStdString();

    // Current working directory must exist
    CPPUNIT_ASSERT_MESSAGE( msg, wxDirExists(cwd));
}

void FileFunctionsTestCase::IsAbsolutePath()
{
    wxString name = wxT("horse.bmp");
    const std::string msg = wxString::Format("File: %s", name).ToStdString();

    // File name is given as relative path
    CPPUNIT_ASSERT_MESSAGE( msg, !wxIsAbsolutePath(name) );

    wxFileName filename(name);
    CPPUNIT_ASSERT( filename.MakeAbsolute() );
    // wxFileName::GetFullPath returns absolute path
    CPPUNIT_ASSERT_MESSAGE( msg, wxIsAbsolutePath(filename.GetFullPath()));
}

void FileFunctionsTestCase::PathOnly()
{
    wxString name = wxT("horse.bmp");
    // Get absolute path to horse.bmp
    wxFileName filename(name);
    CPPUNIT_ASSERT( filename.MakeAbsolute() );

    wxString pathOnly = wxPathOnly(filename.GetFullPath());
    if ( !wxDirExists(pathOnly) )
        CPPUNIT_ASSERT( pathOnly == wxString() );
}

// Unit tests for Mkdir and Rmdir doesn't cover non-ASCII directory names.
// Rmdir fails on them on Linux. See ticket #17644.
void FileFunctionsTestCase::Mkdir()
{
    wxString dirname = wxString::FromUTF8("__wxMkdir_test_dir_with_\xc3\xb6");
    const std::string msg = wxString::Format("Dir: %s", dirname).ToStdString();
    CPPUNIT_ASSERT_MESSAGE( msg, wxMkdir(dirname) );
    CPPUNIT_ASSERT_MESSAGE( msg, wxDirExists(dirname) );
    CPPUNIT_ASSERT_MESSAGE( msg, wxRmdir(dirname) );
}

void FileFunctionsTestCase::Rmdir()
{
    wxString dirname = wxString::FromUTF8("__wxRmdir_test_dir_with_\xc3\xb6");
    const std::string msg = wxString::Format("Dir: %s", dirname).ToStdString();

    CPPUNIT_ASSERT_MESSAGE( msg, wxMkdir(dirname) );
    CPPUNIT_ASSERT_MESSAGE( msg, wxRmdir(dirname) );
    CPPUNIT_ASSERT_MESSAGE( msg, !wxDirExists(dirname) );
}

/*
    TODO: other file functions to test:

wxChar* wxFileNameFromPath(wxChar *path);
wxString wxFileNameFromPath(const wxString& path);

bool wxIsWild(const wxString& pattern);

bool wxMatchWild(const wxString& pattern,  const wxString& text, bool dot_special = true);

bool wxSetWorkingDirectory(const wxString& d);

wxFileKind wxGetFileKind(int fd);
wxFileKind wxGetFileKind(FILE *fp);

bool wxIsWritable(const wxString &path);
bool wxIsReadable(const wxString &path);
bool wxIsExecutable(const wxString &path);
*/

#endif // wxUSE_FILE
