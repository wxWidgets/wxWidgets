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

