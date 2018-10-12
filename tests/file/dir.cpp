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

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/dir.h"
#include "wx/filename.h"
#include "wx/stdpaths.h"

#define DIRTEST_FOLDER      wxString("dirTest_folder")
#define SEP                 wxFileName::GetPathSeparator()

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class DirTestCase : public CppUnit::TestCase
{
public:
    DirTestCase() { }

    virtual void setUp() wxOVERRIDE;
    virtual void tearDown() wxOVERRIDE;

private:
    CPPUNIT_TEST_SUITE( DirTestCase );
        CPPUNIT_TEST( DirExists );
        CPPUNIT_TEST( Traverse );
        CPPUNIT_TEST( Enum );
        CPPUNIT_TEST( GetName );
    CPPUNIT_TEST_SUITE_END();

    void DirExists();
    void Traverse();
    void Enum();
    void GetName();

    void CreateTempFile(const wxString& path);
    wxArrayString DirEnumHelper(wxDir& dir,
                               int flags = wxDIR_DEFAULT,
                               const wxString& filespec = wxEmptyString);

    wxDECLARE_NO_COPY_CLASS(DirTestCase);
};

// ----------------------------------------------------------------------------
// CppUnit macros
// ----------------------------------------------------------------------------

CPPUNIT_TEST_SUITE_REGISTRATION( DirTestCase );
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( DirTestCase, "DirTestCase" );

// ----------------------------------------------------------------------------
// tests implementation
// ----------------------------------------------------------------------------

void DirTestCase::CreateTempFile(const wxString& path)
{
    wxFile f(path, wxFile::write);
    f.Write("dummy test file");
    f.Close();
}

void DirTestCase::setUp()
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

void DirTestCase::tearDown()
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
    CPPUNIT_ASSERT( dir.IsOpened() );

    wxString filename;
    bool cont = dir.GetFirst(&filename, filespec, flags);
    while ( cont )
    {
        ret.push_back(filename);
        cont = dir.GetNext(&filename);
    }

    return ret;
}

void DirTestCase::Enum()
{
    wxDir dir(DIRTEST_FOLDER);
    CPPUNIT_ASSERT( dir.IsOpened() );

    // enumerating everything in test directory
    CPPUNIT_ASSERT_EQUAL(4, DirEnumHelper(dir).size());

    // enumerating really everything in test directory recursively
    CPPUNIT_ASSERT_EQUAL(6, DirEnumHelper(dir, wxDIR_DEFAULT | wxDIR_DOTDOT).size());

    // enumerating object files in test directory
    CPPUNIT_ASSERT_EQUAL(0, DirEnumHelper(dir, wxDIR_DEFAULT, "*.o*").size());

    // enumerating directories in test directory
    CPPUNIT_ASSERT_EQUAL(3, DirEnumHelper(dir, wxDIR_DIRS).size());

    // enumerating files in test directory
    CPPUNIT_ASSERT_EQUAL(1, DirEnumHelper(dir, wxDIR_FILES).size());

    // enumerating files including hidden in test directory
    CPPUNIT_ASSERT_EQUAL(1, DirEnumHelper(dir, wxDIR_FILES | wxDIR_HIDDEN).size());

    // enumerating files and folders in test directory
    CPPUNIT_ASSERT_EQUAL(4, DirEnumHelper(dir, wxDIR_FILES | wxDIR_DIRS).size());
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

void DirTestCase::Traverse()
{
    // enum all files
    wxArrayString files;
    CPPUNIT_ASSERT_EQUAL(4, wxDir::GetAllFiles(DIRTEST_FOLDER, &files));

    // enum all files according to the filter
    CPPUNIT_ASSERT_EQUAL(1, wxDir::GetAllFiles(DIRTEST_FOLDER, &files, "*.foo"));

    // enum again with custom traverser
    wxDir dir(DIRTEST_FOLDER);
    TestDirTraverser traverser;
    dir.Traverse(traverser, wxEmptyString, wxDIR_DIRS | wxDIR_HIDDEN);
    CPPUNIT_ASSERT_EQUAL(6, traverser.dirs.size());
}

void DirTestCase::DirExists()
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

        std::string errDesc = wxString::Format("failed on directory '%s'", dirname).ToStdString();
        CPPUNIT_ASSERT_EQUAL_MESSAGE(errDesc, testData[n].shouldExist, wxDir::Exists(dirname));

        wxDir d(dirname);
        CPPUNIT_ASSERT_EQUAL(testData[n].shouldExist, d.IsOpened());
    }

    CPPUNIT_ASSERT( wxDir::Exists(wxGetCwd()) );
}

void DirTestCase::GetName()
{
    wxDir d;

    CPPUNIT_ASSERT( d.Open(".") );
    CPPUNIT_ASSERT( d.GetName().Last() != wxFILE_SEP_PATH );
    CPPUNIT_ASSERT( d.GetNameWithSep().Last() == wxFILE_SEP_PATH );
    CPPUNIT_ASSERT_EQUAL( d.GetName() + wxFILE_SEP_PATH,
                          d.GetNameWithSep() );

#ifdef __UNIX__
    CPPUNIT_ASSERT( d.Open("/") );
    CPPUNIT_ASSERT_EQUAL( "/", d.GetName() );
    CPPUNIT_ASSERT_EQUAL( "/", d.GetNameWithSep() );
#endif
}
