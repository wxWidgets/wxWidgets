/////////////////////////////////////////////////////////////////////////////
// Name:        src/mobile/generic/mo_sheets_g.cpp
// Purpose:     wxMoActionSheet and wxMoAlertSheet classes
// Author:      Julian Smart
// Modified by:
// Created:     2009-06-07
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     
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

#include "wx/mobile/generic/sheets.h"
#include "wx/mobile/generic/button.h"
#include "wx/mobile/generic/simulator.h"

IMPLEMENT_DYNAMIC_CLASS(wxSheetEvent, wxNotifyEvent)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_SHEET_BUTTON_CLICKED)

IMPLEMENT_DYNAMIC_CLASS(wxMoSheetBase, wxDialog)

BEGIN_EVENT_TABLE(wxMoSheetBase, wxDialog)
    EVT_BUTTON(wxID_ANY, wxMoSheetBase::OnButtonClick)
END_EVENT_TABLE()

void wxMoSheetBase::Init()
{
    m_titleCtrl = NULL;
    m_msgCtrl = NULL;
    m_sheetStyle  = 0;
    m_sheetParent = NULL;
    m_owner = this;
    m_inButtonEvent = false;
    m_sheetType = wxSHEET_TYPE_ACTION;
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
    m_sheetType = sheetType;
    m_sheetParent = parent;
    m_sheetTitle = title;
    m_sheetStyle = style;
    m_sheetMessage = msg;

    wxWindow* actualParent = wxGetTopLevelParent(parent);
    long dialogStyle = 0;

    if ( !wxDialog::Create(actualParent, wxID_ANY, wxEmptyString,  wxDefaultPosition, wxDefaultSize, dialogStyle) )
        return false;

    m_buttonTitles.Add(okButtonTitle);
    m_buttonTitles.Add(cancelButtonTitle);

    size_t i;
    for (i = 0; i < otherButtonTitles.GetCount(); i++)
    {
        m_buttonTitles.Add(otherButtonTitles[i]);
    }

    // Force a solid background colour
    wxColour bgColour;
    if (m_sheetType == wxSHEET_TYPE_ACTION)
        bgColour = wxMoSystemSettings::GetColour(wxMO_COLOUR_ACTION_SHEET_BG);
    else
        bgColour = wxMoSystemSettings::GetColour(wxMO_COLOUR_ALERT_SHEET_BG);
    SetBackgroundColour(bgColour);

    CreateControls();

    wxMoSimulatorScreenWindow* simulatorScreenWindow = wxMoSimulatorScreenWindow::FindSimulatorScreenWindow(parent);
    if (simulatorScreenWindow)
    {
        wxPoint simulatorPos = simulatorScreenWindow->ClientToScreen(wxPoint(0, 0));
        wxSize simulatorSize = simulatorScreenWindow->GetClientSize();

        if (m_sheetType == wxSHEET_TYPE_ACTION)
        {
            m_titleCtrl->Wrap(simulatorSize.x-20); 
            GetSizer()->Fit(this);
            SetSize(wxSize(simulatorSize.x, GetSize().y));
            Move(wxPoint(simulatorPos.x, simulatorPos.y + simulatorSize.y - GetSize().y));
        }
        else if (m_sheetType == wxSHEET_TYPE_ALERT)
        {
            int width = 300;
            m_titleCtrl->Wrap(width-20);
            if (m_msgCtrl)
                m_msgCtrl->Wrap(width-20);
            GetSizer()->Fit(this);
            SetSize(wxSize(width, GetSize().y));
            Move(wxPoint(simulatorPos.x + (simulatorSize.x - GetSize().x)/2,
                         simulatorPos.y + (simulatorSize.y - GetSize().y)/2));
        }
    }

    return true;
}

bool wxMoSheetBase::CreateControls()
{
    wxBoxSizer* topSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(topSizer);

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    topSizer->Add(sizer, 1, wxEXPAND|wxALL, 5);

    sizer->AddSpacer(10);

    m_titleCtrl = new wxStaticText(this, wxID_STATIC, m_sheetTitle, wxDefaultPosition, wxDefaultSize,
        wxALIGN_CENTRE);
    m_titleCtrl->SetForegroundColour(*wxWHITE);
    m_titleCtrl->SetFont(wxMoSystemSettings::GetFont(wxMO_FONT_ACTION_SHEET_TITLE));

    sizer->Add(m_titleCtrl, 0, wxALIGN_CENTRE_HORIZONTAL|wxALL, 5);
    
    sizer->AddSpacer(10);

    if (!m_sheetMessage.IsEmpty())
    {
        m_msgCtrl = new wxStaticText(this, wxID_STATIC, m_sheetMessage, wxDefaultPosition, wxDefaultSize,
            wxALIGN_CENTRE);
        m_msgCtrl->SetForegroundColour(wxMoSystemSettings::GetColour(wxMO_COLOUR_ACTION_SHEET_TEXT));
        m_msgCtrl->SetFont(wxMoSystemSettings::GetFont(wxMO_FONT_ALERT_SHEET_MESSAGE));
        sizer->Add(m_msgCtrl, 0, wxALIGN_CENTRE_HORIZONTAL|wxALL, 5);

        sizer->AddSpacer(10);
    }

    wxFont buttonFont(wxMoSystemSettings::GetFont(wxMO_FONT_ALERT_SHEET_BUTTON));

    // OK button
    m_buttonIds.Add(wxID_OK);
    m_buttonIds.Add(wxID_CANCEL);

    if (!m_buttonTitles[0].IsEmpty())
    {
        wxMoButton* button = new wxMoButton(this, wxID_OK, m_buttonTitles[0], wxDefaultPosition, wxDefaultSize);
        button->SetBackgroundColour(wxMoSystemSettings::GetColour(wxMO_COLOUR_ACTION_SHEET_OK_BG));
        button->SetFont(buttonFont);
        sizer->Add(button, 0, wxEXPAND|wxALL, 5);
    }    

    // Custom buttons
    size_t i;
    for (i = 2; i < m_buttonTitles.GetCount(); i++)
    {
        wxWindowID id = wxID_ANY;

        wxString buttonTitle(m_buttonTitles[i]);
        if (!buttonTitle.IsEmpty())
        {
            wxMoButton* button = new wxMoButton(this, id, buttonTitle, wxDefaultPosition, wxDefaultSize);
            id = button->GetId();
            button->SetBackgroundColour(wxMoSystemSettings::GetColour(wxMO_COLOUR_ACTION_SHEET_OTHER_BG));
            button->SetFont(buttonFont);
    
            sizer->Add(button, 0, wxEXPAND|wxALL, 5);
        }

        m_buttonIds.Add(id);
    }

    // Cancel button
    if (!m_buttonTitles[1].IsEmpty())
    {
        wxMoButton* button = new wxMoButton(this, wxID_CANCEL, m_buttonTitles[1], wxDefaultPosition, wxDefaultSize);
        button->SetBackgroundColour(wxMoSystemSettings::GetColour(wxMO_COLOUR_ACTION_SHEET_CANCEL_BG));
        button->SetFont(buttonFont);
        sizer->Add(button, 0, wxEXPAND|wxALL, 5);
    }    

    return true;
}

// If this returns the empty string, there is no OK (destructive) button.
wxString wxMoSheetBase::GetOKButtonTitle() const
{
    return GetButtonTitle(1);
}

// If this returns the empty string, there is no Cancel button.
wxString wxMoSheetBase::GetCancelButtonTitle() const
{
    return GetButtonTitle(0);
}

// Index starts at zero. 0 is Cancel, 1 is OK, from 2 - custom buttons
wxString wxMoSheetBase::GetButtonTitle(size_t i) const
{
    wxASSERT(i < m_buttonTitles.GetCount());
    if (i < m_buttonTitles.GetCount())
        return m_buttonTitles[i];
    else
        return wxEmptyString;
}

// Get button identifier for the given index. 0 is Cancel, 1 is OK, from 2 - custom buttons
wxWindowID wxMoSheetBase::GetButtonId(size_t i) const
{
    wxASSERT(i < m_buttonIds.GetCount());
    if (i < m_buttonIds.GetCount())
        return (wxWindowID) m_buttonIds[i];
    else
        return wxID_ANY;
}

bool wxMoSheetBase::ShowSheet(wxWindow* WXUNUSED(animateFrom))
{
    wxMoSimulatorScreenWindow* simulatorScreenWindow = wxMoSimulatorScreenWindow::FindSimulatorScreenWindow(m_sheetParent);
    if (simulatorScreenWindow)
        simulatorScreenWindow->Disable();

    bool animate = false;
    if (animate && simulatorScreenWindow)
    {
        wxPoint pos = GetPosition();
        int startY = simulatorScreenWindow->GetPosition().y + simulatorScreenWindow->GetSize().y;
        Move(pos.x, startY);
        Show(true);
        int i;
        for (i = startY; i >= pos.y; i -= 2)
        {
            Move(pos.x, i);
            Update();
        }
        return true;
    }
    else
        return Show(true);
}

bool wxMoSheetBase::DismissSheet(wxWindowID WXUNUSED(id))
{
    wxMoSimulatorScreenWindow* simulatorScreenWindow = wxMoSimulatorScreenWindow::FindSimulatorScreenWindow(m_sheetParent);
    if (simulatorScreenWindow)
        simulatorScreenWindow->Enable();

    return Show(false);
}

void wxMoSheetBase::OnButtonClick(wxCommandEvent& event)
{
    if (m_inButtonEvent)
    {
        event.Skip();
        return;
    }

    m_inButtonEvent = true;

    int buttonIndex = -1;
    size_t i;
    for (i = 0; i < m_buttonIds.GetCount(); i++)
    {
        if (m_buttonIds[i] == event.GetId())
        {
            buttonIndex = 0;
            break;
        }
    }

    wxSheetEvent actionSheetEvent(wxEVT_COMMAND_SHEET_BUTTON_CLICKED, event.GetId(),
        buttonIndex, this);
    actionSheetEvent.SetEventObject(this);

    bool processed = GetOwner()->ProcessEvent(actionSheetEvent);
    if (!processed && GetOwner() != GetEventHandler())
        processed = GetEventHandler()->ProcessEvent(actionSheetEvent);

    if (!processed)
        event.Skip();

    m_inButtonEvent = false;
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
    DismissSheet(wxID_CANCEL);
    this->Destroy();
}

// Shows an alert sheet using the standard wxWidgets message box API,
// except that it returns immediately since modal loops are not supported.
// So you can only use it to show a message, not get input.
int wxIBMessageBox(const wxString& message, const wxString& caption, int style,
                   wxWindow *parent, int WXUNUSED(x), int WXUNUSED(y))
{
    wxString cancelTitle;
    if (style & wxCANCEL)
        cancelTitle = _("Cancel");
    else
        cancelTitle = _("OK");
    wxMoMessageBoxAlertSheet* sheet = new wxMoMessageBoxAlertSheet(parent, caption,
              message, cancelTitle);
    sheet->ShowSheet();

    return wxCANCEL;
}
