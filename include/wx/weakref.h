/////////////////////////////////////////////////////////////////////////////
// Name:        wx/weakref.h
// Purpose:     wxWeakRef - Generic weak references for wxWidgets
// Author:      Arne Steinarson
// Created:     2007-12-27
// RCS-ID:      $Id$
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
    typedef T element_type;
    
    wxWeakRef(T *pobj = NULL) : m_pobj(NULL) { Assign(pobj); }

    virtual ~wxWeakRef() { Assign(NULL); }

    T * get() const                 
    {                               
        return m_pobj;
    }                               
    
    T* operator->()
    { 
        wxASSERT(m_pobj != NULL);    
        return m_pobj; 
    }
    
    T& operator*() const           
    {                               
        wxASSERT(m_pobj != NULL);    
        return *m_pobj;              
    }                               
                                    
    T* operator=(T *pobj)
    {
        Assign(pobj);
        return m_pobj;
    }

    // Assign from another weak ref, point to same object
    T* operator = (const wxWeakRef<T> &wr)
    { 
        Assign(wr); 
        return m_pobj; 
    }
    
    virtual void OnObjectDestroy()
    {
        // Tracked object itself removes us from list of trackers
        wxASSERT( m_pobj );
        m_pobj = NULL;
    }

protected:
    friend class wxTrackableBase; 
    friend class wxEvtHandler;

    virtual wxTrackerNodeType GetType() { return WeakRef; }
    
    void Assign(T* pobj)
    {
        if ( m_pobj == pobj )
            return;

        // First release old object if any
        if ( m_pobj )
        {
            // Remove ourselves from object tracker list
            // This does static_cast if available, otherwise it tries dynamic cast
            wxTrackableBase *pt = wxTrackableCaster<T,wxHasBase<T,wxTrackableBase>::value >::Cast(m_pobj);
            wxASSERT(pt);
            pt->RemoveNode(this);
            m_pobj = NULL;
        }

        // Now set new trackable object
        if ( pobj )
        {
            wxTrackableBase *pt = wxTrackableCaster<T,wxHasBase<T,wxTrackableBase>::value >::Cast(pobj);
            if( pt )
            {
                pt->AddNode( this );
                m_pobj = pobj;
            }
            else
            {
                // If the tracked we want to track does not support wxTackableBase, then 
                // log a message and keep the NULL object pointer. 
                wxLogWarning( _T("wxWeakRef::Assign - Type does not provide wxTrackableBase - resetting tracked object") );
            }
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

