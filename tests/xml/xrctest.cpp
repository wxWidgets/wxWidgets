///////////////////////////////////////////////////////////////////////////////
// Name:        tests/xml/xrctest.cpp
// Purpose:     XRC classes unit test
// Author:      wxWidgets team
// Created:     2010-10-30
// Copyright:   (c) 2010 wxWidgets team
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"


#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif // WX_PRECOMP

#if wxUSE_XRC

#include "wx/fs_inet.h"
#include "wx/imagxpm.h"
#include "wx/xml/xml.h"
#include "wx/scopedptr.h"
#include "wx/sstream.h"
#include "wx/wfstream.h"
#include "wx/xrc/xmlres.h"
#include "wx/xrc/xh_bmp.h"

#include <stdarg.h>

#include "testfile.h"

// ----------------------------------------------------------------------------
// helpers to create/save some xrc
// ----------------------------------------------------------------------------

namespace
{

static const char *TEST_XRC_FILE = "test.xrc";

void LoadXrcFrom(const wxString& xrcText)
{
    wxStringInputStream sis(xrcText);
    wxScopedPtr<wxXmlDocument> xmlDoc(new wxXmlDocument(sis, "UTF-8"));
    REQUIRE( xmlDoc->IsOk() );

    // Load the xrc we've just created
    REQUIRE( wxXmlResource::Get()->LoadDocument(xmlDoc.release(), TEST_XRC_FILE) );
}

// I'm hard-wiring the xrc into this function for now
// If different xrcs are wanted for future tests, it'll be easy to refactor
void LoadTestXrc()
{
    const char *xrcText =
    "<?xml version=\"1.0\" ?>"
    "<resource>"
    "  <object class=\"wxDialog\" name=\"dialog\">"
    "    <object class=\"wxBoxSizer\">"
    "      <orient>wxVERTICAL</orient>"
    "      <object class=\"sizeritem\">"
    "        <object class=\"wxPanel\" name=\"panel1\">"
    "          <object class=\"wxBoxSizer\">"
    "            <object class=\"sizeritem\">"
    "              <object class=\"wxBoxSizer\">"
    "                <orient>wxVERTICAL</orient>"
    "                <object class=\"sizeritem\">"
    "                  <object class=\"wxButton\" name=\"FirstCol[0]\">"
    "                    <label>0</label>"
    "                  </object>"
    "                </object>"
    "                <object class=\"sizeritem\">"
    "                  <object class=\"wxButton\" name=\"FirstCol[1]\">"
    "                    <label>1</label>"
    "                  </object>"
    "                </object>"
    "                <object class=\"sizeritem\">"
    "                  <object class=\"wxButton\" name=\"FirstCol[2]\">"
    "                    <label>2</label>"
    "                  </object>"
    "                </object>"
    "                <object class=\"sizeritem\">"
    "                  <object class=\"wxButton\" name=\"FirstCol[3]\">"
    "                    <label>3</label>"
    "                  </object>"
    "                </object>"
    "              </object>"
    "            </object>"
    "            <object class=\"sizeritem\">"
    "              <object class=\"wxBoxSizer\">"
    "                <orient>wxVERTICAL</orient>"
    "                <object class=\"sizeritem\">"
    "                  <object class=\"wxButton\" name=\"SecondCol[start]\">"
    "                    <label>0</label>"
    "                  </object>"
    "                </object>"
    "                <object class=\"sizeritem\">"
    "                  <object class=\"wxButton\" name=\"SecondCol[1]\">"
    "                    <label>1</label>"
    "                  </object>"
    "                </object>"
    "                <object class=\"sizeritem\">"
    "                  <object class=\"wxButton\" name=\"SecondCol[2]\">"
    "                    <label>2</label>"
    "                  </object>"
    "                </object>"
    "                <object class=\"sizeritem\">"
    "                  <object class=\"wxButton\" name=\"SecondCol[end]\">"
    "                    <label>3</label>"
    "                  </object>"
    "                </object>"
    "              </object>"
    "            </object>"
    "            <orient>wxHORIZONTAL</orient>"
    "          </object>"
    "      </object>"
    "    </object>"
    "      <object class=\"sizeritem\">"
    "        <object class=\"wxPanel\" name=\"ref_of_panel1\">"
    "          <object_ref ref=\"panel1\"/>"
    "        </object>"
    "      </object>"
    "    </object>"
    "    <title>test</title>"
    "  </object>"
    "  <ids-range name=\"FirstCol\" size=\"2\" start=\"10000\"/>"
    "  <ids-range name=\"SecondCol\" size=\"100\" />"
    "</resource>"
      ;

    LoadXrcFrom(wxString::FromAscii(xrcText));
}

} // anon namespace


// ----------------------------------------------------------------------------
// test fixture and the tests using it
// ----------------------------------------------------------------------------

class XrcTestCase
{
public:
    XrcTestCase() { }

private:
    wxDECLARE_NO_COPY_CLASS(XrcTestCase);
};

TEST_CASE_METHOD(XrcTestCase, "XRC::ObjectReferences", "[xrc]")
{
    wxXmlResource::Get()->InitAllHandlers();

    for ( int n = 0; n < 2; ++n )
    {
        LoadTestXrc();

        // In xrc there's now a dialog containing two panels, one an object
        // reference of the other
        wxDialog dlg;
        REQUIRE( wxXmlResource::Get()->LoadDialog(&dlg, NULL, "dialog") );
        // Might as well test XRCCTRL too
        wxPanel* panel1 = XRCCTRL(dlg,"panel1",wxPanel);
        wxPanel* panel2 = XRCCTRL(dlg,"ref_of_panel1",wxPanel);
        // Check that the object reference panel is a different object
        CHECK( panel2 != panel1 );

        // Unload the xrc, so it can be reloaded and the test rerun
        CHECK( wxXmlResource::Get()->Unload(TEST_XRC_FILE) );
    }
}

TEST_CASE_METHOD(XrcTestCase, "XRC::IDRanges", "[xrc]")
{
    // Tests ID ranges
    for ( int n = 0; n < 2; ++n )
    {
        LoadTestXrc();

        // foo[start] should == foo[0]
        CHECK( XRCID("SecondCol[start]") == XRCID("SecondCol[0]") );
        // foo[start] should be < foo[end]. Usually that means more negative
        CHECK( XRCID("SecondCol[start]") < XRCID("SecondCol[end]") );
        // Check it works for the positive values in FirstCol too
        CHECK( XRCID("FirstCol[start]") < XRCID("FirstCol[end]") );

        // Check that values are adjacent
        CHECK( XRCID("SecondCol[0]")+1 == XRCID("SecondCol[1]") );
        CHECK( XRCID("SecondCol[1]")+1 == XRCID("SecondCol[2]") );
        // And for the positive range
        CHECK( XRCID("FirstCol[2]")+1 == XRCID("FirstCol[3]") );

        // Check that a large-enough range was created, despite the small
        // 'size' parameter
        CHECK( XRCID("FirstCol[end]") - XRCID("FirstCol[start]") + 1 == 4 );

        // Check that the far-too-large size range worked off the scale too
        CHECK( XRCID("SecondCol[start]") < XRCID("SecondCol[90]") );
        CHECK( XRCID("SecondCol[90]") < XRCID("SecondCol[end]") );
        CHECK( XRCID("SecondCol[90]")+1 == XRCID("SecondCol[91]") );

        // Check that the positive range-start parameter worked, even after a
        // reload
        CHECK( XRCID("FirstCol[start]") == 10000 );

        // Unload the xrc, so it can be reloaded and the tests rerun
        CHECK( wxXmlResource::Get()->Unload(TEST_XRC_FILE) );
    }
}

TEST_CASE("XRC::PathWithFragment", "[xrc][uri]")
{
    wxXmlResource::Get()->AddHandler(new wxBitmapXmlHandler);
    wxImage::AddHandler(new wxXPMHandler);

    const wxString filename = "image#1.xpm";
    TempFile xpmFile(filename);

    // Simplest possible XPM, just to have something to create a bitmap from.
    static const char* xpm =
        "/* XPM */\n"
        "static const char *const xpm[] = {\n"
        "/* columns rows colors chars-per-pixel */\n"
        "\"1 1 1 1\",\n"
        "\"  c None\",\n"
        "/* pixels */\n"
        "\" \"\n"
        ;

    wxFFile ff;
    REQUIRE( ff.Open(filename, "w") );
    REQUIRE( ff.Write(wxString::FromAscii(xpm)) );
    REQUIRE( ff.Close() );

    // Opening a percent-encoded URI should work.
    wxString url = filename;
    url.Replace("#", "%23");

    LoadXrcFrom
    (
        wxString::Format
        (
            "<?xml version=\"1.0\" ?>"
            "<resource>"
            "  <object class=\"wxBitmap\" name=\"bad\">%s</object>"
            "  <object class=\"wxBitmap\" name=\"good\">%s</object>"
            "</resource>",
            filename,
            url
        )
    );

    CHECK( wxXmlResource::Get()->LoadBitmap("good").IsOk() );
    CHECK( !wxXmlResource::Get()->LoadBitmap("bad").IsOk() );
}

TEST_CASE("XRC::EnvVarInPath", "[xrc]")
{
    wxStringInputStream sis(
#ifdef __WINDOWS__
        "<root><bitmap>%WX_TEST_ENV_IN_PATH%.bmp</bitmap></root>"
#else
        "<root><bitmap>$(WX_TEST_ENV_IN_PATH).bmp</bitmap></root>"
#endif
    );
    wxXmlDocument xmlDoc(sis, "UTF-8");
    REQUIRE( xmlDoc.IsOk() );

    class wxTestEnvXmlHandler : public wxXmlResourceHandler
    {
    public:
        wxTestEnvXmlHandler(wxXmlNode* testNode)
        {
            varIsSet = wxSetEnv("WX_TEST_ENV_IN_PATH", "horse");

            wxXmlResource::Get()->SetFlags(wxXRC_USE_LOCALE | wxXRC_USE_ENVVARS);
            SetParentResource(wxXmlResource::Get());

            m_node = testNode;
        }
        ~wxTestEnvXmlHandler()
        {
            wxUnsetEnv("WX_TEST_ENV_IN_PATH");
            wxXmlResource::Get()->SetFlags(wxXRC_USE_LOCALE);
        }
        virtual wxObject* DoCreateResource() wxOVERRIDE { return NULL; }
        virtual bool CanHandle(wxXmlNode*) wxOVERRIDE { return false; }
        bool varIsSet;
    } handler(xmlDoc.GetRoot());

    REQUIRE( handler.varIsSet );

    wxXmlResourceHandlerImpl *impl = new wxXmlResourceHandlerImpl(&handler);
    handler.SetImpl(impl);

    CHECK( impl->GetBitmap().IsOk() );
    CHECK( impl->GetBitmapBundle().IsOk() );
}

// This test is disabled by default as it requires the environment variable
// below to be defined to point to a HTTP URL with the file to load.
//
// Use something like "python3 -m http.server samples/xrc/rc" and set
// WX_TEST_XRC_URL to http://localhost/menu.xrc to run this test.
TEST_CASE_METHOD(XrcTestCase, "XRC::LoadURL", "[xrc][.]")
{
    wxString url;
    REQUIRE( wxGetEnv("WX_TEST_XRC_URL", &url) );

    // Ensure that loading from HTTP URLs is supported.
    struct InetHandler : wxInternetFSHandler
    {
        InetHandler()
        {
            wxFileSystem::AddHandler(this);
        }

        ~InetHandler()
        {
            wxFileSystem::RemoveHandler(this);
        }
    } inetHandler;

    CHECK( wxXmlResource::Get()->Load(url) );
}

#endif // wxUSE_XRC
