/////////////////////////////////////////////////////////////////////////////
// Name:        choice.cpp
// Purpose:     wxChoice
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "choice.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/choice.h"
#include "wx/utils.h"
#endif

#include "wx/msw/private.h"

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxChoice, wxControl)
#endif

bool wxChoice::MSWCommand(WXUINT param, WXWORD WXUNUSED(id))
{
  if (param == CBN_SELCHANGE)
  {
    wxCommandEvent event(wxEVT_COMMAND_CHOICE_SELECTED, m_windowId);
    event.SetInt(GetSelection());
    event.SetEventObject(this);
    event.SetString(GetStringSelection());
    ProcessCommand(event);

    return TRUE;
  }
  else
      return FALSE;
}

bool wxChoice::Create(wxWindow *parent, wxWindowID id,
           const wxPoint& pos,
           const wxSize& size,
       int n, const wxString choices[],
       long style,
           const wxValidator& validator,
           const wxString& name)
{
  SetName(name);
  SetValidator(validator);
  if (parent) parent->AddChild(this);
  SetBackgroundColour(parent->GetBackgroundColour()) ;
  SetForegroundColour(parent->GetForegroundColour()) ;
  m_noStrings = 0;

  m_windowStyle = style;

  if ( id == -1 )
    m_windowId = (int)NewControlId();
  else
  m_windowId = id;

  int x = pos.x;
  int y = pos.y;
  int width = size.x;
  int height = size.y;

  long msStyle = WS_CHILD | CBS_DROPDOWNLIST | WS_HSCROLL | WS_VSCROLL
                   | WS_TABSTOP | WS_VISIBLE;
  if (m_windowStyle & wxCB_SORT)
    msStyle |= CBS_SORT;

  bool want3D;
  WXDWORD exStyle = Determine3DEffects(WS_EX_CLIENTEDGE, &want3D) ;

  // Even with extended styles, need to combine with WS_BORDER
  // for them to look right.
  if ( want3D || wxStyleHasBorder(m_windowStyle) )
    msStyle |= WS_BORDER;

  m_hWnd = (WXHWND)::CreateWindowEx(exStyle, _T("COMBOBOX"), NULL,
                   msStyle,
                   0, 0, 0, 0, (HWND) parent->GetHWND(), (HMENU)m_windowId,
                   wxGetInstance(), NULL);

  wxCHECK_MSG( m_hWnd, FALSE, _T("Failed to create combobox") );

/*
#if wxUSE_CTL3D
  if (want3D)
  {
    m_useCtl3D = TRUE;
    Ctl3dSubclassCtl(wx_combo); // Does CTL3D affect the combobox? I think not.
  }
#endif
*/

  // Subclass again for purposes of dialog editing mode
  SubclassWin(m_hWnd);

  SetFont(parent->GetFont());

  int i;
  for (i = 0; i < n; i++)
  {
    Append(choices[i]);
  }
  SetSelection(n);

  SetSize(x, y, width, height);

  return TRUE;
}

void wxChoice::Append(const wxString& item)
{
  SendMessage(GetHwnd(), CB_ADDSTRING, 0, (LONG)(const wxChar *)item);

  m_noStrings ++;
}

void wxChoice::Delete(int n)
{
  m_noStrings = (int)SendMessage(GetHwnd(), CB_DELETESTRING, n, 0);
}

void wxChoice::Clear(void)
{
  SendMessage(GetHwnd(), CB_RESETCONTENT, 0, 0);

  m_noStrings = 0;
}


int wxChoice::GetSelection(void) const
{
  return (int)SendMessage(GetHwnd(), CB_GETCURSEL, 0, 0);
}

void wxChoice::SetSelection(int n)
{
  SendMessage(GetHwnd(), CB_SETCURSEL, n, 0);
}

int wxChoice::FindString(const wxString& s) const
{
#if defined(__WATCOMC__) && defined(__WIN386__)
  // For some reason, Watcom in WIN386 mode crashes in the CB_FINDSTRINGEXACT message.
  // Do it the long way instead.
  char buf[512];
  for (int i = 0; i < Number(); i++)
  {
    int len = (int)SendMessage(GetHwnd(), CB_GETLBTEXT, i, (LPARAM)(LPSTR)buf);
    buf[len] = 0;
    if (strcmp(buf, (const char *)s) == 0)
      return i;
  }
  return -1;
#else
 int pos = (int)SendMessage(GetHwnd(), CB_FINDSTRINGEXACT, (WPARAM)-1, (LPARAM)(LPSTR)(const wxChar *)s);
 if (pos == LB_ERR)
   return -1;
 else
   return pos;
#endif
}

wxString wxChoice::GetString(int n) const
{
    size_t len = (size_t)::SendMessage(GetHwnd(), CB_GETLBTEXTLEN, n, 0);
    wxString str;
    if ( ::SendMessage(GetHwnd(), CB_GETLBTEXT, n,
                       (LPARAM)str.GetWriteBuf(len)) == CB_ERR )
    {
        wxLogLastError("SendMessage(CB_GETLBTEXT)");
    }

    str.UngetWriteBuf();

    return str;
}

void wxChoice::DoSetSize(int x, int y,
                         int width, int height,
                         int sizeFlags)
{
    // Ignore height parameter because height doesn't mean 'initially
    // displayed' height, it refers to the drop-down menu as well. The
    // wxWindows interpretation is different; also, getting the size returns
    // the _displayed_ size (NOT the drop down menu size) so
    // setting-getting-setting size would not work.
    wxControl::DoSetSize(x, y, width, -1, sizeFlags);
}

wxSize wxChoice::DoGetBestSize()
{
    // find the widest string
    int wLine;
    int wChoice = 0;
    for ( int i = 0; i < m_noStrings; i++ )
    {
        wxString str(GetString(i));
        GetTextExtent(str, &wLine, NULL);
        if ( wLine > wChoice )
            wChoice = wLine;
    }

    // give it some reasonable default value if there are no strings in the
    // list
    if ( wChoice == 0 )
        wChoice = 100;

    // the combobox should be larger than the widest string
    int cx, cy;
    wxGetCharSize(GetHWND(), &cx, &cy, &GetFont());

    wChoice += 5*cx;

    // Choice drop-down list depends on number of items (limited to 10)
    size_t nStrings = m_noStrings == 0 ? 10 : wxMin(10, m_noStrings) + 1;
    int hChoice = EDIT_HEIGHT_FROM_CHAR_HEIGHT(cy)*nStrings;

    return wxSize(wChoice, hChoice);
}

WXHBRUSH wxChoice::OnCtlColor(WXHDC pDC, WXHWND pWnd, WXUINT nCtlColor,
      WXUINT message, WXWPARAM wParam, WXLPARAM lParam)
{
  return 0;
}

long wxChoice::MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam)
{
    switch (nMsg)
    {
/*
      case WM_GETDLGCODE:
      {
        if (GetWindowStyleFlag() & wxPROCESS_ENTER)
          return DLGC_WANTALLKEYS;
        break;
      }
*/
/*
      case WM_CHAR: // Always an ASCII character
      {
        if (wParam == VK_RETURN)
        {
          wxCommandEvent event(wxEVENT_TYPE_TEXT_ENTER_COMMAND);
          event.commandString = ((wxTextCtrl *)item)->GetValue();
          event.eventObject = item;
          item->ProcessCommand(event);
          return FALSE;
        }
        break;
      }
*/
      case WM_LBUTTONUP:
      {
        int x = (int)LOWORD(lParam);
        int y = (int)HIWORD(lParam);

       // Ok, this is truly weird, but if a panel with a wxChoice loses the
       // focus, then you get a *fake* WM_LBUTTONUP message
       // with x = 65535 and y = 65535.
       // Filter out this nonsense.
       if (x == 65535 && y == 65535)
         return 0;
       break;
      }
    }

  return wxWindow::MSWWindowProc(nMsg, wParam, lParam);
}

wxString wxChoice::GetStringSelection (void) const
{
  int sel = GetSelection ();
  if (sel > -1)
    return wxString(this->GetString (sel));
  else
    return wxString(_T(""));
}

bool wxChoice::SetStringSelection (const wxString& s)
{
  int sel = FindString (s);
  if (sel > -1)
    {
      SetSelection (sel);
      return TRUE;
    }
  else
    return FALSE;
}

void wxChoice::Command(wxCommandEvent & event)
{
  SetSelection (event.GetInt());
  ProcessCommand (event);
}



