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

    #include "wx/combobox.h"
    #include "wx/listbox.h"
    #include "wx/validate.h"
#endif

#include "wx/univ/renderer.h"
#include "wx/univ/inphand.h"
#include "wx/univ/theme.h"

// ----------------------------------------------------------------------------
// event tables and such
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxComboControl, wxControl)
    EVT_BUTTON(-1, wxComboControl::OnButton)
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
    m_ctrlPopup = (wxControl *)NULL;
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
    if ( !wxControl::Create(parent, id, pos, size, style, validator, name) )
        return FALSE;

    // create the text control and the button as our siblings (*not* children),
    // don't care about size/position here - they will be set in DoMoveWindow()
    m_btn = new wxButton(parent, -1, "\\/"); //wxBitmapButton(parent, -1, bmp);
    m_text = new wxTextCtrl(parent, -1, value,
                            wxDefaultPosition, wxDefaultSize,
                            style & wxCB_READONLY ? wxTE_READONLY : 0,
                            validator);

    return TRUE;
}

wxComboControl::~wxComboControl()
{
    delete m_ctrlPopup;
}

// ----------------------------------------------------------------------------
// geometry stuff
// ----------------------------------------------------------------------------

wxSize wxComboControl::DoGetBestSize() const
{
    wxSize sizeBtn = m_btn->GetBestSize(),
           sizeText = m_text->GetBestSize();

    return wxSize(sizeBtn.x + sizeText.x, wxMax(sizeBtn.y, sizeText.y));
}

void wxComboControl::DoMoveWindow(int x, int y, int width, int height)
{
    wxSize sizeBtn = m_btn->GetSize(),
           sizeText = m_text->GetSize();

    wxCoord wText = width - sizeBtn.x;
    m_text->SetSize(x, y + (height - sizeText.y)/2, wText, sizeText.y);
    m_btn->Move(x + wText, y + (height - sizeBtn.y)/2);

    wxControl::DoMoveWindow(x, y, width, height);
}

// ----------------------------------------------------------------------------
// event handling
// ----------------------------------------------------------------------------

void wxComboControl::OnButton(wxCommandEvent& event)
{
    if ( event.GetEventObject() == m_btn )
    {
        ShowPopup();
    }
    else // not ours event
    {
        event.Skip();
    }
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

void wxComboControl::SetPopupControl(wxControl *control)
{
    m_ctrlPopup = control;
    m_ctrlPopup->Hide();
}

void wxComboControl::ShowPopup()
{
    m_ctrlPopup->Show();
    m_ctrlPopup->SetFocus();
    m_ctrlPopup->CaptureMouse();
}

void wxComboControl::HidePopup()
{
    m_ctrlPopup->ReleaseMouse();
    m_ctrlPopup->Hide();
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

    m_lbox = new wxListBox(parent, -1);
    m_lbox->Set(n, choices);

    SetPopupControl(m_lbox);

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
