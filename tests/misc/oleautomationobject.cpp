///////////////////////////////////////////////////////////////////////////////
// Name:        tests/misc/oleautomationobject.cpp
// Purpose:     Test wxAutomationObject
// Author:      PB
// Copyright:   (c) the wxWidgets team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if defined(__WXMSW__) && wxUSE_OLE_AUTOMATION && wxUSE_VARIANT

#include "wx/log.h"
#include "wx/msw/ole/automtn.h"
#include "wx/msw/ole/oleutils.h"

// ----------------------------------------------------------------------------
// wxAutomationObject tests
// ----------------------------------------------------------------------------

namespace
{

// Microsoft Scripting Runtime objects are part of Windows, so they are used
// for testing instead of objects of an application which may not be installed.
bool CreateScriptingObject(wxAutomationObject& object, const wxString& progId)
{
    wxLogNull noLog;

    if ( !object.CreateInstance(progId) )
    {
        WARN("Skipping the test because " << progId << " is not available.");
        return false;
    }

    return true;
}

} // anonymous namespace

// FileSystemObject.BuildPath(Path, Name) is used to test passing method
// arguments by position and by name.
TEST_CASE("wxAutomationObject::CallMethod", "[automation][msw]")
{
    wxAutomationObject fso;
    if ( !CreateScriptingObject(fso, "Scripting.FileSystemObject") )
        return;

    const wxString path("C:\\Dir\\File.txt");

    SECTION("Positional")
    {
        CHECK( fso.CallMethod("BuildPath", "C:\\Dir", "File.txt") == path );
    }

    SECTION("Named")
    {
        CHECK( fso.CallMethod("BuildPath",
                              wxVariant("C:\\Dir", "Path"),
                              wxVariant("File.txt", "Name")) == path );

        CHECK( fso.CallMethod("BuildPath",
                              wxVariant("File.txt", "Name"),
                              wxVariant("C:\\Dir", "Path")) == path );
    }

    SECTION("PositionalAndNamed")
    {
        CHECK( fso.CallMethod("BuildPath",
                              "C:\\Dir",
                              wxVariant("File.txt", "Name")) == path );

        wxVariant args[] = { wxVariant("C:\\Dir"), wxVariant("File.txt", "Name") };
        CHECK( fso.CallMethod("BuildPath", WXSIZEOF(args), args) == path );
    }

    SECTION("PositionalAfterNamed")
    {
        WX_ASSERT_FAILS_WITH_ASSERT( fso.CallMethod("BuildPath",
                                                    wxVariant("File.txt", "Name"),
                                                    "C:\\Dir") );
    }

    SECTION("UnknownName")
    {
        wxLogNull noLog;

        CHECK( fso.CallMethod("BuildPath",
                              "C:\\Dir",
                              wxVariant("File.txt", "NoSuchName")).IsNull() );
    }
}

// Dictionary.Add(Key, Item) is used to test passing method arguments and
// Dictionary.Item(Key) and Dictionary.CompareMode to test properties.
TEST_CASE("wxAutomationObject::Property", "[automation][msw]")
{
    wxAutomationObject dict;
    if ( !CreateScriptingObject(dict, "Scripting.Dictionary") )
        return;

    SECTION("Add")
    {
        dict.CallMethod("Add", "a", 1L);
        dict.CallMethod("Add", wxVariant("b", "Key"), wxVariant(2L, "Item"));
        dict.CallMethod("Add", wxVariant(3L, "Item"), wxVariant("c", "Key"));
        dict.CallMethod("Add", "d", wxVariant(4L, "Item"));

        CHECK( dict.GetProperty("Count") == 4L );
        CHECK( dict.GetProperty("Item", "a") == 1L );
        CHECK( dict.GetProperty("Item", "b") == 2L );
        CHECK( dict.GetProperty("Item", "c") == 3L );
        CHECK( dict.GetProperty("Item", "d") == 4L );
    }

    SECTION("Get")
    {
        dict.CallMethod("Add", "a", 1L);

        CHECK( dict.GetProperty("Item", wxVariant("a", "Key")) == 1L );
    }

    SECTION("Put")
    {
        CHECK( dict.PutProperty("CompareMode", 1L) );
        CHECK( dict.GetProperty("CompareMode") == 1L );

        // The name of the value is ignored.
        CHECK( dict.PutProperty("CompareMode", wxVariant(0L, "CompareMode")) );
        CHECK( dict.GetProperty("CompareMode") == 0L );
    }

    SECTION("PutIndexed")
    {
        CHECK( dict.PutProperty("Item", "a", 1L) );
        CHECK( dict.GetProperty("Item", "a") == 1L );

        CHECK( dict.PutProperty("Item", wxVariant("a", "Key"), 2L) );
        CHECK( dict.GetProperty("Item", "a") == 2L );

        CHECK( dict.PutProperty("Item", "a", wxVariant(3L, "Item")) );
        CHECK( dict.GetProperty("Item", "a") == 3L );
    }

    SECTION("PutWithoutValue")
    {
        wxVariant result;
        WX_ASSERT_FAILS_WITH_ASSERT( dict.Invoke("CompareMode",
                                                 DISPATCH_PROPERTYPUT,
                                                 result, 0, nullptr) );
    }
}

#endif // __WXMSW__ && wxUSE_OLE_AUTOMATION && wxUSE_VARIANT
