/////////////////////////////////////////////////////////////////////////////
// Name:        checkbox.cpp
// Purpose:     wxCheckBox
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "checkbox.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/checkbox.h"
#endif

#include "wx/msw/private.h"

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxCheckBox, wxControl)
IMPLEMENT_DYNAMIC_CLASS(wxBitmapCheckBox, wxCheckBox)
#endif

bool wxCheckBox::MSWCommand(WXUINT WXUNUSED(param), WXWORD WXUNUSED(id))
{
  wxCommandEvent event(wxEVT_COMMAND_CHECKBOX_CLICKED, m_windowId);
  event.SetInt(GetValue());
  event.SetEventObject(this);
  ProcessCommand(event);
  return TRUE;
}

// Single check box item
bool wxCheckBox::Create(wxWindow *parent, wxWindowID id, const wxString& label,
           const wxPoint& pos,
           const wxSize& size, long style,
           const wxValidator& validator,
           const wxString& name)
{
  SetName(name);
  SetValidator(validator);
  if (parent) parent->AddChild(this);

  SetBackgroundColour(parent->GetBackgroundColour()) ;
  SetForegroundColour(parent->GetForegroundColour()) ;

  m_windowStyle = style;

  wxString Label = label;
  if (Label == "")
    Label = " "; // Apparently needed or checkbox won't show

	if ( id == -1 )
  		m_windowId = NewControlId();
	else
		m_windowId = id;

  int x = pos.x;
  int y = pos.y;
  int width = size.x;
  int height = size.y;

  long msStyle = BS_AUTOCHECKBOX | WS_TABSTOP | WS_CHILD | WS_VISIBLE;
  if ( style & wxALIGN_RIGHT )
    msStyle |= BS_LEFTTEXT;

  // We perhaps have different concepts of 3D here - a 3D border,
  // versus a 3D button.
  // So we only wish to give a border if this is specified
  // in the style.
  bool want3D;
  WXDWORD exStyle = Determine3DEffects(0, &want3D) ;

  // Even with extended styles, need to combine with WS_BORDER
  // for them to look right.
/*
  if ( want3D || wxStyleHasBorder(m_windowStyle) )
    msStyle |= WS_BORDER;
*/

  m_hWnd = (WXHWND)CreateWindowEx(exStyle, "BUTTON", Label,
                                  msStyle,
                                  0, 0, 0, 0,
                                  (HWND)parent->GetHWND(), (HMENU)m_windowId,
                                  wxGetInstance(), NULL);

#if CTL3D
  if (want3D)
  {
    Ctl3dSubclassCtl((HWND)m_hWnd);
    m_useCtl3D = TRUE;
  }
#endif

  // Subclass again for purposes of dialog editing mode
  SubclassWin(m_hWnd);
  
  SetFont(parent->GetFont());

  SetSize(x, y, width, height);

  return TRUE;
}

void wxCheckBox::SetLabel(const wxString& label)
{
  SetWindowText((HWND)GetHWND(), label);
}

void wxCheckBox::SetSize(int x, int y, int width, int height, int sizeFlags)
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

  int current_width, cyf;
  HWND button = (HWND) GetHWND();

  int nLen = GetWindowTextLength(button);
  wxString str;
  GetWindowText(button, str.GetWriteBuf(nLen), nLen);
  str.UngetWriteBuf();

  if ( !str.IsEmpty() )
  {
    GetTextExtent(str, &current_width, &cyf, NULL, NULL, & GetFont());
    if (w1 < 0)
      w1 = (int)(current_width + RADIO_SIZE);
    if (h1 < 0)
    {
      h1 = (int)(cyf);
      if (h1 < RADIO_SIZE)
        h1 = RADIO_SIZE;
    }
  }
  else
  {
    if (w1 < 0)
      w1 = RADIO_SIZE;
    if (h1 < 0)
      h1 = RADIO_SIZE;
  }

  MoveWindow(button, x1, y1, w1, h1, TRUE);
}

void wxCheckBox::SetValue(bool val)
{
  SendMessage((HWND) GetHWND(), BM_SETCHECK, val, 0);
}

bool wxCheckBox::GetValue(void) const
{
#ifdef __WIN32__
  return (SendMessage((HWND) GetHWND(), BM_GETCHECK, 0, 0) == BST_CHECKED);
#else
  return ((0x003 & SendMessage((HWND) GetHWND(), BM_GETCHECK, 0, 0)) == 0x003);
#endif
}

WXHBRUSH wxCheckBox::OnCtlColor(WXHDC pDC, WXHWND pWnd, WXUINT nCtlColor,
			WXUINT message, WXWPARAM wParam, WXLPARAM lParam)
{
#if CTL3D
  if ( m_useCtl3D )
  {
    HBRUSH hbrush = Ctl3dCtlColorEx(message, wParam, lParam);
      
      return (WXHBRUSH) hbrush;
  }
#endif

  if (GetParent()->GetTransparentBackground())
    SetBkMode((HDC) pDC, TRANSPARENT);
  else
    SetBkMode((HDC) pDC, OPAQUE);

  ::SetBkColor((HDC) pDC, RGB(GetBackgroundColour().Red(), GetBackgroundColour().Green(), GetBackgroundColour().Blue()));
  ::SetTextColor((HDC) pDC, RGB(GetForegroundColour().Red(), GetForegroundColour().Green(), GetForegroundColour().Blue()));

  wxBrush *backgroundBrush = wxTheBrushList->FindOrCreateBrush(GetBackgroundColour(), wxSOLID);

  // Note that this will be cleaned up in wxApp::OnIdle, if backgroundBrush
  // has a zero usage count.
//  backgroundBrush->RealizeResource();
  return (WXHBRUSH) backgroundBrush->GetResourceHandle();
}

void wxCheckBox::Command (wxCommandEvent & event)
{
  SetValue ((event.GetInt() != 0));
  ProcessCommand (event);
}

bool wxBitmapCheckBox::Create(wxWindow *parent, wxWindowID id, const wxBitmap *label,
           const wxPoint& pos,
           const wxSize& size, long style,
           const wxValidator& validator,
           const wxString& name)
{
  SetName(name);
  SetValidator(validator);
  if (parent) parent->AddChild(this);

  SetBackgroundColour(parent->GetBackgroundColour()) ;
  SetForegroundColour(parent->GetForegroundColour()) ;
  m_windowStyle = style;

	if ( id == -1 )
  		m_windowId = NewControlId();
	else
		m_windowId = id;

  int x = pos.x;
  int y = pos.y;
  int width = size.x;
  int height = size.y;

  checkWidth = -1 ;
  checkHeight = -1 ;
  long msStyle = CHECK_FLAGS;

  HWND wx_button = CreateWindowEx(MakeExtendedStyle(m_windowStyle), CHECK_CLASS, "toggle",
                    msStyle,
                    0, 0, 0, 0, (HWND) parent->GetHWND(), (HMENU)m_windowId,
                    wxGetInstance(), NULL);

#if CTL3D
  if (!(GetParent()->GetWindowStyleFlag() & wxUSER_COLOURS))
  {
    Ctl3dSubclassCtl(wx_button);
  	  m_useCtl3D = TRUE;
  }
#endif

  m_hWnd = (WXHWND)wx_button;

  // Subclass again for purposes of dialog editing mode
  SubclassWin((WXHWND)wx_button);

//  SetFont(parent->GetFont());

  SetSize(x, y, width, height);

  ShowWindow(wx_button, SW_SHOW);
  return TRUE;
}

void wxBitmapCheckBox::SetLabel(const wxBitmap *bitmap)
{
}

void wxBitmapCheckBox::SetSize(int x, int y, int width, int height, int sizeFlags)
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

  HWND button = (HWND) GetHWND();
/*
    if (w1<0)
      w1 = checkWidth + FB_MARGIN ;
    if (h1<0)
      h1 = checkHeight + FB_MARGIN ;
*/
  MoveWindow(button, x1, y1, w1, h1, TRUE);
}

void wxBitmapCheckBox::SetValue(bool val)
{
  SendMessage((HWND) GetHWND(), BM_SETCHECK, val, 0);
}

bool wxBitmapCheckBox::GetValue(void) const
{
  return ((0x003 & SendMessage((HWND) GetHWND(), BM_GETCHECK, 0, 0)) == 0x003);
}


