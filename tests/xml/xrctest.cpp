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
#include "wx/xml/xml.h"
#include "wx/sstream.h"
#include "wx/wfstream.h"
#include "wx/xrc/xmlres.h"

#include <stdarg.h>

// ----------------------------------------------------------------------------
// helpers to create/save some xrc
// ----------------------------------------------------------------------------

namespace
{

static const char *TEST_XRC_FILE = "test.xrc";

// I'm hard-wiring the xrc into this function for now
// If different xrcs are wanted for future tests, it'll be easy to refactor
void CreateXrc()
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

    // afaict there's no elegant way to load xrc direct from a string
    // So save it as a file, from which it can be loaded
    wxStringInputStream sis(xrcText);
    wxFFileOutputStream fos(TEST_XRC_FILE);
    REQUIRE(fos.IsOk());
    fos.Write(sis);
    REQUIRE(fos.Close());
}

} // anon namespace


// ----------------------------------------------------------------------------
// test fixture and the tests using it
// ----------------------------------------------------------------------------

class XrcTestCase
{
public:
    XrcTestCase() { CreateXrc(); }
    ~XrcTestCase() { wxRemoveFile(TEST_XRC_FILE); }

private:
    wxDECLARE_NO_COPY_CLASS(XrcTestCase);
};

TEST_CASE_METHOD(XrcTestCase, "XRC::ObjectReferences", "[xrc]")
{
    wxXmlResource::Get()->InitAllHandlers();

    for ( int n = 0; n < 2; ++n )
    {
        // Load the xrc file we're just created
        REQUIRE( wxXmlResource::Get()->Load(TEST_XRC_FILE) );

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
        // Load the xrc file we're just created
        REQUIRE( wxXmlResource::Get()->Load(TEST_XRC_FILE) );

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
