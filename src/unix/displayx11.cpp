///////////////////////////////////////////////////////////////////////////
// Name:        displayx11.cpp
// Purpose:     Unix/X11 implementation of wxDisplay class
// Author:      Brian Victor
// Modified by:
// Created:     12/05/02
// RCS-ID:      $Id$
// Copyright:   (c) wxWindows team
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
  #include <X11/extensions/xf86vmode.h>
}

//free private data common to x (usually s3) servers
#define wxClearXVM(vm)  if(vm.privsize) XFree(vm.c_private)
class wxDisplayUnixPriv
{
  public:
    wxRect m_rect;
    int m_depth;
    XF86VidModeModeInfo m_DefaultVidMode; 
    ~wxDisplayUnixPriv()
    {
        wxClearXVM(m_DefaultVidMode);
    }
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
          p.x <= screenarr[i].x_org + screenarr[i].width &&
          p.y >= screenarr[i].y_org &&
          p.y <= screenarr[i].y_org + screenarr[i].height)
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
        p.x <= size.GetWidth() && 
        p.y > 0 &&
        p.y <= size.GetHeight())
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


  XF86VidModeModeLine VM;
  int nDotClock;

  XF86VidModeGetModeLine((Display*)wxGetDisplay(), DefaultScreen((Display*)wxGetDisplay()),
                         &nDotClock, &VM);

  //A XF86VidModeModeLine is a XF86ModeModeInfo without the dotclock
  //field.  It would be nice to mayby use a memcpy() here instead of
  //all this?
  m_priv->m_DefaultVidMode.dotclock = nDotClock;
  m_priv->m_DefaultVidMode.hdisplay = VM.hdisplay;
  m_priv->m_DefaultVidMode.hsyncstart = VM.hsyncstart;
  m_priv->m_DefaultVidMode.hsyncend = VM.hsyncend;
  m_priv->m_DefaultVidMode.htotal = VM.htotal;
  m_priv->m_DefaultVidMode.hskew = VM.hskew;
  m_priv->m_DefaultVidMode.vdisplay = VM.vdisplay;
  m_priv->m_DefaultVidMode.vsyncstart = VM.vsyncstart;
  m_priv->m_DefaultVidMode.vsyncend = VM.vsyncend;
  m_priv->m_DefaultVidMode.vtotal = VM.vtotal;
  m_priv->m_DefaultVidMode.flags = VM.flags;
  m_priv->m_DefaultVidMode.privsize = VM.privsize;
  m_priv->m_DefaultVidMode.c_private = VM.c_private;
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

//
//  See (http://www.xfree86.org/4.2.0/XF86VidModeDeleteModeLine.3.html) for more
//  info about xf86 video mode extensions
//

#define wxCVM(v) wxVideoMode(v.hdisplay, v.vdisplay, v.dotclock /*BPP in X? */,((v.hsyncstart + v.vsyncstart) / 2) )
#define wxCVM2(v) wxVideoMode(v.hdisplay, v.vdisplay, 0 /*BPP in X? */,((v.hsyncstart + v.vsyncstart) / 2) )

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
                mode.Matches(wxCVM2((*ppXModes[i]))) ) //...?
            {
                Modes.Add(wxCVM2((*ppXModes[i])));
            }
            wxClearXVM((*ppXModes[i]));
        //  XFree(ppXModes[i]); //supposed to free?
        }
        XFree(ppXModes);
    }
    else //OOPS!
    {
        wxLogSysError("XF86VidModeGetAllModeLines Failed in wxX11Display::GetModes()!");
    }

    return Modes;
}

wxVideoMode wxDisplay::GetCurrentMode() const
{
    return wxCVM2(m_priv->m_DefaultVidMode);
}

bool wxDisplay::ChangeMode(const wxVideoMode& mode)
{
    if (mode == wxDefaultVideoMode)
    {
        return XF86VidModeSwitchToMode((Display*)wxGetDisplay(), DefaultScreen((Display*)wxGetDisplay()),
                             &m_priv->m_DefaultVidMode) == TRUE;
    }
    else //This gets kind of tricky AND complicated :) :\ :( :)
    {
        bool bRet = false;
        //Some variables..
        XF86VidModeModeInfo** ppXModes; //Enumerated Modes (Don't forget XFree() :))
        int nNumModes; //Number of modes enumerated....

        if(XF86VidModeGetAllModeLines((Display*)wxGetDisplay(), DefaultScreen((Display*)wxGetDisplay()), &nNumModes, &ppXModes) == TRUE)
        {
            for (int i = 0; i < nNumModes; ++i)
            {
                if (!bRet &&
                    ppXModes[i]->hdisplay == mode.w &&
                    ppXModes[i]->vdisplay == mode.h &&
                    ((ppXModes[i]->hsyncstart + ppXModes[i]->vsyncstart) / 2) == mode.refresh)
                {
                    //switch!
                    bRet = XF86VidModeSwitchToMode((Display*)wxGetDisplay(), DefaultScreen((Display*)wxGetDisplay()),
                             ppXModes[i]) == TRUE;
                }
                wxClearXVM((*ppXModes[i]));
            //  XFree(ppXModes[i]); //supposed to free?
            }
            XFree(ppXModes);

            return bRet;
        }
        else //OOPS!
        {
            wxLogSysError("XF86VidModeGetAllModeLines Failed in wxX11Display::ChangeMode()!");
            return false;
        }
    }
}

#endif /* wxUSE_DISPLAY */

