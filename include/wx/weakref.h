/////////////////////////////////////////////////////////////////////////////
// Name:        wx/weakref.h
// Purpose:     wxWeakRef - Generic weak references for wxWidgets
// Author:      Arne Steinarson
// Created:     27 Dec 07
// RCS-ID:      $Id$
// Copyright:   (c) 2007 Arne Steinarson
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WEAKREF_H_
#define _WX_WEAKREF_H_

#include "wx/tracker.h"

#include "wx/meta/convertible.h"
#include "wx/meta/int2type.h"

template <class T>
struct wxIsStaticTrackable
{
    enum { value = wxConvertibleTo<T, wxTrackable>::value };
};

// Weak ref implementation when T has wxTrackable as a known base class
template <class T>
class wxWeakRefStatic : public wxTrackerNode
{
public:
    wxWeakRefStatic() : m_pobj(NULL) { }

    void Release()
    {
        // Release old object if any
        if ( m_pobj )
        {
            // Remove ourselves from object tracker list
            wxTrackable *pt = static_cast<wxTrackable*>(m_pobj);
            pt->RemoveNode(this);
            m_pobj = NULL;
        }
    }

protected:
    void Assign(T* pobj)
    {
        if ( m_pobj == pobj )
            return;

        Release();

        // Now set new trackable object
        if ( pobj )
        {
            // Add ourselves to object tracker list
            wxTrackable *pt = static_cast<wxTrackable*>(pobj);
            pt->AddNode(this);
            m_pobj = pobj;
        }
    }

    void AssignCopy(const wxWeakRefStatic& wr)
    {
        Assign( wr.m_pobj );
    }

    virtual void OnObjectDestroy()
    {
        // Tracked object itself removes us from list of trackers
        wxASSERT( m_pobj!=NULL );
        m_pobj = NULL;
    }

    T *m_pobj;
};


#if !defined(HAVE_PARTIAL_SPECIALIZATION) || !defined(HAVE_TEMPLATE_OVERLOAD_RESOLUTION)
    #define USE_STATIC_WEAKREF
#endif 


#ifndef USE_STATIC_WEAKREF
template<class T,bool use_static>
struct wxWeakRefImpl;

// Intermediate class, to select the static case above.
template <class T>
struct wxWeakRefImpl<T, true> : public wxWeakRefStatic<T>
{
    enum { value = 1 };
};

// Weak ref implementation when T does not have wxTrackable as known base class
template<class T>
struct wxWeakRefImpl<T, false> : public wxTrackerNode
{
    void Release()
    {
        // Release old object if any
        if ( m_pobj )
        {
            // Remove ourselves from object tracker list
            m_ptbase->RemoveNode(this);
            m_pobj = NULL;
            m_ptbase = NULL;
        }
    }

protected:
    wxWeakRefImpl() : m_pobj(NULL), m_ptbase(NULL) { }

    // Assign receives most derived class here and can use that
    template <class TDerived>
    void Assign( TDerived* pobj )
    {
        AssignHelper( pobj, wxInt2Type<wxIsStaticTrackable<TDerived>::value>() );
    }

    template <class TDerived>
    void AssignHelper(TDerived* pobj, wxInt2Type<true>)
    {
        wxTrackable *ptbase = static_cast<wxTrackable*>(pobj);
        DoAssign( pobj, ptbase );
    }

#ifdef HAVE_DYNAMIC_CAST
    void AssignHelper(T* pobj, wxInt2Type<false>)
    {
        // A last way to get a trackable pointer
        wxTrackable *ptbase = dynamic_cast<wxTrackable*>(pobj);
        if ( ptbase )
        {
            DoAssign( pobj, ptbase );
        }
        else
        {
            wxFAIL_MSG( "Tracked class should inherit from wxTrackable" );

            Release();
        }
    }
#endif // HAVE_DYNAMIC_CAST

    void AssignCopy(const wxWeakRefImpl& wr)
    {
        DoAssign(wr.m_pobj, wr.m_ptbase);
    }

    void DoAssign( T* pobj, wxTrackable *ptbase ) {
        if( m_pobj==pobj ) return;
        Release();

        // Now set new trackable object
        if( pobj )
        {
            // Add ourselves to object tracker list
            wxASSERT( ptbase );
            ptbase->AddNode( this );
            m_pobj = pobj;
            m_ptbase = ptbase;
        }
    }

    virtual void OnObjectDestroy()
    {
        // Tracked object itself removes us from list of trackers
        wxASSERT( m_pobj!=NULL );
        m_pobj = NULL;
        m_ptbase = NULL;
    }

    T *m_pobj;
    wxTrackable *m_ptbase;
};

#endif // #ifndef USE_STATIC_WEAKREF



// A weak reference to an object of type T, where T has type wxTrackable
// (usually statically but if not dynamic_cast<> is tried).
template <class T>
class wxWeakRef : public
#ifdef USE_STATIC_WEAKREF
                  wxWeakRefStatic<T>
#else
                  wxWeakRefImpl<T, wxIsStaticTrackable<T>::value>
#endif
{
public:
    // Default ctor
    wxWeakRef() { }

    // When we have the full type here, static_cast<> will always work
    // (or give a straight compiler error).
    template <class TDerived>
    wxWeakRef(TDerived* pobj)
    {
        Assign(pobj);
    }

    template <class TDerived>
    wxWeakRef<T>& operator=(TDerived* pobj)
    {
        Assign(pobj);
        return *this;
    }

    wxWeakRef<T>& operator=(const wxWeakRef<T>& wr)
    {
        AssignCopy(wr);
        return *this;
    }

    virtual ~wxWeakRef() { this->Release(); }

    // Smart pointer functions
    T& operator*() const { return *this->m_pobj; }
    T* operator->() const { return this->m_pobj; }

    T* get() const { return this->m_pobj; }
    operator T*() const { return get(); }
};


// Weak ref implementation assign objects are queried for wxTrackable
// using dynamic_cast<>
template <class T>
class wxWeakRefDynamic : public wxTrackerNode
{
public:
    wxWeakRefDynamic() : m_pobj(NULL) { }

    wxWeakRefDynamic(T* pobj) : m_pobj(pobj)
    {
        Assign(pobj);
    }

    virtual ~wxWeakRefDynamic() { Release(); }

    // Smart pointer functions
    T& operator * (){ wxASSERT(this->m_pobj); return *m_pobj; }
    T* operator -> (){ wxASSERT(this->m_pobj); return m_pobj; }
    T* operator = (T* pobj) { Assign(pobj); return m_pobj; }

    T* get(){ return this->m_pobj; }

    // operator T* (){ return this->m_pobj; }

    // test for pointer validity: defining conversion to unspecified_bool_type
    // and not more obvious bool to avoid implicit conversions to integer types
    typedef T *(wxWeakRef<T>::*unspecified_bool_type)() const;
    operator unspecified_bool_type() const
    {
        return m_pobj ? &wxWeakRef<T>::get : NULL;
    }

    // Assign from another weak ref, point to same object
    T* operator = (const wxWeakRef<T> &wr) { Assign( wr.get() ); return this->m_pobj; }

    void Release()
    {
        // Release old object if any
        if( m_pobj )
        {
            // Remove ourselves from object tracker list
            wxTrackable *pt = dynamic_cast<wxTrackable*>(m_pobj);
            wxASSERT(pt);
            pt->RemoveNode(this);
            m_pobj = NULL;
        }
    }

protected:
    void Assign(T *pobj)
    {
        if ( m_pobj == pobj )
            return;

        Release();

        // Now set new trackable object
        if ( pobj )
        {
            // Add ourselves to object tracker list
            wxTrackable *pt = dynamic_cast<wxTrackable*>(pobj);
            if ( pt )
            {
                pt->AddNode(this);
                m_pobj = pobj;
            }
            else
            {
                // If the object we want to track does not support wxTackable, then
                // log a message and keep the NULL object pointer.
                wxFAIL_MSG( "Tracked class should inherit from wxTrackable" );
            }
        }
    }

    virtual void OnObjectDestroy()
    {
        wxASSERT_MSG( m_pobj, "tracked object should have removed us itself" );

        m_pobj = NULL;
    }

    T *m_pobj;
};

// Provide some basic types of weak references
class WXDLLIMPEXP_FWD_BASE wxEvtHandler;
class WXDLLIMPEXP_FWD_CORE wxWindow;

typedef wxWeakRef<wxEvtHandler>  wxEvtHandlerRef;
typedef wxWeakRef<wxWindow>      wxWindowRef;

#endif // _WX_WEAKREF_H_

