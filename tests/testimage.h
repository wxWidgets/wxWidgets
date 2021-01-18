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
    ImageRGBMatcher(const wxImage& image, int tolerance)
        : m_image(image)
        , m_tolerance(tolerance)
    {
    }

    bool match(const wxImage& other) const wxOVERRIDE
    {
        if ( other.GetWidth() != m_image.GetWidth() )
            return false;

        if ( other.GetHeight() != m_image.GetHeight() )
            return false;

        if ( memcmp(other.GetData(), m_image.GetData(),
                    other.GetWidth()*other.GetHeight()*3) == 0 )
            return true;

        const unsigned char* d1 = m_image.GetData();
        const unsigned char* d2 = other.GetData();
        for ( int x = 0; x < m_image.GetWidth(); ++x )
        {
            for ( int y = 0; y < m_image.GetHeight(); ++y )
            {
                const unsigned char diff = *d1 > * d2 ? *d1 - *d2 : *d2 - *d1;
                if (diff > m_tolerance)
                {
                    m_diffDesc.Printf
                               (
                                    "first mismatch is at (%d, %d) which "
                                    "has value 0x%06x instead of the "
                                    "expected 0x%06x",
                                    x, y, *d2, *d1
                               );

                    // Don't show all mismatches, there may be too many of them.
                    return false;
                }

                ++d1;
                ++d2;
            }
        }

        // We can only get here when the images are different AND we've not exited the
        // method from the loop. That implies the tolerance must have caused this.
        wxASSERT_MSG(m_tolerance > 0, "Unreachable without tolerance");

        return true;
    }

    std::string describe() const wxOVERRIDE
    {
        std::string desc = "doesn't have the same RGB data as " +
                                Catch::toString(m_image);

        if ( !m_diffDesc.empty() )
            desc += + ": " + m_diffDesc.ToStdString(wxConvUTF8);

        return desc;
    }

private:
    const wxImage m_image;
    const int m_tolerance;
    mutable wxString m_diffDesc;
};

inline ImageRGBMatcher RGBSameAs(const wxImage& image)
{
    return ImageRGBMatcher(image, 0);
}

// Allows small differences (within given tolerance) for r, g, and b values.
inline ImageRGBMatcher RGBSimilarTo(const wxImage& image, int tolerance)
{
    return ImageRGBMatcher(image, tolerance);
}

class ImageAlphaMatcher : public Catch::MatcherBase<wxImage>
{
public:
    ImageAlphaMatcher(unsigned char alpha)
        : m_alpha(alpha)
    {
    }

    bool match(const wxImage& other) const wxOVERRIDE
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

    std::string describe() const wxOVERRIDE
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
