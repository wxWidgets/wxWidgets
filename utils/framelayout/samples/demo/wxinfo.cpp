/////////////////////////////////////////////////////////////////////////////
// Name:        No names yet.
// Purpose:     Contrib. demo
// Author:      Aleksandras Gluchovas
// Modified by:
// Created:     23/11/98
// RCS-ID:      $Id$
// Copyright:   1998 (c) Aleksandras Gluchovas
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "wxinifo.cpp"
#pragma interface "wxinifo.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/hash.h"
#include "wxinfo.h"

inline static void expand_item( wxTreeCtrl* pTree, wxTreeItemId& itemId )
{
	pTree->Expand( itemId );
}

void wxCreateClassInfoTree( wxTreeCtrl* pTree, 
						    wxTreeItemId parentBranchId,
							long classImageNo  
					      )
{
	expand_item( pTree, parentBranchId );

	wxHashTable hash;

	wxList lst;

	// collect all classes into list

	{
		wxClassInfo* pCur = wxClassInfo::GetFirst();

		wxClassInfo::InitializeClasses();

		while( pCur )
		{
			lst.Append( (wxObject*)pCur );

			pCur = pCur->GetNext();
		}
	}

	wxClassInfo::InitializeClasses();

	// reflect class-hierarchy into the tree nodes

	int nHanged;
	
	do
	{
		nHanged = 0;

		wxNode* pCur = lst.First();
		
		// repeat passes through class list, untill all of
		// the class items are "hanged" onto their parent-items in the tree

		while( pCur )
		{
			wxClassInfo& info = *((wxClassInfo*)pCur->Data());

			if ( info.GetBaseClass1() == NULL )
			{
				// parentless classes are put into the root branch

				wxTreeItemId* pId = new wxTreeItemId();
				*pId = pTree->AppendItem( parentBranchId, info.GetClassName(), classImageNo );

				expand_item( pTree, *pId );

				// "remember" it
				hash.Put( long(&info), (wxObject*)pId );

				// class is "hanged", remove it from the list
				wxNode* pTmp = pCur;

				pCur = pCur->Next();

				delete pTmp;

				++nHanged;
			}
			else
			{
				wxTreeItemId* pParentId = (wxTreeItemId*)hash.Get( (long)info.GetBaseClass1() );

				if ( pParentId != NULL )
				{
					wxTreeItemId* pId = new wxTreeItemId();

					*pId = pTree->AppendItem( *pParentId, info.GetClassName(), classImageNo );

					expand_item( pTree, *pId );

					hash.Put( long(&info), (wxObject*)pId );

					wxNode* pTmp = pCur;

					pCur = pCur->Next();

					// class is "hanged", remove it from the list
					delete pTmp;

					++nHanged;
				}
				else
				{
					// otherwise there's a parent, but it's not in the tree yet...
					// hope to "hang" it in the subsequent passes

					pCur = pCur->Next();
				}
			}
		}
		
	} while( nHanged != 0 );
}


