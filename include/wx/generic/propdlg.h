/////////////////////////////////////////////////////////////////////////////
// Name:        wx/generic/propdlg.h
// Purpose:     wxPropertySheetDialog
// Author:      Julian Smart
// Modified by:
// Created:     2005-03-12
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PROPDLG_H_
#define _WX_PROPDLG_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "propdlg.h"
#endif

#include "wx/defs.h"

#if wxUSE_BOOKCTRL

class WXDLLEXPORT wxBookCtrlBase;

//-----------------------------------------------------------------------------
// wxPropertySheetDialog
// A platform-independent properties dialog.
//
//   * on PocketPC, a flat-look 'property sheet' notebook will be used, with
//     no OK/Cancel/Help buttons
//   * on other platforms, a normal notebook will be used, with standard buttons
//
// To use this class, call Create from your derived class.
// Then create pages and add to the book control. Finally call CreateButtons and
// LayoutDialog.
//
// For example:
//
// MyPropertySheetDialog::Create(...)
// {
//     wxPropertySheetDialog::Create(...);
//
//     // Add page
//     wxPanel* panel = new wxPanel(GetBookCtrl(), ...);
//     GetBookCtrl()->AddPage(panel, wxT("General"));
//
//     CreateButtons();
//     LayoutDialog();
// }
//
// Override CreateBookCtrl and AddBookCtrl to create and add a different
// kind of book control.
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_ADV wxPropertySheetDialog : public wxDialog
{
public:
    wxPropertySheetDialog() : wxDialog() { Init(); }

    wxPropertySheetDialog(wxWindow* parent, wxWindowID id,
                       const wxString& title,
                       const wxPoint& pos = wxDefaultPosition,
                       const wxSize& sz = wxDefaultSize,
                       long style = wxDEFAULT_DIALOG_STYLE,
                       const wxString& name = wxDialogNameStr)
    {
        Init();
        Create(parent, id, title, pos, sz, style, name);
    }

    bool Create(wxWindow* parent, wxWindowID id,
                       const wxString& title,
                       const wxPoint& pos = wxDefaultPosition,
                       const wxSize& sz = wxDefaultSize,
                       long style = wxDEFAULT_DIALOG_STYLE,
                       const wxString& name = wxDialogNameStr);

//// Accessors

    // Set and get the notebook
    void SetBookCtrl(wxBookCtrlBase* book) { m_bookCtrl = book; }
    wxBookCtrlBase* GetBookCtrl() const { return m_bookCtrl; }

    // Set and get the inner sizer
    void SetInnerSize(wxSizer* sizer) { m_innerSizer = sizer; }
    wxSizer* GetInnerSizer() const { return m_innerSizer ; }

/// Operations

    // Creates the buttons (none on PocketPC)
    virtual void CreateButtons(int flags = wxOK|wxCANCEL);

    // Lay out the dialog, to be called after pages have been created
    virtual void LayoutDialog();

/// Implementation

    // Creates the book control. If you want to use a different kind of
    // control, override.
    virtual wxBookCtrlBase* CreateBookCtrl();

    // Adds the book control to the inner sizer.
    virtual void AddBookCtrl(wxSizer* sizer);

    // Set the focus
    void OnActivate(wxActivateEvent& event);

private:
    void Init();

protected:
    wxBookCtrlBase* m_bookCtrl;
    wxSizer*        m_innerSizer; // sizer for extra space

    DECLARE_DYNAMIC_CLASS(wxPropertySheetDialog)
    DECLARE_EVENT_TABLE()
};

#endif // wxUSE_BOOKCTRL

#endif // _WX_PROPDLG_H_

