///////////////////////////////////////////////////////////////////////////
// Name:        displayx11.cpp
// Purpose:     Unix/X11 implementation of wxDisplay class
// Author:      Brian Victor
// Modified by:
// Created:     12/05/02
// RCS-ID:      $Id$
// Copyright:   (c) wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "display.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
  #pragma hdrstop
#endif

#include "wx/display.h"
#include "wx/intl.h"
#include "wx/log.h"

#ifndef WX_PRECOMP
  #include "wx/dynarray.h"
  #include "wx/gdicmn.h"
  #include "wx/string.h"
  #include "wx/utils.h"
#endif /* WX_PRECOMP */

#if wxUSE_DISPLAY

/* These must be included after the wx files.  Otherwise the Data macro in
 * Xlibint.h conflicts with a function declaration in wx/list.h.  */
extern "C" {
  #include <X11/Xlib.h>
  #include <X11/Xlibint.h>
  #include <X11/extensions/Xinerama.h>
#ifdef HAVE_X11_EXTENSIONS_XF86VMODE_H
  #include <X11/extensions/xf86vmode.h>
#endif
}

class wxDisplayUnixPriv
{
  public:
    wxRect m_rect;
    int m_depth;
};

size_t wxDisplayBase::GetCount()
{
  Display *disp = (Display*)wxGetDisplay();

  if ( XineramaIsActive(disp) )
  {
    XineramaScreenInfo *screenarr;
    int numscreens;
    screenarr = XineramaQueryScreens(disp, &numscreens);
    XFree(screenarr);
    return numscreens;
  }
  else
  {
    return 1;
  }
}

int wxDisplayBase::GetFromPoint(const wxPoint &p)
{
  Display *disp = (Display*)wxGetDisplay();

  if ( XineramaIsActive(disp) )
  {
    int which_screen = -1;
    XineramaScreenInfo *screenarr;
    int numscreens;
    screenarr = XineramaQueryScreens(disp, &numscreens);

    int i;
    for (i = 0; i < numscreens; ++i)
    {
      if (p.x >= screenarr[i].x_org &&
          p.x < screenarr[i].x_org + screenarr[i].width &&
          p.y >= screenarr[i].y_org &&
          p.y < screenarr[i].y_org + screenarr[i].height)
      {
        which_screen = i;
      }
    }

    XFree(screenarr);
    return which_screen;
  }
  else
  {
    wxSize size = wxGetDisplaySize();
    if (p.x >= 0 &&
        p.x < size.GetWidth() &&
        p.y >= 0 &&
        p.y < size.GetHeight())
    {
        return 0;
    }

    return -1;
  }
}

wxDisplay::wxDisplay(size_t index) : wxDisplayBase ( index ), m_priv( new wxDisplayUnixPriv )
{
  Display *disp = (Display*)wxGetDisplay();

  if ( XineramaIsActive(disp) )
  {
    XineramaScreenInfo *screenarr;
    int numscreens;
    screenarr = XineramaQueryScreens(disp, &numscreens);

    m_priv->m_rect = wxRect(screenarr[index].x_org, screenarr[index].y_org,
                            screenarr[index].width, screenarr[index].height);
    m_priv->m_depth = DefaultDepth(disp, DefaultScreen(disp));
    XFree(screenarr);
  }
  else
  {
    wxSize size = wxGetDisplaySize();
    m_priv->m_rect = wxRect(0, 0, size.GetWidth(), size.GetHeight());
    m_priv->m_depth = wxDisplayDepth();
  }
}

wxDisplay::~wxDisplay()
{
  delete m_priv;
}

wxRect wxDisplay::GetGeometry() const
{
  return m_priv->m_rect;
}

int wxDisplay::GetDepth() const
{
  return m_priv->m_depth;
}

wxString wxDisplay::GetName() const
{
  return wxEmptyString;
}


#ifdef HAVE_X11_EXTENSIONS_XF86VMODE_H

//
//  See (http://www.xfree86.org/4.2.0/XF86VidModeDeleteModeLine.3.html) for more
//  info about xf86 video mode extensions
//

//free private data common to x (usually s3) servers
#define wxClearXVM(vm)  if(vm.privsize) XFree(vm.c_private)

// Correct res rate from GLFW
#define wxCRR2(v,dc) (int) (((1000.0f * (float) dc) /*PIXELS PER SECOND */) / ((float) v.htotal * v.vtotal /*PIXELS PER FRAME*/) + 0.5f)
#define wxCRR(v) wxCRR2(v,v.dotclock)
#define wxCVM2(v, dc) wxVideoMode(v.hdisplay, v.vdisplay, DefaultDepth((Display*)wxGetDisplay(), DefaultScreen((Display*)wxGetDisplay())), wxCRR2(v,dc))
#define wxCVM(v) wxCVM2(v, v.dotclock)

wxArrayVideoModes wxDisplay::GetModes(const wxVideoMode& mode) const
{
    //Convenience...
    Display* pDisplay = (Display*) wxGetDisplay(); //default display
    int nScreen = DefaultScreen(pDisplay); //default screen of (default) display...

    //Some variables..
    XF86VidModeModeInfo** ppXModes; //Enumerated Modes (Don't forget XFree() :))
    int nNumModes; //Number of modes enumerated....

    wxArrayVideoModes Modes; //modes to return...

    if (XF86VidModeGetAllModeLines(pDisplay, nScreen, &nNumModes, &ppXModes) == TRUE)
    {
        for (int i = 0; i < nNumModes; ++i)
        {
            if (mode == wxDefaultVideoMode || //According to display.h All modes valid if dafault mode...
                mode.Matches(wxCVM((*ppXModes[i]))) ) //...?
            {
                Modes.Add(wxCVM((*ppXModes[i])));
            }
            wxClearXVM((*ppXModes[i]));
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

wxVideoMode wxDisplay::GetCurrentMode() const
{
  XF86VidModeModeLine VM;
  int nDotClock;
  XF86VidModeGetModeLine((Display*)wxGetDisplay(), DefaultScreen((Display*)wxGetDisplay()),
                         &nDotClock, &VM);
  wxClearXVM(VM);
  return wxCVM2(VM, nDotClock);
}

bool wxDisplay::ChangeMode(const wxVideoMode& mode)
{
    XF86VidModeModeInfo** ppXModes; //Enumerated Modes (Don't forget XFree() :))
    int nNumModes; //Number of modes enumerated....

    if( !XF86VidModeGetAllModeLines((Display*)wxGetDisplay(), DefaultScreen((Display*)wxGetDisplay()), &nNumModes, &ppXModes) )
    {
        wxLogSysError(_("Failed to change video mode"));
        return false;
    }

    bool bRet = false;
    if (mode == wxDefaultVideoMode)
    {
        bRet = XF86VidModeSwitchToMode((Display*)wxGetDisplay(), DefaultScreen((Display*)wxGetDisplay()),
                     ppXModes[0]) == TRUE;

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
                ppXModes[i]->hdisplay == mode.w &&
                ppXModes[i]->vdisplay == mode.h &&
                wxCRR((*ppXModes[i])) == mode.refresh)
            {
                //switch!
                bRet = XF86VidModeSwitchToMode((Display*)wxGetDisplay(), DefaultScreen((Display*)wxGetDisplay()),
                         ppXModes[i]) == TRUE;
            }
            wxClearXVM((*ppXModes[i]));
        //  XFree(ppXModes[i]); //supposed to free?
        }
    }

    XFree(ppXModes);

    return bRet;
}


#else // !HAVE_X11_EXTENSIONS_XF86VMODE_H

wxArrayVideoModes wxDisplay::GetModes(const wxVideoMode& mode) const
{
  Display *disp = (Display*)wxGetDisplay();
  int count_return;
  int* depths = XListDepths(disp, 0, &count_return);
  wxArrayVideoModes modes;
  if (depths)
  {
    int x;
    for (x = 0; x < count_return; ++x)
    {
      modes.Add(wxVideoMode(m_priv->m_rect.GetWidth(), m_priv->m_rect.GetHeight(), depths[x]));
    }
  }
  return modes;
}

wxVideoMode wxDisplay::GetCurrentMode() const
{
    // Not implemented
    return wxVideoMode();
}

bool wxDisplay::ChangeMode(const wxVideoMode& mode)
{
    // Not implemented
    return false;
}

#endif // !HAVE_X11_EXTENSIONS_XF86VMODE_H

#endif /* wxUSE_DISPLAY */
