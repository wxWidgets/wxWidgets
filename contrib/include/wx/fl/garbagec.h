/////////////////////////////////////////////////////////////////////////////
// Name:        garbagec.h
// Purpose:     GarbageCollector class.
// Author:      Aleksandras Gluchovas (@Lithuania)
// Modified by:
// Created:     ??/10/98
// RCS-ID:      $Id$
// Copyright:   (c) Aleksandras Gluchovas
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __GARBAGEC_G__
#define __GARBAGEC_G__

#include "wx/list.h"
#include "wx/fl/fldefs.h"

struct GCItem
{
    void*      mpObj;
    wxList    mRefs;   // references to other nodes
};

inline void* gc_node_to_obj( wxObjectList::compatibility_iterator pGCNode )
{
    return ( (GCItem*) (pGCNode->GetData()) )->mpObj;
}

/*
This class implements an extremely slow but simple garbage collection algorithm.
*/

class WXDLLIMPEXP_FL GarbageCollector
{
protected:
    wxList mAllNodes;
    wxList mRegularLst;
    wxList mCycledLst;

        // Internal method for finding a node.
    wxNode* FindItemNode( void* pForObj );

        // Internal method for resolving references.
    void    ResolveReferences();

        // Internal method for findind and freeing a node.
    wxNode* FindReferenceFreeItemNode();

        // Remove references to this node.
    void    RemoveReferencesToNode( wxNode* pItemNode );

        // Destroys a list of items.
    void    DestroyItemList( wxList& lst );

public:

        // Default constructor.
    GarbageCollector() {}

        // Destructor.
    virtual ~GarbageCollector();

        // Prepare data for garbage collection.

    virtual void AddObject( void* pObj, int refCnt = 1 );

        // Prepare data for garbage collection.

    virtual void AddDependency( void* pObj, void* pDependsOnObj );

        // Executes garbage collection algorithm.

    virtual void ArrangeCollection();

        // Accesses the results of the algorithm.

    wxList& GetRegularObjects();

        // Get cycled objects.

    wxList& GetCycledObjects();

        // Removes all data from the garbage collector.

    void Reset();
};

#endif /* __GARBAGEC_G__ */

