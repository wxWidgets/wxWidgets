/////////////////////////////////////////////////////////////////////////////
// Name:        rgncmn.cpp
// Purpose:     Methods of wxRegion that have a generic implementation
// Author:      Robin Dunn
// Modified by:
// Created:     27-Mar-2003
// RCS-ID:      $Id$
// Copyright:   (c) Robin Dunn
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "rgncmn.h"
#endif


// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "wx/region.h"
#include "wx/bitmap.h"
#if wxUSE_IMAGE
#include "wx/image.h"
#endif
#include "wx/dcmemory.h"


//---------------------------------------------------------------------------



wxBitmap wxRegion::ConvertToBitmap() const
{
    wxRect box = GetBox();
    wxBitmap bmp(box.GetRight(), box.GetBottom());
    wxMemoryDC dc;
    dc.SelectObject(bmp);
    dc.SetBackground(*wxBLACK_BRUSH);
    dc.Clear();
    dc.SetClippingRegion(*this);
    dc.SetBackground(*wxWHITE_BRUSH);
    dc.Clear();
    dc.SelectObject(wxNullBitmap);
    return bmp;
}

//---------------------------------------------------------------------------

#if wxUSE_IMAGE
static bool DoRegionUnion(wxRegion& region,
                          const wxImage& image,
                          unsigned char loR,
                          unsigned char loG,
                          unsigned char loB,
                          int tolerance)
{
    unsigned char hiR, hiG, hiB;

    hiR = wxMin(0xFF, loR + tolerance);
    hiG = wxMin(0xFF, loG + tolerance);
    hiB = wxMin(0xFF, loB + tolerance);

    // Loop through the image row by row, pixel by pixel, building up
    // rectangles to add to the region.
    int width = image.GetWidth();
    int height = image.GetHeight();
    for (int y=0; y < height; y++)
    {
        wxRect rect;
        rect.y = y;
        rect.height = 1;

        for (int x=0; x < width; x++)
        {
            // search for a continuous range of non-transparent pixels
            int x0 = x;
            while ( x < width)
            {
                unsigned char R = image.GetRed(x,y);
                unsigned char G = image.GetGreen(x,y);
                unsigned char B = image.GetBlue(x,y);
                if (( R >= loR && R <= hiR) &&
                    ( G >= loG && G <= hiG) &&
                    ( B >= loB && B <= hiB))  // It's transparent
                    break;
                x++;
            }

            // Add the run of non-transparent pixels (if any) to the region
            if (x > x0) {
                rect.x = x0;
                rect.width = x - x0;
                region.Union(rect);
            }
        }
    }

    return true;
}


bool wxRegion::Union(const wxBitmap& bmp)
{
    if (bmp.GetMask())
    {
        wxImage image = bmp.ConvertToImage();
        wxASSERT_MSG( image.HasMask(), _T("wxBitmap::ConvertToImage doesn't preserve mask?") );
        return DoRegionUnion(*this, image,
                             image.GetMaskRed(),
                             image.GetMaskGreen(),
                             image.GetMaskBlue(),
                             0);
    }
    else
    {
        return Union(0, 0, bmp.GetWidth(), bmp.GetHeight());
    }
}

bool wxRegion::Union(const wxBitmap& bmp,
                     const wxColour& transColour,
                     int   tolerance)
{
    wxImage image = bmp.ConvertToImage();
    return DoRegionUnion(*this, image,
                         transColour.Red(),
                         transColour.Green(),
                         transColour.Blue(),
                         tolerance);
}

#else

bool wxRegion::Union(const wxBitmap& WXUNUSED(bmp))
{
    // No wxImage support
    return false;
}

bool wxRegion::Union(const wxBitmap& WXUNUSED(bmp),
                     const wxColour& WXUNUSED(transColour),
                     int   WXUNUSED(tolerance))
{
    // No wxImage support
    return false;
}

#endif

//---------------------------------------------------------------------------
