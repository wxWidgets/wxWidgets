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

#ifndef __PF_SAMPLE_G__
#define __PF_SAMPLE_G__
	  
#include "objstore.h"

// forward decl.
class classA;
class classB;

// sample classes

class notStorableClass
{};

class classA : public wxObject
{
	DECLARE_DYNAMIC_CLASS( classA )
public:

	int x;
	classB*           mpBObj;
	notStorableClass* mpBackRef;
};

class classB : public wxObject
{
	DECLARE_DYNAMIC_CLASS( classB )	
public:

	int y;
	classA*           mpAObj;
	notStorableClass* mpBackRef;
};

// serialization handlers for the above classes

class classASerializer : public wxSerializerBase
{
	DECLARE_SERIALIZER_CLASS( classASerializer )

	static void Serialize( wxObject* pObj, wxObjectStorage& store )
	{
		classA* pA = (classA*)pObj; // cast

		store.XchgInt   ( pA->x );
		store.XchgObjPtr( (wxObject**) &(pA->mpBObj)    );
		store.XchgObjPtr( (wxObject**) &(pA->mpBackRef) );
	}
};

class classBSerializer : public wxSerializerBase
{
	DECLARE_SERIALIZER_CLASS( classBSerializer )

	static void Serialize( wxObject* pObj, wxObjectStorage& store )
	{
		classB* pB = (classB*)pObj; // cast

		store.XchgInt   ( pB->y );
		store.XchgObjPtr( (wxObject**) &(pB->mpAObj)    );
		store.XchgObjPtr( (wxObject**) &(pB->mpBackRef) );
	}
};

/*---------------------------*/
/*   Main testing function   */
/*---------------------------*/

// include this header and .cpp file into any of your
// wxWindows projects, and invoke the below function 
// to peform tests

void test_obj_storage();

#endif