/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/tglbtn.cpp
// Purpose:     Definition of the wxToggleButton class, which implements a
//              toggle button under wxMSW.
// Author:      John Norris, minor changes by Axel Schlueter
// Modified by:
// Created:     08.02.01
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Johnny C. Norris II
// License:     Rocketeer license
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declatations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/tglbtn.h"

#if wxUSE_TOGGLEBTN

#ifndef WX_PRECOMP
    #include "wx/button.h"
    #include "wx/brush.h"
    #include "wx/dcscreen.h"
    #include "wx/settings.h"

    #include "wx/log.h"
#endif // WX_PRECOMP

#include "wx/msw/private.h"

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxToggleButton, wxControl)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_TOGGLEBUTTON_CLICKED)

#define BUTTON_HEIGHT_FROM_CHAR_HEIGHT(cy) (11*EDIT_HEIGHT_FROM_CHAR_HEIGHT(cy)/10)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxToggleButton
// ----------------------------------------------------------------------------

bool wxToggleButton::MSWCommand(WXUINT WXUNUSED(param), WXWORD WXUNUSED(id))
{
   wxCommandEvent event(wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, m_windowId);
   event.SetInt(GetValue());
   event.SetEventObject(this);
   ProcessCommand(event);
   return TRUE;
}

// Single check box item
bool wxToggleButton::Create(wxWindow *parent, wxWindowID id,
                            const wxString& label,
                            const wxPoint& pos,
                            const wxSize& size, long style,
                            const wxValidator& validator,
                            const wxString& name)
{
   if (!CreateBase(parent, id, pos, size, style, validator, name))
      return FALSE;

   parent->AddChild(this);

   m_backgroundColour = parent->GetBackgroundColour();
   m_foregroundColour = parent->GetForegroundColour();

   long msStyle = BS_AUTOCHECKBOX | BS_PUSHLIKE | WS_TABSTOP | WS_CHILD | WS_VISIBLE;
#ifdef __WIN32__
   if(m_windowStyle & wxBU_LEFT)
      msStyle |= BS_LEFT;
   if(m_windowStyle & wxBU_RIGHT)
      msStyle |= BS_RIGHT;
   if(m_windowStyle & wxBU_TOP)
      msStyle |= BS_TOP;
   if(m_windowStyle & wxBU_BOTTOM)
      msStyle |= BS_BOTTOM;
#endif

   m_hWnd = (WXHWND)CreateWindowEx(MakeExtendedStyle(m_windowStyle),
                                   wxT("BUTTON"), label,
                                   msStyle, 0, 0, 0, 0,
                                   (HWND)parent->GetHWND(),
                                   (HMENU)m_windowId,
                                   wxGetInstance(), NULL);

   if ( m_hWnd == 0 )
   {
        wxLogError(_T("Failed to create a toggle button"));

        return FALSE;
    }

    // Subclass again for purposes of dialog editing mode
    SubclassWin(m_hWnd);

    SetFont(parent->GetFont());

    SetSize(pos.x, pos.y, size.x, size.y);

    return TRUE;
}

void wxToggleButton::SetLabel(const wxString& label)
{
    SetWindowText(GetHwnd(), label);
}

wxSize wxToggleButton::DoGetBestSize() const
{
   wxString label = wxGetWindowText(GetHWND());
   int wBtn;
   GetTextExtent(label, &wBtn, NULL);

   int wChar, hChar;
   wxGetCharSize(GetHWND(), &wChar, &hChar, &GetFont());

   // add a margin - the button is wider than just its label
   wBtn += 3*wChar;

   // the button height is proportional to the height of the font used
   int hBtn = BUTTON_HEIGHT_FROM_CHAR_HEIGHT(hChar);

   wxSize sz = wxButton::GetDefaultSize();
   if (wBtn > sz.x)
       sz.x = wBtn;
   if (hBtn > sz.y)
       sz.y = hBtn;

   return sz;
}

void wxToggleButton::SetValue(bool val)
{
   SendMessage(GetHwnd(), BM_SETCHECK, val, 0);
}

#ifndef BST_CHECKED
#define BST_CHECKED 0x0001
#endif

bool wxToggleButton::GetValue() const
{
#ifdef __WIN32__
   return (SendMessage(GetHwnd(), BM_GETCHECK, 0, 0) == BST_CHECKED);
#else
   return ((0x001 & SendMessage(GetHwnd(), BM_GETCHECK, 0, 0)) == 0x001);
#endif
}

void wxToggleButton::Command(wxCommandEvent & event)
{
   SetValue((event.GetInt() != 0));
   ProcessCommand(event);
}

#endif // wxUSE_TOGGLEBTN

