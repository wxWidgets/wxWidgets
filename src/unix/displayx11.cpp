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

#ifdef __GNUG__
    #pragma implementation "display.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
  #pragma hdrstop
#endif

#include "wx/display.h"

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
    return 0;
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
  return "";
}

#endif /* wxUSE_DISPLAY */

