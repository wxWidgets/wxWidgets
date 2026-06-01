/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/bmpcbox.cpp
// Purpose:     wxWinUI wxBitmapComboBox (WinUI ComboBox with image items)
// Author:      wxWidgets development team
// Created:     2026-06-01
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_BITMAPCOMBOBOX

#include "wx/bmpcbox.h"

#ifndef WX_PRECOMP
    #include "wx/log.h"
#endif

// wxBitmapComboBox builds on the WinUI wxChoice/wxComboBox image-item hook, so
// no direct WinRT use is needed here.

wxIMPLEMENT_DYNAMIC_CLASS(wxBitmapComboBox, wxComboBox);

bool wxBitmapComboBox::Create(wxWindow *parent, wxWindowID id,
                              const wxString& value, const wxPoint& pos,
                              const wxSize& size, int n, const wxString choices[],
                              long style, const wxValidator& validator,
                              const wxString& name)
{
    return wxComboBox::Create(parent, id, value, pos, size, n, choices,
                              style, validator, name);
}

bool wxBitmapComboBox::Create(wxWindow *parent, wxWindowID id,
                              const wxString& value, const wxPoint& pos,
                              const wxSize& size, const wxArrayString& choices,
                              long style, const wxValidator& validator,
                              const wxString& name)
{
    return wxComboBox::Create(parent, id, value, pos, size, choices,
                              style, validator, name);
}

void wxBitmapComboBox::SetItemBitmap(unsigned int n, const wxBitmapBundle& bitmap)
{
    // Make sure the bundle vector is grown to match the item count, then set
    // the requested one (DoSetItemBitmap requires the slot to already exist).
    UpdateInternals();
    if ( n < GetCount() )
    {
        DoSetItemBitmap(n, bitmap);
        WinUIRefreshItems();
    }
}

int wxBitmapComboBox::Append(const wxString& item, const wxBitmapBundle& bitmap)
{
    const int n = wxComboBox::Append(item);
    if ( n != wxNOT_FOUND )
        SetItemBitmap(n, bitmap);
    return n;
}

int wxBitmapComboBox::Append(const wxString& item, const wxBitmapBundle& bitmap,
                             void *clientData)
{
    const int n = wxComboBox::Append(item, clientData);
    if ( n != wxNOT_FOUND )
        SetItemBitmap(n, bitmap);
    return n;
}

int wxBitmapComboBox::Append(const wxString& item, const wxBitmapBundle& bitmap,
                             wxClientData *clientData)
{
    const int n = wxComboBox::Append(item, clientData);
    if ( n != wxNOT_FOUND )
        SetItemBitmap(n, bitmap);
    return n;
}

int wxBitmapComboBox::Insert(const wxString& item, const wxBitmapBundle& bitmap,
                             unsigned int pos)
{
    const int n = wxComboBox::Insert(item, pos);
    if ( n != wxNOT_FOUND )
        SetItemBitmap(n, bitmap);
    return n;
}

int wxBitmapComboBox::Insert(const wxString& item, const wxBitmapBundle& bitmap,
                             unsigned int pos, void *clientData)
{
    const int n = wxComboBox::Insert(item, pos, clientData);
    if ( n != wxNOT_FOUND )
        SetItemBitmap(n, bitmap);
    return n;
}

int wxBitmapComboBox::Insert(const wxString& item, const wxBitmapBundle& bitmap,
                             unsigned int pos, wxClientData *clientData)
{
    const int n = wxComboBox::Insert(item, pos, clientData);
    if ( n != wxNOT_FOUND )
        SetItemBitmap(n, bitmap);
    return n;
}

wxBitmap wxBitmapComboBox::WinUIGetItemBitmap(unsigned int n) const
{
    if ( n < m_bitmapbundles.size() )
        return GetItemBitmap(n);
    return wxBitmap();
}

void wxBitmapComboBox::DoClear()
{
    BCBDoClear();
    wxComboBox::DoClear();
}

void wxBitmapComboBox::DoDeleteOneItem(unsigned int n)
{
    BCBDoDeleteOneItem(n);
    wxComboBox::DoDeleteOneItem(n);
}

#endif // wxUSE_BITMAPCOMBOBOX
