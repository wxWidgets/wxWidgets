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


#ifdef __GNUG__
    #pragma implementation "garbagec.h"
#endif

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
    return *( (GCItem*)(pNode->Data()) );
}

GarbageCollector::~GarbageCollector()
{
    Reset();
}

/*** GC alg. helpers ***/

void GarbageCollector::DestroyItemList( wxList& lst )
{
    wxNode* pNode = lst.First();

    while( pNode )
    {
        delete &node_to_item( pNode );

        pNode = pNode->Next();
    }

    lst.Clear();
}

wxNode* GarbageCollector::FindItemNode( void* pForObj )
{
    wxNode* pNode = mAllNodes.First();

    while( pNode )
    {
        if ( node_to_item( pNode ).mpObj == pForObj )

            return pNode;

        pNode = pNode->Next();
    }

    int avoidCompilerWarning = 0;
    wxASSERT(avoidCompilerWarning); // DBG:: item should be present

    return 0;
}

wxNode* GarbageCollector::FindReferenceFreeItemNode()
{
    wxNode* pNode = mAllNodes.First();

    while( pNode )
    {
        if ( node_to_item( pNode ).mRefs.Number() == 0 ) 

            return pNode;

        pNode = pNode->Next();
    }

    return 0;
}

void GarbageCollector::RemoveReferencesToNode( wxNode* pItemNode )
{
    wxNode* pNode = mAllNodes.First();

    while( pNode )
    {
        wxList& refLst   = node_to_item( pNode ).mRefs;
        wxNode* pRefNode = refLst.First();

        while( pRefNode )
        {
            if ( pRefNode->Data() == (wxObject*)pItemNode )
            {
                wxNode* pNext = pRefNode->Next();

                refLst.DeleteNode( pRefNode );

                pRefNode = pNext;

                continue;
            }
            else pRefNode = pRefNode->Next();
        }

        pNode = pNode->Next();
    }
}

void GarbageCollector::ResolveReferences()
{
    wxNode* pNode = mAllNodes.First();

    while( pNode )
    {
        GCItem& item = node_to_item( pNode );

        wxNode* pRefNode = item.mRefs.First();

        while( pRefNode )
        {
            pRefNode->SetData( (wxObject*) FindItemNode( (void*)pRefNode->Data() ) );

            pRefNode = pRefNode->Next();
        }

        pNode = pNode->Next();
    }
}

void GarbageCollector::AddObject( void* pObj, int refCnt )
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

            mRegularLst.Append( pItemNode->Data() );

            mAllNodes.DeleteNode( pItemNode );

            // remove references to this current "least-dependent" node
            // from reference lists of all the other nodes

            RemoveReferencesToNode( pItemNode );
        }
        else
        {
            // otherwise, what is left - all nodes, which
            // are involved into cycled chains (rings)

            wxNode* pNode = mAllNodes.First();

            while( pNode )
            {
                mCycledLst.Append( pNode->Data() );

                pNode = pNode->Next();
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

