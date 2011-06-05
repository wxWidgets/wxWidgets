/////////////////////////////////////////////////////////////////////////////
// Name:        src/mobile/generic/mo_listbox_g.cpp
// Purpose:     wxMoListBox class
// Author:      Julian Smart
// Modified by:
// Created:     2009-06-07
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/dcscreen.h"
#include "wx/settings.h"

#include "wx/mobile/generic/listbox.h"

IMPLEMENT_CLASS(wxMoListBox, wxMoWheelsCtrl)

BEGIN_EVENT_TABLE(wxMoListBox, wxMoWheelsCtrl)
    EVT_WHEEL_SELECTED(wxID_ANY, wxMoListBox::OnWheelSelected)
END_EVENT_TABLE()


bool wxMoListBox::Create(wxWindow *parent,
            wxWindowID id,
            const wxPoint& pos,
            const wxSize& size,
            const wxArrayString& strings,
            long style,
            const wxValidator& validator,
            const wxString& name)
{
    wxASSERT_MSG((style & wxLB_MULTIPLE) == 0, _("Sorry, wxLB_MULTIPLE not currently supported."));

    style &= ~wxLB_MULTIPLE;

    wxMoWheelsCtrl::Create(parent, id, pos, size, style, validator, name);

    wxArrayString strings1(strings);
    if (HasFlag(wxLB_SORT))
        strings1.Sort();

    wxMoWheelsTextDataSource* dataSource = new wxMoWheelsTextDataSource();
    dataSource->SetStrings(strings1);

    SetDataSource(dataSource, true /* control owns the data source */);

    SetFont(wxMoSystemSettings::GetFont(wxMO_FONT_NORMAL_BUTTON));

    SetInitialSize(size);

    ReloadAllComponents();

    return true;
}

// common part of all ctors
void wxMoListBox::Init()
{
}

wxMoWheelsTextDataSource* wxMoListBox::GetTextDataSource() const
{
    return wxDynamicCast(GetDataSource(), wxMoWheelsTextDataSource);
}

void wxMoListBox::Clear()
{
    wxMoWheelsTextDataSource* textDataSource = GetTextDataSource();
    if (textDataSource)
    {
        textDataSource->SetStrings(wxArrayString());
        ReloadAllComponents();
    }
}

void wxMoListBox::Set(const wxArrayString& strings)
{
    Clear();

    wxArrayString strings1(strings);

    if (HasFlag(wxLB_SORT))
        strings1.Sort();

    ReloadAllComponents();
}

void wxMoListBox::Append(const wxString& str)
{
    Insert(str, GetCount());
}

int wxMoListBox::Insert(const wxString& str, int pos)
{
    wxASSERT( pos <= GetCount());

    wxMoWheelsTextDataSource* textDataSource = GetTextDataSource();
    if (textDataSource)
    {
        wxArrayString strings(textDataSource->GetStrings());
        if (pos < GetCount())
        {
            strings.Insert(str, pos);
        }
        else
        {
            strings.Add(str, pos);
        }
        
        if (HasFlag(wxLB_SORT))
        {
            if (HasFlag(wxLB_MULTIPLE))
            {
                // TODO: restore selections after sort, somehow
                textDataSource->SetStrings(strings);
            }
            else
            {
                wxString strSelection = GetStringSelection();
                
                strings.Sort();
                
                textDataSource->SetStrings(strings);

                if (!strSelection.IsEmpty())
                    SetStringSelection(strSelection);
            }
        }
        else
        {
            textDataSource->SetStrings(strings);
        }
        ReloadAllComponents();
    }

    return pos;
}

void wxMoListBox::InsertItems(const wxArrayString& items, int pos)
{
    wxASSERT( pos <= GetCount());

    wxMoWheelsTextDataSource* textDataSource = GetTextDataSource();
    if (textDataSource)
    {
        wxArrayString strings(textDataSource->GetStrings());
        if (pos < GetCount())
        {
            size_t i;
            for (i = items.GetCount(); i >= 0; i --)
            {
                strings.Insert(items[i], pos);
            }
        }
        else
        {
            size_t i;
            for (i = 0; i < items.GetCount(); i++)
            {
                strings.Add(items[i]);
            }
        }
        
        if (HasFlag(wxLB_SORT))
        {
            if (HasFlag(wxLB_MULTIPLE))
            {
                // TODO: restore selections after sort, somehow
                textDataSource->SetStrings(strings);
            }
            else
            {
                wxString strSelection = GetStringSelection();
                strings.Sort();
                textDataSource->SetStrings(strings);
                if (!strSelection.IsEmpty())
                    SetStringSelection(strSelection);
            }
        }
        else
        {
            textDataSource->SetStrings(strings);
        }
        ReloadAllComponents();
    }
}

wxString wxMoListBox::GetStringSelection() const
{
    size_t pos = GetSelection(0);
    if (pos != wxNOT_FOUND && pos < (size_t) GetCount())
    {
        wxMoWheelsTextDataSource* textDataSource = GetTextDataSource();
        if (textDataSource)
        {
            // TODO: efficiency improvement
            return textDataSource->GetStrings()[pos];
        }
    }
    return wxEmptyString;
}

void wxMoListBox::SetStringSelection(const wxString& str)
{
    wxMoWheelsTextDataSource* textDataSource = GetTextDataSource();
    if (textDataSource)
    {
        size_t pos = textDataSource->GetStrings().Index(str);
        if (pos != wxNOT_FOUND)
            SetSelection(0, pos);
    }
}

wxString wxMoListBox::GetString(int n) const
{
    wxASSERT( n < GetCount());

    if (n < GetCount())
    {
        wxMoWheelsTextDataSource* textDataSource = GetTextDataSource();
        if (textDataSource)
        {
            return textDataSource->GetStrings()[n];
        }
    }
    return wxEmptyString;
}

void wxMoListBox::SetString(int n, const wxString& str)
{
    wxASSERT( n < GetCount());

    if (n < GetCount())
    {
        wxMoWheelsTextDataSource* textDataSource = GetTextDataSource();
        if (textDataSource)
        {
            wxArrayString strings(textDataSource->GetStrings());
            strings[n] = str;
            textDataSource->SetStrings(strings);
            Refresh();
        }
    }
}

wxArrayString wxMoListBox::GetStrings() const
{
    wxMoWheelsTextDataSource* textDataSource = GetTextDataSource();
    if (textDataSource)
        return textDataSource->GetStrings();
    else
        return wxArrayString();
}

int wxMoListBox::GetSelections(wxArrayInt& selections) const
{
    selections.Clear();
#if 0
    if (HasFlag(wxLB_MULTIPLE))
    {
        int i;
        for (i = 0; i < GetCount(); i++)
        {
            if (IsSelected(i))
                selections.Add(i);
        }
    }
    else
#endif
    {
        int sel = GetSelection(0);
        if (sel != wxNOT_FOUND)
            selections.Add(sel);
    }
    return selections.GetCount();
}

// Returns the number of strings in the listbox.
int wxMoListBox::GetCount() const
{
    return GetComponentRowCount(0);
}

wxSize wxMoListBox::DoGetBestSize() const
{
    return wxMoWheelsCtrl::DoGetBestSize();
}

void wxMoListBox::OnWheelSelected(wxWheelsCtrlEvent& event)
{
    wxCommandEvent cmdEvent(wxEVT_COMMAND_LISTBOX_SELECTED, GetId());
    cmdEvent.SetEventObject(this);
    cmdEvent.SetInt(event.GetSelection());

    (void)GetEventHandler()->ProcessEvent(cmdEvent);
}
