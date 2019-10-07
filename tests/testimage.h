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
    ImageRGBMatcher(const wxImage& image)
        : m_image(image)
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
                if ( *d1 != *d2 )
                {
                    m_diffDesc.Printf
                               (
                                    "first mismatch is at (%d, %d) which "
                                    "has value 0x%06x instead of the "
                                    "expected 0x%06x",
                                    x, y, *d2, *d1
                               );
                    break;
                }

                ++d1;
                ++d2;
            }
        }

        return false;
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
    mutable wxString m_diffDesc;
};

inline ImageRGBMatcher RGBSameAs(const wxImage& image)
{
    return ImageRGBMatcher(image);
}

#endif // _WX_TESTS_TESTIMAGE_H_
