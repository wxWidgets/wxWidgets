/////////////////////////////////////////////////////////////////////////////
// Name:        nativdlg.cpp
// Purpose:     Native dialog loading code (part of wxWindow)
// Author:      David Webster
// Modified by:
// Created:     10/12/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// ===========================================================================
// declarations
// ===========================================================================

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include <stdio.h>
    #include "wx/wx.h"
#endif

#include "wx/os2/private.h"
#include "wx/spinbutt.h"

// ---------------------------------------------------------------------------
// global functions
// ---------------------------------------------------------------------------

extern wxWindow* wxWndHook;
extern MRESULT wxDlgProc(HWND hWnd, UINT message,
                         MPARAM wParam, MPARAM lParam);

// ===========================================================================
// implementation
// ===========================================================================

bool wxWindow::LoadNativeDialog(wxWindow* parent, wxWindowID& id)
{
    m_windowId = id;
    wxWndHook = this;

    m_hWnd = 0; // TODO (WXHWND)::CreateDialog((HINSTANCE)wxGetInstance(),
                //                     MAKEINTRESOURCE(id),
                //                     parent ? (HWND)parent->GetHWND() : 0,
                //                     (DLGPROC) wxDlgProc);
    wxWndHook = NULL;

    if ( !m_hWnd )
        return FALSE;

    SubclassWin(GetHWND());

    if ( parent )
        parent->AddChild(this);
    else
        wxTopLevelWindows.Append(this);

    // Enumerate all children
    HWND hWndNext;
// TODO    hWndNext = ::GetWindow((HWND) m_hWnd, GW_CHILD);

    wxWindow* child = NULL;
    if (hWndNext)
        child = CreateWindowFromHWND(this, (WXHWND) hWndNext);

    while (hWndNext != (HWND) NULL)
    {
// TODO:        hWndNext = ::GetWindow(hWndNext, GW_HWNDNEXT);
        if (hWndNext)
            child = CreateWindowFromHWND(this, (WXHWND) hWndNext);
    }

    return TRUE;
}

bool wxWindow::LoadNativeDialog(wxWindow* parent, const wxString& name)
{
    SetName(name);

    wxWndHook = this;
    m_hWnd = 0; //TODO: (WXHWND)::CreateDialog((HINSTANCE) wxGetInstance(),
                //                    name.c_str(),
                //                    parent ? (HWND)parent->GetHWND() : 0,
                //                    (DLGPROC)wxDlgProc);
    wxWndHook = NULL;

    if ( !m_hWnd )
        return FALSE;

    SubclassWin(GetHWND());

    if ( parent )
        parent->AddChild(this);
    else
        wxTopLevelWindows.Append(this);

    // FIXME why don't we enum all children here?

    return TRUE;
}

// ---------------------------------------------------------------------------
// look for child by id
// ---------------------------------------------------------------------------

wxWindow* wxWindow::GetWindowChild1(wxWindowID id)
{
    if ( m_windowId == id )
        return this;

    wxWindowList::Node *node = GetChildren().GetFirst();
    while ( node )
    {
        wxWindow* child = node->GetData();
        wxWindow* win = child->GetWindowChild1(id);
        if ( win )
            return win;

        node = node->GetNext();
    }

    return NULL;
}

wxWindow* wxWindow::GetWindowChild(wxWindowID id)
{
    wxWindow* win = GetWindowChild1(id);
    if ( !win )
    {
        HWND hWnd = 0; // TODO: ::GetDlgItem((HWND) GetHWND(), id);

        if (hWnd)
        {
            wxWindow* child = CreateWindowFromHWND(this, (WXHWND) hWnd);
            if (child)
            {
                child->AddChild(this);
                return child;
            }
        }
    }

    return NULL;
}

// ---------------------------------------------------------------------------
// create wxWin window from a native HWND
// ---------------------------------------------------------------------------

wxWindow* wxWindow::CreateWindowFromHWND(wxWindow* parent, WXHWND hWnd)
{
    wxString str(wxGetWindowClass(hWnd));
    str.UpperCase();

    long id = wxGetWindowId(hWnd);
    long style = 0; // TODO: GetWindowLong((HWND) hWnd, GWL_STYLE);

    wxWindow* win = NULL;

// TODO:
/*
    if (str == wxT("BUTTON"))
    {
        int style1 = (style & 0xFF);
        if ((style1 == BS_3STATE) || (style1 == BS_AUTO3STATE) || (style1 == BS_AUTOCHECKBOX) ||
            (style1 == BS_CHECKBOX))
        {
            win = new wxCheckBox;
        }
        else if ((style1 == BS_AUTORADIOBUTTON) || (style1 == BS_RADIOBUTTON))
        {
            win = new wxRadioButton;
        }
        else if (style & BS_BITMAP)
        {
            // TODO: how to find the bitmap?
            win = new wxBitmapButton;
            wxLogError(wxT("Have not yet implemented bitmap button as BS_BITMAP button."));
        }
        else if (style1 == BS_OWNERDRAW)
        {
            // TODO: how to find the bitmap?
            // TODO: can't distinguish between bitmap button and bitmap static.
            // Change implementation of wxStaticBitmap to SS_BITMAP.
            // PROBLEM: this assumes that we're using resource-based bitmaps.
            // So maybe need 2 implementations of bitmap buttons/static controls,
            // with a switch in the drawing code. Call default proc if BS_BITMAP.
            win = new wxBitmapButton;
        }
        else if ((style1 == BS_PUSHBUTTON) || (style1 == BS_DEFPUSHBUTTON))
        {
            win = new wxButton;
        }
        else if (style1 == BS_GROUPBOX)
        {
            win = new wxStaticBox;
        }
        else
        {
            wxLogError(wxT("Don't know what kind of button this is: id = %d"),
                       id);
        }
    }
    else if (str == wxT("COMBOBOX"))
    {
        win = new wxComboBox;
    }
    // TODO: Problem if the user creates a multiline - but not rich text - text control,
    // since wxWin assumes RichEdit control for this. Should have m_isRichText in
    // wxTextCtrl. Also, convert as much of the window style as is necessary
    // for correct functioning.
    // Could have wxWindow::AdoptAttributesFromHWND(WXHWND)
    // to be overridden by each control class.
    else if (str == wxT("EDIT"))
    {
        win = new wxTextCtrl;
    }
    else if (str == wxT("LISTBOX"))
    {
        win = new wxListBox;
    }
    else if (str == wxT("SCROLLBAR"))
    {
        win = new wxScrollBar;
    }
    else if (str == wxT("MSCTLS_UPDOWN32"))
    {
        win = new wxSpinButton;
    }
    else if (str == wxT("MSCTLS_TRACKBAR32"))
    {
        // Need to ascertain if it's horiz or vert
        win = new wxSlider;
    }
    else if (str == wxT("STATIC"))
    {
        int style1 = (style & 0xFF);

        if ((style1 == SS_LEFT) || (style1 == SS_RIGHT) || (style1 == SS_SIMPLE))
            win = new wxStaticText;
        else if (style1 == SS_BITMAP)
        {
            win = new wxStaticBitmap;

            // Help! this doesn't correspond with the wxWin implementation.
            wxLogError(wxT("Please make SS_BITMAP statics into owner-draw buttons."));
        }
    }
    else
    {
        wxString msg(wxT("Don't know how to convert from Windows class "));
        msg += str;
        wxLogError(msg);
    }
*/
    if (win)
    {
        parent->AddChild(win);
        win->SetEventHandler(win);
        win->SetHWND(hWnd);
        win->SetId(id);
        win->SubclassWin(hWnd);
        win->AdoptAttributesFromHWND();
        win->SetupColours();

        return win;
    }
    else
        return NULL;
}

// Make sure the window style (etc.) reflects the HWND style (roughly)
void wxWindow::AdoptAttributesFromHWND(void)
{
    HWND hWnd = (HWND) GetHWND();
// TODO:
/*
    long style = GetWindowLong((HWND) hWnd, GWL_STYLE);

    if (style & WS_VSCROLL)
        m_windowStyle |= wxVSCROLL;
    if (style & WS_HSCROLL)
        m_windowStyle |= wxHSCROLL;
*/
}

