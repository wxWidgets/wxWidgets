/////////////////////////////////////////////////////////////////////////////
// Name:        wx/osx/iphone/sheetdlg.h
// Purpose:     wxActionSheetDialog and wxAlertSheetDialog classes
// Author:      Julian Smart, Linas Valiukas
// Modified by:
// Created:     2009-06-07
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Linas Valiukas
// Licence:     wxWidgets Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_SHEETDLG_H_
#define _WX_SHEETDLG_H_

#include "wx/osx/private.h"

#pragma mark -
#pragma mark wxActionSheetDialog

/**
    @class wxActionSheetDialog

    This sheet is anchored from an edge of the screen and
    can take a title, OK (destructive action) button, Cancel button,
    and further custom buttons.

    See wxSheetEvent for information about events sent
    when a button is pressed.

    @category{wxbile}
*/

class wxActionSheetCtrlIPhoneImpl;

class WXDLLEXPORT wxActionSheetDialog: public wxActionSheetDialogBase
{
public:
    /// Default constructor.
    wxActionSheetDialog() { Init(); }

    /// Constructor.
    wxActionSheetDialog(wxWindow *parent,
                        const wxString& title,
                        const wxString& cancelButtonTitle = _("Cancel"),
                        const wxArrayString& otherButtonTitles = wxArrayString(),
                        long style = wxACTION_SHEET_DIALOG_DEFAULT)
    {
        Init();

        Create(parent, title, cancelButtonTitle, otherButtonTitles, style);
    }

    /// Creation function.
    bool Create(wxWindow *parent,
                const wxString& title,
                const wxString& cancelButtonTitle = _("Cancel"),
                const wxArrayString& otherButtonTitles = wxArrayString(),
                long style = wxACTION_SHEET_DIALOG_DEFAULT);
                
    /// Shows the sheet.
    bool ShowSheetDialog(wxWindow* animateFrom = NULL);

    /// Dismisses the sheet.
    bool DismissSheetDialog(wxWindowID id);
    

protected:

    // Common initialisation
    void Init();

    DECLARE_DYNAMIC_CLASS_NO_COPY(wxActionSheetDialog)
    DECLARE_EVENT_TABLE()
    
private:
    // Used instead of m_peer, because SetPeer() attemts to Embed() the sheet,
    // and we don't want to add it as a subview to anything
    wxActionSheetCtrlIPhoneImpl *m_sheetImpl;
    
    // A reference to parent wxWindow (as wxTopLevelWindow returns NULL for GetParent(), apparently)
    wxWindow *m_sheetParent;
};


#pragma mark -
#pragma mark wxAlertSheetDialog

/**
    @class wxAlertSheetDialog

    This sheet is a free-floating window and can take a
    title, Cancel button, and further custom buttons.

    See wxSheetEvent for information about events sent
    when a button is pressed.

    @category{wxbile}
*/

class wxAlertSheetCtrlIPhoneImpl;

class WXDLLEXPORT wxAlertSheetDialog: public wxAlertSheetDialogBase
{
public:
    /// Default constructor.
    wxAlertSheetDialog()
    {
        Init();
    }

    /// Constructor.
    wxAlertSheetDialog(wxWindow *parent,
        const wxString& title,
        const wxString& msg = wxEmptyString,
        const wxString& cancelButtonTitle = _("Cancel"),
        const wxArrayString& otherButtonTitles = wxArrayString(),
        long style = wxACTION_SHEET_DIALOG_DEFAULT)
    {
        Init();

        Create(parent, title, msg, cancelButtonTitle, otherButtonTitles, style);
    }

    /// Creation function.
    bool Create(wxWindow *parent,
                const wxString& title,
                const wxString& msg = wxEmptyString,
                const wxString& cancelButtonTitle = _("Cancel"),
                const wxArrayString& otherButtonTitles = wxArrayString(),
                long style = wxACTION_SHEET_DIALOG_DEFAULT);
                
    /// Shows the sheet.
    bool ShowSheetDialog(wxWindow* animateFrom = NULL);

    /// Dismisses the sheet.
    bool DismissSheetDialog(wxWindowID id);

protected:

    // Common initialisation
    void Init();

    DECLARE_DYNAMIC_CLASS_NO_COPY(wxAlertSheetDialog)
    DECLARE_EVENT_TABLE()
    
private:
    // Used instead of m_peer, because SetPeer() attemts to Embed() the sheet,
    // and we don't want to add it as a subview to anything
    wxAlertSheetCtrlIPhoneImpl *m_sheetImpl;
    
};

#endif
    // _WX_SHEETDLG_H_
