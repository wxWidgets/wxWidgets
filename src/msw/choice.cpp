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
    event.SetString(copystring(GetStringSelection()));
    ProcessCommand(event);
    delete[] event.GetString();
    return TRUE;
  }
  else return FALSE;
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
  m_noStrings = n;

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

  m_hWnd = (WXHWND)::CreateWindowEx(exStyle, "COMBOBOX", NULL,
                   msStyle,
                   0, 0, 0, 0, (HWND) parent->GetHWND(), (HMENU)m_windowId,
                   wxGetInstance(), NULL);

  wxCHECK_MSG( m_hWnd, FALSE, "Failed to create combobox" );

/*
#if CTL3D
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
  SendMessage((HWND) GetHWND(), CB_ADDSTRING, 0, (LONG)(const char *)item);

  m_noStrings ++;
}

void wxChoice::Delete(int n)
{
  m_noStrings = (int)SendMessage((HWND) GetHWND(), CB_DELETESTRING, n, 0);
}

void wxChoice::Clear(void)
{
  SendMessage((HWND) GetHWND(), CB_RESETCONTENT, 0, 0);

  m_noStrings = 0;
}


int wxChoice::GetSelection(void) const
{
  return (int)SendMessage((HWND) GetHWND(), CB_GETCURSEL, 0, 0);
}

void wxChoice::SetSelection(int n)
{
  SendMessage((HWND) GetHWND(), CB_SETCURSEL, n, 0);
}

int wxChoice::FindString(const wxString& s) const
{
#if defined(__WATCOMC__) && defined(__WIN386__)
  // For some reason, Watcom in WIN386 mode crashes in the CB_FINDSTRINGEXACT message.
  // Do it the long way instead.
  char buf[512];
  for (int i = 0; i < Number(); i++)
  {
    int len = (int)SendMessage((HWND) GetHWND(), CB_GETLBTEXT, i, (LPARAM)(LPSTR)buf);
    buf[len] = 0;
    if (strcmp(buf, (const char *)s) == 0)
      return i;
  }
  return -1;
#else
 int pos = (int)SendMessage((HWND) GetHWND(), CB_FINDSTRINGEXACT, (WPARAM)-1, (LPARAM)(LPSTR)(const char *)s);
 if (pos == LB_ERR)
   return -1;
 else
   return pos;
#endif
}

wxString wxChoice::GetString(int n) const
{
  int len = (int)SendMessage((HWND) GetHWND(), CB_GETLBTEXT, n, (long)wxBuffer);
  wxBuffer[len] = 0;
  return wxString(wxBuffer);
}

void wxChoice::SetSize(int x, int y, int width, int height, int sizeFlags)
{
  int currentX, currentY;
  GetPosition(&currentX, &currentY);

  int x1 = x;
  int y1 = y;
  int w1 = width;
  int h1 = height;

  if (x == -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
    x1 = currentX;
  if (y == -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
    y1 = currentY;

  AdjustForParentClientOrigin(x1, y1, sizeFlags);

  // If we're prepared to use the existing size, then...
  if (width == -1 && height == -1 && ((sizeFlags & wxSIZE_AUTO) != wxSIZE_AUTO))
  {
    GetSize(&w1, &h1);
  }

  int cx; // button font dimensions
  int cy;
  wxGetCharSize(GetHWND(), &cx, &cy, & this->GetFont());

  int control_width, control_height;

  // Ignore height parameter because height doesn't
  // mean 'initially displayed' height, it refers to the
  // drop-down menu as well. The wxWindows interpretation
  // is different; also, getting the size returns the
  // _displayed_ size (NOT the drop down menu size)
  // so setting-getting-setting size would not work.
  h1 = -1;

  // Deal with default size (using -1 values)
  if (width <= 0)
  {
    // Find the longest string
    if (m_noStrings == 0)
    {
      control_width = 100;
    }
    else
    {
      int len, ht;
      int longest = 0;
      int i;
      for (i = 0; i < m_noStrings; i++)
      {
        wxString str(GetString(i));
        GetTextExtent(str, &len, &ht, NULL, NULL, & this->GetFont());
        if ( len > longest)
            longest = len;
      }

      control_width = longest + cx*5;
    }
  }
  else
  {
    // If non-default width...
    control_width = w1;
  }


  // Choice drop-down list depends on number of items (limited to 10)
  if (h1 <= 0)
  {
    if (m_noStrings == 0)
        h1 = EDIT_HEIGHT_FROM_CHAR_HEIGHT(cy)*10;
    else
        h1 = EDIT_HEIGHT_FROM_CHAR_HEIGHT(cy)*(wxMin(10, m_noStrings) + 1);
  }

  control_height = h1;

  // Calculations may have made text size too small
  if (control_height <= 0)
    control_height = EDIT_HEIGHT_FROM_CHAR_HEIGHT(cy);

  if (control_width <= 0)
    control_width = 100;

  MoveWindow((HWND)GetHWND(), x1, y1,
             control_width, control_height, TRUE);
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
         return Default();
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
    return wxString("");
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



