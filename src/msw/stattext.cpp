/////////////////////////////////////////////////////////////////////////////
// Name:        stattext.cpp
// Purpose:     wxStaticText
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "stattext.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/event.h"
#include "wx/app.h"
#include "wx/brush.h"
#endif

#include "wx/stattext.h"
#include "wx/msw/private.h"
#include <stdio.h>

IMPLEMENT_DYNAMIC_CLASS(wxStaticText, wxControl)

bool wxStaticText::Create(wxWindow *parent, wxWindowID id,
           const wxString& label,
           const wxPoint& pos,
           const wxSize& size,
           long style,
           const wxString& name)
{
  SetName(name);
  if (parent) parent->AddChild(this);

  SetBackgroundColour(parent->GetBackgroundColour()) ;
  SetForegroundColour(parent->GetForegroundColour()) ;

  if ( id == -1 )
    m_windowId = (int)NewControlId();
  else
  m_windowId = id;

  int x = pos.x;
  int y = pos.y;
  int width = size.x;
  int height = size.y;

  m_windowStyle = style;

  long msStyle = WS_CHILD | WS_VISIBLE /* | WS_CLIPSIBLINGS */ ;
  if (m_windowStyle & wxALIGN_CENTRE)
    msStyle |= SS_CENTER;
  else if (m_windowStyle & wxALIGN_RIGHT)
    msStyle |= SS_RIGHT;
  else
    msStyle |= SS_LEFT;

  // Even with extended styles, need to combine with WS_BORDER
  // for them to look right.
  if ( wxStyleHasBorder(m_windowStyle) )
    msStyle |= WS_BORDER;

  m_hWnd = (WXHWND)::CreateWindowEx(MakeExtendedStyle(m_windowStyle), wxT("STATIC"), (const wxChar *)label,
                         msStyle,
                         0, 0, 0, 0, (HWND) parent->GetHWND(), (HMENU)m_windowId,
                         wxGetInstance(), NULL);

  wxCHECK_MSG( m_hWnd, FALSE, wxT("Failed to create static ctrl") );

#if wxUSE_CTL3D
/*
  if (!(GetParent()->GetWindowStyleFlag() & wxUSER_COLOURS))
    Ctl3dSubclassCtl(static_item);
*/
#endif

  SubclassWin(m_hWnd);

  wxControl::SetFont(parent->GetFont());
  SetSize(x, y, width, height);

  return TRUE;
}

wxSize wxStaticText::DoGetBestSize() const
{
    wxString text(wxGetWindowText(GetHWND()));

    int widthTextMax = 0, widthLine,
        heightTextTotal = 0, heightLineDefault = 0, heightLine = 0;

    wxString curLine;
    for ( const wxChar *pc = text; ; pc++ ) {
        if ( *pc == wxT('\n') || *pc == wxT('\0') ) {
            if ( !curLine ) {
                // we can't use GetTextExtent - it will return 0 for both width
                // and height and an empty line should count in height
                // calculation
                if ( !heightLineDefault )
                    heightLineDefault = heightLine;
                if ( !heightLineDefault )
                    GetTextExtent(_T("W"), NULL, &heightLineDefault);

                heightTextTotal += heightLineDefault;
            }
            else {
                GetTextExtent(curLine, &widthLine, &heightLine);
                if ( widthLine > widthTextMax )
                    widthTextMax = widthLine;
                heightTextTotal += heightLine;
            }

            if ( *pc == wxT('\n') ) {
               curLine.Empty();
            }
            else {
               // the end of string
               break;
            }
        }
        else {
            curLine += *pc;
        }
    }

    return wxSize(widthTextMax, heightTextTotal);
}

void wxStaticText::SetLabel(const wxString& label)
{
    SetWindowText(GetHwnd(), label);

    // adjust the size of the window to fit to the label unless autoresizing is
    // disabled
    if ( !(GetWindowStyle() & wxST_NO_AUTORESIZE) )
    {
        DoSetSize(-1, -1, -1, -1, wxSIZE_AUTO_WIDTH | wxSIZE_AUTO_HEIGHT);
    }
}


bool wxStaticText::SetFont(const wxFont& font)
{
    bool ret = wxControl::SetFont(font);

    // adjust the size of the window to fit to the label unless autoresizing is
    // disabled
    if ( !(GetWindowStyle() & wxST_NO_AUTORESIZE) )
    {
        DoSetSize(-1, -1, -1, -1, wxSIZE_AUTO_WIDTH | wxSIZE_AUTO_HEIGHT);
    }

    return ret;
}


long wxStaticText::MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam)
{
  // Ensure that static items get messages. Some controls don't like this
  // message to be intercepted (e.g. RichEdit), hence the tests.
  if (nMsg == WM_NCHITTEST)
    return (long)HTCLIENT;

  return wxWindow::MSWWindowProc(nMsg, wParam, lParam);
}


