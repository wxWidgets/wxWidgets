/////////////////////////////////////////////////////////////////////////////
// Name:        src/unix/utilsx11.cpp
// Purpose:     Miscellaneous X11 functions
// Author:      Mattia Barbon
// Modified by:
// Created:     25.03.02
// RCS-ID:      $Id$
// Copyright:   (c) wxWindows team
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#if defined(__WXX11__) || defined(__WXGTK__) || defined(__WXMOTIF__)

#include "wx/unix/utilsx11.h"
#include "wx/iconbndl.h"
#include "wx/image.h"
#include "wx/icon.h"

#include <X11/Xlib.h>
#include <X11/Xatom.h>

void wxSetIconsX11( WXDisplay* display, WXWindow window,
                    const wxIconBundle& ib )
{
    size_t size = 0;
    size_t i, max = ib.m_icons.GetCount();

    for( i = 0; i < max; ++i )
        size += 2 + ib.m_icons[i].GetWidth() * ib.m_icons[i].GetHeight();

    Atom net_wm_icon = XInternAtom( (Display*)display, "_NET_WM_ICON", 0 );

    if( size > 0 )
    {
        wxUint32* data = new wxUint32[size];
        wxUint32* ptr = data;

        for( i = 0; i < max; ++i )
        {
            const wxImage image = ib.m_icons[i].ConvertToImage();
            int width = image.GetWidth(), height = image.GetHeight();
            unsigned char* imageData = image.GetData();
            unsigned char* imageDataEnd = imageData + ( width * height * 3 );
            bool hasMask = image.HasMask();
            unsigned char rMask, gMask, bMask;
            unsigned char r, g, b, a;

            if( hasMask )
            {
                rMask = image.GetMaskRed();
                gMask = image.GetMaskGreen();
                bMask = image.GetMaskBlue();
            }

            *ptr++ = width;
            *ptr++ = height;

            while( imageData < imageDataEnd ) {
                r = imageData[0];
                g = imageData[1];
                b = imageData[2];
                if( hasMask && r == rMask && g == gMask && b == bMask )
                    a = 0;
                else
                    a = 255;

                *ptr++ = ( a << 24 ) | ( r << 16 ) | ( g << 8 ) | b;

                imageData += 3;
            }
        }

        XChangeProperty( (Display*)display,
                         (Window)window,
                         net_wm_icon,
                         XA_CARDINAL, 32,
                         PropModeReplace,
                         (unsigned char*)data, size );
        delete[] data;
    }
    else
    {
        XDeleteProperty( (Display*)display,
                         (Window)window,
                         net_wm_icon );
    }
}

#endif
