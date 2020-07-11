///////////////////////////////////////////////////////////////////////////////
// Name:        tests/graphics/colour.cpp
// Purpose:     wxColour unit test
// Author:      Vadim Zeitlin
// Created:     2009-09-19
// Copyright:   (c) 2009 Vadim Zeitlin <vadim@wxwidgets.org>
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/colour.h"
#include "asserthelper.h"

// ----------------------------------------------------------------------------
// helpers for checking wxColour RGB[A] values
// ----------------------------------------------------------------------------

typedef wxColour::ChannelType ChannelType;

class ColourRGBMatcher : public Catch::MatcherBase<wxColour>
{
public:
    ColourRGBMatcher(ChannelType red, ChannelType green, ChannelType blue)
        : m_red(red),
          m_green(green),
          m_blue(blue)
    {
    }

    bool match(const wxColour& c) const wxOVERRIDE
    {
        return c.Red() == m_red && c.Green() == m_green && c.Blue() == m_blue;
    }

    std::string describe() const wxOVERRIDE
    {
        return wxString::Format("!= RGB(%#02x, %#02x, %#02x)",
                                m_red, m_green, m_blue).ToStdString();
    }

protected:
    const ChannelType m_red, m_green, m_blue;
};

class ColourRGBAMatcher : public ColourRGBMatcher
{
public:
    ColourRGBAMatcher(ChannelType red, ChannelType green, ChannelType blue,
                      ChannelType alpha)
        : ColourRGBMatcher(red, green, blue),
          m_alpha(alpha)
    {
    }

    bool match(const wxColour& c) const wxOVERRIDE
    {
        return ColourRGBMatcher::match(c) && c.Alpha() == m_alpha;
    }

    std::string describe() const wxOVERRIDE
    {
        return wxString::Format("!= RGBA(%#02x, %#02x, %#02x, %#02x)",
                                m_red, m_green, m_blue, m_alpha).ToStdString();
    }

private:
    const ChannelType m_alpha;
};

inline
ColourRGBMatcher
RGBSameAs(ChannelType red, ChannelType green, ChannelType blue)
{
    return ColourRGBMatcher(red, green, blue);
}

inline
ColourRGBAMatcher
RGBASameAs(ChannelType red, ChannelType green, ChannelType blue, ChannelType alpha)
{
    return ColourRGBAMatcher(red, green, blue, alpha);
}

// ----------------------------------------------------------------------------
// tests
// ----------------------------------------------------------------------------

TEST_CASE("wxColour::GetSetRGB", "[colour][rgb]")
{
    wxColour c;
    c.SetRGB(0x123456);

    CHECK( c.Red()   == 0x56 );
    CHECK( c.Green() == 0x34 );
    CHECK( c.Blue()  == 0x12 );
    CHECK( c.Alpha() == wxALPHA_OPAQUE );

    CHECK( c == wxColour(0x123456) );
    CHECK( c.GetRGB() == 0x123456 );

    c.SetRGBA(0xaabbccdd);

    CHECK( c.Red()   == 0xdd);
    CHECK( c.Green() == 0xcc);
    CHECK( c.Blue()  == 0xbb);

    // wxX11 doesn't support alpha at all currently.
#ifndef __WXX11__
    CHECK( c.Alpha() == 0xaa );
#endif // __WXX11__

    // FIXME: at least under wxGTK wxColour ctor doesn't take alpha channel
    //        into account: bug or feature?
    //CHECK( c == wxColour(0xaabbccdd) );
    CHECK( c.GetRGB() == 0xbbccdd );
#ifndef __WXX11__
    CHECK( c.GetRGBA() == 0xaabbccdd );
#endif // __WXX11__
}

TEST_CASE("wxColour::FromString", "[colour][string]")
{
    CHECK_THAT( wxColour("rgb(11, 22, 33)"), RGBSameAs(11, 22, 33) );
    // wxX11 doesn't support alpha at all currently.
#ifndef __WXX11__
    CHECK_THAT( wxColour("rgba(11, 22, 33, 0.5)"), RGBASameAs(11, 22, 33, 128) );
    CHECK_THAT( wxColour("rgba( 11, 22, 33, 0.5 )"), RGBASameAs(11, 22, 33, 128) );
#endif // __WXX11__

    CHECK_THAT( wxColour("#aabbcc"), RGBSameAs(0xaa, 0xbb, 0xcc) );

    CHECK_THAT( wxColour("red"), RGBSameAs(0xff, 0, 0) );

    wxColour col;
    CHECK( !wxFromString("rgb(1, 2)", &col) );
    CHECK( !wxFromString("rgba(1, 2, 3.456)", &col) );
    CHECK( !wxFromString("rgba(1, 2, 3.456, foo)", &col) );
}

TEST_CASE("wxColour::GetAsString", "[colour][string]")
{
    CHECK( wxColour().GetAsString() == "" );

    wxColour red("red");
    CHECK( red.GetAsString() == "red" );
    CHECK( red.GetAsString(wxC2S_CSS_SYNTAX) == "rgb(255, 0, 0)" );
    CHECK( red.GetAsString(wxC2S_HTML_SYNTAX) == "#FF0000" );
}

TEST_CASE("wxColour::GetLuminance", "[colour][luminance]")
{
    CHECK( wxBLACK->GetLuminance() == Approx(0.0) );
    CHECK( wxWHITE->GetLuminance() == Approx(1.0) );
    CHECK( wxRED->GetLuminance() > 0 );
    CHECK( wxRED->GetLuminance() < 1 );
}
