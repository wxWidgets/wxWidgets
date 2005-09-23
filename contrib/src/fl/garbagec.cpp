/////////////////////////////////////////////////////////////////////////////
// Name:        garbagec.cpp
// Purpose:     Garbage collection algorithm for optimizing refresh.
// Author:      Aleksandras Gluchovas
// Modified by:
// Created:     18/10/98
// RCS-ID:      $Id$
// Copyright:   (c) Aleksandras Gluchovas
// Licence:       wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif


#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/fl/garbagec.h"

/***** Implementation for class GarbageCollector *****/

inline static GCItem& node_to_item( wxNode* pNode )
{
    return *( (GCItem*)(pNode->GetData()) );
}

GarbageCollector::~GarbageCollector()
{
    Reset();
}

/*** GC alg. helpers ***/

void GarbageCollector::DestroyItemList( wxList& lst )
{
    wxNode* pNode = lst.GetFirst();

    while( pNode )
    {
        delete &node_to_item( pNode );

        pNode = pNode->GetNext();
    }

    lst.Clear();
}

wxNode* GarbageCollector::FindItemNode( void* pForObj )
{
    wxNode* pNode = mAllNodes.GetFirst();

    while( pNode )
    {
        if ( node_to_item( pNode ).mpObj == pForObj )

            return pNode;

        pNode = pNode->GetNext();
    }

    return NULL;
}

wxNode* GarbageCollector::FindReferenceFreeItemNode()
{
    wxNode* pNode = mAllNodes.GetFirst();

    while( pNode )
    {
        if ( node_to_item( pNode ).mRefs.GetCount() == 0 ) 

            return pNode;

        pNode = pNode->GetNext();
    }

    return 0;
}

void GarbageCollector::RemoveReferencesToNode( wxNode* pItemNode )
{
    wxNode* pNode = mAllNodes.GetFirst();

    while( pNode )
    {
        wxList& refLst   = node_to_item( pNode ).mRefs;
        wxNode* pRefNode = refLst.GetFirst();

        while( pRefNode )
        {
            if ( pRefNode->GetData() == (wxObject*)pItemNode )
            {
                wxNode* pNext = pRefNode->GetNext();

                refLst.DeleteNode( pRefNode );

                pRefNode = pNext;

                continue;
            }
            else pRefNode = pRefNode->GetNext();
        }

        pNode = pNode->GetNext();
    }
}

void GarbageCollector::ResolveReferences()
{
    wxNode* pNode = mAllNodes.GetFirst();

    while( pNode )
    {
        GCItem& item = node_to_item( pNode );

        wxNode* pRefNode = item.mRefs.GetFirst();

        while( pRefNode )
        {
            pRefNode->SetData( (wxObject*) FindItemNode( (void*)pRefNode->GetData() ) );

            pRefNode = pRefNode->GetNext();
        }

        pNode = pNode->GetNext();
    }
}

void GarbageCollector::AddObject( void* pObj, int WXUNUSED(refCnt) )
{
    // FOR NOW:: initial ref-count is not used

    GCItem* pItem = new GCItem();

    pItem->mpObj  = pObj;

    mAllNodes.Append( (wxObject*) pItem );
}

void GarbageCollector::AddDependency( void* pObj, void* pDependsOnObj )
{
    node_to_item( FindItemNode( pObj ) ).mRefs.Append( (wxObject*)pDependsOnObj );
}

/*** GC alg. implementation ***/

void GarbageCollector::ArrangeCollection()
{
    ResolveReferences();

    do
    {
        // find node, which does not depend on anything

        wxNode* pItemNode = FindReferenceFreeItemNode();

        if ( pItemNode )
        {
            // append it to the list, where items are contained
            // in the increasing order of dependencies

            mRegularLst.Append( pItemNode->GetData() );

            mAllNodes.DeleteNode( pItemNode );

            // remove references to this current "least-dependent" node
            // from reference lists of all the other nodes

            RemoveReferencesToNode( pItemNode );
        }
        else
        {
            // otherwise, what is left - all nodes, which
            // are involved into cycled chains (rings)

            wxNode* pNode = mAllNodes.GetFirst();

            while( pNode )
            {
                mCycledLst.Append( pNode->GetData() );

                pNode = pNode->GetNext();
            }

            mAllNodes.Clear();
            break;
        }

        // continue search for "least-dependent" nodes

    } while(1);
}

wxList& GarbageCollector::GetRegularObjects()
{
    return mRegularLst;
}

wxList& GarbageCollector::GetCycledObjects()
{
    return mCycledLst;
}

void GarbageCollector::Reset()
{
    DestroyItemList( mAllNodes   );
    DestroyItemList( mRegularLst );
    DestroyItemList( mCycledLst );
}

