/////////////////////////////////////////////////////////////////////////////
// Name:        wx/sheetdlg.h
// Purpose:     wxActionSheetDialog and wxAlertSheetDialog classes
// Author:      Julian Smart, Linas Valiukas
// Modified by:
// Created:     2009-06-07
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Linas Valiukas
// Licence:     wxWidgets Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_SHEETDLG_H_BASE_
#define _WX_SHEETDLG_H_BASE_

#include "wx/dialog.h"
#include "wx/stattext.h"
#include "wx/panel.h"

// Action sheet styles
#define wxACTION_SHEET_DIALOG_DEFAULT      0x1000
#define wxACTION_SHEET_DIALOG_TRANSLUCENT  0x2000
#define wxACTION_SHEET_DIALOG_OPAQUE       0x4000


#pragma mark -
#pragma mark wxSheetDialogBase

/**
    @class wxSheetDialogBase

    This is the base class for alert and action sheets.

    See wxSheetEvent for information about events sent
    when a button is pressed.

    @category{wxbile}
*/

class WXDLLEXPORT wxSheetDialogBase: public wxDialog
{
public:
    // Type of sheet
    enum wxSheetDialogType {
        wxSHEET_DIALOG_TYPE_ACTION,
        wxSHEET_DIALOG_TYPE_ALERT
    };

    /// Default constructor.
    wxSheetDialogBase()
    {
        Init();
    }

    virtual ~wxSheetDialogBase() { }

    /// Common creation function. Do not use directly.
    virtual bool CreateSheet(wxSheetDialogType sheetDialogType,
                             wxWindow *parent,
                             const wxString& title,
                             const wxString& msg,
                             const wxString& cancelButtonTitle = _("Cancel"),
                             const wxArrayString& otherButtonTitles = wxArrayString(),
                             long style = wxACTION_SHEET_DIALOG_DEFAULT)
    {
        m_sheetDialogType = sheetDialogType;
        m_sheetDialogParent = parent;
        m_sheetDialogTitle = title;
        m_sheetDialogMessage = msg;
        m_cancelButtonTitle = cancelButtonTitle;
        m_buttonTitles = otherButtonTitles;
    }

    /// Shows the sheet.
    virtual bool ShowSheetDialog(wxWindow* animateFrom = NULL) = 0;

    /// Dismisses the sheet.
    virtual bool DismissSheetDialog(wxWindowID id) = 0;

    /// Sets the sheet message.
    virtual void SetSheetDialogMessage(const wxString& msg) { m_sheetDialogMessage = msg; }

    /// Gets the sheet message.
    virtual wxString GetSheetDialogMessage() const { return m_sheetDialogMessage; }

    /// Sets the Cancel button title.
    virtual void SetCancelButtonTitle(const wxString& title) { m_cancelButtonTitle = title; }

    /// Gets the Cancel button title. If getter returns the empty string,
    /// there is no Cancel button.
    virtual wxString GetCancelButtonTitle() const { return m_cancelButtonTitle; }
    
    /// Sets a button title. Index starts at zero.
    virtual void SetButtonTitle(const wxString& title, size_t i) { m_buttonTitles[i] = title; }

    /// Gets a button title. Index starts at zero.
    virtual wxString GetButtonTitle(size_t i) const { return m_buttonTitles[i]; }

    /// Set a button identifier for the given index.
    virtual void SetButtonId(const wxWindowID buttonID, size_t i) { m_buttonIds[i] = buttonID; }

    /// Get a button identifier for the given index.
    virtual wxWindowID GetButtonId(size_t i) const { return m_buttonIds[i]; }

    /// Sets the owner - the event handler to receive sheet events.
    virtual void SetOwner(wxEvtHandler* owner) { m_owner = owner; }

    /// Gets the owner - the event handler to receive sheet events.
    virtual wxEvtHandler* GetOwner() const { return m_owner; }

protected:

    // Common initialisation
    void Init()
    {
        m_sheetDialogTitle = wxEmptyString;
        m_sheetDialogMessage = wxEmptyString;
        m_cancelButtonTitle = wxEmptyString;
        m_sheetDialogStyle  = 0;
        m_sheetDialogParent = NULL;
        m_owner = this;
        m_inButtonEvent = false;
        m_sheetDialogType = wxSHEET_DIALOG_TYPE_ACTION;
    }

    void OnButtonClick(wxCommandEvent& event);

    wxString              m_sheetDialogTitle;
    wxString              m_sheetDialogMessage;
    wxString              m_cancelButtonTitle;
    wxArrayString         m_buttonTitles;
    wxArrayInt            m_buttonIds;
    wxWindow*             m_sheetDialogParent; // the ostensible parent; the real parent is a TLW
    long                  m_sheetDialogStyle;
    wxEvtHandler*         m_owner;
    bool                  m_inButtonEvent;
    wxSheetDialogType     m_sheetDialogType;

    wxDECLARE_NO_COPY_CLASS(wxSheetDialogBase);
};


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

class WXDLLEXPORT wxActionSheetDialogBase: public wxSheetDialogBase
{
public:
    /// Default constructor.
    wxActionSheetDialogBase()
    {
        Init();
    }

    /// Constructor.
    wxActionSheetDialogBase(wxWindow *parent,
                            const wxString& title,
                            const wxString& cancelButtonTitle = _("Cancel"),
                            const wxArrayString& otherButtonTitles = wxArrayString(),
                            long style = wxACTION_SHEET_DIALOG_DEFAULT)
    {
        Init();

        Create(parent, title, cancelButtonTitle, otherButtonTitles, style);
    }

    /// Creation function.
    virtual bool Create(wxWindow *parent,
                        const wxString& title,
                        const wxString& cancelButtonTitle = _("Cancel"),
                        const wxArrayString& otherButtonTitles = wxArrayString(),
                        long style = wxACTION_SHEET_DIALOG_DEFAULT)
    {
        return CreateSheet(wxSHEET_DIALOG_TYPE_ACTION, parent, title, wxEmptyString, cancelButtonTitle, otherButtonTitles, style);
    }

protected:

    // Common initialisation
    void Init()
    {
        
    }

private:
    wxDECLARE_NO_COPY_CLASS(wxActionSheetDialogBase);
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

class WXDLLEXPORT wxAlertSheetDialogBase: public wxSheetDialogBase
{
public:
    /// Default constructor.
    wxAlertSheetDialogBase()
    {
        Init();
    }

    /// Constructor.
    wxAlertSheetDialogBase(wxWindow *parent,
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
    virtual bool Create(wxWindow *parent,
                        const wxString& title,
                        const wxString& msg = wxEmptyString,
                        const wxString& cancelButtonTitle = _("Cancel"),
                        const wxArrayString& otherButtonTitles = wxArrayString(),
                        long style = wxACTION_SHEET_DIALOG_DEFAULT)
    {
        return CreateSheet(wxSHEET_DIALOG_TYPE_ALERT, parent, title, msg, cancelButtonTitle, otherButtonTitles, style);
    }
    
    /// Sets the sheet title.
    virtual void SetSheetDialogTitle(const wxString& title) { m_sheetDialogTitle = title; }

    /// Gets the sheet title.
    virtual wxString GetSheetDialogTitle() const { return m_sheetDialogTitle; }

protected:

    // Common initialisation
    void Init()
    {
        m_sheetDialogTitle = wxEmptyString;
    }
    
    wxString              m_sheetDialogTitle;

private:
    wxDECLARE_NO_COPY_CLASS(wxAlertSheetDialogBase);
};


#pragma mark -
#pragma mark wxSheetDialogEvent

/**
    @class wxSheetDialogEvent

    Use this event for both action sheets and alert sheets.

    An action sheet event is sent to the sheet or sheet owner when a button on the sheet
    is pressed. This is to allow the application to easily distinguish an action
    sheet button press from other button events. Also the buttons can be identified
    by button index rather than id if necessary.

    Use with e.g. EVT_SHEET_BUTTON(MyFrame::OnSheetCommand)

    The sheet doesn't take an id, therefore the event table entry doesn't take
    an id and all sheet events are routed to the same function. Or, Connect
    can be used for a given sheet, with Disconnect called before the sheet
    is destroyed.

    If not processed, ordinary button events will be generated and sent to
    the sheet or sheet owner.

    OK (destructive action) and Cancel buttons have ids wxID_OK and wxID_CANCEL.
    The ids of the other buttons can be retrieved using wxSheetBase::GetButtonId,
    for use with Connect.

    @category{wxbile}
*/

class WXDLLEXPORT wxSheetDialogEvent: public wxNotifyEvent
{
public:
    /// Default constructor.
    wxSheetDialogEvent()
    {
        m_sheet = NULL;
        m_buttonIndex = -1;
    }
    
    /// Copy constructor.
    wxSheetDialogEvent(const wxSheetDialogEvent& event): wxNotifyEvent(event)
    {
        m_sheet = event.m_sheet;
        m_buttonIndex = event.m_buttonIndex;
    }
    
    /// Constructor.
    wxSheetDialogEvent(wxEventType commandType, int sheetId, int buttonIndex, wxSheetDialogBase* sheet)
        : wxNotifyEvent(commandType, sheetId)
    {
         m_sheet = sheet;
         m_buttonIndex = buttonIndex;
    }

    /// Sets the sheet
    void SetSheet(wxSheetDialogBase* sheet) { m_sheet = sheet; }

    /// Gets the sheet
    wxSheetDialogBase* GetSheet() const { return m_sheet; }

    /// Sets the button index
    void SetButtonIndex(int buttonIndex) { m_buttonIndex = buttonIndex; }

    /// Gets the button index
    int GetButtonIndex() const { return m_buttonIndex; }

    virtual wxEvent *Clone() const { return new wxSheetDialogEvent(*this); }

private:
    wxSheetDialogBase*    m_sheet;
    int                   m_buttonIndex;

    DECLARE_DYNAMIC_CLASS_NO_ASSIGN(wxSheetDialogEvent)
};

typedef void (wxEvtHandler::*wxSheetDialogEventFunction)(wxSheetDialogEvent&);

BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EVENT_TYPE(wxEVT_COMMAND_SHEET_DIALOG_BUTTON_CLICKED,     870)
END_DECLARE_EVENT_TYPES()

#define wxSheetDialogEventHandler(func) \
    (wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(wxSheetDialogEventFunction, &func)

#define EVT_SHEET_DIALOG_BUTTON(fn) DECLARE_EVENT_TABLE_ENTRY(wxEVT_COMMAND_SHEET_DIALOG_BUTTON_CLICKED, \
  wxID_ANY, wxID_ANY, (wxObjectEventFunction) (wxEventFunction)  wxStaticCastEvent( wxSheetDialogEventFunction, & fn ), NULL),

/// Shows an alert sheet using the standard wxWidgets message box API
int wxIBMessageBox(const wxString& message,
                   const wxString& caption = _("Message"),
                   int style = wxOK,
                   wxWindow *parent = NULL,
                   int x = -1,
                   int y = -1);


// ----------------------------------------------------------------------------
// wxSheetDialog classes themselves
// ----------------------------------------------------------------------------

#if defined(__WXOSX_IPHONE__)  // iPhone-only
    #include  "wx/osx/iphone/sheetdlg.h"
#endif


#endif
    // _WX_SHEETDLG_H_BASE_
