/****************************************************************************
*
*                       wxWindows HTML Applet Package
*
*               Copyright (C) 1991-2001 SciTech Software, Inc.
*                            All rights reserved.
*
*  ========================================================================
*
*    The contents of this file are subject to the wxWindows License
*    Version 3.0 (the "License"); you may not use this file except in
*    compliance with the License. You may obtain a copy of the License at
*    http://www.wxwindows.org/licence3.txt
*
*    Software distributed under the License is distributed on an
*    "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
*    implied. See the License for the specific language governing
*    rights and limitations under the License.
*
*  ========================================================================
*
* Language:     ANSI C++
* Environment:  Any
*
* Description:  Combobox wrapper. This file implements the custom
*               combo boxes used for this sample program.
*
****************************************************************************/

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/wx.h"
#include "combobox.h"

/*------------------------- Implementation --------------------------------*/

ComboBox::ComboBox(
    wxWindow *parent,
    int listid,
    int textid)
    : m_Parent(parent), m_ListBoxId(listid), m_TextCtrlId(textid)
{
    m_ListBox  = wxDynamicCast(m_Parent->FindWindow(listid),wxListBox);
    m_TextCtrl = wxDynamicCast(m_Parent->FindWindow(textid),wxTextCtrl);
}

int ComboBox::GetListBoxId()
{
    return m_ListBoxId;
}

int ComboBox::GetSelection()
{
    return m_ListBox->GetSelection();
}

wxString ComboBox::GetStringSelection()
{
    return m_ListBox->GetStringSelection();
}

bool ComboBox::SetStringSelection(const wxString& s, bool select)
{
    select = TRUE;
    select = m_ListBox->SetStringSelection(s, select);
    m_TextCtrl->SetValue(GetStringSelection());
    return select;
}

void ComboBox::Select(int n)
{
    m_ListBox->Select(n);
    m_TextCtrl->SetValue(GetStringSelection());
}

void ComboBox::Deselect(int n)
{
    m_ListBox->Deselect(n);
}

void ComboBox::Insert(const wxString& item, int pos)
{
    m_ListBox->Insert(item,pos);
}

void ComboBox::Insert(const wxString& item, int pos, void *clientData)
{
    m_ListBox->Insert(item, pos, clientData);
}

void ComboBox::Insert(const wxString& item, int pos, wxClientData *clientData)
{
    m_ListBox->Insert(item, pos, clientData);
}

void ComboBox::InsertItems(int nItems, const wxString *items, int pos)
{
    m_ListBox->InsertItems(nItems, items, pos);
}

void ComboBox::InsertItems(const wxArrayString& items, int pos)
{
    m_ListBox->InsertItems(items, pos);
}

void ComboBox::Set(int n, const wxString* items, void **clientData)
{
    m_ListBox->Set(n, items, clientData);
    m_TextCtrl->SetValue(GetStringSelection());
}

void ComboBox::Set(const wxArrayString& items, void **clientData)
{
    m_ListBox->Set(items, clientData);
    m_TextCtrl->SetValue(GetStringSelection());
}

int ComboBox::FindString(const wxString &s)
{
    return (m_ListBox->FindString(s));
}

void ComboBox::SetFirstItem(int n)
{
    m_ListBox->SetFirstItem(n);
    m_TextCtrl->SetValue(GetStringSelection());
}

void ComboBox::SetFirstItem(const wxString &s)
{
    m_ListBox->SetFirstItem(s);
    m_TextCtrl->SetValue(GetStringSelection());
}

void ComboBox::Append(const wxString &item)
{
    m_ListBox->Append(item);
    m_TextCtrl->SetValue(GetStringSelection());
}

void ComboBox::Append(const wxString& item, void *clientData)
{
    m_ListBox->Append(item, clientData);
    m_TextCtrl->SetValue(GetStringSelection());
}

void ComboBox::Append(const wxString& item, wxClientData *clientData)
{
    m_ListBox->Append(item, clientData);
    m_TextCtrl->SetValue(GetStringSelection());
}

void ComboBox::Clear()
{
    m_ListBox->Clear();
    m_TextCtrl->SetValue(GetStringSelection());
}

void ComboBox::Delete(int n)
{
    m_ListBox->Delete(n);
    m_TextCtrl->SetValue(GetStringSelection());
}

void ComboBox::OnChange(wxCommandEvent &)
{
    m_TextCtrl->SetValue(GetStringSelection());
}

