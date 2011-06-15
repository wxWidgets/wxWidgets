/////////////////////////////////////////////////////////////////////////////
// Name:        src/mobile/generic/mo_sheets_n.mm
// Purpose:     wxMoActionSheet and wxMoAlertSheet classes
// Author:      Julian Smart, Linas Valiukas
// Modified by:
// Created:     2011-06-14
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Linas Valiukas
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

/*
For explanation of UIAlertView versus UIAlertSheet versus UIActionSheet, see:

http://my.safaribooksonline.com/9780321591180/ch04lev1sec1

UIAlertSheet became UIActionSheet. UIAlertView is more of a popup style.
You can add extra controls to alerts, but it's undocumented and may not
work in future versions. So we might not support that here.

  */

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/object.h"
    #include "wx/sizer.h"
    #include "wx/font.h"
    #include "wx/colour.h"
    #include "wx/settings.h"
    #include "wx/log.h"
    #include "wx/panel.h"
    #include "wx/containr.h"
#endif

#include "wx/mobile/native/sheets.h"
#include "wx/mobile/native/button.h"

IMPLEMENT_DYNAMIC_CLASS(wxSheetEvent, wxNotifyEvent)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_SHEET_BUTTON_CLICKED)

IMPLEMENT_DYNAMIC_CLASS(wxMoSheetBase, wxDialog)

BEGIN_EVENT_TABLE(wxMoSheetBase, wxDialog)
    EVT_BUTTON(wxID_ANY, wxMoSheetBase::OnButtonClick)
END_EVENT_TABLE()

void wxMoSheetBase::Init()
{
    // FIXME stub
}

wxMoSheetBase::~wxMoSheetBase()
{
}

bool wxMoSheetBase::CreateSheet(
            wxMoSheetType sheetType,
            wxWindow *parent,
            const wxString& title,
            const wxString& msg,
            const wxString& cancelButtonTitle,
            const wxString& okButtonTitle,
            const wxArrayString& otherButtonTitles,
            long style)
{
    // FIXME stub

    return true;
}

bool wxMoSheetBase::CreateControls()
{
    // FIXME stub

    return true;
}

// If this returns the empty string, there is no OK (destructive) button.
wxString wxMoSheetBase::GetOKButtonTitle() const
{
    // FIXME stub

    return wxEmptyString;
}

// If this returns the empty string, there is no Cancel button.
wxString wxMoSheetBase::GetCancelButtonTitle() const
{
    return wxEmptyString;
}

// Index starts at zero. 0 is Cancel, 1 is OK, from 2 - custom buttons
wxString wxMoSheetBase::GetButtonTitle(size_t i) const
{
    // FIXME stub

    return wxEmptyString;
}

// Get button identifier for the given index. 0 is Cancel, 1 is OK, from 2 - custom buttons
wxWindowID wxMoSheetBase::GetButtonId(size_t i) const
{
    // FIXME stub

    return wxID_ANY;
}

bool wxMoSheetBase::ShowSheet(wxWindow* WXUNUSED(animateFrom))
{
    // FIXME stub

    return true;
}

bool wxMoSheetBase::DismissSheet(wxWindowID WXUNUSED(id))
{
    // FIXME stub

    return true;
}

void wxMoSheetBase::OnButtonClick(wxCommandEvent& event)
{
    // FIXME stub
}

// Action sheet

IMPLEMENT_DYNAMIC_CLASS(wxMoActionSheet, wxMoSheetBase)

void wxMoActionSheet::Init()
{
}

// Alert sheet

IMPLEMENT_DYNAMIC_CLASS(wxMoAlertSheet, wxMoSheetBase)

void wxMoAlertSheet::Init()
{
}

// Message box alert sheet
class wxMoMessageBoxAlertSheet: public wxMoAlertSheet
{
public:
    wxMoMessageBoxAlertSheet();

    wxMoMessageBoxAlertSheet(
            wxWindow *parent,
            const wxString& title,
            const wxString& msg = wxEmptyString,
            const wxString& cancelButtonTitle = _("Cancel"),
            const wxArrayString& otherButtonTitles = wxArrayString(),
            long style = wxACTION_SHEET_DEFAULT)
    {
        Init();

        Create(parent, title, msg, cancelButtonTitle, otherButtonTitles, style);
    }

    void OnAlertButton(wxSheetEvent& event);

    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(wxMoMessageBoxAlertSheet, wxMoAlertSheet)
    EVT_SHEET_BUTTON(wxMoMessageBoxAlertSheet::OnAlertButton)
END_EVENT_TABLE()

void wxMoMessageBoxAlertSheet::OnAlertButton(wxSheetEvent& WXUNUSED(event))
{
    // FIXME stub
}

// Shows an alert sheet using the standard wxWidgets message box API,
// except that it returns immediately since modal loops are not supported.
// So you can only use it to show a message, not get input.
int wxIBMessageBox(const wxString& message, const wxString& caption, int style,
                   wxWindow *parent, int WXUNUSED(x), int WXUNUSED(y))
{
    // FIXME stub

    return 0;
}
