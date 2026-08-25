///////////////////////////////////////////////////////////////////////////////
// Name:        tests/misc/settings.cpp
// Purpose:     test wxSettings
// Author:      Francesco Montorsi
// Created:     2009-03-24
// Copyright:   (c) 2009 Francesco Montorsi
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"


#include "wx/settings.h"
#include "wx/fontenum.h"
#include "wx/brush.h"
#include "wx/pen.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3
    #include "wx/msw/private/darkmode.h"
    #include "wx/msw/private/metrics.h"
#endif

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class SettingsTestCase : public CppUnit::TestCase
{
public:
    SettingsTestCase() { }

private:
    CPPUNIT_TEST_SUITE( SettingsTestCase );
        CPPUNIT_TEST( GetColour );
        CPPUNIT_TEST( GetFont );
        CPPUNIT_TEST( GlobalColours );
        CPPUNIT_TEST( GlobalFonts );
        CPPUNIT_TEST( GlobalBrushes );
        CPPUNIT_TEST( GlobalPens );
    CPPUNIT_TEST_SUITE_END();

    void GetColour();
    void GetFont();

    // not really wxSystemSettings stuff but still nice to test:
    void GlobalColours();
    void GlobalFonts();
    void GlobalBrushes();
    void GlobalPens();

    wxDECLARE_NO_COPY_CLASS(SettingsTestCase);
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( SettingsTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( SettingsTestCase, "SettingsTestCase" );


void SettingsTestCase::GetColour()
{
    for (unsigned int i=wxSYS_COLOUR_SCROLLBAR; i < wxSYS_COLOUR_MAX; i++)
        CPPUNIT_ASSERT( wxSystemSettings::GetColour((wxSystemColour)i).IsOk() );

#if defined(__WXWINUI__) && wxUSE_WINUI3
    if ( !wxMSWDarkMode::IsActive() && !wxMSWImpl::IsHighContrast() )
    {
        const wxSystemColour textColours[] =
        {
            wxSYS_COLOUR_WINDOWTEXT,
            wxSYS_COLOUR_LISTBOXTEXT,
            wxSYS_COLOUR_BTNTEXT,
            wxSYS_COLOUR_CAPTIONTEXT,
            wxSYS_COLOUR_INACTIVECAPTIONTEXT,
            wxSYS_COLOUR_MENUTEXT,
            wxSYS_COLOUR_INFOTEXT,
            wxSYS_COLOUR_GRAYTEXT,
            wxSYS_COLOUR_HIGHLIGHTTEXT,
            wxSYS_COLOUR_LISTBOXHIGHLIGHTTEXT
        };

        for ( const wxSystemColour textColour : textColours )
        {
            // Exact black is reserved for the WinUI Mica composition key and
            // must never be returned as a default light-theme text colour.
            CPPUNIT_ASSERT(
                wxSystemSettings::GetColour(textColour) != wxColour(0, 0, 0));
        }
    }
#endif
}

void SettingsTestCase::GetFont()
{
    const wxSystemFont ids[] =
    {
        wxSYS_OEM_FIXED_FONT,
        wxSYS_ANSI_FIXED_FONT,
        wxSYS_ANSI_VAR_FONT,
        wxSYS_SYSTEM_FONT,
        wxSYS_DEVICE_DEFAULT_FONT,
        wxSYS_SYSTEM_FIXED_FONT,
        wxSYS_DEFAULT_GUI_FONT
    };

    for (unsigned int i=0; i < WXSIZEOF(ids); i++)
    {
        const wxFont& font = wxSystemSettings::GetFont(ids[i]);
        CPPUNIT_ASSERT( font.IsOk() );
        CPPUNIT_ASSERT( wxFontEnumerator::IsValidFacename(font.GetFaceName()) );
    }
}

void SettingsTestCase::GlobalColours()
{
    wxColour col[] =
    {
        *wxBLACK,
        *wxBLUE,
        *wxCYAN,
        *wxGREEN,
        *wxLIGHT_GREY,
        *wxRED,
        *wxWHITE
    };

    for (unsigned int i=0; i < WXSIZEOF(col); i++)
        CPPUNIT_ASSERT( col[i].IsOk() );
}

void SettingsTestCase::GlobalFonts()
{
    const wxFont font[] =
    {
        *wxNORMAL_FONT,
        *wxSMALL_FONT,
        *wxITALIC_FONT,
        *wxSWISS_FONT
    };

    for (unsigned int i=0; i < WXSIZEOF(font); i++)
    {
        CPPUNIT_ASSERT( font[i].IsOk() );

        const wxString facename = font[i].GetFaceName();
        if ( !facename.empty() )
        {
            WX_ASSERT_MESSAGE(
                ("font #%u: facename \"%s\" is invalid", i, facename),
                wxFontEnumerator::IsValidFacename(facename)
            );
        }
    }
}

void SettingsTestCase::GlobalBrushes()
{
    wxBrush brush[] =
    {
        *wxBLACK_BRUSH,
        *wxBLUE_BRUSH,
        *wxCYAN_BRUSH,
        *wxGREEN_BRUSH,
        *wxGREY_BRUSH,
        *wxLIGHT_GREY_BRUSH,
        *wxMEDIUM_GREY_BRUSH,
        *wxRED_BRUSH,
        *wxTRANSPARENT_BRUSH,
        *wxWHITE_BRUSH
    };

    for (unsigned int i=0; i < WXSIZEOF(brush); i++)
        CPPUNIT_ASSERT( brush[i].IsOk() );
}

void SettingsTestCase::GlobalPens()
{
    wxPen pen[] =
    {
        *wxBLACK_DASHED_PEN,
        *wxBLACK_PEN,
        *wxBLUE_PEN,
        *wxCYAN_PEN,
        *wxGREEN_PEN,
        *wxGREY_PEN,
        *wxLIGHT_GREY_PEN,
        *wxMEDIUM_GREY_PEN,
        *wxRED_PEN,
        *wxTRANSPARENT_PEN,
        *wxWHITE_PEN
    };

    for (unsigned int i=0; i < WXSIZEOF(pen); i++)
        CPPUNIT_ASSERT( pen[i].IsOk() );
}
