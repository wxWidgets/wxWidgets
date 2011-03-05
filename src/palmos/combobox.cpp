/////////////////////////////////////////////////////////////////////////////
// Name:        src/palmos/combobox.cpp
// Purpose:     wxComboBox class
// Author:      William Osborne - minimal working wxPalmOS port
// Modified by:
// Created:     10/13/04
// RCS-ID:      $Id$
// Copyright:   (c) William Osborne
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_COMBOBOX

#include "wx/combobox.h"

#ifndef WX_PRECOMP
    #include "wx/settings.h"
    #include "wx/log.h"
    // for wxEVT_COMMAND_TEXT_ENTER
    #include "wx/textctrl.h"
    #include "wx/brush.h"
#endif

#include "wx/clipbrd.h"
#include "wx/palmos/private.h"

#if wxUSE_TOOLTIPS
    #include "wx/tooltip.h"
#endif // wxUSE_TOOLTIPS

// ----------------------------------------------------------------------------
// wxWin macros
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// function prototypes
// ----------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// global vars
// ---------------------------------------------------------------------------

// the pointer to standard radio button wnd proc
static WNDPROC gs_wndprocEdit = (WNDPROC)NULL;

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxComboBox callbacks
// ----------------------------------------------------------------------------

WXLRESULT wxComboBox::MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam)
{
    return 0;
}

bool wxComboBox::MSWProcessEditMsg(WXUINT msg, WXWPARAM wParam, WXLPARAM lParam)
{
    return false;
}

bool wxComboBox::MSWCommand(WXUINT param, WXWORD WXUNUSED(id))
{
    return false;
}

WXHWND wxComboBox::GetEditHWND() const
{
    return (WXHWND)0;
}

// ----------------------------------------------------------------------------
// wxComboBox creation
// ----------------------------------------------------------------------------

bool wxComboBox::Create(wxWindow *parent, wxWindowID id,
                        const wxString& value,
                        const wxPoint& pos,
                        const wxSize& size,
                        int n, const wxString choices[],
                        long style,
                        const wxValidator& validator,
                        const wxString& name)
{
    return false;
}

bool wxComboBox::Create(wxWindow *parent, wxWindowID id,
                        const wxString& value,
                        const wxPoint& pos,
                        const wxSize& size,
                        const wxArrayString& choices,
                        long style,
                        const wxValidator& validator,
                        const wxString& name)
{
    return false;
}

WXDWORD wxComboBox::MSWGetStyle(long style, WXDWORD *exstyle) const
{
    return 0;
}

// ----------------------------------------------------------------------------
// wxComboBox text control-like methods
// ----------------------------------------------------------------------------

void wxComboBox::SetValue(const wxString& value)
{
}

// Clipboard operations
void wxComboBox::Copy()
{
}

void wxComboBox::Cut()
{
}

void wxComboBox::Paste()
{
}

void wxComboBox::SetEditable(bool WXUNUSED(editable))
{
}

void wxComboBox::SetInsertionPoint(long pos)
{
}

void wxComboBox::SetInsertionPointEnd()
{
}

long wxComboBox::GetInsertionPoint() const
{
    return 0;
}

wxTextPos wxComboBox::GetLastPosition() const
{
    return 0;
}

void wxComboBox::Replace(long from, long to, const wxString& value)
{
}

void wxComboBox::Remove(long from, long to)
{
}

void wxComboBox::SetSelection(long from, long to)
{
}

bool wxComboBox::IsEditable() const
{
    return false;
}

void wxComboBox::Undo()
{
}

void wxComboBox::Redo()
{
}

void wxComboBox::SelectAll()
{
}

bool wxComboBox::CanCopy() const
{
    return false;
}

bool wxComboBox::CanCut() const
{
    return false;
}

bool wxComboBox::CanPaste() const
{
    return false;
}

bool wxComboBox::CanUndo() const
{
    return false;
}

bool wxComboBox::CanRedo() const
{
    return false;
}


#endif // wxUSE_COMBOBOX
