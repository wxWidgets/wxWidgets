/////////////////////////////////////////////////////////////////////////////
// Name:        univ/combobox.cpp
// Purpose:     wxComboControl and wxComboBox implementation
// Author:      Vadim Zeitlin
// Modified by:
// Created:     15.12.00
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "univcombobox.h"
#endif

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_COMBOBOX

#ifndef WX_PRECOMP
    #include "wx/log.h"

    #include "wx/button.h"
    #include "wx/combobox.h"
    #include "wx/listbox.h"
    #include "wx/textctrl.h"

    #include "wx/validate.h"
#endif

#include "wx/univ/renderer.h"
#include "wx/univ/inphand.h"
#include "wx/univ/theme.h"

// ----------------------------------------------------------------------------
// wxComboButton is just a normal button except that it sends commands to the
// combobox and not its parent
// ----------------------------------------------------------------------------

class wxComboButton : public wxBitmapButton
{
public:
    wxComboButton(wxComboControl *combo)
        : wxBitmapButton(combo->GetParent(), -1, wxNullBitmap,
                         wxDefaultPosition, wxDefaultSize,
                         wxBORDER_NONE)
    {
        m_combo = combo;

        wxBitmap bmpNormal, bmpPressed, bmpDisabled;

        GetRenderer()->GetComboBitmaps(&bmpNormal, &bmpPressed, &bmpDisabled);
        SetBitmapLabel(bmpNormal);
        SetBitmapFocus(bmpNormal);
        SetBitmapSelected(bmpPressed);
        SetBitmapDisabled(bmpDisabled);

        SetSize(bmpNormal.GetWidth(), bmpNormal.GetHeight());
    }

protected:
    void OnButton(wxCommandEvent& event) { m_combo->ShowPopup(); }

    virtual wxSize DoGetBestSize() const { return GetSize(); }

private:
    wxComboControl *m_combo;

    DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// wxComboListBox is a listbox modified to be used as a popup window in a
// combobox
// ----------------------------------------------------------------------------

class wxComboListBox : public wxListBox, public wxComboPopup
{
public:
    // ctor
    wxComboListBox(wxComboControl *combo);

    // implement wxComboPopup methods
    virtual bool SetSelection(const wxString& value);
    virtual wxControl *GetControl() { return this; }
    virtual void OnShow();

protected:
    // called whenever the user selects a listbox item
    void OnSelect(wxCommandEvent& event);

    DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// wxComboPopupEventHandler implements hides the popup if the mouse is clicked
// outside it
// ----------------------------------------------------------------------------

class wxComboPopupEventHandler : public wxEvtHandler
{
public:
    wxComboPopupEventHandler(wxComboControl *combo) { m_combo = combo; }

    virtual bool ProcessEvent(wxEvent& event);

private:
    wxComboControl *m_combo;
};

// ----------------------------------------------------------------------------
// event tables and such
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxComboButton, wxButton)
    EVT_BUTTON(-1, wxComboButton::OnButton)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(wxComboListBox, wxListBox)
    EVT_LISTBOX(-1, wxComboListBox::OnSelect)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(wxComboControl, wxControl)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(wxComboBox, wxControl);

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxComboControl creation
// ----------------------------------------------------------------------------

void wxComboControl::Init()
{
    m_popup = (wxComboPopup *)NULL;
}

bool wxComboControl::Create(wxWindow *parent,
                            wxWindowID id,
                            const wxString& value,
                            const wxPoint& pos,
                            const wxSize& size,
                            long style,
                            const wxValidator& validator,
                            const wxString& name)
{
    // first create our own window, i.e. the one which will contain all
    // subcontrols
    style &= ~wxBORDER_NONE;
    style |= wxBORDER_SUNKEN;
    if ( !wxControl::Create(parent, id, pos, size, style, validator, name) )
        return FALSE;

    // create the text control and the button as our siblings (*not* children),
    // don't care about size/position here - they will be set in DoMoveWindow()
    m_btn = new wxComboButton(this);
    m_text = new wxTextCtrl(parent, -1, value,
                            wxDefaultPosition, wxDefaultSize,
                            wxBORDER_NONE |
                            (style & wxCB_READONLY ? wxTE_READONLY : 0),
                            validator);

    // for compatibility with the otherp orts, the height specified is the
    // combined height of the combobox itself and the popup
    if ( size.y == -1 )
    {
        // ok, use default height for popup too
        m_heightPopup = -1;
    }
    else
    {
        m_heightPopup = size.y - DoGetBestSize().y;
    }

    DoSetSize(pos.x, pos.y, size.x, size.y);

    // have to disable this window to avoid interfering it with message
    // processing to the text and the button... but pretend it is enabled to
    // make IsEnabled() return TRUE
    wxControl::Enable(FALSE); // don't use non virtual Disable() here!
    m_isEnabled = TRUE;

    return TRUE;
}

wxComboControl::~wxComboControl()
{
    RemoveEventHandler();
}

// ----------------------------------------------------------------------------
// geometry stuff
// ----------------------------------------------------------------------------

wxSize wxComboControl::DoGetBestClientSize() const
{
    wxSize sizeBtn = m_btn->GetBestSize(),
           sizeText = m_text->GetBestSize();

    return wxSize(sizeBtn.x + sizeText.x, wxMax(sizeBtn.y, sizeText.y));
}

void wxComboControl::DoMoveWindow(int x, int y, int width, int height)
{
    wxControl::DoMoveWindow(x, y, width, height);

    // position the subcontrols inside the client area
    wxRect rectBorders = GetRenderer()->GetBorderDimensions(GetBorder());
    x += rectBorders.x;
    y += rectBorders.y;
    width -= rectBorders.x + rectBorders.width;
    height -= rectBorders.y + rectBorders.height;

    wxSize sizeBtn = m_btn->GetSize(),
           sizeText = m_text->GetSize();

    wxCoord wText = width - sizeBtn.x;
    m_text->SetSize(x, y, wText, height);
    m_btn->SetSize(x + wText, y, -1, height);
}

// ----------------------------------------------------------------------------
// operations
// ----------------------------------------------------------------------------

bool wxComboControl::Enable(bool enable)
{
    if ( !wxControl::Enable(enable) )
        return FALSE;

    m_btn->Enable(enable);
    m_text->Enable(enable);

    return TRUE;
}

bool wxComboControl::Show(bool show)
{
    if ( !wxControl::Show(show) )
        return FALSE;

    m_btn->Show(show);
    m_text->Show(show);

    return TRUE;
}

// ----------------------------------------------------------------------------
// popup window handling
// ----------------------------------------------------------------------------

void wxComboControl::RemoveEventHandler()
{
    if ( m_popup )
    {
        // remove our event handler from it as it won't be used with a combobox
        // any longer
        m_popup->GetControl()->PopEventHandler(TRUE /* delete it */);
    }
}

void wxComboControl::SetPopupControl(wxComboPopup *popup)
{
    RemoveEventHandler();

    m_popup = popup;
    wxControl *control = m_popup->GetControl();
    control->PushEventHandler(new wxComboPopupEventHandler(this));
    control->Hide();
}

void wxComboControl::ShowPopup()
{
    wxCHECK_RET( m_popup, _T("no popup to show in wxComboControl") );

    wxControl *control = m_popup->GetControl();

    // position the control below the combo
    wxPoint ptCombo = ClientToScreen(wxPoint(0, 0));
    wxSize sizeCombo = GetSize();

    // FIXME: should check that the combo doesn't popup beyond the screen
    //        (can we share this code with menus? we should...)
    control->SetSize(ptCombo.x, ptCombo.y + sizeCombo.y,
                     sizeCombo.x, m_heightPopup);

    // show it
    control->Show();
    control->SetFocus();
    control->CaptureMouse();
    m_popup->SetSelection(m_text->GetValue());
}

void wxComboControl::HidePopup()
{
    wxCHECK_RET( m_popup, _T("no popup to hide in wxComboControl") );

    wxControl *control = m_popup->GetControl();
    control->ReleaseMouse();
    control->Hide();
}

void wxComboControl::OnSelect(const wxString& value)
{
    HidePopup();
    m_text->SetValue(value);
    m_text->SetFocus();
}

void wxComboControl::OnDismiss()
{
    HidePopup();
    m_text->SetFocus();
}

// ----------------------------------------------------------------------------
// wxComboPopupEventHandler
// ----------------------------------------------------------------------------

bool wxComboPopupEventHandler::ProcessEvent(wxEvent& event)
{
    bool dismiss;

    switch ( event.GetEventType() )
    {
        case wxEVT_LEFT_DOWN:
            // clicking outside the popup window makesi t disappear
            {
                wxCoord x, y;
                ((wxMouseEvent &)event).GetPosition(&x, &y);

                dismiss = (x < 0) || (y < 0);
                if ( !dismiss )
                {
                    // check the right/bottom bounds too
                    wxCoord w, h;
                    ((wxWindow *)event.GetEventObject())->GetSize(&w, &h);
                    dismiss = (x > w) || (y > h);
                }
            }
            break;

        case wxEVT_KILL_FOCUS:
            // as well as losing the focus in any other way (i.e. using
            // keyboard)
            dismiss = TRUE;
            break;

        default:
            dismiss = FALSE;
    }

    if ( dismiss )
    {
        // clicking outside the window dismisses the popup
        m_combo->OnDismiss();

        return TRUE;
    }

    event.Skip();

    return wxEvtHandler::ProcessEvent(event);
}

// ----------------------------------------------------------------------------
// wxComboListBox
// ----------------------------------------------------------------------------

wxComboListBox::wxComboListBox(wxComboControl *combo)
              : wxListBox(combo->GetParent(), -1,
                          wxDefaultPosition, wxDefaultSize,
                          0, NULL,
                          wxBORDER_SIMPLE | wxPOPUP_WINDOW),
                wxComboPopup(combo)
{
}

bool wxComboListBox::SetSelection(const wxString& value)
{
    int i = FindString(value);
    if ( i == wxNOT_FOUND )
        return FALSE;

    wxListBox::SetSelection(i);
    wxListBox::EnsureVisible(i);

    return TRUE;
}

void wxComboListBox::OnSelect(wxCommandEvent& event)
{
    m_combo->OnSelect(event.GetString());
}

void wxComboListBox::OnShow()
{
}

// ----------------------------------------------------------------------------
// wxComboBox
// ----------------------------------------------------------------------------

void wxComboBox::Init()
{
    m_lbox = (wxListBox *)NULL;
}

bool wxComboBox::Create(wxWindow *parent,
                        wxWindowID id,
                        const wxString& value,
                        const wxPoint& pos,
                        const wxSize& size,
                        int n,
                        const wxString *choices,
                        long style,
                        const wxValidator& validator,
                        const wxString& name)
{
    if ( !wxComboControl::Create(parent, id, value, pos, size, style,
                                 validator, name) )
    {
        return FALSE;
    }

    wxComboListBox *combolbox = new wxComboListBox(this);
    m_lbox = combolbox;
    m_lbox->Set(n, choices);

    SetPopupControl(combolbox);

    return TRUE;
}

// ----------------------------------------------------------------------------
// wxComboBox methods forwarded to wxTextCtrl
// ----------------------------------------------------------------------------

void wxComboBox::Copy()
{
    GetText()->Copy();
}

void wxComboBox::Cut()
{
    GetText()->Cut();
}

void wxComboBox::Paste()
{
    GetText()->Paste();
}

void wxComboBox::SetInsertionPoint(long pos)
{
    GetText()->SetInsertionPoint(pos);
}

void wxComboBox::SetInsertionPointEnd()
{
    GetText()->SetInsertionPointEnd();
}

long wxComboBox::GetInsertionPoint() const
{
    return GetText()->GetInsertionPoint();
}

long wxComboBox::GetLastPosition() const
{
    return GetText()->GetLastPosition();
}

void wxComboBox::Replace(long from, long to, const wxString& value)
{
    GetText()->Replace(from, to, value);
}

void wxComboBox::Remove(long from, long to)
{
    GetText()->Remove(from, to);
}

void wxComboBox::SetSelection(long from, long to)
{
    GetText()->SetSelection(from, to);
}

void wxComboBox::SetEditable(bool editable)
{
    GetText()->SetEditable(editable);
}

// ----------------------------------------------------------------------------
// wxComboBox methods forwarded to wxListBox
// ----------------------------------------------------------------------------

void wxComboBox::Clear()
{
    GetLBox()->Clear();
}

void wxComboBox::Delete(int n)
{
    GetLBox()->Delete(n);
}

int wxComboBox::GetCount() const
{
    return GetLBox()->GetCount();
}

wxString wxComboBox::GetString(int n) const
{
    return GetLBox()->GetString(n);
}

void wxComboBox::SetString(int n, const wxString& s)
{
    GetLBox()->SetString(n, s);
}

int wxComboBox::FindString(const wxString& s) const
{
    return GetLBox()->FindString(s);
}

void wxComboBox::Select(int n)
{
    wxCHECK_RET( (n >= 0) && (n < GetCount()), _T("invalid combobox index") );

    GetLBox()->SetSelection(n);
    GetText()->SetValue(GetLBox()->GetString(n));
}

int wxComboBox::GetSelection() const
{
    // the listbox probably doesn't remember its selection when popped down?
    wxFAIL_MSG(_T("TODO"));

    return GetLBox()->GetSelection();
}

int wxComboBox::DoAppend(const wxString& item)
{
    return GetLBox()->Append(item);
}

void wxComboBox::DoSetItemClientData(int n, void* clientData)
{
    GetLBox()->SetClientData(n, clientData);
}

void *wxComboBox::DoGetItemClientData(int n) const
{
    return GetLBox()->GetClientData(n);
}

void wxComboBox::DoSetItemClientObject(int n, wxClientData* clientData)
{
    GetLBox()->SetClientObject(n, clientData);
}

wxClientData* wxComboBox::DoGetItemClientObject(int n) const
{
    return GetLBox()->GetClientObject(n);
}

#endif // wxUSE_COMBOBOX
