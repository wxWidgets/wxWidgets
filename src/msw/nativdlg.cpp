/////////////////////////////////////////////////////////////////////////////
// Name:        nativdlg.cpp
// Purpose:     Native dialog loading code (part of wxWindow)
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <stdio.h>
#include "wx/wx.h"
#endif

#include "wx/spinbutt.h"
#include "wx/msw/private.h"

extern wxWindow *wxWndHook;
extern LONG APIENTRY _EXPORT wxDlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

bool wxWindow::LoadNativeDialog(wxWindow* parent, wxWindowID& id)
{
	m_windowId = id;
    wxWndHook = this;
	m_hWnd = (WXHWND) ::CreateDialog((HINSTANCE) wxGetInstance(), MAKEINTRESOURCE(id),
		(HWND) (parent ? parent->GetHWND() : (WXHWND) NULL), (DLGPROC) wxDlgProc);
    wxWndHook = NULL;

	if ( !m_hWnd )
		return FALSE;

  	SubclassWin(GetHWND());

    if (!parent)
      wxTopLevelWindows.Append(this);

	if ( parent )
		parent->AddChild(this);

	// Enumerate all children
    HWND hWndNext;
    hWndNext = ::GetWindow((HWND) m_hWnd, GW_CHILD);

	wxWindow* child = NULL;
	if (hWndNext)
	  child = CreateWindowFromHWND(this, (WXHWND) hWndNext);

    while (hWndNext != NULL)
    {
       hWndNext = ::GetWindow(hWndNext, GW_HWNDNEXT);
	   if (hWndNext)
	     child = CreateWindowFromHWND(this, (WXHWND) hWndNext);
    }

	return TRUE;
}

bool wxWindow::LoadNativeDialog(wxWindow* parent, const wxString& name)
{
	SetName(name);

    wxWndHook = this;
	m_hWnd = (WXHWND) ::CreateDialog((HINSTANCE) wxGetInstance(), (const char *) name,
		(HWND) (parent ? parent->GetHWND() : (WXHWND) NULL), (DLGPROC) wxDlgProc);
    wxWndHook = NULL;

	if ( !m_hWnd )
		return FALSE;

  	SubclassWin(GetHWND());

    if (!parent)
      wxTopLevelWindows.Append(this);

	if ( parent )
		parent->AddChild(this);

	return TRUE;
}

wxWindow* wxWindow::GetWindowChild1(wxWindowID& id)
{
	if ( m_windowId == id )
		return this;

	wxNode *node = GetChildren()->First();
	while ( node )
	{
		wxWindow* child = (wxWindow*) node->Data();
		wxWindow* win = child->GetWindowChild1(id);
		if ( win )
			return win;
		node = node->Next();
	}

	return NULL;
}

wxWindow* wxWindow::GetWindowChild(wxWindowID& id)
{
	wxWindow* win = GetWindowChild1(id);
	if ( !win )
	{
		HWND hWnd =	::GetDlgItem((HWND) GetHWND(), id);

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


wxWindow* wxWindow::CreateWindowFromHWND(wxWindow* parent, WXHWND hWnd)
{
	char buf[256];

#ifndef __WIN32__
	GetClassName((HWND) hWnd, buf, 256);
#else
#ifdef UNICODE
	GetClassNameW((HWND) hWnd, buf, 256);
#else
	GetClassNameA((HWND) hWnd, buf, 256);
#endif
#endif

	wxString str(buf);
	str.UpperCase();

#ifndef __WIN32__
	long id = (long) GetWindowWord((HWND) hWnd, GWW_ID);
#else
	long id = GetWindowLong((HWND) hWnd, GWL_ID);
#endif

	long style = GetWindowLong((HWND) hWnd, GWL_STYLE);

	wxWindow* win = NULL;

	if (str == "BUTTON")
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
#ifdef __WIN32__
		else if (style & BS_BITMAP)
		{
			// TODO: how to find the bitmap?
			win = new wxBitmapButton;
			wxMessageBox("Have not yet implemented bitmap button as BS_BITMAP button.");
		}
#endif
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
			char buf[256];
			sprintf(buf, "Don't know what kind of button this is: id = %d", (int) id);
			wxMessageBox(buf);
		}
	}
	else if (str == "COMBOBOX")
	{
		win = new wxComboBox;
	}
	// TODO: Problem if the user creates a multiline - but not rich text - text control,
	// since wxWin assumes RichEdit control for this. Should have m_isRichText in
	// wxTextCtrl. Also, convert as much of the window style as is necessary
	// for correct functioning.
	// Could have wxWindow::AdoptAttributesFromHWND(WXHWND)
	// to be overridden by each control class.
	else if (str == "EDIT")
	{
		win = new wxTextCtrl;
	}
	else if (str == "LISTBOX")
	{
		win = new wxListBox;
	}
	else if (str == "SCROLLBAR")
	{
		win = new wxScrollBar;
	}
#if defined(__WIN95__)
	else if (str == "MSCTLS_UPDOWN32")
	{
		win == new wxSpinButton;
	}
#endif
	else if (str == "MSCTLS_TRACKBAR32")
	{
		// Need to ascertain if it's horiz or vert
		win = new wxSlider;
	}
	else if (str == "STATIC")
	{
		int style1 = (style & 0xFF);

		if ((style1 == SS_LEFT) || (style1 == SS_RIGHT) || (style1 == SS_SIMPLE))
			win = new wxStaticText;
#ifdef __WIN32__
		else if (style1 == SS_BITMAP)
		{
			win = new wxStaticBitmap;

			// Help! this doesn't correspond with the wxWin implementation.
			wxMessageBox("Please make SS_BITMAP statics into owner-draw buttons.");
		}
#endif
	}
	else
	{
		wxString msg("Don't know how to convert from Windows class ");
		msg += str;
		wxMessageBox(msg);
	}

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
	long style = GetWindowLong((HWND) hWnd, GWL_STYLE);

	if (style & WS_VSCROLL)
		m_windowStyle |= wxVSCROLL;
	if (style & WS_HSCROLL)
		m_windowStyle |= wxHSCROLL;
}


