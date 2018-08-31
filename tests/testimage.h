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

        return memcmp(other.GetData(), m_image.GetData(),
                      other.GetWidth()*other.GetHeight()*3) == 0;
    }

    std::string describe() const wxOVERRIDE
    {
        return "has same RGB data as " + Catch::toString(m_image);
    }

private:
    const wxImage m_image;
};

inline ImageRGBMatcher RGBSameAs(const wxImage& image)
{
    return ImageRGBMatcher(image);
}

#endif // _WX_TESTS_TESTIMAGE_H_
