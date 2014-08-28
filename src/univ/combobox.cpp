/////////////////////////////////////////////////////////////////////////////
// Name:        src/univ/combobox.cpp
// Purpose:     wxComboBox implementation
// Author:      Vadim Zeitlin
// Modified by:
// Created:     15.12.00
// Copyright:   (c) 2000 SciTech Software, Inc. (www.scitechsoft.com)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

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
#include "wx/combo.h"

#include "wx/univ/renderer.h"
#include "wx/univ/inphand.h"
#include "wx/univ/theme.h"

// ----------------------------------------------------------------------------
// wxStdComboBoxInputHandler: allows the user to open/close the combo from kbd
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxStdComboBoxInputHandler : public wxStdInputHandler
{
public:
    wxStdComboBoxInputHandler(wxInputHandler *inphand);

    virtual bool HandleKey(wxInputConsumer *consumer,
                           const wxKeyEvent& event,
                           bool pressed);
};

// ----------------------------------------------------------------------------
// wxComboListBox is a listbox modified to be used as a popup window in a
// combobox
// ----------------------------------------------------------------------------

class wxComboListBox : public wxListBox, public wxComboPopup
{
public:
    // ctor and dtor
    wxComboListBox();
    virtual ~wxComboListBox();

    // implement wxComboPopup methods
    virtual bool Create(wxWindow* parent);
    virtual void SetStringValue(const wxString& s);
    virtual wxString GetStringValue() const;
    virtual wxWindow *GetControl() { return this; }
    virtual void OnPopup();
    virtual wxSize GetAdjustedSize(int minWidth, int prefHeight, int maxHeight);

    // fix virtual function hiding
    virtual void SetSelection(int n) { DoSetSelection(n, true); }
    void SetSelection(int n, bool select) { DoSetSelection(n, select); }

    // used to process wxUniv actions
    bool PerformAction(const wxControlAction& action,
                       long numArg,
                       const wxString& strArg);

protected:
    // set m_clicked value from here
    void OnLeftUp(wxMouseEvent& event);

private:
    friend class wxComboBox; // it accesses our DoGetItemClientData()

    DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// event tables and such
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxComboListBox, wxListBox)
    EVT_LEFT_UP(wxComboListBox::OnLeftUp)
END_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxComboListBox
// ----------------------------------------------------------------------------

wxComboListBox::wxComboListBox() : wxListBox(), wxComboPopup()
{
}

bool wxComboListBox::Create(wxWindow* parent)
{
    if ( !wxListBox::Create(parent, wxID_ANY,
                            wxDefaultPosition, wxDefaultSize,
                            0, NULL,
                            wxBORDER_SIMPLE |
                            ( m_combo->GetWindowStyle() & wxCB_SORT ? wxLB_SORT : 0 ) ) )
        return false;

    // we don't react to the mouse events outside the window at all
    StopAutoScrolling();

    return true;
}

wxComboListBox::~wxComboListBox()
{
}

wxString wxComboListBox::GetStringValue() const
{
    return wxListBox::GetStringSelection();
}

void wxComboListBox::SetStringValue(const wxString& value)
{
    if ( !value.empty() )
    {
        if (FindString(value) != wxNOT_FOUND)
            wxListBox::SetStringSelection(value);
    }
    else
        wxListBox::SetSelection(-1);
}

void wxComboListBox::OnPopup()
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
        // and this is done by wxComboCtrl itself
        return true;
    }

    return wxListBox::PerformAction(action, numArg, strArg);
}

void wxComboListBox::OnLeftUp(wxMouseEvent& event)
{
    // we should dismiss the combo now
    // first update the combo and close the listbox
    Dismiss();
    m_combo->SetValue(wxListBox::GetStringSelection());

    // next let the user code have the event
    wxCommandEvent evt(wxEVT_COMBOBOX,m_combo->GetId());
    evt.SetInt(wxListBox::GetSelection());
    evt.SetEventObject(m_combo);
    m_combo->ProcessWindowEvent(evt);

    event.Skip();
}

wxSize wxComboListBox::GetAdjustedSize(int minWidth,
                                       int WXUNUSED(prefHeight),
                                       int maxHeight)
{
    wxSize bestSize = wxListBox::GetBestSize();
    return wxSize(wxMax(bestSize.x,minWidth),
                  wxMin(bestSize.y,maxHeight));
}

// ----------------------------------------------------------------------------
// wxComboBox
// ----------------------------------------------------------------------------

void wxComboBox::Init()
{
    m_lbox = NULL;
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
    if ( !wxComboCtrl::Create(parent, id, value, pos, size, style,
                                 validator, name) )
    {
        return false;
    }

    wxComboListBox *combolbox = new wxComboListBox();
    SetPopupControl(combolbox);

    m_lbox = combolbox;
    m_lbox->Set(n, choices);

    return true;
}

wxComboBox::~wxComboBox()
{
}

// ----------------------------------------------------------------------------
// wxComboBox methods forwarded to wxTextCtrl
// ----------------------------------------------------------------------------

wxString wxComboBox::DoGetValue() const
{
    return GetTextCtrl() ? GetTextCtrl()->GetValue() : m_valueString;
}

void wxComboBox::SetValue(const wxString& value)
{
    if ( GetTextCtrl() )
        GetTextCtrl()->SetValue(value);
    else
        m_valueString = value;
}

void wxComboBox::WriteText(const wxString& value)
{
    if ( GetTextCtrl() ) GetTextCtrl()->WriteText(value);
}

void wxComboBox::Copy()
{
    if ( GetTextCtrl() ) GetTextCtrl()->Copy();
}

void wxComboBox::Cut()
{
    if ( GetTextCtrl() ) GetTextCtrl()->Cut();
}

void wxComboBox::Paste()
{
    if ( GetTextCtrl() ) GetTextCtrl()->Paste();
}

void wxComboBox::SetInsertionPoint(long pos)
{
    if ( GetTextCtrl() ) GetTextCtrl()->SetInsertionPoint(pos);
}

void wxComboBox::SetInsertionPointEnd()
{
    if ( GetTextCtrl() ) GetTextCtrl()->SetInsertionPointEnd();
}

long wxComboBox::GetInsertionPoint() const
{
    if ( GetTextCtrl() )
        return GetTextCtrl()->GetInsertionPoint();
    return -1;
}

wxTextPos wxComboBox::GetLastPosition() const
{
    if ( GetTextCtrl() )
        return GetTextCtrl()->GetLastPosition();
    return -1;
}

void wxComboBox::Replace(long from, long to, const wxString& value)
{
    if ( GetTextCtrl() ) GetTextCtrl()->Replace(from, to, value);
}

void wxComboBox::Remove(long from, long to)
{
    if ( GetTextCtrl() ) GetTextCtrl()->Remove(from, to);
}

void wxComboBox::SetSelection(long from, long to)
{
    if ( GetTextCtrl() ) GetTextCtrl()->SetSelection(from, to);
}

void wxComboBox::GetSelection(long *from, long *to) const
{
    if ( GetTextCtrl() ) GetTextCtrl()->GetSelection(from, to);
}

void wxComboBox::SetEditable(bool editable)
{
    if ( GetTextCtrl() ) GetTextCtrl()->SetEditable(editable);
}

// ----------------------------------------------------------------------------
// wxComboBox methods forwarded to wxListBox
// ----------------------------------------------------------------------------

void wxComboBox::DoClear()
{
    GetLBox()->Clear();
    SetValue(wxEmptyString);
}

void wxComboBox::DoDeleteOneItem(unsigned int n)
{
    wxCHECK_RET( IsValid(n), wxT("invalid index in wxComboBox::Delete") );

    if (GetSelection() == (int)n)
        SetValue(wxEmptyString);

    GetLBox()->Delete(n);
}

unsigned int wxComboBox::GetCount() const
{
    return GetLBox()->GetCount();
}

wxString wxComboBox::GetString(unsigned int n) const
{
    wxCHECK_MSG( IsValid(n), wxEmptyString, wxT("invalid index in wxComboBox::GetString") );

    return GetLBox()->GetString(n);
}

void wxComboBox::SetString(unsigned int n, const wxString& s)
{
    wxCHECK_RET( IsValid(n), wxT("invalid index in wxComboBox::SetString") );

    GetLBox()->SetString(n, s);
}

int wxComboBox::FindString(const wxString& s, bool bCase) const
{
    return GetLBox()->FindString(s, bCase);
}

void wxComboBox::SetSelection(int n)
{
    wxCHECK_RET( (n == wxNOT_FOUND || IsValid(n)), wxT("invalid index in wxComboBox::Select") );

    GetLBox()->SetSelection(n);

    wxString str;
    if ( n != wxNOT_FOUND )
        str = GetLBox()->GetString(n);

    SetText(str);
}

int wxComboBox::GetSelection() const
{
#if 1 // FIXME:: What is the correct behaviour?
    // if the current value isn't one of the listbox strings, return -1
    return GetLBox()->GetSelection();
#else
    // Why oh why is this done this way?
    // It is not because the value displayed in the text can be found
    // in the list that it is the item that is selected!
    return FindString(if ( GetTextCtrl() ) GetTextCtrl()->GetValue());
#endif
}

wxString wxComboBox::GetStringSelection() const
{
    return GetLBox()->GetStringSelection();
}

wxClientDataType wxComboBox::GetClientDataType() const
{
    return GetLBox()->GetClientDataType();
}

void wxComboBox::SetClientDataType(wxClientDataType clientDataItemsType)
{
    GetLBox()->SetClientDataType(clientDataItemsType);
}

int wxComboBox::DoInsertItems(const wxArrayStringsAdapter & items,
                              unsigned int pos,
                              void **clientData, wxClientDataType type)
{
    return GetLBox()->DoInsertItems(items, pos, clientData, type);
}

void wxComboBox::DoSetItemClientData(unsigned int n, void* clientData)
{
    GetLBox()->DoSetItemClientData(n, clientData);
}

void *wxComboBox::DoGetItemClientData(unsigned int n) const
{
    return GetLBox()->DoGetItemClientData(n);
}

bool wxComboBox::IsEditable() const
{
    return GetTextCtrl() != NULL && (!HasFlag(wxCB_READONLY) || GetTextCtrl()->IsEditable() );
}

void wxComboBox::Undo()
{
    if (IsEditable())
        if ( GetTextCtrl() ) GetTextCtrl()->Undo();
}

void wxComboBox::Redo()
{
    if (IsEditable())
        if ( GetTextCtrl() ) GetTextCtrl()->Redo();
}

void wxComboBox::SelectAll()
{
    if ( GetTextCtrl() ) GetTextCtrl()->SelectAll();
}

bool wxComboBox::CanCopy() const
{
    if (GetTextCtrl() != NULL)
        return GetTextCtrl()->CanCopy();
    else
        return false;
}

bool wxComboBox::CanCut() const
{
    if (GetTextCtrl() != NULL)
        return GetTextCtrl()->CanCut();
    else
        return false;
}

bool wxComboBox::CanPaste() const
{
    if (IsEditable())
        return GetTextCtrl()->CanPaste();
    else
        return false;
}

bool wxComboBox::CanUndo() const
{
    if (IsEditable())
        return GetTextCtrl()->CanUndo();
    else
        return false;
}

bool wxComboBox::CanRedo() const
{
    if (IsEditable())
        return GetTextCtrl()->CanRedo();
    else
        return false;
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

/* static */
wxInputHandler *wxComboBox::GetStdInputHandler(wxInputHandler *handlerDef)
{
    static wxStdComboBoxInputHandler s_handler(handlerDef);

    return &s_handler;
}

#endif // wxUSE_COMBOBOX
