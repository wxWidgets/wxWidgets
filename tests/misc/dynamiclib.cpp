///////////////////////////////////////////////////////////////////////////////
// Name:        tests/misc/dynamiclib.cpp
// Purpose:     Test wxDynamicLibrary
// Author:      Francesco Montorsi (extracted from console sample)
// Created:     2010-06-13
// Copyright:   (c) 2010 wxWidgets team
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#include "wx/dynlib.h"

#ifdef __UNIX__
    #include "wx/filename.h"
    #include "wx/log.h"
#endif

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class DynamicLibraryTestCase : public CppUnit::TestCase
{
public:
    DynamicLibraryTestCase() { }

private:
    CPPUNIT_TEST_SUITE( DynamicLibraryTestCase );
        CPPUNIT_TEST( Load );
    CPPUNIT_TEST_SUITE_END();

    void Load();

    wxDECLARE_NO_COPY_CLASS(DynamicLibraryTestCase);
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( DynamicLibraryTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( DynamicLibraryTestCase, "DynamicLibraryTestCase" );

void DynamicLibraryTestCase::Load()
{
#if defined(__WINDOWS__)
    static const wxChar *LIB_NAME = wxT("kernel32.dll");
    static const wxChar *FUNC_NAME = wxT("lstrlenA");
#elif defined(__UNIX__)
#ifdef __DARWIN__
    static const wxChar *LIB_NAME = wxT("/usr/lib/libc.dylib");
#else
    // weird: using just libc.so does *not* work!
    static const wxChar *LIB_NAME = wxT("/lib/libc.so.6");
#endif
    static const wxChar *FUNC_NAME = wxT("strlen");

    if ( !wxFileName::Exists(LIB_NAME) )
    {
        wxLogWarning("Shared library \"%s\" doesn't exist, "
                     "skipping DynamicLibraryTestCase::Load() test.");
        return;
    }
#else
    #error "don't know how to test wxDllLoader on this platform"
#endif

    wxDynamicLibrary lib(LIB_NAME);
    CPPUNIT_ASSERT( lib.IsLoaded() );

    typedef int (wxSTDCALL *wxStrlenType)(const char *);
    wxStrlenType pfnStrlen = (wxStrlenType)lib.GetSymbol(FUNC_NAME);

    wxString errMsg = wxString::Format("ERROR: function '%s' wasn't found in '%s'.\n",
                                       FUNC_NAME, LIB_NAME);
    CPPUNIT_ASSERT_MESSAGE( errMsg.ToStdString(), (pfnStrlen != NULL) );

    // Call the function dynamically loaded
    CPPUNIT_ASSERT( pfnStrlen("foo") == 3 );

#ifdef __WINDOWS__
    static const wxChar *FUNC_NAME_AW = wxT("lstrlen");

    typedef int (wxSTDCALL *wxStrlenTypeAorW)(const wxChar *);
    wxStrlenTypeAorW
        pfnStrlenAorW = (wxStrlenTypeAorW)lib.GetSymbolAorW(FUNC_NAME_AW);

    wxString errMsg2 = wxString::Format("ERROR: function '%s' wasn't found in '%s'.\n",
                                       FUNC_NAME_AW, LIB_NAME);
    CPPUNIT_ASSERT_MESSAGE( errMsg2.ToStdString(), (pfnStrlenAorW != NULL) );

    CPPUNIT_ASSERT( pfnStrlenAorW(wxT("foobar")) == 6 );
#endif // __WINDOWS__
}
