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


#if wxUSE_FILE

#include "wx/ffile.h"
#include "wx/filefn.h"
#include "wx/textfile.h"
#include "wx/filesys.h"

#include "testfile.h"

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class FileFunctionsTestCase
{
protected:
    FileFunctionsTestCase();
    ~FileFunctionsTestCase();

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
// test fixture implementation
// ----------------------------------------------------------------------------

FileFunctionsTestCase::FileFunctionsTestCase()
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

FileFunctionsTestCase::~FileFunctionsTestCase()
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

// ----------------------------------------------------------------------------
// tests themselves start here
// ----------------------------------------------------------------------------

TEST_CASE_METHOD(FileFunctionsTestCase,
                 "FileFunctions::GetTempFolder",
                 "[filefn]")
{
    // Verify that obtained temporary folder is not empty.
    wxString tmpDir = wxFileName::GetTempDir();

    CHECK( !tmpDir.IsEmpty() );
}

TEST_CASE_METHOD(FileFunctionsTestCase,
                 "FileFunctions::CopyFile",
                 "[filefn]")
{
    const wxString filename1(wxS("horse.xpm"));
    const wxString& filename2 = m_fileNameWork;

    INFO("File 1: " << filename1 << "  File 2: " << filename2);

    REQUIRE( wxCopyFile(filename1, filename2) );

    // verify that the two files have the same contents!
    wxFFile f1(filename1, wxT("rb")),
            f2(filename2, wxT("rb"));

    REQUIRE( f1.IsOpened() );
    REQUIRE( f2.IsOpened() );

    wxString s1, s2;
    REQUIRE( f1.ReadAll(&s1) );
    REQUIRE( f2.ReadAll(&s2) );
    CHECK( s1 == s2 );

    CHECK( f1.Close() );
    CHECK( f2.Close() );
    CHECK( wxRemoveFile(filename2) );
}

TEST_CASE_METHOD(FileFunctionsTestCase,
                 "FileFunctions::CreateFile",
                 "[filefn]")
{
    // Create file name containing ASCII characters only.
    DoCreateFile(m_fileNameASCII);
    // Create file name containing non-ASCII characters.
    DoCreateFile(m_fileNameNonASCII);
}

void FileFunctionsTestCase::DoCreateFile(const wxString& filePath)
{
    INFO("File: " << filePath);

    // Create temporary file.
    wxTextFile file;
    REQUIRE( file.Create(filePath) );
    CHECK( file.Close() );

    wxRemoveFile(filePath);
}

TEST_CASE_METHOD(FileFunctionsTestCase,
                 "FileFunctions::FileExists",
                 "[filefn]")
{
    CHECK( wxFileExists(wxT("horse.png")) );
    CHECK( !wxFileExists(wxT("horse.___")) );

    // Check file name containing ASCII characters only.
    DoFileExists(m_fileNameASCII);
    // Check file name containing non-ASCII characters.
    DoFileExists(m_fileNameNonASCII);
}

void FileFunctionsTestCase::DoFileExists(const wxString& filePath)
{
    INFO("File: " << filePath);

    // Create temporary file.
    wxTextFile file;
    REQUIRE( file.Create(filePath) );
    CHECK( file.Close() );

    // Verify that file exists with 2 methods.
    CHECK( file.Exists() );
    CHECK( wxFileExists(filePath) );

    wxRemoveFile(filePath);
}

TEST_CASE_METHOD(FileFunctionsTestCase,
                 "FileFunctions::FindFile",
                 "[filefn]")
{
    // Find file name containing ASCII characters only.
    DoFindFile(m_fileNameASCII);
    // Find file name containing non-ASCII characters.
    DoFindFile(m_fileNameNonASCII);
}

void FileFunctionsTestCase::DoFindFile(const wxString& filePath)
{
    INFO("File: " << filePath);

    // Create temporary file.
    wxTextFile file;
    REQUIRE( file.Create(filePath) );
    CHECK( file.Close() );

    // Check if file can be found (method 1).
    wxString foundFile = wxFindFirstFile(filePath, wxFILE);
    CHECK( foundFile == filePath );

    // Check if file can be found (method 2).
    wxFileSystem fs;
    wxString furl = fs.FindFirst(filePath, wxFILE);
    wxFileName fname = wxFileName::URLToFileName(furl);
    foundFile = fname.GetFullPath();
    CHECK( foundFile == filePath );

    wxRemoveFile(filePath);
}

TEST_CASE_METHOD(FileFunctionsTestCase,
                 "FileFunctions::FindFileNext",
                 "[filefn]")
{
    // Construct file name containing ASCII characters only.
    const wxString fileMask(wxT("horse.*"));

    // Check using method 1.
    wxString foundFile1 = wxFindFirstFile(fileMask, wxFILE);
    wxString foundFile2 = wxFindNextFile();
    wxFileName fn1(foundFile1);
    wxFileName fn2(foundFile2);
    // Full names must be different.
    CHECK( foundFile1 != foundFile2 );
    // Base names must be the same.
    CHECK( fn1.GetName() == fn2.GetName() );

    // Check using method 2.
    wxFileSystem fs;
    wxString furl = fs.FindFirst(fileMask, wxFILE);
    fn1 = wxFileName::URLToFileName(furl);
    foundFile1 = fn1.GetFullPath();
    furl = fs.FindNext();
    fn2 = wxFileName::URLToFileName(furl);
    foundFile2 = fn2.GetFullPath();
    // Full names must be different.
    CHECK( fn1.GetFullPath() != fn2.GetFullPath() );
    // Base names must be the same.
    CHECK( fn1.GetName() == fn2.GetName() );
}

TEST_CASE_METHOD(FileFunctionsTestCase,
                 "FileFunctions::RemoveFile",
                 "[filefn]")
{
    // Create & remove file with name containing ASCII characters only.
    DoRemoveFile(m_fileNameASCII);
    // Create & remove file with name containing non-ASCII characters.
    DoRemoveFile(m_fileNameNonASCII);
}

void FileFunctionsTestCase::DoRemoveFile(const wxString& filePath)
{
    INFO("File: " << filePath);

    // Create temporary file.
    wxTextFile file;
    REQUIRE( file.Create(filePath) );
    CHECK( file.Close() );

    CHECK( file.Exists() );
    CHECK( wxRemoveFile(filePath) );
    CHECK( !file.Exists() );
}

TEST_CASE_METHOD(FileFunctionsTestCase,
                 "FileFunctions::RenameFile",
                 "[filefn]")
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
    INFO("File 1:" << oldFilePath << "  File 2: " << newFilePath);

    // Create temporary source file.
    wxTextFile file;
    REQUIRE( file.Create(oldFilePath) );
    CHECK( file.Close() );

    if ( withNew )
    {
        // Create destination file to test overwriting.
        wxTextFile file2;
        REQUIRE( file2.Create(newFilePath) );
        CHECK( file2.Close() );

        CHECK( wxFileExists(newFilePath) );
    }
    else
    {
        // Remove destination file
        if ( wxFileExists(newFilePath) )
        {
            wxRemoveFile(newFilePath);
        }

        CHECK( !wxFileExists(newFilePath) );
    }

    CHECK( wxFileExists(oldFilePath) );
    CHECK( wxFileExists(oldFilePath) );
    CHECK( wxFileExists(oldFilePath) );
    bool shouldFail = !overwrite && withNew;
    if ( shouldFail )
    {
        CHECK( !wxRenameFile(oldFilePath, newFilePath, overwrite));
        // Verify that file has not been renamed.
        CHECK( wxFileExists(oldFilePath) );
        CHECK( wxFileExists(newFilePath) );

        // Cleanup.
        wxRemoveFile(oldFilePath);
    }
    else
    {
        CHECK( wxRenameFile(oldFilePath, newFilePath, overwrite) );
        // Verify that file has been renamed.
        CHECK( !wxFileExists(oldFilePath) );
        CHECK( wxFileExists(newFilePath) );
    }

    // Cleanup.
    wxRemoveFile(newFilePath);
}

TEST_CASE_METHOD(FileFunctionsTestCase,
                 "FileFunctions::ConcatenateFiles",
                 "[filefn]")
{
    DoConcatFile(m_fileNameASCII, m_fileNameNonASCII, m_fileNameWork);
    DoConcatFile(m_fileNameNonASCII, m_fileNameASCII, m_fileNameWork);
}

void FileFunctionsTestCase::DoConcatFile(const wxString& filePath1,
                                         const wxString& filePath2,
                                         const wxString& destFilePath)
{
    INFO("File 1:" << filePath1
         << "  File 2: " << filePath2
         << "  File 3: " << destFilePath);

    // Prepare source data
    wxFFile f1(filePath1, wxT("wb")),
            f2(filePath2, wxT("wb"));
    REQUIRE( f1.IsOpened() );
    REQUIRE( f2.IsOpened() );

    wxString s1(wxT("1234567890"));
    wxString s2(wxT("abcdefghij"));
    CHECK( f1.Write(s1) );
    CHECK( f2.Write(s2) );

    CHECK( f1.Close() );
    CHECK( f2.Close() );

    // Concatenate source files
    CHECK( wxConcatFiles(filePath1, filePath2, destFilePath) );

    // Verify content of destination file
    REQUIRE( wxFileExists(destFilePath) );
    wxString sSrc = s1 + s2;
    wxString s3;
    wxFFile f3(destFilePath, wxT("rb"));
    CHECK( f3.ReadAll(&s3) );
    CHECK( sSrc.length() == s3.length() );
    CHECK( memcmp(sSrc.c_str(), s3.c_str(), sSrc.length()) == 0 );
    CHECK( f3.Close() );

    CHECK( wxRemoveFile(filePath1) );
    CHECK( wxRemoveFile(filePath2) );
    CHECK( wxRemoveFile(destFilePath) );
}

TEST_CASE_METHOD(FileFunctionsTestCase,
                 "FileFunctions::GetCwd",
                 "[filefn]")
{
    // Verify that obtained working directory is not empty.
    wxString cwd = wxGetCwd();

    CHECK( !cwd.IsEmpty() );
}

TEST_CASE_METHOD(FileFunctionsTestCase,
                 "FileFunctions::Eof",
                 "[filefn]")
{
    const wxString filename(wxT("horse.bmp"));
    INFO("File: " << filename);

    wxFFile file(filename, wxT("r"));
    // wxFFile::Eof must be false at start
    CHECK( !file.Eof() );
    CHECK( file.SeekEnd() );
    // wxFFile::Eof returns true only after attempt to read last byte
    char array[1];
    CHECK( file.Read(array, 1) == 0 );
    CHECK( file.Eof() );

    CHECK( file.Close() );
    // wxFFile::Eof after close should not cause crash but fail instead
    WX_ASSERT_FAILS_WITH_ASSERT( file.Eof() );
}

TEST_CASE_METHOD(FileFunctionsTestCase,
                 "FileFunctions::Error",
                 "[filefn]")
{
    const wxString filename(wxT("horse.bmp"));
    INFO("File: " << filename);

    wxFFile file(filename, wxT("r"));
    // wxFFile::Error must be false at start assuming file "horse.bmp" exists.
    CHECK( !file.Error() );
    // Attempt to write to file opened in readonly mode should cause error
    CHECK( !file.Write(filename) );
    CHECK( file.Error() );

    CHECK( file.Close() );
    // wxFFile::Error after close should not cause crash but fail instead
    WX_ASSERT_FAILS_WITH_ASSERT( file.Error() );
}

TEST_CASE_METHOD(FileFunctionsTestCase,
                 "FileFunctions::DirExists",
                 "[filefn]")
{
    wxString cwd = wxGetCwd();
    INFO("CWD: " << cwd);

    // Current working directory must exist
    CHECK(wxDirExists(cwd));
}

TEST_CASE_METHOD(FileFunctionsTestCase,
                 "FileFunctions::IsAbsolutePath",
                 "[filefn]")
{
    wxString name = wxT("horse.bmp");
    INFO("File: " << name);

    // File name is given as relative path
    CHECK( !wxIsAbsolutePath(name) );

    wxFileName filename(name);
    CHECK( filename.MakeAbsolute() );
    // wxFileName::GetFullPath returns absolute path
    CHECK( wxIsAbsolutePath(filename.GetFullPath()));

#ifdef __WINDOWS__
    CHECK( wxIsAbsolutePath("\\"));
    CHECK( wxIsAbsolutePath("c:"));
    CHECK( !wxIsAbsolutePath("c"));
#endif
}

TEST_CASE_METHOD(FileFunctionsTestCase,
                 "FileFunctions::PathOnly",
                 "[filefn]")
{
    wxString name = wxT("horse.bmp");
    // Get absolute path to horse.bmp
    wxFileName filename(name);
    CHECK( filename.MakeAbsolute() );

    wxString pathOnly = wxPathOnly(filename.GetFullPath());
    if ( !wxDirExists(pathOnly) )
        CHECK( pathOnly == wxString() );
}

// Unit tests for Mkdir and Rmdir doesn't cover non-ASCII directory names.
// Rmdir fails on them on Linux. See ticket #17644.
TEST_CASE_METHOD(FileFunctionsTestCase,
                 "FileFunctions::Mkdir",
                 "[filefn]")
{
    wxString dirname = wxString::FromUTF8("__wxMkdir_test_dir_with_\xc3\xb6");
    INFO("Dir: " << dirname);

    CHECK( wxMkdir(dirname) );
    CHECK( wxDirExists(dirname) );
    CHECK( wxRmdir(dirname) );
}

TEST_CASE_METHOD(FileFunctionsTestCase,
                 "FileFunctions::Rmdir",
                 "[filefn]")
{
    wxString dirname = wxString::FromUTF8("__wxRmdir_test_dir_with_\xc3\xb6");
    INFO("Dir: " << dirname);

    CHECK( wxMkdir(dirname) );
    CHECK( wxRmdir(dirname) );
    CHECK( !wxDirExists(dirname) );
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
