/////////////////////////////////////////////////////////////////////////////
// Name:        logctrl.cpp
// Purpose:     Data model implementation for log tree control
// Author:      Carlos Alberto Rodrigues Azevedo
// Created:     04/09/2022
// Copyright:   (c) Carlos Alberto Rodrigues Azevedo
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "logctrl.h"

LogEntry::LogEntry(unsigned id, LogEntry* parent, const wxString& msg, const wxColour& colour, const wxDateTime& timestamp) :
                   m_id(id), m_parent(parent), m_msg(msg), m_colour(colour), m_timestamp(timestamp)
{
    if (parent != nullptr) parent->AppendChild(this);
}

LogEntry::~LogEntry()
{
    for (auto child : m_children) delete child;
}


LogDataModel::~LogDataModel()
{
    // Deleting the first root will delete everything
    delete m_root_vector[0];
}

void LogDataModel::Start()
{
    LogEntry* root = new LogEntry(m_current_id++, nullptr, "Log Server started",
                                  wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT), wxDateTime::Now());
    m_root_vector.push_back(root);
    ItemAdded(wxDataViewItem((void*)nullptr), wxDataViewItem((void*)root));
    m_last_entry = wxDataViewItem((void*)root);
}

void LogDataModel::Fail()
{
    LogEntry* root = new LogEntry(m_current_id++, nullptr, "Log Server could not start", *wxRED, wxDateTime::Now());
    m_root_vector.push_back(root);
    ItemAdded(wxDataViewItem((void*)nullptr), wxDataViewItem((void*)root));
    m_last_entry = wxDataViewItem((void*)root);
}

void LogDataModel::Reset()
{
    LogEntry* root = new LogEntry(m_current_id++, nullptr, "Log cleared",
                                  wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT), wxDateTime::Now());

    m_root_vector.push_back(root);
    ItemAdded(wxDataViewItem((void*)nullptr), wxDataViewItem((void*)root));
    m_last_entry = wxDataViewItem((void*)root);
}

void LogDataModel::AddEntry(const wxString& msg, const wxColour& colour, const wxDateTime& timestamp)
{
    // Calculate the depth
    wxString msg_no_ident(msg);
    msg_no_ident = msg_no_ident.Trim(false);
    int tabs = 0;
    int spaces = 0;
    auto stop = msg.begin() + (msg.Len() - msg_no_ident.Len());
    for (auto iter = msg.begin(); iter != stop; iter++)
    {
        if (*iter == '\t')
        {
            tabs++;
            spaces = 0;
        }
        else if (*iter == ' ')
        {
            if (++spaces == 2)
            {
                tabs++;
                spaces = 0;
            }
        }
    }
    // Trim or grow the root vector as needed
    while (tabs < m_current_ident)
    {
        m_root_vector.pop_back();
        m_current_ident--;
    }
    while (tabs > (m_current_ident + 1))
    {
        LogEntry* parent = m_root_vector[++m_current_ident];
        LogEntry* new_root = new LogEntry(m_current_id++, parent, wxEmptyString,
                                          wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT), timestamp);
        m_root_vector.push_back(new_root);
        ItemAdded(wxDataViewItem((void*)parent), wxDataViewItem((void*)new_root));
        m_ctrl->Expand(wxDataViewItem((void*)new_root));
    }
    // Add the new entry
    LogEntry* parent = m_root_vector[tabs];
    LogEntry* entry = new LogEntry(m_current_id++, parent, msg_no_ident, colour, timestamp);
    if (tabs > m_current_ident) m_root_vector.push_back(entry);
    else m_root_vector[tabs + 1] = entry;
    m_last_entry = wxDataViewItem((void*)entry);
    ItemAdded(wxDataViewItem((void*)parent), m_last_entry);
    m_ctrl->Expand(m_last_entry);
    m_current_ident = tabs;
}

void LogDataModel::Clear()
{
    delete m_root_vector[0];
    m_root_vector.clear();
    m_current_ident = -1;
    m_current_id = 0;
    Cleared();
}

int LogDataModel::Compare(const wxDataViewItem &item1, const wxDataViewItem &item2,
                          unsigned int column, bool ascending) const
{
    return ((LogEntry*)item1.GetID())->GetID() - ((LogEntry*)item2.GetID())->GetID();
}

void LogDataModel::GetValue(wxVariant &variant, const wxDataViewItem &item, unsigned int col) const
{
    wxASSERT(item.IsOk());

    LogEntry *node = (LogEntry*)item.GetID();
    switch (col)
    {
        case 0: variant = node->GetMessage(); break;
        case 1: variant = node->GetTimeStamp().FormatTime(); break;
        default: break;
    }
}

wxDataViewItem LogDataModel::GetParent(const wxDataViewItem &item) const
{
    // The invisible root node has no parent
    if (!item.IsOk()) return wxDataViewItem(0);

    LogEntry* node = (LogEntry*)item.GetID();
    return wxDataViewItem((void*)node->GetParent());
}

unsigned int LogDataModel::GetChildren(const wxDataViewItem &parent, wxDataViewItemArray &array) const
{
    LogEntry* node = (LogEntry*)parent.GetID();
    if (!node)
    {
        if (m_root_vector.empty()) return 0;
        array.Add(wxDataViewItem((void*)m_root_vector[0]));
        return 1;
    }
    unsigned int count = node->GetChildren().size();
    for (unsigned int pos = 0; pos < count; pos++)
    {
        LogEntry* child = node->GetChildren()[pos];
        array.Add(wxDataViewItem((void*)child));
    }
    return count;
}

bool LogDataModel::GetAttr(const wxDataViewItem& item, unsigned int col, wxDataViewItemAttr& attr) const
{
    if (col == 0)
    {
        attr.SetColour(((LogEntry*)item.GetID())->GetColour());
        return true;
    }
    return false;
}
