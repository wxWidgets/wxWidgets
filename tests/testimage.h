///////////////////////////////////////////////////////////////////////////////
// Name:        tests/testimage.h
// Purpose:     Unit test helpers for dealing with wxImage.
// Author:      Vadim Zeitlin
// Copyright:   (c) 2011 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TESTS_TESTIMAGE_H_
#define _WX_TESTS_TESTIMAGE_H_

#include "wx/image.h"

namespace Catch
{
    template <>
    struct StringMaker<wxImage>
    {
        static std::string convert(const wxImage& image)
        {
            return wxString::Format("image of size %d*%d with%s alpha",
                                    image.GetWidth(),
                                    image.GetHeight(),
                                    image.HasAlpha() ? "" : "out")
                    .ToStdString();
        }
    };
}

class ImageRGBMatcher : public Catch::MatcherBase<wxImage>
{
public:
    ImageRGBMatcher(const wxImage& image, int tolerance, bool checkAlpha = false)
        : m_image(image)
        , m_tolerance(tolerance)
        , m_checkAlpha(checkAlpha)
    {
    }

    bool match(const wxImage& other) const override
    {
        if ( other.GetWidth() != m_image.GetWidth() )
            return false;

        if ( other.GetHeight() != m_image.GetHeight() )
            return false;

        if ( memcmp(other.GetData(), m_image.GetData(),
                    other.GetWidth()*other.GetHeight()*3) == 0 )
        {
            if ( m_checkAlpha )
            {
                if ( !other.HasAlpha() && !m_image.HasAlpha() )
                    return true;

                if ( other.HasAlpha() && m_image.HasAlpha() &&
                     memcmp(other.GetAlpha(), m_image.GetAlpha(), other.GetWidth() * other.GetHeight()) == 0 )
                    return true;
            }
            else
            {
                return true;
            }
        }

        const wxString dispAlphaValNull("--");
        const unsigned char* d1 = m_image.GetData();
        const unsigned char* d2 = other.GetData();
        const unsigned char* a1 = m_image.GetAlpha();
        const unsigned char* a2 = other.GetAlpha();
        bool dispAlphaVal = a1 || a2;
        for ( int y = 0; y < m_image.GetHeight(); ++y )
        {
            for ( int x = 0; x < m_image.GetWidth(); ++x )
            {
                wxString a1txt = dispAlphaVal ? (a1 != nullptr ? wxString::Format("%02x", *a1) : dispAlphaValNull) : wxString();
                wxString a2txt = dispAlphaVal ? (a2 != nullptr ? wxString::Format("%02x", *a2) : dispAlphaValNull) : wxString();

                for ( int i = 0; i < 3; i++ )
                {
                    const unsigned char diff = d1[i] > d2[i] ? d1[i] - d2[i] : d2[i] - d1[i];
                    if ( diff > m_tolerance )
                    {
                        m_diffDesc.Printf
                        (
                            "first mismatch is at (%d, %d) which "
                            "has value 0x%02x%02x%02x%s instead of the "
                            "expected 0x%02x%02x%02x%s",
                            x, y, d2[0], d2[1], d2[2], a2txt, d1[0], d1[1], d1[2], a1txt
                        );

                        // Don't show all mismatches, there may be too many of them.
                        return false;
                    }
                }

                if ( m_checkAlpha )
                {
                    if ( a1 && a2 )
                    {
                        const unsigned char diff = *a1 > *a2 ? *a1 - *a2 : *a2 - *a1;
                        if ( diff > m_tolerance )
                        {
                            m_diffDesc.Printf
                            (
                                "first mismatch is at (%d, %d) which "
                                "has value 0x%02x%02x%02x%02x instead of the "
                                "expected 0x%02x%02x%02x%02x",
                                x, y, d2[0], d2[1], d2[2], *a2, d1[0], d1[1], d1[2], *a1
                            );

                            // Don't show all mismatches, there may be too many of them.
                            return false;
                        }
                    }
                    else if ( a1txt != a2txt )
                    {
                        m_diffDesc.Printf
                        (
                            "first mismatch is at (%d, %d) which "
                            "has value 0x%02x%02x%02x%s instead of the "
                            "expected 0x%02x%02x%02x%s",
                            x, y, d2[0], d2[1], d2[2], a2txt, d1[0], d1[1], d1[2], a1txt
                        );

                        // Don't show all mismatches, there may be too many of them.
                        return false;
                    }
                }

                d1 += 3;
                d2 += 3;
                if ( a1 ) a1++;
                if ( a2 ) a2++;
            }
        }

        // We can only get here when the images are different AND we've not exited the
        // method from the loop. That implies the tolerance must have caused this.
        wxASSERT_MSG(m_tolerance > 0, "Unreachable without tolerance");

        return true;
    }

    std::string describe() const override
    {
        std::string desc = "doesn't have the same RGB data as " +
                                Catch::StringMaker<wxImage>::convert(m_image);

        if ( !m_diffDesc.empty() )
            desc += + ": " + m_diffDesc.ToStdString(wxConvUTF8);

        return desc;
    }

private:
    const wxImage m_image;
    const int m_tolerance;
    const bool m_checkAlpha;
    mutable wxString m_diffDesc;
};

inline ImageRGBMatcher RGBSameAs(const wxImage& image)
{
    return ImageRGBMatcher(image, 0, false);
}

inline ImageRGBMatcher RGBASameAs(const wxImage& image)
{
    return ImageRGBMatcher(image, 0, true);
}

// Allows small differences (within given tolerance) for r, g, and b values.
inline ImageRGBMatcher RGBSimilarTo(const wxImage& image, int tolerance)
{
    return ImageRGBMatcher(image, tolerance, false);
}

inline ImageRGBMatcher RGBASimilarTo(const wxImage& image, int tolerance)
{
    return ImageRGBMatcher(image, tolerance, true);
}

class ImageAlphaMatcher : public Catch::MatcherBase<wxImage>
{
public:
    ImageAlphaMatcher(unsigned char alpha)
        : m_alpha(alpha)
    {
    }

    bool match(const wxImage& other) const override
    {
        if (!other.HasAlpha())
        {
            m_diffDesc = "no alpha data";
            return false;
        }

        unsigned char center_alpha =
            *(other.GetAlpha() + (other.GetWidth() / 2) + (other.GetHeight() / 2 * other.GetWidth()));

        if (m_alpha != center_alpha)
        {
            m_diffDesc.Printf("got alpha %u", center_alpha);
            return false;
        }

        return true;
    }

    std::string describe() const override
    {
        std::string desc;

        if (!m_diffDesc.empty())
            desc = m_diffDesc.ToStdString(wxConvUTF8);

        return desc;
    }

private:
    const unsigned char m_alpha;
    mutable wxString m_diffDesc;
};

inline ImageAlphaMatcher CenterAlphaPixelEquals(unsigned char alpha)
{
    return ImageAlphaMatcher(alpha);
}

#endif // _WX_TESTS_TESTIMAGE_H_
