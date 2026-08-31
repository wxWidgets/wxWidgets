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
// tests
// ----------------------------------------------------------------------------

TEST_CASE("Settings::GetColour", "[settings]")
{
    for (unsigned int i=wxSYS_COLOUR_SCROLLBAR; i < wxSYS_COLOUR_MAX; i++)
        REQUIRE( wxSystemSettings::GetColour((wxSystemColour)i).IsOk() );

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
            REQUIRE(
                wxSystemSettings::GetColour(textColour) != wxColour(0, 0, 0));
        }
    }
#endif
}

TEST_CASE("Settings::GetFont", "[settings]")
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
        CHECK( font.IsOk() );
        CHECK( wxFontEnumerator::IsValidFacename(font.GetFaceName()) );
    }
}

// The tests below are not really about wxSystemSettings but still nice to have.
TEST_CASE("Settings::GlobalColours", "[settings]")
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
        CHECK( col[i].IsOk() );
}

TEST_CASE("Settings::GlobalFonts", "[settings]")
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
        CHECK( font[i].IsOk() );

        const wxString facename = font[i].GetFaceName();
        if ( !facename.empty() )
        {
            wxINFO_FMT("font #%u: facename \"%s\" is invalid", i, facename);
            CHECK(wxFontEnumerator::IsValidFacename(facename));
        }
    }
}

TEST_CASE("Settings::GlobalBrushes", "[settings]")
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
        CHECK( brush[i].IsOk() );
}

TEST_CASE("Settings::GlobalPens", "[settings]")
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
        CHECK( pen[i].IsOk() );
}
