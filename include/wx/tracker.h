/////////////////////////////////////////////////////////////////////////////
// Name:        wx/tracker.h
// Purpose:     Support class for object lifetime tracking (wxWeakRef<T>)
// Author:      Arne Steinarson
// Created:     2007-12-28
// RCS-ID:      $Id:$
// Copyright:   (c) 2007 Arne Steinarson
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TRACKER_H_
#define _WX_TRACKER_H_

// This structure represents an object tracker and is stored in a linked list
// in the tracked object. It is only used in one of its derived forms.
struct wxTrackerNode
{
    wxTrackerNode() : m_next(NULL) { }
    virtual ~wxTrackerNode() { }

    virtual void OnObjectDestroy() = 0;

    // This is to tell the difference between different tracker node types.
    // It's a replacement of dynamic_cast<> for this class since dynamic_cast
    // may be disabled (and we don't have wxDynamicCast since wxTrackerNode
    // is not derived wxObject).
    enum wxTrackerNodeType { WeakRef, EventConnectionRef };

    virtual wxTrackerNodeType GetType() = 0;

protected:
    wxTrackerNode *m_next;

    friend class wxTrackableBase;    // For list access
    friend class wxEvtHandler;       // For list access
};


// Add-on base class for a trackable object.
struct wxTrackableBase
{
    wxTrackableBase() : m_first(NULL) { }

    ~wxTrackableBase()
    {
        // Notify all registered refs

        // OnObjectDestroy has to remove the item from the link chain
        while ( m_first )
        {
            wxTrackerNode *first = m_first;
            first->OnObjectDestroy();
            RemoveNode(first);
        }
    }

    void AddNode(wxTrackerNode *node)
    {
        node->m_next = m_first;
        m_first = node;
    }

    void RemoveNode(wxTrackerNode *node)
    {
        for ( wxTrackerNode **pn = &m_first; *pn; pn = &(*pn)->m_next )
        {
            if ( *pn == node )
            {
                *pn = node->m_next;
                return;
            }
        }

        wxFAIL_MSG( "node should be present" );
    }

    wxTrackerNode *GetFirst() { return m_first; }

protected:
    wxTrackerNode *m_first;
};

// The difference to wxTrackableBase is that this class adds
// a VTable to enable dynamic_cast query for wxTrackable.
struct wxTrackable : wxTrackableBase
{
    virtual ~wxTrackable() { }
};


// Helper to decide if an object has a base class or not
// (strictly speaking, this test succeeds if a type is convertible
//  to another type in some way.)
template <class T, class B>
struct wxHasBase
{
	static char Match(B *pb);
	static int Match(...);

	enum { value = sizeof(Match((T*)NULL)) == sizeof(char) };
};

// A structure to cast to wxTrackableBase, using either static_cast<> or
// dynamic_cast<>.
template <class T, bool is_static>
struct wxTrackableCaster;

template <class T>
struct wxTrackableCaster<T, true>
{
    static wxTrackableBase* Cast(T* pt)
    {
        return static_cast<wxTrackableBase *>(pt);
    }
};

#ifdef HAVE_DYNAMIC_CAST

template <class T>
struct wxTrackableCaster<T, false>
{
    static wxTrackableBase *Cast(T* pt)
    {
        return dynamic_cast<wxTrackableBase *>(pt);
    }
};

#else // !HAVE_DYNAMIC_CAST

template <class T>
struct wxTrackableCaster<T, false>
{
    static wxTrackableBase *Cast(T* pt) { return NULL; }
};

#endif // HAVE_DYNAMIC_CAST/!HAVE_DYNAMIC_CAST

#endif // _WX_TRACKER_H_

