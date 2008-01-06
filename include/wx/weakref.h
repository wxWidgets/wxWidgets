/////////////////////////////////////////////////////////////////////////////
// Name:        wx/weakref.h
// Purpose:     wxWeakRef - Generic weak references for wxWidgets
// Author:      Arne Steinarson
// Created:     2007-12-27
// RCS-ID:      $Id:$
// Copyright:   (c) 2007 Arne Steinarson
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WEAKREF_H_
#define _WX_WEAKREF_H_

#include <wx/tracker.h>

// A weak reference to an object of type T, where T has type wxTrackable
// as one of its base classes (in a static or dynamic sense).
template<class T>
class wxWeakRef : public wxTrackerNode
{
public:
    wxWeakRef(T *pobj = NULL) : m_pobj(NULL) { Assign(pobj); }

    virtual ~wxWeakRef() { Assign(NULL); }

    // Smart pointer functions
    operator T*(){ return m_pobj; }
    T* operator->(){ return m_pobj; }
    T* operator=(T *pobj)
    {
        Assign(pobj);
        return m_pobj;
    }

    virtual void OnObjectDestroy()
    {
        // Tracked object itself removes us from list of trackers
        wxASSERT( m_pobj );
        m_pobj = NULL;
    }

    virtual wxTrackerNodeType GetType() { return WeakRef; }

protected:
    wxTrackableBase *GetTrackable(T *pobj)
    {
        // this uses static_cast if possible or dynamic_cast otherwise
        return wxTrackableCaster<T, wxHasBase<T, wxTrackableBase>::value>
                ::Cast(pobj);
    }

    void Assign(T* pobj)
    {
        if ( m_pobj == pobj )
            return;

        // First release old object if any
        if ( m_pobj )
        {
            // Remove ourselves from object tracker list
            GetTrackable(m_pobj)->RemoveNode(this);
            m_pobj = NULL;
        }

        // Now set new trackable object
        if ( pobj )
        {
            wxTrackableBase * const pt = GetTrackable(pobj);
            wxCHECK_RET( pt, "type must derive from wxTrackableBase" );

            pt->AddNode(this);
            m_pobj = pobj;
        }
    }

    T *m_pobj;
};

// Provide some basic types of weak references
class WXDLLIMPEXP_FWD_BASE wxObject;
class WXDLLIMPEXP_FWD_BASE wxEvtHandler;
class WXDLLIMPEXP_FWD_CORE wxWindow;

typedef wxWeakRef<wxObject>      wxObjectRef;
typedef wxWeakRef<wxEvtHandler>  wxEvtHandlerRef;
typedef wxWeakRef<wxWindow>      wxWindowRef;

#endif // _WX_WEAKREF_H_

