/////////////////////////////////////////////////////////////////////////////
// Name:        No names yet.
// Purpose:     Contrib. demo
// Author:      Aleksandras Gluchovas
// Modified by:
// Created:     26/10/98
// RCS-ID:      $Id$
// Copyright:   (c) Aleksandras Gluchovas
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////
 
#ifdef __GNUG__
#pragma implementation "pf_sample.h"
// #pragma interface
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "pf_sample.h"

IMPLEMENT_DYNAMIC_CLASS( classA, wxObject )
IMPLEMENT_DYNAMIC_CLASS( classB, wxObject )

IMPLEMENT_SERIALIZER_CLASS( classA, 
							classASerializer,
							classASerializer::Serialize,
							NO_CLASS_INIT )

IMPLEMENT_SERIALIZER_CLASS( classB, 
							classBSerializer,
							classBSerializer::Serialize,
							NO_CLASS_INIT )

// somehow original wxASSERT(0) statements get not compiled in...

#undef  wxASSERT
#define wxASSERT(x) if ( !(x) ) throw;

extern notStorableClass gNotStorable;

typedef notStorableClass* BackRefType;

void test_storing( const char* fname, wxObjectStorage& store )
{
	// create objects

	classA* pA = new classA();
	pA->x = 1;

	classB* pB = new classB();

	pB->y = 2;

	// put cross-references

	pB->mpAObj = pA;

	pA->mpBObj = pB;

	// put back-references to not-storable obj

	pA->mpBackRef = &gNotStorable;
	pB->mpBackRef = &gNotStorable;

	// create stream object for output

	wxIOStreamWrapper outFile;

	bool success = outFile.Create( fname, FALSE );

	wxASSERT( success ); 

	store.SetDataStream( outFile );

	// store everything starting from "pA" object

	store.XchgObjPtr( (wxObject**) &pA );

	// flushes stream
	store.Finalize();
}

void test_loading( const char* fname, wxObjectStorage& store )
{
	classA* pA = 0;

	// create stream-object for input 

	wxIOStreamWrapper inFile;

	bool success = inFile.Create( fname, TRUE );

	wxASSERT( success ); 

	store.SetDataStream( inFile );

	// load everything

	store.XchgObjPtr( (wxObject**) &pA );

	// calls initializing procedures for serializer
	// which provide them

	store.Finalize();

	// short-cut

	classB* pB = pA->mpBObj;

	// assertain correctness of class members

	wxASSERT( pA->x == 1 );
	wxASSERT( pB->y == 2 );

	// assertain correctness of cross-references

	wxASSERT( pA->mpBObj == pB );
	wxASSERT( pB->mpAObj == pA );

	// asssertain correctness of inital references

	wxASSERT( pA->mpBackRef == &gNotStorable );
	wxASSERT( pB->mpBackRef == &gNotStorable );
}

void setup_inital_refs( wxObjectStorage& store )
{
	store.AddInitialRef( (wxObject*) &gNotStorable );	
}

// global instance of the object, which we do not want to store/load for
// some reason, even though other stored/loaded objects have refernces to it

notStorableClass gNotStorable;

void test_storing_of_list( const char* fname, wxObjectStorage& store );
void test_loading_of_list( const char* fname, wxObjectStorage& store );

/*---------------------------*/
/*   Main testing function   */
/*---------------------------*/

void test_obj_storage()
{
	// NOTE:: for brevity, the heap clean-ups are omitted in the tests

	wxObjectStorage store;

	setup_inital_refs( store );

	test_storing( "testdata.dat", store );
	test_loading( "testdata.dat", store );

	test_storing_of_list( "testdata.dat", store );
	test_loading_of_list( "testdata.dat", store );
}

void test_storing_of_list( const char* fname, wxObjectStorage& store )
{
	// create objects

	classA* pA = new classA();
	pA->x = 1;

	classB* pB = new classB();

	pB->y = 2;

	// put cross-references

	pB->mpAObj = pA;

	pA->mpBObj = pB;

	// create list object

	wxList* pLst = new wxList;

	// put objects to the list

	wxNode* pNode = pLst->Append( pA );

	pA->mpBackRef = (BackRefType)pNode;

	pNode = pLst->Append( pB );

	pB->mpBackRef = (BackRefType)pNode;

	// create stream object for output

	wxIOStreamWrapper outFile;

	bool success = outFile.Create( fname, FALSE );

	wxASSERT( success ); 

	store.SetDataStream( outFile );

	// store everything starting from "pLst" object

	store.XchgObjPtr( (wxObject**) &pLst );

	// flushes stream
	store.Finalize();
}

void test_loading_of_list( const char* fname, wxObjectStorage& store )
{
	// create stream-object for input 

	wxIOStreamWrapper inFile;

	bool success = inFile.Create( fname, TRUE );

	wxASSERT( success ); 

	store.SetDataStream( inFile );

	// load everything

	wxList* pLst;

	// (NOTE:: serializers for wxList/wxNode is file objstore.cpp)

	store.XchgObjPtr( (wxObject**) &pLst );

	// assertain correctness of list and it's contents

	wxASSERT( pLst->Number() == 2 );

	int n = 0;
	wxNode* pNode = pLst->First();

	while( pNode )
	{
		if ( n == 0 )
		{
			classA* pA = (classA*)pNode->Data();

			// assertain correctness of class members

			wxASSERT( pA->x == 1 );

			// assertain correctness of cross-references

			wxASSERT( pA->mpBObj == (classB*)pNode->Next()->Data() );

			// asssertain correctness of inital references

			wxASSERT( (wxNode*)pA->mpBackRef == pNode )
		}
		if ( n == 1 )
		{
			classB* pB = (classB*)pNode->Data();

			// assertain correctness of class members

			wxASSERT( pB->y == 2 );

			// assertain correctness of cross-references

			wxASSERT( pB->mpAObj == (classA*)pNode->Previous()->Data() );

			// asssertain correctness of inital references

			wxASSERT( (wxNode*)pB->mpBackRef == pNode )
		}

		pNode = pNode->Next();
		++n;
	}

	// calls initializing procedures for serializer
	// which provide them

	store.Finalize();
}
