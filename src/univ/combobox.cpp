/////////////////////////////////////////////////////////////////////////////
// Name:        univ/combobox.cpp
// Purpose:     wxComboControl and wxComboBox implementation
// Author:      Vadim Zeitlin
// Modified by:
// Created:     15.12.00
// RCS-ID:      $Id$
// Copyright:   (c) 2000 SciTech Software, Inc. (www.scitechsoft.com)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
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
    #include "wx/bmpbuttn.h"

    #include "wx/validate.h"
#endif

#include "wx/tooltip.h"
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

// constants
// ----------------------------------------------------------------------------

// the margin between the text control and the combo button
static const wxCoord g_comboMargin = 2;

// ----------------------------------------------------------------------------
// wxComboButton is just a normal button except that it sends commands to the
// combobox and not its parent
// ----------------------------------------------------------------------------

class wxComboButton : public wxBitmapButton
{
public:
    wxComboButton(wxComboControl *combo)
        : wxBitmapButton(combo->GetParent(), wxID_ANY, wxNullBitmap,
                         wxDefaultPosition, wxDefaultSize,
                         wxBORDER_NONE | wxBU_EXACTFIT)
    {
        m_combo = combo;

        wxBitmap bmpNormal, bmpFocus, bmpPressed, bmpDisabled;

        GetRenderer()->GetComboBitmaps(&bmpNormal,
                                       &bmpFocus,
                                       &bmpPressed,
                                       &bmpDisabled);

        SetBitmapLabel(bmpNormal);
        SetBitmapFocus(bmpFocus.Ok() ? bmpFocus : bmpNormal);
        SetBitmapSelected(bmpPressed.Ok() ? bmpPressed : bmpNormal);
        SetBitmapDisabled(bmpDisabled.Ok() ? bmpDisabled : bmpNormal);

        SetBestSize(wxDefaultSize);
    }

protected:
    void OnButton(wxCommandEvent& WXUNUSED(event)) { m_combo->ShowPopup(); }

    virtual wxSize DoGetBestClientSize() const
    {
        const wxBitmap& bmp = GetBitmapLabel();

        return wxSize(bmp.GetWidth(), bmp.GetHeight());

    }

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
    wxComboListBox(wxComboControl *combo, int style = 0);
    virtual ~wxComboListBox();

    // implement wxComboPopup methods
    virtual bool SetSelection(const wxString& value);
    virtual void SetSelection(int n, bool select)
        { wxListBox::SetSelection( n, select); };
    virtual wxControl *GetControl() { return this; }
    virtual void OnShow();
    virtual wxCoord GetBestWidth() const;

protected:
    // we shouldn't return height too big from here
    virtual wxSize DoGetBestClientSize() const;

    // filter mouse move events happening outside the list box
    void OnMouseMove(wxMouseEvent& event);

    // set m_clicked value from here
    void OnLeftUp(wxMouseEvent& event);

    // called whenever the user selects or activates a listbox item
    void OnSelect(wxCommandEvent& event);

    // used to process wxUniv actions
    bool PerformAction(const wxControlAction& action,
                       long numArg,
                       const wxString& strArg);

private:
    // has the mouse been released on this control?
    bool m_clicked;

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
    EVT_BUTTON(wxID_ANY, wxComboButton::OnButton)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(wxComboListBox, wxListBox)
    EVT_LISTBOX(wxID_ANY, wxComboListBox::OnSelect)
    EVT_LISTBOX_DCLICK(wxID_ANY, wxComboListBox::OnSelect)
    EVT_MOTION(wxComboListBox::OnMouseMove)
    EVT_LEFT_UP(wxComboListBox::OnLeftUp)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(wxComboControl, wxControl)
    EVT_KEY_DOWN(wxComboControl::OnKey)
    EVT_KEY_UP(wxComboControl::OnKey)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(wxComboTextCtrl, wxTextCtrl)
    EVT_KEY_DOWN(wxComboTextCtrl::OnKey)
    EVT_KEY_UP(wxComboTextCtrl::OnKey)
    EVT_TEXT(wxID_ANY, wxComboTextCtrl::OnText)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(wxComboBox, wxControl)

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
    m_isPopupShown = false;
    m_btn = NULL;
    m_text = NULL;
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
        return false;

    // create the text control and the button as our siblings (*not* children),
    // don't care about size/position here - they will be set in DoMoveWindow()
    m_btn = new wxComboButton(this);
    m_text = new wxComboTextCtrl(this,
                                 value,
                                 style & wxCB_READONLY ? wxTE_READONLY : 0,
                                 validator);

    // for compatibility with the other ports, the height specified is the
    // combined height of the combobox itself and the popup
    if ( size.y == wxDefaultCoord )
    {
        // ok, use default height for popup too
        m_heightPopup = wxDefaultCoord;
    }
    else
    {
        m_heightPopup = size.y - DoGetBestSize().y;
    }

    SetBestSize(size);
    Move(pos);

    // create the popup window immediately here to allow creating the controls
    // with parent == GetPopupWindow() from the derived class ctor
    m_winPopup = new wxPopupComboWindow(this);

    // have to disable this window to avoid interfering it with message
    // processing to the text and the button... but pretend it is enabled to
    // make IsEnabled() return true
    wxControl::Enable(false); // don't use non virtual Disable() here!
    m_isEnabled = true;

    CreateInputHandler(wxINP_HANDLER_COMBOBOX);

    return true;
}

wxComboControl::~wxComboControl()
{
    // as the button and the text control are the parent's children and not
    // ours, we have to delete them manually - they are not deleted
    // automatically by wxWidgets when we're deleted
    delete m_btn;
    delete m_text;

    delete m_winPopup;
}

// ----------------------------------------------------------------------------
// geometry stuff
// ----------------------------------------------------------------------------

void wxComboControl::DoSetSize(int x, int y,
                               int width, int WXUNUSED(height),
                               int sizeFlags)
{
    // combo height is always fixed
    wxControl::DoSetSize(x, y, width, DoGetBestSize().y, sizeFlags);
}

wxSize wxComboControl::DoGetBestClientSize() const
{
    wxSize sizeBtn = m_btn->GetBestSize(),
           sizeText = m_text->GetBestSize();
    wxCoord widthPopup = 0;

    if (m_popup)
    {
        widthPopup = m_popup->GetBestWidth();
    }

    return wxSize(wxMax(sizeText.x + g_comboMargin + sizeBtn.x, widthPopup), 
                  wxMax(sizeBtn.y, sizeText.y));
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

    wxSize sizeBtn = m_btn->GetBestSize();

    wxCoord wText = width - sizeBtn.x;
    wxPoint p = GetParent() ? GetParent()->GetClientAreaOrigin() : wxPoint(0,0);
    m_text->SetSize(x - p.x, y - p.y, wText, height);
    m_btn->SetSize(x - p.x + wText, y - p.y, sizeBtn.x, height);
}

// ----------------------------------------------------------------------------
// operations
// ----------------------------------------------------------------------------

bool wxComboControl::Enable(bool enable)
{
    if ( !wxControl::Enable(enable) )
        return false;

    m_btn->Enable(enable);
    m_text->Enable(enable);

    return true;
}

bool wxComboControl::Show(bool show)
{
    if ( !wxControl::Show(show) )
        return false;

    if (m_btn)
        m_btn->Show(show);

    if (m_text)
        m_text->Show(show);

    return true;
}

#if wxUSE_TOOLTIPS
void wxComboControl::DoSetToolTip(wxToolTip *tooltip)
{
    wxControl::DoSetToolTip(tooltip);    

    // Set tool tip for button and text box
    if (m_text && m_btn)
    {
        if (tooltip)
        {
            const wxString &tip = tooltip->GetTip();
            m_text->SetToolTip(tip);
            m_btn->SetToolTip(tip);
        }
        else
        {
            m_text->SetToolTip(NULL);
            m_btn->SetToolTip(NULL);
        }
    }
}
#endif // wxUSE_TOOLTIPS

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
                        m_heightPopup == wxDefaultCoord ? control->GetBestSize().y
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

    m_winPopup->PositionNearCombo();

    // show it
    m_popup->OnShow();
    m_winPopup->Popup(m_text);
    m_text->SelectAll();
    m_popup->SetSelection(m_text->GetValue());

    m_isPopupShown = true;
}

void wxComboControl::HidePopup()
{
    wxCHECK_RET( m_popup, _T("no popup to hide in wxComboControl") );
    wxCHECK_RET( IsPopupShown(), _T("popup window not shown") );

    m_winPopup->Dismiss();

    m_isPopupShown = false;
}

void wxComboControl::OnSelect(const wxString& value)
{
    m_text->SetValue(value);
    m_text->SelectAll();

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
               : wxTextCtrl(combo->GetParent(), wxID_ANY, value,
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

    // we need to make a copy of the event to have the correct originating
    // object and id
    wxCommandEvent event2 = event;
    event2.SetEventObject(m_combo);
    event2.SetId(m_combo->GetId());

    // there is a small incompatibility with wxMSW here: the combobox gets the
    // event before the text control in our case which corresponds to SMW
    // CBN_EDITUPDATE notification and not CBN_EDITCHANGE one wxMSW currently
    // uses
    //
    // if this is really a problem, we can play games with the event handlers
    // to circumvent this
    (void)m_combo->ProcessEvent(event2);

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

wxComboListBox::wxComboListBox(wxComboControl *combo, int style)
              : wxListBox(combo->GetPopupWindow(), wxID_ANY,
                          wxDefaultPosition, wxDefaultSize,
                          0, NULL,
                          wxBORDER_SIMPLE | wxLB_INT_HEIGHT | style),
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
        return false;
    }

    return true;
}

void wxComboListBox::OnSelect(wxCommandEvent& event)
{
    if ( m_clicked )
    {
        // first update the combo and close the listbox
        m_combo->OnSelect(event.GetString());

        // next let the user code have the event

        // all fields are already filled by the listbox, just change the event
        // type and send it to the combo
        wxCommandEvent event2 = event;
        event2.SetEventType(wxEVT_COMMAND_COMBOBOX_SELECTED);
        event2.SetEventObject(m_combo);
        event2.SetId(m_combo->GetId());
        m_combo->ProcessEvent(event2);
    }
    //else: ignore the events resultign from just moving the mouse initially
}

void wxComboListBox::OnShow()
{
    // nobody clicked us yet
    m_clicked = false;
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
        return true;
    }

    return wxListBox::PerformAction(action, numArg, strArg);
}

void wxComboListBox::OnLeftUp(wxMouseEvent& event)
{
    // we should dismiss the combo now
    m_clicked = true;

    event.Skip();
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

wxCoord wxComboListBox::GetBestWidth() const
{
    wxSize size = wxListBox::GetBestSize();
    return size.x;
}

wxSize wxComboListBox::DoGetBestClientSize() const
{
    // don't return size too big or we risk to not fit on the screen
    wxSize size = wxListBox::DoGetBestClientSize();
    wxCoord hChar = GetCharHeight();

    int nLines = size.y / hChar;

    // 10 is the same limit as used by wxMSW
    if ( nLines > 10 )
    {
        size.y = 10*hChar;
    }

    return size;
}

// ----------------------------------------------------------------------------
// wxComboBox
// ----------------------------------------------------------------------------

void wxComboBox::Init()
{
    m_lbox = (wxListBox *)NULL;
}

wxComboBox::wxComboBox(wxWindow *parent,
                       wxWindowID id,
                       const wxString& value,
                       const wxPoint& pos,
                       const wxSize& size,
                       const wxArrayString& choices,
                       long style,
                       const wxValidator& validator,
                       const wxString& name)
{
    Init();

    Create(parent, id, value, pos, size, choices, style, validator, name);
}

bool wxComboBox::Create(wxWindow *parent,
                        wxWindowID id,
                        const wxString& value,
                        const wxPoint& pos,
                        const wxSize& size,
                        const wxArrayString& choices,
                        long style,
                        const wxValidator& validator,
                        const wxString& name)
{
    wxCArrayString chs(choices);

    return Create(parent, id, value, pos, size, chs.GetCount(),
                  chs.GetStrings(), style, validator, name);
}

bool wxComboBox::Create(wxWindow *parent,
                        wxWindowID id,
                        const wxString& value,
                        const wxPoint& pos,
                        const wxSize& size,
                        int n,
                        const wxString choices[],
                        long style,
                        const wxValidator& validator,
                        const wxString& name)
{
    if ( !wxComboControl::Create(parent, id, value, pos, size, style,
                                 validator, name) )
    {
        return false;
    }

    wxComboListBox *combolbox =
        new wxComboListBox(this, style & wxCB_SORT ? wxLB_SORT : 0);
    m_lbox = combolbox;
    m_lbox->Set(n, choices);

    SetPopupControl(combolbox);

    return true;
}

wxComboBox::~wxComboBox()
{
}

// ----------------------------------------------------------------------------
// wxComboBox methods forwarded to wxTextCtrl
// ----------------------------------------------------------------------------

wxString wxComboBox::GetValue() const
{
    return GetText()->GetValue();
}

void wxComboBox::SetValue(const wxString& value)
{
    GetText()->SetValue(value);
}

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
    GetText()->SetValue(wxEmptyString);
}

void wxComboBox::Delete(int n)
{
    wxCHECK_RET( (n >= 0) && (n < GetCount()), _T("invalid index in wxComboBox::Delete") );

    if (GetSelection() == n)
        GetText()->SetValue(wxEmptyString);

    GetLBox()->Delete(n);
}

int wxComboBox::GetCount() const
{
    return GetLBox()->GetCount();
}

wxString wxComboBox::GetString(int n) const
{
    wxCHECK_MSG( (n >= 0) && (n < GetCount()), wxEmptyString, _T("invalid index in wxComboBox::GetString") );

    return GetLBox()->GetString(n);
}

void wxComboBox::SetString(int n, const wxString& s)
{
    wxCHECK_RET( (n >= 0) && (n < GetCount()), _T("invalid index in wxComboBox::SetString") );

    GetLBox()->SetString(n, s);
}

int wxComboBox::FindString(const wxString& s) const
{
    return GetLBox()->FindString(s);
}

void wxComboBox::Select(int n)
{
    wxCHECK_RET( (n >= 0) && (n < GetCount()), _T("invalid index in wxComboBox::Select") );

    GetLBox()->SetSelection(n);
    GetText()->SetValue(GetLBox()->GetString(n));
}

int wxComboBox::GetSelection() const
{
#if 1 // FIXME:: What is the correct behavior?
    // if the current value isn't one of the listbox strings, return -1
    return GetLBox()->GetSelection();
#else    
    // Why oh why is this done this way? 
    // It is not because the value displayed in the text can be found 
    // in the list that it is the item that is selected!
    return FindString(GetText()->GetValue());
#endif
}

int wxComboBox::DoAppend(const wxString& item)
{
    return GetLBox()->Append(item);
}

int wxComboBox::DoInsert(const wxString& item, int pos)
{
    wxCHECK_MSG(!(GetWindowStyle() & wxCB_SORT), -1, wxT("can't insert into sorted list"));
    wxCHECK_MSG((pos>=0) && (pos<=GetCount()), -1, wxT("invalid index"));

    if (pos == GetCount())
        return DoAppend(item);

    GetLBox()->Insert(item, pos);
    return pos;
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

void wxComboControl::OnKey(wxKeyEvent& event)
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
    bool processed = false;
    if ( action == wxACTION_COMBOBOX_POPUP )
    {
        if ( !m_isPopupShown )
        {
            ShowPopup();

            processed = true;
        }
    }
    else if ( action == wxACTION_COMBOBOX_DISMISS )
    {
        if ( m_isPopupShown )
        {
            HidePopup();

            processed = true;
        }
    }

    if ( !processed )
    {
        // pass along
        return wxControl::PerformAction(action, numArg, strArg);
    }

    return true;
}

// ----------------------------------------------------------------------------
// wxStdComboBoxInputHandler
// ----------------------------------------------------------------------------

wxStdComboBoxInputHandler::wxStdComboBoxInputHandler(wxInputHandler *inphand)
                         : wxStdInputHandler(inphand)
{
}

bool wxStdComboBoxInputHandler::HandleKey(wxInputConsumer *consumer,
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

        if ( !action.IsEmpty() )
        {
            consumer->PerformAction(action);

            return true;
        }
    }

    return wxStdInputHandler::HandleKey(consumer, event, pressed);
}

#endif // wxUSE_COMBOBOX
