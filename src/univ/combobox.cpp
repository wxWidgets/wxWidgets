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

/*
    TODO:

   +1. typing in the text should select the string in listbox
   +2. scrollbars in listbox are unusable
   +3. the initially selected item is not selected
   ?4. kbd interface (what does GTK do?)
    5. there is still autoscrolling without scrollbars - but is it bad?
 */

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

#include "wx/popupwin.h"

#include "wx/univ/renderer.h"
#include "wx/univ/inphand.h"
#include "wx/univ/theme.h"

/*
   The keyboard event flow:

   1. they always come to the text ctrl
   2. it forwards the ones it doesn't process to the wxComboControl
   3. which passes them to the popup window if it is popped up
 */

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
    // ctor and dtor
    wxComboListBox(wxComboControl *combo);
    virtual ~wxComboListBox();

    // implement wxComboPopup methods
    virtual bool SetSelection(const wxString& value);
    virtual wxControl *GetControl() { return this; }
    virtual void OnShow();

protected:
    // filter mouse move events happening outside the list box
    void OnMouseMove(wxMouseEvent& event);

    // called whenever the user selects or activates a listbox item
    void OnSelect(wxCommandEvent& event);

    // used to process wxUniv actions
    bool PerformAction(const wxControlAction& action,
                       long numArg,
                       const wxString& strArg);

private:
    DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// wxComboTextCtrl is a simple text ctrl which forwards
// wxEVT_COMMAND_TEXT_UPDATED events and all key events to the combobox
// ----------------------------------------------------------------------------

class wxComboTextCtrl : public wxTextCtrl
{
public:
    wxComboTextCtrl(wxComboControl *combo,
                    const wxString& value,
                    long style,
                    const wxValidator& validator);

protected:
    void OnKey(wxKeyEvent& event);
    void OnText(wxCommandEvent& event);

private:
    wxComboControl *m_combo;

    DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// event tables and such
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxComboButton, wxButton)
    EVT_BUTTON(-1, wxComboButton::OnButton)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(wxComboListBox, wxListBox)
    EVT_LISTBOX(-1, wxComboListBox::OnSelect)
    EVT_LISTBOX_DCLICK(-1, wxComboListBox::OnSelect)
    EVT_MOTION(wxComboListBox::OnMouseMove)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(wxComboControl, wxControl)
    EVT_KEY_DOWN(wxComboControl::OnKey)
    EVT_KEY_UP(wxComboControl::OnKey)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(wxComboTextCtrl, wxTextCtrl)
    EVT_KEY_DOWN(wxComboTextCtrl::OnKey)
    EVT_KEY_UP(wxComboTextCtrl::OnKey)
    EVT_TEXT(-1, wxComboTextCtrl::OnText)
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
    m_winPopup = (wxPopupComboWindow *)NULL;
    m_isPopupShown = FALSE;
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
    m_text = new wxComboTextCtrl(this,
                                 value,
                                 style & wxCB_READONLY ? wxTE_READONLY : 0,
                                 validator);

    // for compatibility with the other ports, the height specified is the
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

    // create the popup window immediately here to allow creating the controls
    // with parent == GetPopupWindow() from the derived class ctor
    m_winPopup = new wxPopupComboWindow(this);

    // have to disable this window to avoid interfering it with message
    // processing to the text and the button... but pretend it is enabled to
    // make IsEnabled() return TRUE
    wxControl::Enable(FALSE); // don't use non virtual Disable() here!
    m_isEnabled = TRUE;

    CreateInputHandler(wxINP_HANDLER_COMBOBOX);

    return TRUE;
}

wxComboControl::~wxComboControl()
{
}

// ----------------------------------------------------------------------------
// geometry stuff
// ----------------------------------------------------------------------------

void wxComboControl::DoSetSize(int x, int y,
                               int width, int height,
                               int sizeFlags)
{
    // combo height is always fixed
    wxControl::DoSetSize(x, y, width, DoGetBestSize().y, sizeFlags);
}

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

void wxComboControl::SetPopupControl(wxComboPopup *popup)
{
    m_popup = popup;
}

void wxComboControl::ShowPopup()
{
    wxCHECK_RET( m_popup, _T("no popup to show in wxComboControl") );
    wxCHECK_RET( !IsPopupShown(), _T("popup window already shown") );

    wxControl *control = m_popup->GetControl();

    // size and position the popup window correctly
    m_winPopup->SetSize(GetSize().x,
                        m_heightPopup == -1 ? control->GetBestSize().y
                                            : m_heightPopup);
    wxSize sizePopup = m_winPopup->GetClientSize();
    control->SetSize(0, 0, sizePopup.x, sizePopup.y);

    // some controls don't accept the size we give then: e.g. a listbox may
    // require more space to show its last row
    wxSize sizeReal = control->GetSize();
    if ( sizeReal != sizePopup )
    {
        m_winPopup->SetClientSize(sizeReal);
    }

    m_winPopup->Position();

    // show it
    m_winPopup->Popup(m_text);
    m_text->SelectAll();
    m_popup->SetSelection(m_text->GetValue());

    m_isPopupShown = TRUE;
}

void wxComboControl::HidePopup()
{
    wxCHECK_RET( m_popup, _T("no popup to hide in wxComboControl") );
    wxCHECK_RET( IsPopupShown(), _T("popup window not shown") );

    m_winPopup->Dismiss();

    m_isPopupShown = FALSE;
}

void wxComboControl::OnSelect(const wxString& value)
{
    m_text->SetValue(value);

    OnDismiss();
}

void wxComboControl::OnDismiss()
{
    HidePopup();
    m_text->SetFocus();
}

// ----------------------------------------------------------------------------
// wxComboTextCtrl
// ----------------------------------------------------------------------------

wxComboTextCtrl::wxComboTextCtrl(wxComboControl *combo,
                                 const wxString& value,
                                 long style,
                                 const wxValidator& validator)
               : wxTextCtrl(combo->GetParent(), -1, value,
                            wxDefaultPosition, wxDefaultSize,
                            wxBORDER_NONE | style,
                            validator)
{
    m_combo = combo;
}

void wxComboTextCtrl::OnText(wxCommandEvent& event)
{
    if ( m_combo->IsPopupShown() )
    {
        m_combo->GetPopupControl()->SetSelection(GetValue());
    }

    // there is a small incompatibility with wxMSW here: the combobox gets the
    // event before the text control in our case which corresponds to SMW
    // CBN_EDITUPDATE notification and not CBN_EDITCHANGE one wxMSW currently
    // uses
    //
    // if this is really a problem, we can play games with the event handlers
    // to circumvent this
    (void)m_combo->ProcessEvent(event);

    event.Skip();
}

// pass the keys we don't process to the combo first
void wxComboTextCtrl::OnKey(wxKeyEvent& event)
{
    switch ( event.GetKeyCode() )
    {
        case WXK_RETURN:
            // the popup control gets it first but only if it is shown
            if ( !m_combo->IsPopupShown() )
                break;
            //else: fall through

        case WXK_UP:
        case WXK_DOWN:
        case WXK_ESCAPE:
        case WXK_PAGEDOWN:
        case WXK_PAGEUP:
        case WXK_PRIOR:
        case WXK_NEXT:
            (void)m_combo->ProcessEvent(event);
            return;
    }

    event.Skip();
}

// ----------------------------------------------------------------------------
// wxComboListBox
// ----------------------------------------------------------------------------

wxComboListBox::wxComboListBox(wxComboControl *combo)
              : wxListBox(combo->GetPopupWindow(), -1,
                          wxDefaultPosition, wxDefaultSize,
                          0, NULL,
                          wxBORDER_SIMPLE | wxLB_INT_HEIGHT),
                wxComboPopup(combo)
{
    // we don't react to the mouse events outside the window at all
    StopAutoScrolling();
}

wxComboListBox::~wxComboListBox()
{
}

bool wxComboListBox::SetSelection(const wxString& value)
{
    // FindItem() would just find the current item for an empty string (it
    // always matches), but we want to show the first one in such case
    if ( value.empty() )
    {
        if ( GetCount() )
        {
            wxListBox::SetSelection(0);
        }
        //else: empty listbox - nothing to do
    }
    else if ( !FindItem(value) )
    {
        // no match att all
        return FALSE;
    }

    return TRUE;
}

void wxComboListBox::OnSelect(wxCommandEvent& event)
{
    // first let the user code have the event

    // all fields are already filled by the listbox, just change the event
    // type and send it to the combo
    wxCommandEvent event2 = (wxCommandEvent &)event;
    event2.SetEventType(wxEVT_COMMAND_COMBOBOX_SELECTED);
    m_combo->ProcessEvent(event2);

    // next update the combo and close the listbox
    m_combo->OnSelect(event.GetString());
}

void wxComboListBox::OnShow()
{
}

bool wxComboListBox::PerformAction(const wxControlAction& action,
                                   long numArg,
                                   const wxString& strArg)

{
    if ( action == wxACTION_LISTBOX_FIND )
    {
        // we don't let the listbox handle this as instead of just using the
        // single key presses, as usual, we use the text ctrl value as prefix
        // and this is done by wxComboControl itself
        return TRUE;
    }

    return wxListBox::PerformAction(action, numArg, strArg);
}

void wxComboListBox::OnMouseMove(wxMouseEvent& event)
{
    // while a wxComboListBox is shown, it always has capture, so if it doesn't
    // we're about to go away anyhow (normally this shouldn't happen at all,
    // but I don't put assert here as it just might do on other platforms and
    // it doesn't break anythign anyhow)
    if ( this == wxWindow::GetCapture() )
    {
        if ( HitTest(event.GetPosition()) == wxHT_WINDOW_INSIDE )
        {
            event.Skip();
        }
        //else: popup shouldn't react to the mouse motions outside it, it only
        //      captures the mouse to be able to detect when it must be
        //      dismissed, so don't call Skip()
    }
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

wxComboBox::~wxComboBox()
{
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

// ----------------------------------------------------------------------------
// input handling
// ----------------------------------------------------------------------------

void wxComboControl::OnKey(wxCommandEvent& event)
{
    if ( m_isPopupShown )
    {
        // pass it to the popped up control
        (void)m_popup->GetControl()->ProcessEvent(event);
    }
    else // no popup
    {
        event.Skip();
    }
}

bool wxComboControl::PerformAction(const wxControlAction& action,
                                   long numArg,
                                   const wxString& strArg)
{
    bool processed = FALSE;
    if ( action == wxACTION_COMBOBOX_POPUP )
    {
        if ( !m_isPopupShown )
        {
            ShowPopup();

            processed = TRUE;
        }
    }
    else if ( action == wxACTION_COMBOBOX_DISMISS )
    {
        if ( m_isPopupShown )
        {
            HidePopup();

            processed = TRUE;
        }
    }

    if ( !processed )
    {
        // pass along
        return wxControl::PerformAction(action, numArg, strArg);
    }

    return TRUE;
}

// ----------------------------------------------------------------------------
// wxStdComboBoxInputHandler
// ----------------------------------------------------------------------------

wxStdComboBoxInputHandler::wxStdComboBoxInputHandler(wxInputHandler *inphand)
                         : wxStdInputHandler(inphand)
{
}

bool wxStdComboBoxInputHandler::HandleKey(wxControl *control,
                                          const wxKeyEvent& event,
                                          bool pressed)
{
    if ( pressed )
    {
        wxControlAction action;
        switch ( event.GetKeyCode() )
        {
            case WXK_DOWN:
                action = wxACTION_COMBOBOX_POPUP;
                break;

            case WXK_ESCAPE:
                action = wxACTION_COMBOBOX_DISMISS;
                break;
        }

        if ( !!action )
        {
            control->PerformAction(action);

            return TRUE;
        }
    }

    return wxStdInputHandler::HandleKey(control, event, pressed);
}

#endif // wxUSE_COMBOBOX
