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


class wxPyTreeItemData : public wxTreeItemData {
public:
    wxPyTreeItemData(PyObject* obj = NULL) {
        if (obj == NULL)
            obj = Py_None;
        Py_INCREF(obj);
        m_obj = obj;
    }

    ~wxPyTreeItemData() {
        bool blocked = wxPyBeginBlockThreads();
        Py_DECREF(m_obj);
        wxPyEndBlockThreads(blocked);
    }

    PyObject* GetData() {
        Py_INCREF(m_obj);
        return m_obj;
    }

    void SetData(PyObject* obj) {
        bool blocked = wxPyBeginBlockThreads();
        Py_DECREF(m_obj);
        wxPyEndBlockThreads(blocked);
        m_obj = obj;
        Py_INCREF(obj);
    }

    PyObject* m_obj;
};



#endif
