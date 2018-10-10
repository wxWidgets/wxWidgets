///////////////////////////////////////////////////////////////////////////////
// Name:        wx/unix/private/displayx11.h
// Purpose:     Helper functions used by wxX11 and wxGTK ports
// Author:      Vadim Zeitlin
// Created:     2018-10-04 (extracted from src/unix/displayx11.cpp)
// Copyright:   (c) 2002-2018 wxWindows team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UNIX_PRIVATE_DISPLAYX11_H_
#define _WX_UNIX_PRIVATE_DISPLAYX11_H_

#include "wx/defs.h"

#include <X11/Xlib.h>
#include <X11/Xatom.h>

#if wxUSE_DISPLAY

#include "wx/log.h"
#include "wx/translation.h"

#ifdef HAVE_X11_EXTENSIONS_XF86VMODE_H

#include <X11/extensions/xf86vmode.h>

//
//  See (http://www.xfree86.org/4.2.0/XF86VidModeDeleteModeLine.3.html) for more
//  info about xf86 video mode extensions
//

//free private data common to x (usually s3) servers
#define wxClearXVM(vm)  if(vm.privsize) XFree(vm.c_private)

// Correct res rate from GLFW
#define wxCRR2(v,dc) (int) (((1000.0f * (float) dc) /*PIXELS PER SECOND */) / ((float) v.htotal * v.vtotal /*PIXELS PER FRAME*/) + 0.5f)
#define wxCRR(v) wxCRR2(v,v.dotclock)
#define wxCVM2(v, dc, display, nScreen) wxVideoMode(v.hdisplay, v.vdisplay, DefaultDepth(display, nScreen), wxCRR2(v,dc))
#define wxCVM(v, display, nScreen) wxCVM2(v, v.dotclock, display, nScreen)

wxArrayVideoModes wxXF86VidMode_GetModes(const wxVideoMode& mode, Display* display, int nScreen)
{
    XF86VidModeModeInfo** ppXModes; //Enumerated Modes (Don't forget XFree() :))
    int nNumModes; //Number of modes enumerated....

    wxArrayVideoModes Modes; //modes to return...

    if (XF86VidModeGetAllModeLines(display, nScreen, &nNumModes, &ppXModes))
    {
        for (int i = 0; i < nNumModes; ++i)
        {
            XF86VidModeModeInfo& info = *ppXModes[i];
            const wxVideoMode vm = wxCVM(info, display, nScreen);
            if (vm.Matches(mode))
            {
                Modes.Add(vm);
            }
            wxClearXVM(info);
        //  XFree(ppXModes[i]); //supposed to free?
        }
        XFree(ppXModes);
    }
    else //OOPS!
    {
        wxLogSysError(_("Failed to enumerate video modes"));
    }

    return Modes;
}

wxVideoMode wxXF86VidMode_GetCurrentMode(Display* display, int nScreen)
{
  XF86VidModeModeLine VM;
  int nDotClock;
  if ( !XF86VidModeGetModeLine(display, nScreen, &nDotClock, &VM) )
      return wxVideoMode();

  wxClearXVM(VM);
  return wxCVM2(VM, nDotClock, display, nScreen);
}

bool wxXF86VidMode_ChangeMode(const wxVideoMode& mode, Display* display, int nScreen)
{
    XF86VidModeModeInfo** ppXModes; //Enumerated Modes (Don't forget XFree() :))
    int nNumModes; //Number of modes enumerated....

    if(!XF86VidModeGetAllModeLines(display, nScreen, &nNumModes, &ppXModes))
    {
        wxLogSysError(_("Failed to change video mode"));
        return false;
    }

    bool bRet = false;
    if (mode == wxDefaultVideoMode)
    {
        bRet = XF86VidModeSwitchToMode(display, nScreen, ppXModes[0]) != 0;

        for (int i = 0; i < nNumModes; ++i)
        {
            wxClearXVM((*ppXModes[i]));
        //  XFree(ppXModes[i]); //supposed to free?
        }
    }
    else
    {
        for (int i = 0; i < nNumModes; ++i)
        {
            if (!bRet &&
                ppXModes[i]->hdisplay == mode.GetWidth() &&
                ppXModes[i]->vdisplay == mode.GetHeight() &&
                wxCRR((*ppXModes[i])) == mode.GetRefresh())
            {
                //switch!
                bRet = XF86VidModeSwitchToMode(display, nScreen, ppXModes[i]) != 0;
            }
            wxClearXVM((*ppXModes[i]));
        //  XFree(ppXModes[i]); //supposed to free?
        }
    }

    XFree(ppXModes);

    return bRet;
}

#else // !HAVE_X11_EXTENSIONS_XF86VMODE_H

wxArrayVideoModes wxX11_GetModes(const wxDisplayImpl* impl, const wxVideoMode& modeMatch, Display* display)
{
    int count_return;
    int* depths = XListDepths(display, 0, &count_return);
    wxArrayVideoModes modes;
    if ( depths )
    {
        const wxRect rect = impl->GetGeometry();
        for ( int x = 0; x < count_return; ++x )
        {
            wxVideoMode mode(rect.width, rect.height, depths[x]);
            if ( mode.Matches(modeMatch) )
            {
                modes.Add(mode);
            }
        }

        XFree(depths);
    }
    return modes;
}

#endif // !HAVE_X11_EXTENSIONS_XF86VMODE_H

#endif // wxUSE_DISPLAY

void wxGetWorkAreaX11(Screen* screen, int& x, int& y, int& width, int& height)
{
    Display* display = DisplayOfScreen(screen);
    Atom property = XInternAtom(display, "_NET_WORKAREA", true);
    if (property)
    {
        Atom actual_type;
        int actual_format;
        unsigned long nitems;
        unsigned long bytes_after;
        unsigned char* data = NULL;
        Status status = XGetWindowProperty(
            display, RootWindowOfScreen(screen), property,
            0, 4, false, XA_CARDINAL,
            &actual_type, &actual_format, &nitems, &bytes_after, &data);
        if (status == Success && actual_type == XA_CARDINAL &&
            actual_format == 32 && nitems == 4)
        {
            const long* p = (long*)data;
            x = p[0];
            y = p[1];
            width = p[2];
            height = p[3];
        }
        if (data)
            XFree(data);
    }
}

#endif // _WX_UNIX_PRIVATE_DISPLAYX11_H_
