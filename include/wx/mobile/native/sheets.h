/////////////////////////////////////////////////////////////////////////////
// Name:        wx/mobile/native/sheets.h
// Purpose:     wxMobile/wxiOS (GSoC 2011)
// Author:      Julian Smart, Linas Valiukas
// Modified by:
// Created:     2011-06-14
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Linas Valiukas
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MOBILE_NATIVE_SHEETS_H_
#define _WX_MOBILE_NATIVE_SHEETS_H_

#include "wx/dialog.h"
#include "wx/stattext.h"
#include "wx/mobile/native/panel.h"

// Action sheet styles
#define wxACTION_SHEET_DEFAULT      0x1000
#define wxACTION_SHEET_TRANSLUCENT  0x2000
#define wxACTION_SHEET_OPAQUE       0x4000

/**
 @class wxMoSheetBase
 
 This is the base class for alert and action sheets.
 
 See wxSheetEvent for information about events sent
 when a button is pressed.
 
 @category{wxMobile}
 */

class WXDLLEXPORT wxMoSheetBase: public wxDialog
{
public:
    // Type of sheet
    enum wxMoSheetType { wxSHEET_TYPE_ACTION, wxSHEET_TYPE_ALERT } ;
    
    /// Default constructor.
    wxMoSheetBase() { Init(); }
    
    virtual ~wxMoSheetBase();
    
    /// Common creation function. Do not use directly.
    bool CreateSheet(
                     wxMoSheetType sheetType,
                     wxWindow *parent,
                     const wxString& title,
                     const wxString& msg,
                     const wxString& cancelButtonTitle = _("Cancel"),
                     const wxString& okButtonTitle = _("OK"),
                     const wxArrayString& otherButtonTitles = wxArrayString(),
                     long style = wxACTION_SHEET_DEFAULT);
    
    /// Shows the sheet.
    bool ShowSheet(wxWindow* animateFrom = NULL);
    
    /// Dismisses the sheet.
    bool DismissSheet(wxWindowID id);
    
    /// Sets the sheet title.
    virtual void SetSheetTitle(const wxString& title) { m_sheetTitle = title; }
    
    /// Gets the sheet title.
    virtual wxString GetSheetTitle() const { return m_sheetTitle; }
    
    /// Sets the sheet message.
    virtual void SetSheetMessage(const wxString& msg) { m_sheetMessage = msg; }
    
    /// Gets the sheet message.
    virtual wxString GetSheetMessage() const { return m_sheetMessage; }
    
    /// Returns the OK button title. If this returns the empty string,
    /// there is no OK (destructive) button.
    virtual wxString GetOKButtonTitle() const;
    
    /// Returns the Cancel button title. If this returns the empty string,
    /// there is no Cancel button.
    virtual wxString GetCancelButtonTitle() const;
    
    /// Gets a button title. Index starts at zero.
    /// 0 is OK, 1 is Cancel, from 2 - custom buttons
    virtual wxString GetButtonTitle(size_t i) const;
    
    /// Get a button identifier for the given index.
    /// 0 is OK, 1 is Cancel, from 2 - custom buttons
    virtual wxWindowID GetButtonId(size_t i) const;
    
    /// Sets the owner - the event handler to receive sheet events.
    void SetOwner(wxEvtHandler* owner) { m_owner = owner; }
    
    /// Gets the owner - the event handler to receive sheet events.
    wxEvtHandler* GetOwner() const { return m_owner; }
    
protected:
    
    // Common initialisation
    void Init();
    
    void OnButtonClick(wxCommandEvent& event);
    
    virtual bool CreateControls();
    
    wxString        m_sheetTitle;
    wxString        m_sheetMessage;
    wxArrayString   m_buttonTitles;
    wxArrayInt      m_buttonIds;
    wxStaticText*   m_titleCtrl;
    wxStaticText*   m_msgCtrl;
    wxWindow*       m_sheetParent; // the ostensible parent; the real parent is a TLW
    long            m_sheetStyle;
    wxEvtHandler*   m_owner;
    bool            m_inButtonEvent;
    wxMoSheetType   m_sheetType;
    
private:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxMoSheetBase)
    DECLARE_EVENT_TABLE()
};

/**
 @class wxMoActionSheet
 
 This sheet is anchored from an edge of the screen and
 can take a title, OK (destructive action) button, Cancel button,
 and further custom buttons.
 
 See wxSheetEvent for information about events sent
 when a button is pressed.
 
 @category{wxMobile}
 */

class WXDLLEXPORT wxMoActionSheet: public wxMoSheetBase
{
public:
    /// Default constructor.
    wxMoActionSheet() { Init(); }
    
    /// Constructor.
    wxMoActionSheet(
                    wxWindow *parent,
                    const wxString& title,
                    const wxString& cancelButtonTitle = _("Cancel"),
                    const wxString& okButtonTitle = _("OK"),
                    const wxArrayString& otherButtonTitles = wxArrayString(),
                    long style = wxACTION_SHEET_DEFAULT)
    {
        Init();
        
        Create(parent, title, cancelButtonTitle, okButtonTitle, otherButtonTitles, style);
    }
    
    /// Creation function.
    bool Create(
                wxWindow *parent,
                const wxString& title,
                const wxString& cancelButtonTitle = _("Cancel"),
                const wxString& okButtonTitle = _("OK"),
                const wxArrayString& otherButtonTitles = wxArrayString(),
                long style = wxACTION_SHEET_DEFAULT)
    {
        return CreateSheet(wxSHEET_TYPE_ACTION, parent, title, wxEmptyString, cancelButtonTitle, okButtonTitle, otherButtonTitles, style);
    }
    
protected:
    
    // Common initialisation
    void Init();
    
private:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxMoActionSheet)
};

/**
 @class wxMoAlertSheet
 
 This sheet is a free-floating window and can take a
 title, Cancel button, and further custom buttons.
 
 See wxSheetEvent for information about events sent
 when a button is pressed.
 
 @category{wxMobile}
 */

class WXDLLEXPORT wxMoAlertSheet: public wxMoSheetBase
{
public:
    /// Default constructor.
    wxMoAlertSheet() { Init(); }
    
    /// Constructor.
    wxMoAlertSheet(
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
    
    /// Creation function.
    bool Create(
                wxWindow *parent,
                const wxString& title,
                const wxString& msg = wxEmptyString,
                const wxString& cancelButtonTitle = _("Cancel"),
                const wxArrayString& otherButtonTitles = wxArrayString(),
                long style = wxACTION_SHEET_DEFAULT)
    {
        return CreateSheet(wxSHEET_TYPE_ALERT, parent, title, msg, cancelButtonTitle, wxEmptyString, otherButtonTitles, style);
    }
    
protected:
    
    // Common initialisation
    void Init();
    
private:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxMoAlertSheet)
};

/**
 @class wxSheetEvent
 
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
 The ids of the other buttons can be retrieved using wxMoSheetBase::GetButtonId,
 for use with Connect.
 
 @category{wxMobile}
 */

class WXDLLEXPORT wxSheetEvent: public wxNotifyEvent
{
public:
    /// Default constructor.
    wxSheetEvent()
    {
        m_sheet = NULL;
        m_buttonIndex = -1;
    }
    /// Copy constructor.
    wxSheetEvent(const wxSheetEvent& event): wxNotifyEvent(event)
    {
        m_sheet = event.m_sheet;
        m_buttonIndex = event.m_buttonIndex;
    }
    /// Constructor.
    wxSheetEvent(wxEventType commandType, int sheetId, int buttonIndex, wxMoSheetBase* sheet)
    : wxNotifyEvent(commandType, sheetId)
    {
        m_sheet = sheet;
        m_buttonIndex = buttonIndex;
    }
    
    /// Sets the sheet
    void SetSheet(wxMoSheetBase* sheet) { m_sheet = sheet; }
    
    /// Gets the sheet
    wxMoSheetBase* GetSheet() const { return m_sheet; }
    
    /// Sets the button index
    void SetButtonIndex(int buttonIndex) { m_buttonIndex = buttonIndex; }
    
    /// Gets the button index
    int GetButtonIndex() const { return m_buttonIndex; }
    
    virtual wxEvent *Clone() const { return new wxSheetEvent(*this); }
    
private:
    wxMoSheetBase*    m_sheet;
    int                     m_buttonIndex;
    
    DECLARE_DYNAMIC_CLASS_NO_ASSIGN(wxSheetEvent)
};

typedef void (wxEvtHandler::*wxSheetEventFunction)(wxSheetEvent&);

BEGIN_DECLARE_EVENT_TYPES()
DECLARE_EVENT_TYPE(wxEVT_COMMAND_SHEET_BUTTON_CLICKED,     870)
END_DECLARE_EVENT_TYPES()

#define wxSheetEventHandler(func) \
(wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(wxSheetEventFunction, &func)

#define EVT_SHEET_BUTTON(fn) DECLARE_EVENT_TABLE_ENTRY(wxEVT_COMMAND_SHEET_BUTTON_CLICKED, \
wxID_ANY, wxID_ANY, (wxObjectEventFunction) (wxEventFunction)  wxStaticCastEvent( wxSheetEventFunction, & fn ), NULL),

/// Shows an alert sheet using the standard wxWidgets message box API
int wxIBMessageBox(const wxString& message, const wxString& caption = _("Message"), int style = wxOK,
                   wxWindow *parent = NULL, int x = -1, int y = -1);

#endif
    // _WX_MOBILE_NATIVE_SHEETS_H_
