/////////////////////////////////////////////////////////////////////////////
// Name:        pytree.h
// Purpose:     Common declarations of tree stuff for wxTreeCtrl in the core
//              and wxTreeListCtrl in gizmos
//
// Author:      Robin Dunn
//
// Created:     30-April-2003
// RCS-ID:      $Id$
// Copyright:   (c) 2003 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////


#ifndef __pytree_h__
#define __pytree_h__

#include <wx/listctrl.h>
#include <wx/treectrl.h>


class wxPyListCtrl : public wxListCtrl {
    DECLARE_ABSTRACT_CLASS(wxPyListCtrl);
public:
    wxPyListCtrl() : wxListCtrl() {}
    wxPyListCtrl(wxWindow* parent, wxWindowID id,
                 const wxPoint& pos,
                 const wxSize& size,
                 long style,
                 const wxValidator& validator,
                 const wxString& name) :
        wxListCtrl(parent, id, pos, size, style, validator, name) {}

    bool Create(wxWindow* parent, wxWindowID id,
                const wxPoint& pos,
                const wxSize& size,
                long style,
                const wxValidator& validator,
                const wxString& name) {
        return wxListCtrl::Create(parent, id, pos, size, style, validator, name);
    }

    DEC_PYCALLBACK_STRING_LONGLONG(OnGetItemText);
    DEC_PYCALLBACK_INT_LONG(OnGetItemImage);
    DEC_PYCALLBACK_LISTATTR_LONG(OnGetItemAttr);

    PYPRIVATE;
};




class wxPyTreeCtrl : public wxTreeCtrl {
    DECLARE_ABSTRACT_CLASS(wxPyTreeCtrl);
public:
    wxPyTreeCtrl() : wxTreeCtrl() {}
    wxPyTreeCtrl(wxWindow *parent, wxWindowID id,
                 const wxPoint& pos,
                 const wxSize& size,
                 long style,
                 const wxValidator& validator,
                 const wxString& name) :
        wxTreeCtrl(parent, id, pos, size, style, validator, name) {}

    bool Create(wxWindow *parent, wxWindowID id,
                const wxPoint& pos,
                const wxSize& size,
                long style,
                const wxValidator& validator,
                const wxString& name) {
        return wxTreeCtrl::Create(parent, id, pos, size, style, validator, name);
    }


    int OnCompareItems(const wxTreeItemId& item1,
                       const wxTreeItemId& item2);
    
    PYPRIVATE;
};


class wxPyTreeItemData : public wxTreeItemData {
public:
    wxPyTreeItemData(PyObject* obj = NULL) {
        if (obj == NULL)
            obj = Py_None;
        Py_INCREF(obj);
        m_obj = obj;
    }

    ~wxPyTreeItemData() {
        wxPyBeginBlockThreads();
        Py_DECREF(m_obj);
        wxPyEndBlockThreads();
    }

    PyObject* GetData() {
        Py_INCREF(m_obj);
        return m_obj;
    }

    void SetData(PyObject* obj) {
        wxPyBeginBlockThreads();
        Py_DECREF(m_obj);
        wxPyEndBlockThreads();
        m_obj = obj;
        Py_INCREF(obj);
    }

    PyObject* m_obj;
};

#endif
