/////////////////////////////////////////////////////////////////////////////
// Name:        wx/tracker.h
// Purpose:     Support class for object lifetime tracking (wxWeakRef<T>)
// Author:      Arne Steinarson
// Created:     2007-12-28
// RCS-ID:      $Id$
// Copyright:   (c) 2007 Arne Steinarson
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TRACKER_H_
#define _WX_TRACKER_H_


// This structure represents an object tracker and is stored in a linked list
// in the tracked object. It is only used in one of its derived forms.
struct wxTrackerNode {
    wxTrackerNode( ) : m_nxt(0) { }
    virtual ~wxTrackerNode() { }
    
    virtual void OnObjectDestroy( ) = 0;
    
    // This is to tell the difference between different tracker node types. 
    // It's a replacement of dynamic_cast<> for this class since dynamic_cast 
    // may be disabled (and we don't have wxDynamicCast since wxTrackerNode 
    // is not derived wxObject).
    enum wxTrackerNodeType { WeakRef, EventConnectionRef }; 

    virtual wxTrackerNodeType GetType() = 0;
    
protected:
    wxTrackerNode *m_nxt;
    friend class wxTrackableBase;    // For list access
    friend class wxEvtHandler;       // For list access
};


// Add-on base class for a trackable object.
struct wxTrackableBase {
    wxTrackableBase() : m_first(0) { }
    ~wxTrackableBase()
    { 
        // Notify all registered refs
        
        wxTrackerNode *first;
        while( m_first )
        {
        	first = m_first;
            first->OnObjectDestroy( );
            RemoveNode(first);
        }
    }
    
    void AddNode( wxTrackerNode *prn )
    {
        prn->m_nxt = m_first;
        m_first = prn;
    }
    void RemoveNode( wxTrackerNode *prn )
    {
        for( wxTrackerNode **pprn=&m_first; *pprn; pprn=&(*pprn)->m_nxt )
        {
            if( *pprn==prn )
            {
                *pprn = prn->m_nxt;
                return;
            }
        }
        // Not found, an error.
        wxASSERT( false );
    }
    
    wxTrackerNode* GetFirst( ){ return m_first; }

    // If trying to copy this object, then do not copy its ref list.
    wxTrackableBase& operator = (const wxTrackableBase& other) { return *this; }
    
protected:    
    wxTrackerNode *m_first;
};


// The difference to wxTrackableBase is that this class adds 
// a VTable to enable dynamic_cast query for wxTrackable.
struct wxTrackable : public wxTrackableBase 
{
    virtual ~wxTrackable(){ }
};


// Helper to decide if an object has a base class or not
// (strictly speaking, this test succeeds if a type is convertible
//  to another type in some way.)
template<class T, class B>
struct wxHasBase{
	static char Match( B* pb );	   
	static int Match( ... );       
	enum { value = (sizeof(Match((T*)NULL))==sizeof(char)) };
};

// VC++ before 7.1 does not have partial template specialization 
#ifdef __VISUALC__
    #if __VISUALC__ < 1310
        #define HAVE_NO_PARTIAL_SPECIALIZATION
    #endif
#endif 

#if defined(HAVE_DYNAMIC_CAST) && !defined(HAVE_NO_PARTIAL_SPECIALIZATION) 
    // A structure to cast to wxTrackableBase, using either static_cast<> or dynamic_cast<>. 
    template<class T,bool is_static>
    struct wxTrackableCaster;

    template <class T>
    struct wxTrackableCaster<T,true> {
        static wxTrackableBase* Cast(T* pt){ return static_cast<wxTrackableBase*>(pt); }
    };

    template <class T>
    struct wxTrackableCaster<T,false> {
        static wxTrackableBase* Cast(T* pt){ return dynamic_cast<wxTrackableBase*>(pt); }
    };
#else 
    #if defined(HAVE_DYNAMIC_CAST) 
        // If we have dynamic_cast, default to that. For gcc, dynamic_cast<> does the job 
        // of both the dynamic and the static case. It could be that all compilers do it 
        // that way, rendering the specialization code above rednundant.
        template <class T,bool is_static>
        struct wxTrackableCaster {
            static wxTrackableBase* Cast(T* pt){ return dynamic_cast<wxTrackableBase*>(pt); }
        };
    #else
        // No dynamic_cast<> is available.
        // We use static_cast<>, that gives support for wxEvtHandler and wxWindow references. 
        // We don't get weak refs to other wxObject derived types.
        template <class T,bool is_static>
        struct wxTrackableCaster {
            static wxTrackableBase* Cast(T* pt){ return static_cast<wxTrackableBase*>(pt); }
        };
    #endif
#endif 

#endif // _WX_TRACKER_H_

