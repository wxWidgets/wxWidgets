///////////////////////////////////////////////////////////////////////////////
// Name:        tests/misc/misctests.cpp
// Purpose:     test miscellaneous GUI functions
// Author:      Vadim Zeitlin
// Created:     2008-09-22
// Copyright:   (c) 2008 Vadim Zeitlin
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"


#include "wx/defs.h"

#ifndef WX_PRECOMP
    #include "wx/gdicmn.h"
    #include "wx/filefn.h"
#endif // !PCH

#include "wx/app.h"
#include "wx/button.h"
#include "wx/clipbrd.h"
#include "wx/dataobj.h"
#include "wx/panel.h"

#if defined(__WXMSW__) && wxUSE_PRINTING_ARCHITECTURE
    #include "wx/msw/wrapwin.h"
    #include "wx/msw/private.h"
    #include "wx/msw/printdlg.h"
#endif

#ifdef wxHAS_VIRTUAL_FILE_DATA_OBJECT
    #include "wx/datetime.h"
    #include "wx/file.h"
    #include "wx/filename.h"
    #include "wx/stream.h"

    #include "wx/msw/wrapshl.h"

    #include "testfile.h"

    #include <vector>
#endif // wxHAS_VIRTUAL_FILE_DATA_OBJECT

#include "asserthelper.h"
#include "waitfor.h"

#include <memory>

// gcc 16.x gives many bogus warnings about array subscripts being out of
// bounds in basic_string.h, so disable them for this particular version as it
// looks like a compiler bug.
#if wxCHECK_GCC_VERSION(16, 0) && !wxCHECK_GCC_VERSION(16, 3)
wxGCC_WARNING_SUPPRESS(array-bounds)
#endif

// ----------------------------------------------------------------------------
// the tests
// ----------------------------------------------------------------------------

TEST_CASE("GUI::DisplaySize", "[guifuncs]")
{
    // test that different (almost) overloads return the same results
    int w, h;
    wxDisplaySize(&w, &h);
    wxSize sz = wxGetDisplaySize();

    CHECK( sz.x == w );
    CHECK( sz.y == h );

    // test that passing nullptr works as expected, e.g. doesn't crash
    wxDisplaySize(nullptr, nullptr);
    wxDisplaySize(&w, nullptr);
    wxDisplaySize(nullptr, &h);

    CHECK( sz.x == w );
    CHECK( sz.y == h );

    // test that display PPI is something reasonable
    sz = wxGetDisplayPPI();
    CHECK( sz.x < 1000 );
    CHECK( sz.y < 1000 );
}

#if wxUSE_DATAOBJ
TEST_CASE("GUI::TextDataObject", "[guifuncs][clipboard]")
{
    const wxString text("Hello clipboard!");

    wxTextDataObject* const dobj = new wxTextDataObject(text);
    CHECK( dobj->GetText() == text );

    wxClipboardLocker lockClip;
    CHECK( wxTheClipboard->SetData(dobj) );
    wxTheClipboard->Flush();

    wxTextDataObject dobj2;
    REQUIRE( wxTheClipboard->GetData(dobj2) );
    CHECK( dobj2.GetText() == text );
}

TEST_CASE("GUI::URLDataObject", "[guifuncs][clipboard]")
{
    // this tests for buffer overflow, see #11102
    const char * const
        url = "http://something.long.to.overwrite.plenty.memory.example.com";
    wxURLDataObject * const dobj = new wxURLDataObject(url);
    CHECK( dobj->GetURL() == url );

    wxClipboardLocker lockClip;
    CHECK( wxTheClipboard->SetData(dobj) );
    wxTheClipboard->Flush();

    wxURLDataObject dobj2;
    REQUIRE( wxTheClipboard->GetData(dobj2) );
    CHECK( dobj2.GetURL() == url );
}

TEST_CASE("GUI::HTMLDataObject", "[guifuncs][clipboard]")
{
    const wxString text("<h1>Hello clipboard!</h1>");

    wxHTMLDataObject* const dobj = new wxHTMLDataObject(text);
    CHECK( dobj->GetHTML() == text );

    wxClipboardLocker lockClip;
    CHECK( wxTheClipboard->SetData(dobj) );
    wxTheClipboard->Flush();

    wxHTMLDataObject dobj2;
    REQUIRE( wxTheClipboard->GetData(dobj2) );
    CHECK( dobj2.GetHTML() == text );
}

#ifdef wxHAS_VIRTUAL_FILE_DATA_OBJECT

TEST_CASE("GUI::VirtualFileDataObject", "[guifuncs][clipboard]")
{
    // Create the descriptor for a file, the directory containing it and an
    // item with an invalid name, which should be ignored.
    const size_t count = 3;
    std::vector<char>
        buf(sizeof(UINT) + count*sizeof(FILEDESCRIPTORW));
    FILEGROUPDESCRIPTORW* const
        fgd = reinterpret_cast<FILEGROUPDESCRIPTORW*>(buf.data());
    fgd->cItems = count;

    const char contents[] = "Hello, virtual world!";
    const DWORD size = sizeof(contents) - 1;
    const wxDateTime mtime(1, wxDateTime::Feb, 2020, 12, 34, 56, 789);

    FILEDESCRIPTORW& file = fgd->fgd[0];
    file.dwFlags = FD_FILESIZE | FD_WRITESTIME;
    file.nFileSizeLow = size;
    REQUIRE( mtime.GetAsMSWFileTime(&file.ftLastWriteTime) );
    wxStrcpy(file.cFileName, L"dir\\file.txt");

    FILEDESCRIPTORW& dir = fgd->fgd[1];
    dir.dwFlags = FD_ATTRIBUTES;
    dir.dwFileAttributes = FILE_ATTRIBUTE_DIRECTORY;
    wxStrcpy(dir.cFileName, L"dir");

    wxStrcpy(fgd->fgd[2].cFileName, L"..\\evil.txt");

    // Note that wx data objects ignore the item index, so we can only provide
    // the contents of the first item, but this is enough for testing.
    wxDataObjectComposite* const source = new wxDataObjectComposite;

    wxCustomDataObject* const descriptor =
        new wxCustomDataObject(CFSTR_FILEDESCRIPTORW);
    descriptor->SetData(buf.size(), buf.data());
    source->Add(descriptor);

    wxCustomDataObject* const fileContents =
        new wxCustomDataObject(CFSTR_FILECONTENTS);
    fileContents->SetData(size, contents);
    source->Add(fileContents);

    // Don't call Flush() here, as this would render the data on the clipboard
    // without the item index and the file contents couldn't be retrieved.
    wxClipboardLocker lockClip;
    REQUIRE( wxTheClipboard->SetData(source) );

    wxVirtualFileDataObject data;
    REQUIRE( wxTheClipboard->GetData(data) );

    REQUIRE( data.GetCount() == 2 );

    CHECK( data.GetName(0) == "dir\\file.txt" );
    CHECK( !data.IsDir(0) );
    CHECK( data.GetSize(0) == size );
    CHECK( data.GetModificationTime(0) == mtime );

    CHECK( data.GetName(1) == "dir" );
    CHECK( data.IsDir(1) );
    CHECK( data.GetSize(1) == wxInvalidOffset );
    CHECK( !data.GetModificationTime(1).IsValid() );

    std::unique_ptr<wxInputStream> stream = data.OpenStream(0);
    REQUIRE( stream );

    char readBuf[256];
    stream->Read(readBuf, sizeof(readBuf));
    CHECK( std::string(readBuf, stream->LastRead()) == contents );

    CHECK( !data.OpenStream(1) );

    TempDir tempDir("wxvfdo");
    tempDir.RequireOk();

    const std::vector<wxString> paths = data.SaveAll(tempDir.GetName());
    REQUIRE( paths.size() == 1 );
    CHECK( paths[0] == tempDir.GetName() + "\\dir" );

    const wxString savedFile = paths[0] + "\\file.txt";
    wxFile f(savedFile);
    REQUIRE( f.IsOpened() );

    const ssize_t savedLen = f.Read(readBuf, sizeof(readBuf));
    REQUIRE( savedLen >= 0 );
    CHECK( std::string(readBuf, savedLen) == contents );

    CHECK( wxFileName(savedFile).GetModificationTime() == mtime );
    f.Close();

    const std::vector<wxString> tempPaths = data.SaveAllToTempDir();
    REQUIRE( tempPaths.size() == 1 );
    CHECK( wxFileExists(tempPaths[0] + "\\file.txt") );

    const wxString tempDir2 = wxFileName(tempPaths[0]).GetPath();
    CHECK( tempDir2.StartsWith(wxFileName::GetTempDir()) );

    // Check that another call creates a different directory.
    const std::vector<wxString> tempPaths2 = data.SaveAllToTempDir();
    REQUIRE( tempPaths2.size() == 1 );
    CHECK( wxFileExists(tempPaths2[0] + "\\file.txt") );
    CHECK( tempPaths2[0] != tempPaths[0] );

    CHECK( data.DeleteAllTempFiles() );
    CHECK( !wxDirExists(tempDir2) );
    CHECK( !wxDirExists(wxFileName(tempPaths2[0]).GetPath()) );
}

TEST_CASE("GUI::VirtualFileDataObject::SaveError", "[guifuncs][clipboard]")
{
    // Create the descriptor for a directory and a file inside it, but don't
    // provide the file contents, so that saving the directory succeeds but
    // saving the file fails.
    const size_t count = 2;
    std::vector<char>
        buf(sizeof(UINT) + count*sizeof(FILEDESCRIPTORW));
    FILEGROUPDESCRIPTORW* const
        fgd = reinterpret_cast<FILEGROUPDESCRIPTORW*>(buf.data());
    fgd->cItems = count;

    FILEDESCRIPTORW& dir = fgd->fgd[0];
    dir.dwFlags = FD_ATTRIBUTES;
    dir.dwFileAttributes = FILE_ATTRIBUTE_DIRECTORY;
    wxStrcpy(dir.cFileName, L"dir");

    wxStrcpy(fgd->fgd[1].cFileName, L"dir\\file.txt");

    wxCustomDataObject* const descriptor =
        new wxCustomDataObject(CFSTR_FILEDESCRIPTORW);
    descriptor->SetData(buf.size(), buf.data());

    wxClipboardLocker lockClip;
    REQUIRE( wxTheClipboard->SetData(descriptor) );

    wxVirtualFileDataObject data;
    REQUIRE( wxTheClipboard->GetData(data) );
    REQUIRE( data.GetCount() == 2 );

    TempDir tempDir("wxvfdo");
    tempDir.RequireOk();

    const wxString base = tempDir.GetName() + "\\";

    wxLogNull noLog;

    SECTION("SaveTo")
    {
        // All the parent directories created by SaveTo() must be removed.
        CHECK( !data.SaveTo(1, base + "new\\sub\\file.txt") );
        CHECK( !wxDirExists(base + "new") );

        // But the existing ones must be left alone.
        REQUIRE( wxMkdir(base + "existing") );
        CHECK( !data.SaveTo(1, base + "existing\\file.txt") );
        CHECK( wxDirExists(base + "existing") );
        CHECK( !wxFileExists(base + "existing\\file.txt") );
    }

    SECTION("SaveAll")
    {
        // The directory saved before the error occurred must be removed and
        // so must be the target directory itself, as it didn't exist before.
        CHECK( data.SaveAll(base + "new").empty() );
        CHECK( !wxDirExists(base + "new") );

        // But not the existing target directory.
        CHECK( data.SaveAll(tempDir.GetName()).empty() );
        CHECK( wxDirExists(tempDir.GetName()) );
        CHECK( !wxDirExists(base + "dir") );

        // Nor the existing subdirectory.
        REQUIRE( wxMkdir(base + "dir") );
        CHECK( data.SaveAll(tempDir.GetName()).empty() );
        CHECK( wxDirExists(base + "dir") );
        CHECK( !wxFileExists(base + "dir\\file.txt") );
    }

    SECTION("SaveAllToTempDir")
    {
        CHECK( data.SaveAllToTempDir().empty() );
    }
}

#endif // wxHAS_VIRTUAL_FILE_DATA_OBJECT

// This disabled by default test allows to check that we retrieve HTML data
// from the system clipboard correctly.
TEST_CASE("GUI::ShowHTML", "[.]")
{
    wxClipboardLocker lockClip;

    wxHTMLDataObject dobj;
    REQUIRE( wxTheClipboard->GetData(dobj) );

    WARN("Clipboard contents:\n---start---\n" << dobj.GetHTML() << "\n---end--");
}

TEST_CASE("GUI::DataFormatCompare", "[guifuncs][dataformat]")
{
    const wxDataFormat df(wxDF_TEXT);
    CHECK( df == wxDF_TEXT );
    CHECK( df != wxDF_INVALID );
}
#endif // wxUSE_DATAOBJ

TEST_CASE("GUI::ParseFileDialogFilter", "[guifuncs]")
{
    wxArrayString descs,
                  filters;

    REQUIRE
    (
        wxParseCommonDialogsFilter("Image files|*.jpg;*.png", descs, filters)
        == 1
    );

    CHECK( descs[0] == "Image files" );
    CHECK( filters[0] == "*.jpg;*.png" );

    REQUIRE
    (
        wxParseCommonDialogsFilter
        (
            "All files (*.*)|*.*|Python source (*.py)|*.py",
            descs, filters
        )
        == 2
    );

    CHECK( filters[0] == "*.*" );
    CHECK( filters[1] == "*.py" );

    // Test some invalid ones too.
    WX_ASSERT_FAILS_WITH_ASSERT
    (
        wxParseCommonDialogsFilter
        (
            "All files (*.*)|*.*|Python source (*.py)|*.py|",
            descs, filters
        )
    );
}

#if defined(__WXMSW__) && wxUSE_PRINTING_ARCHITECTURE

TEST_CASE("wxPrintData::MSWDuplexDefault", "[print][msw]")
{
    wxWindowsPrintNativeData nativeData;
    GlobalPtr hDevMode(sizeof(DEVMODE), GMEM_FIXED | GMEM_ZEROINIT);
    REQUIRE( static_cast<HGLOBAL>(hDevMode) );

    DEVMODE * const devMode =
        static_cast<DEVMODE *>(static_cast<HGLOBAL>(hDevMode));
    devMode->dmSize = sizeof(DEVMODE);
    devMode->dmFields = DM_DUPLEX;
    devMode->dmDuplex = DMDUP_VERTICAL;

    // wxWindowsPrintNativeData takes ownership and frees the handle.
    nativeData.SetDevMode(hDevMode.Release());

    SECTION("Default")
    {
        wxPrintData data;

        REQUIRE( nativeData.TransferFrom(data) );

        CHECK( (devMode->dmFields & DM_DUPLEX) != 0 );
        CHECK( devMode->dmDuplex == DMDUP_VERTICAL );
    }

    SECTION("ExplicitSimplex")
    {
        wxPrintData data;
        data.SetDuplex(wxDUPLEX_SIMPLEX);

        REQUIRE( nativeData.TransferFrom(data) );

        CHECK( (devMode->dmFields & DM_DUPLEX) != 0 );
        CHECK( devMode->dmDuplex == DMDUP_SIMPLEX );
    }
}

#endif // defined(__WXMSW__) && wxUSE_PRINTING_ARCHITECTURE

TEST_CASE("GUI::ClientToScreen", "[guifuncs]")
{
    wxWindow* const tlw = wxTheApp->GetTopWindow();
    REQUIRE( tlw );

    SECTION("Right to left layout [RTL]") { }
    {
        tlw->SetLayoutDirection(wxLayout_RightToLeft);
    }

    SECTION("Left to right layout [LTR]")
    {
        tlw->SetLayoutDirection(wxLayout_LeftToRight);
    }

    tlw->Refresh();
    tlw->Update();
    wxYield();

    auto const p1 = make_unique<wxPanel>(tlw, wxID_ANY, wxPoint(0, 0),
                                         wxSize(100, 50));
    auto const p2 = make_unique<wxPanel>(tlw, wxID_ANY, wxPoint(0, 50),
                                         wxSize(100, 50));
    wxWindow* const
        b = new wxWindow(p2.get(), wxID_ANY, wxPoint(10, 10), wxSize(30, 10));

    // We need this to realize the windows created above under wxGTK.
    YieldForAWhile();

    const wxPoint tlwOrig = tlw->ClientToScreen(wxPoint(0, 0));
    const int xx = tlw->GetLayoutDirection() == wxLayout_RightToLeft ? -10 : 10;

    wxPoint c2sCoords = p2->ClientToScreen(wxPoint(0, 0));
    wxPoint c2sCoordsExpected = tlwOrig + wxPoint(0, 50);

    CHECK( c2sCoords == c2sCoordsExpected );

    c2sCoords = b->ClientToScreen(wxPoint(0, 0));
    c2sCoordsExpected = tlwOrig + wxPoint(xx, 60);

    CHECK( c2sCoords == c2sCoordsExpected );

    // Ensure that "ScreenToClient(ClientToScreen(coords)) == coords" is also true.
    c2sCoords = b->ScreenToClient(c2sCoords);
    c2sCoordsExpected = wxPoint(0, 0);
    CHECK( c2sCoords == c2sCoordsExpected );
}

namespace
{

// This class is used as a test window here. We can't use a real wxButton
// because we can't create other windows as its children in wxGTK.
class TestButton : public wxWindow
{
public:
    TestButton(wxWindow* parent, const wxString& label, const wxPoint& pos)
        : wxWindow(parent, wxID_ANY, pos, wxSize(100, 50))
    {
        SetLabel(label);
    }
};

// Helper function returning the label of the window at the given point or
// "NONE" if there is no window there.
wxString GetLabelOfWindowAtPoint(wxWindow* parent, int x, int y)
{
    wxWindow* const
        win = wxFindWindowAtPoint(parent->ClientToScreen(wxPoint(x, y)));
    return win ? win->GetLabel() : wxString("NONE");
}

} // anonymous namespace

TEST_CASE("GUI::FindWindowAtPoint", "[guifuncs]")
{
    wxWindow* const parent = wxTheApp->GetTopWindow();
    REQUIRE( parent );

    // Set a label to allow distinguishing it from the other windows in the
    // assertion messages.
    parent->SetLabel("parent");

    auto btn1 = make_unique<TestButton>(parent, "1", wxPoint(10, 10));
    auto btn2 = make_unique<TestButton>(parent, "2", wxPoint(10, 90));

    // No need to use std::unique_ptr<> for this one, it will be deleted by btn2.
    wxWindow* btn3 = new TestButton(btn2.get(), "3", wxPoint(20, 20));

    // We need this to realize the windows created above under wxGTK.
    wxYield();

    INFO("No window for a point outside of the window");
    CHECK( GetLabelOfWindowAtPoint(parent, 900, 900) == "NONE" );

    INFO( "Point over a child control corresponds to it" );
    CHECK( GetLabelOfWindowAtPoint(parent, 11, 11) == btn1->GetLabel() );

    INFO("Point outside of any child control returns the TLW itself");
    CHECK( GetLabelOfWindowAtPoint(parent, 5, 5) == parent->GetLabel() );

    btn2->Disable();
    INFO("Point over a disabled child control still corresponds to it");
    CHECK( GetLabelOfWindowAtPoint(parent, 11, 91) == btn2->GetLabel() );

    btn2->Hide();
    INFO("Point over a hidden child control doesn't take it into account");
    CHECK( GetLabelOfWindowAtPoint(parent, 11, 91) == parent->GetLabel() );

    btn2->Show();
    INFO("Point over child control corresponds to the child");
    CHECK( GetLabelOfWindowAtPoint(parent, 31, 111) == btn3->GetLabel() );

    btn3->Disable();
    INFO("Point over disabled child controls still corresponds to this child");
    CHECK( GetLabelOfWindowAtPoint(parent, 31, 111) == btn3->GetLabel() );
}

TEST_CASE("wxWindow::Dump", "[window]")
{
    CHECK_NOTHROW( wxDumpWindow(nullptr) );

    auto button = make_unique<wxButton>(wxTheApp->GetTopWindow(), wxID_ANY,
                                        "bloordyblop");

    const std::string s = wxDumpWindow(button.get()).utf8_string();

    CHECK_THAT( s, Catch::Contains("wxButton") );
    CHECK_THAT( s, Catch::Contains("bloordyblop") );
}
