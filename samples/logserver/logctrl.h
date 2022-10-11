/////////////////////////////////////////////////////////////////////////////
// Name:        logctrl.h
// Purpose:     Data model implementation for log tree control
// Author:      Carlos Alberto Rodrigues Azevedo
// Created:     04/09/2022
// Copyright:   (c) Carlos Alberto Rodrigues Azevedo
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include <vector>

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include <wx/wx.h>
    #include <wx/dataview.h>
#endif

class LogEntry
{
    public:
        LogEntry(unsigned id, LogEntry* parent, const wxString& msg, const wxColour& colour, const wxDateTime& timestamp);
        ~LogEntry();
        bool IsContainer() const { return !m_children.empty(); }
        unsigned GetID() { return m_id; }
        LogEntry* GetParent() { return m_parent; }
        wxString GetMessage() { return m_msg; }
        wxColour GetColour() { return m_colour; }
        wxDateTime& GetTimeStamp() { return m_timestamp; }
        std::vector<LogEntry*>& GetChildren() { return m_children; }
        void AppendChild(LogEntry* child) { m_children.push_back(child); }
        unsigned int GetChildCount() const { return m_children.size(); }
    protected:
        unsigned m_id;
        LogEntry* m_parent;
        wxString m_msg;
        wxColour m_colour;
        wxDateTime m_timestamp;
        std::vector<LogEntry*> m_children;
};

class LogDataModel: public wxDataViewModel
{
    public:
        LogDataModel(wxDataViewCtrl* ctrl) : m_ctrl(ctrl), m_current_id(0), m_current_ident(-1) {}
        ~LogDataModel();

        void Start();
        void Fail();
        void Reset();
        void AddEntry(const wxString& msg, const wxColour& colour, const wxDateTime& timestamp);
        void Clear();
        wxDataViewItem GetLastEntry() { return m_last_entry; }
        virtual int Compare(const wxDataViewItem &item1, const wxDataViewItem &item2,
                            unsigned int column, bool ascending) const wxOVERRIDE;
        virtual void GetValue(wxVariant &variant, const wxDataViewItem &item,
                              unsigned int col) const wxOVERRIDE;
        virtual bool SetValue(const wxVariant &variant, const wxDataViewItem &item,
                              unsigned int col) wxOVERRIDE { return true; }
        virtual wxDataViewItem GetParent(const wxDataViewItem &item) const wxOVERRIDE;
        virtual bool IsContainer(const wxDataViewItem &item) const wxOVERRIDE { return true; }
        virtual unsigned int GetChildren(const wxDataViewItem &parent,
                                         wxDataViewItemArray &array) const wxOVERRIDE;
        virtual bool HasContainerColumns(const wxDataViewItem &item) const { return true; }
        virtual bool GetAttr(const wxDataViewItem& item, unsigned int col, wxDataViewItemAttr& attr) const;
    private:
        wxDataViewCtrl* m_ctrl;
        std::vector<LogEntry*> m_root_vector;
        unsigned m_current_id;
        int m_current_ident;
        wxDataViewItem m_last_entry;
};

