///////////////////////////////////////////////////////////////////////////////
// Name:        tests/file/dir.cpp
// Purpose:     wxDir unit test
// Author:      Francesco Montorsi (extracted from console sample)
// Created:     2010-06-19
// Copyright:   (c) 2010 wxWidgets team
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"


#include "wx/dir.h"
#include "wx/filename.h"
#include "wx/stdpaths.h"

#define DIRTEST_FOLDER      wxString("dirTest_folder")
#define SEP                 wxFileName::GetPathSeparator()

// ----------------------------------------------------------------------------
// test fixture
// ----------------------------------------------------------------------------

class DirTestCase
{
public:
    DirTestCase();
    ~DirTestCase();

protected:
    void CreateTempFile(const wxString& path);
    wxArrayString DirEnumHelper(wxDir& dir,
                               int flags = wxDIR_DEFAULT,
                               const wxString& filespec = wxEmptyString);

    wxDECLARE_NO_COPY_CLASS(DirTestCase);
};

// ----------------------------------------------------------------------------
// test fixture implementation
// ----------------------------------------------------------------------------

void DirTestCase::CreateTempFile(const wxString& path)
{
    wxFile f(path, wxFile::write);
    f.Write("dummy test file");
    f.Close();
}

DirTestCase::DirTestCase()
{
    // create a test directory hierarchy
    wxDir::Make(DIRTEST_FOLDER + SEP + "folder1" + SEP + "subfolder1", wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
    wxDir::Make(DIRTEST_FOLDER + SEP + "folder1" + SEP + "subfolder2", wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
    wxDir::Make(DIRTEST_FOLDER + SEP + "folder2", wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
    wxDir::Make(DIRTEST_FOLDER + SEP + "folder3" + SEP + "subfolder1", wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);

    CreateTempFile(DIRTEST_FOLDER + SEP + "folder1" + SEP + "subfolder2" + SEP + "dummy");
    CreateTempFile(DIRTEST_FOLDER + SEP + "dummy");
    CreateTempFile(DIRTEST_FOLDER + SEP + "folder3" + SEP + "subfolder1" + SEP + "dummy.foo");
    CreateTempFile(DIRTEST_FOLDER + SEP + "folder3" + SEP + "subfolder1" + SEP + "dummy.foo.bar");
}

DirTestCase::~DirTestCase()
{
    wxRemove(DIRTEST_FOLDER + SEP + "folder1" + SEP + "subfolder2" + SEP + "dummy");
    wxRemove(DIRTEST_FOLDER + SEP + "dummy");
    wxRemove(DIRTEST_FOLDER + SEP + "folder3" + SEP + "subfolder1" + SEP + "dummy.foo");
    wxRemove(DIRTEST_FOLDER + SEP + "folder3" + SEP + "subfolder1" + SEP + "dummy.foo.bar");
    wxDir::Remove(DIRTEST_FOLDER, wxPATH_RMDIR_RECURSIVE);
}

wxArrayString DirTestCase::DirEnumHelper(wxDir& dir,
                                         int flags,
                                         const wxString& filespec)
{
    wxArrayString ret;
    CHECK( dir.IsOpened() );

    wxString filename;
    bool cont = dir.GetFirst(&filename, filespec, flags);
    while ( cont )
    {
        ret.push_back(filename);
        cont = dir.GetNext(&filename);
    }

    return ret;
}

// ----------------------------------------------------------------------------
// tests themselves start here
// ----------------------------------------------------------------------------

TEST_CASE_METHOD(DirTestCase, "Dir::Enum", "[dir]")
{
    wxDir dir(DIRTEST_FOLDER);
    CHECK( dir.IsOpened() );

    // enumerating everything in test directory
    CHECK( DirEnumHelper(dir).size() == 4 );

    // enumerating really everything in test directory recursively
    CHECK( DirEnumHelper(dir, wxDIR_DEFAULT | wxDIR_DOTDOT).size() == 6 );

    // enumerating object files in test directory
    CHECK( DirEnumHelper(dir, wxDIR_DEFAULT, "*.o*").size() == 0 );

    // enumerating directories in test directory
    CHECK( DirEnumHelper(dir, wxDIR_DIRS).size() == 3 );

    // enumerating files in test directory
    CHECK( DirEnumHelper(dir, wxDIR_FILES).size() == 1 );

    // enumerating files including hidden in test directory
    CHECK( DirEnumHelper(dir, wxDIR_FILES | wxDIR_HIDDEN).size() == 1 );

    // enumerating files and folders in test directory
    CHECK( DirEnumHelper(dir, wxDIR_FILES | wxDIR_DIRS).size() == 4 );
}

class TestDirTraverser : public wxDirTraverser
{
public:
    wxArrayString dirs;

    virtual wxDirTraverseResult OnFile(const wxString& WXUNUSED(filename)) wxOVERRIDE
    {
        return wxDIR_CONTINUE;
    }

    virtual wxDirTraverseResult OnDir(const wxString& dirname) wxOVERRIDE
    {
        dirs.push_back(dirname);
        return wxDIR_CONTINUE;
    }
};

TEST_CASE_METHOD(DirTestCase, "Dir::Traverse", "[dir]")
{
    // enum all files
    wxArrayString files;
    CHECK( wxDir::GetAllFiles(DIRTEST_FOLDER, &files) == 4 );

    // enum all files according to the filter
    CHECK( wxDir::GetAllFiles(DIRTEST_FOLDER, &files, "*.foo") == 1 );

    // enum again with custom traverser
    wxDir dir(DIRTEST_FOLDER);
    TestDirTraverser traverser;
    dir.Traverse(traverser, wxEmptyString, wxDIR_DIRS | wxDIR_HIDDEN);
    CHECK( traverser.dirs.size() == 6 );
}

TEST_CASE_METHOD(DirTestCase, "Dir::Exists", "[dir]")
{
    struct
    {
        const char *dirname;
        bool shouldExist;
    } testData[] =
    {
        { ".", true },
        { "..", true },
        { "$USER_DOCS_DIR", true },
#if defined(__WINDOWS__)
        { "$USER_DOCS_DIR\\", true },
        { "$USER_DOCS_DIR\\\\", true },
        { "..\\..", true },
        { "$MSW_DRIVE", true },
        { "$MSW_DRIVE\\", true },
        { "$MSW_DRIVE\\\\", true },
        { "\\\\non_existent_share\\file", false },
        { "$MSW_DRIVE\\a\\directory\\which\\does\\not\\exist", false },
        { "$MSW_DRIVE\\a\\directory\\which\\does\\not\\exist\\", false },
        { "$MSW_DRIVE\\a\\directory\\which\\does\\not\\exist\\\\", false },
        { "test.exe", false }            // not a directory!
#elif defined(__UNIX__)
        { "../..", true },
        { "/", true },
        { "//", true },
        { "/usr/bin", true },
        { "/usr//bin", true },
        { "/usr///bin", true },
        { "/tmp/a/directory/which/does/not/exist", false },
        { "/bin/ls", false }             // not a directory!
#endif
    };

#ifdef __WINDOWS__
    wxString homedrive = wxGetenv("HOMEDRIVE");
    if ( homedrive.empty() )
        homedrive = "c:";
#endif // __WINDOWS__

    for ( size_t n = 0; n < WXSIZEOF(testData); n++ )
    {
        wxString dirname = testData[n].dirname;
        dirname.Replace("$USER_DOCS_DIR", wxStandardPaths::Get().GetDocumentsDir());

#ifdef __WINDOWS__
        dirname.Replace("$MSW_DRIVE", homedrive);
#endif // __WINDOWS__

        const bool shouldExist = testData[n].shouldExist;

        INFO("Directory " << dirname << ", should exist: " << shouldExist);
        CHECK( wxDir::Exists(dirname) == shouldExist );

        wxDir d(dirname);
        CHECK( d.IsOpened() == shouldExist );
    }

    CHECK( wxDir::Exists(wxGetCwd()) );
}

TEST_CASE_METHOD(DirTestCase, "Dir::GetName", "[dir]")
{
    wxDir d;

    CHECK( d.Open(".") );
    CHECK( d.GetName().Last() != wxFILE_SEP_PATH );
    CHECK( d.GetNameWithSep().Last() == wxFILE_SEP_PATH );
    CHECK( d.GetNameWithSep() == d.GetName() + wxFILE_SEP_PATH );

#ifdef __UNIX__
    CHECK( d.Open("/") );
    CHECK( d.GetName() == "/" );
    CHECK( d.GetNameWithSep() == "/" );
#endif
}
