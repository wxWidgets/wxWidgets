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
#pragma implementation "objstore.h"
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

#include <fstream.h>
#include "objstore.h"
#include <string.h>

// FIXME::
// BUG?:: somehow assertion statements with oritinal wxASSERT do not get compiled in

//#undef  wxASSERT
//#define wxASSERT(x) if ( !(x) ) throw;

/***** Implementation for class wxSerializerInfo *****/

bool wxSerializerInfo::alreadyInitialized = FALSE;
wxSerializerInfo *wxSerializerInfo::first = NULL;
wxHashTable wxSerializerInfo::serInfoHash;

wxSerializerInfo::wxSerializerInfo( char*                    theClassName, 
								    wxObjectSerializationFn  serializationFun,
									wxObjectInitializationFn initializationFun,
									char*                    classVersionName
								  )
	: classInfo    ( NULL ),
	  next         ( NULL ),
	  nextByVersion( NULL ),
	  className    ( theClassName      ),
	  serFn        ( serializationFun  ),
	  initFn       ( initializationFun ),
	  classVersion ( classVersionName  )  
{
	next = first;

	first = this;
}

int serializer_ver_cmp_fun( const void* arg1, const void* arg2 )
{
	// "no-version" is considered being the highest version

	if ( ((wxSerializerInfo*)arg1)->classVersion == NO_CLASS_VER )
	{
		// DBG:: two serializers for the same version of the class should not be present!
		wxASSERT( ((wxSerializerInfo*)arg1)->classVersion != NO_CLASS_VER );

		return -1; // (inverted already)
	}

	if ( ((wxSerializerInfo*)arg2)->classVersion == NO_CLASS_VER )
	{
		// DBG:: two serializers for the same version of the class should not be present!
		wxASSERT( ((wxSerializerInfo*)arg1)->classVersion != NO_CLASS_VER );

		return 1; // (inverted already)
	}

	// versions are compared lexicographically ignoring the char-case

	wxString v1( ((wxSerializerInfo*)arg1)->classVersion );
	wxString v2( ((wxSerializerInfo*)arg2)->classVersion );
	
	bool result = v1.CompareTo( v2, wxString::ignoreCase );

	// DBG:: two serializers for the same version of the class should not be present!
	wxASSERT( result == FALSE );

	// invert the sense of "greater than" for storting in decreasing order

	return ( result > 0 ) ? -1 : 1;
}

void wxSerializerInfo::InitializeSerializers(void)
{
	if ( alreadyInitialized ) return;

	alreadyInitialized = TRUE;

	wxSerializerInfo* pCur = first;
	
	// first resolve references to class information structures

	while( pCur )
	{
		pCur->classInfo = wxClassInfo::FindClass( pCur->className );

		wxASSERT( pCur->classInfo ); // DBG:: class info should already be present somewhere!

		// check if serializer for the class is already present,

		wxSerializerInfo* pFound = (wxSerializerInfo*)serInfoHash.Get( (long)pCur->classInfo );

		if ( pFound )
		{
			// if present, then it must be serializer for the certain version of that class, 
			// put it at the end of the chain of versioned serializers for that class

			// go to the end of chain

			while( pFound->nextByVersion ) pFound = pFound->nextByVersion;

			// append it

			pFound->nextByVersion = pCur;

			pCur->next = (wxSerializerInfo*)(-1); // label it as member of local chain
											      // of "versioned" serializers

			pCur->nextByVersion = NULL;
		}
		else
		{
			// otherwise, serializer for the class found for the first time -
			// hash it

			serInfoHash.Put( (long)pCur->classInfo, (wxObject*)pCur );

			// and include it to the list of serializers for the highest-versions 
			pCur = pCur->next;
		}
	}

	// sort chains of "versioned" serializers in the order of decreasing version 
	//
	// (since, when loading, the newest version of an object
	//  is expected first, rather then the older one)

	wxSerializerInfo* pPrev = NULL;
	pCur = first;

	while ( pCur )
	{
		// chain present?

		if ( pCur->nextByVersion )
		{
			// sort it

			wxSerializerInfo* pStart = pCur;
			wxSerializerInfo* pNext = pCur->next;

			// let wxList do the sorting, we're too lazy :)

			wxList sorted;

			while( pCur )
			{
				sorted.Append( (wxObject*) pCur );

				pCur = pCur->nextByVersion;
			}

			sorted.Sort( serializer_ver_cmp_fun );

			wxNode* pNode = sorted.First();

			while( pNode )
			{
				wxSerializerInfo* pInfo = (wxSerializerInfo*)pNode->Data();

				if ( pNode == sorted.First() )
				{				
					// make node with the highest version, a member of the global
					// list of serializers

					if ( pPrev ) pPrev->next = pInfo;
							else first       = pInfo;

					pInfo->next = pNext;
				}
				else
					pInfo->next = (wxSerializerInfo*)(-1); // otherwise label it as a member of local
														   //  chain of "versioned" serializers

				if ( pNode->Next() )
				{
					pInfo->nextByVersion = (wxSerializerInfo*)( pNode->Next()->Data() );
				}
				else
					pInfo->nextByVersion = 0;

				pNode = pNode->Next();
			}

		} // end of if ( nextByVersion ) 

		pPrev = pCur;
		pCur = pCur->next;

	} // end of while(...)
}

wxSerializerInfo* wxSerializerInfo::FindSerializer( char* className )
{
	wxSerializerInfo::InitializeSerializers();

	wxSerializerInfo* pInfo = (wxSerializerInfo*)
		
		serInfoHash.Get( (long)wxClassInfo::FindClass( className ) );

	return pInfo;
}

static void invoke_for_inherited( wxObject* pObj, wxClassInfo* pCInfo, wxObjectStorage* pStore, bool invokeSerFn )
{
	wxSerializerInfo* pSrzInfo = (wxSerializerInfo*)
		
		wxSerializerInfo::serInfoHash.Get( (long)wxClassInfo::FindClass( pCInfo->GetClassName() ) );

	if ( pSrzInfo )
	{	
		// if found, serialize/initialize and don't go "any higher"

		if ( invokeSerFn )
			
			(*pSrzInfo->serFn) ( pObj, *pStore );
		else
			(*pSrzInfo->initFn)( pObj );

	}
	else
	{
		// go up the hierarchy, if no serializer present for the current class

		if ( pCInfo->GetBaseClass1() )	

			invoke_for_inherited( pObj, pCInfo->GetBaseClass1(), pStore, invokeSerFn );

		if ( pCInfo->GetBaseClass2() )

			invoke_for_inherited( pObj, pCInfo->GetBaseClass2(), pStore, invokeSerFn );
	}
}

void wxSerializerInfo::SerializeInherited( wxObject* pObj, wxObjectStorage& store )
{
	// search recursivelly up the hierarchy for serializers of base
	// classes, and invoke serialization function for the given object

	if ( classInfo->GetBaseClass1() )	

		invoke_for_inherited( pObj, classInfo->GetBaseClass1(), &store, TRUE );

	if ( classInfo->GetBaseClass2() )	

		invoke_for_inherited( pObj, classInfo->GetBaseClass2(), &store, TRUE );
}

void wxSerializerInfo::InitializeInherited( wxObject* pObj )
{
	// search recursivelly up the hierarchy for serializers of base
	// classes, and invoke initialization function for the given object

	if ( classInfo->GetBaseClass1() )	

		invoke_for_inherited( pObj, classInfo->GetBaseClass1(), NULL, FALSE );

	if ( classInfo->GetBaseClass2() )

		invoke_for_inherited( pObj, classInfo->GetBaseClass2(), NULL, FALSE );
}

/***** Implementation for class wxDataStreamBase *****/

IMPLEMENT_ABSTRACT_CLASS( wxDataStreamBase, wxObject )

/***** Implementation for class wxObjectStorage *****/

// codes, used as tokens written/read from the stream

enum STORED_OBJ_TYPES
{
	SOT_NULL_POINTER   = 'N',
	SOT_POINTER_TO_OBJ = 'P',
	SOT_INITIAL_REF    = 'I', 
	SOT_OBJ_DATA       = 'D' 
};

// veraion-encoding in object-name string fromat defaults:

char wxObjectStorage::mVerSepartorCh        = '#';
char wxObjectStorage::mMinorMajorSepartorCh = '-';

IMPLEMENT_DYNAMIC_CLASS( wxObjectStorage, wxObject )

wxObjectStorage::wxObjectStorage()
	: mpStm           ( 0 ),
	  mIsLoading      ( TRUE ),
	  mInitialRefsCnt ( 0 ),
	  mFinalizePending( FALSE )
{}

wxObjectStorage::wxObjectStorage( wxDataStreamBase& stm )
	: mpStm           ( &stm ),
	  mIsLoading      ( stm.IsForInput() ),
	  mInitialRefsCnt ( 0 ),
	  mFinalizePending( FALSE )
{
	wxSerializerInfo::InitializeSerializers();

	mFinalizePending = TRUE; // stream object was given - store/load is 
	                         // started
}

wxObjectStorage::~wxObjectStorage()
{
	if ( mFinalizePending )

		Finalize(); // <- do it now, if "user" forgot about it
}

/*** protected members ***/

void wxObjectStorage::ClearHashesAndLists()
{
	mNewObjs.Clear();
	mSerializersForNewObjs.Clear();
	mRefHash.Clear();
}

/*** public members ***/

void wxObjectStorage::SetDataStream( wxDataStreamBase& stm )
{
	if ( mFinalizePending )

		Finalize();

	wxSerializerInfo::InitializeSerializers();

 	ClearHashesAndLists();

	mpStm = &stm;

	mIsLoading = stm.IsForInput();

	mFinalizePending = TRUE;
}

void wxObjectStorage::Finalize()
{
	wxASSERT( mpStm ); // DBG:: finalize should called be after loading/storing has proceeded

	mFinalizePending = FALSE;

	if ( mIsLoading )
	{
		// initializaiton is performed after all objects successfully
		// loaded, and references among them are established

		wxNode* pObjNode = mNewObjs.First();
		wxNode* pSrzNode = mSerializersForNewObjs.First();

		while( pObjNode )
		{
			wxSerializerInfo* pSrzInfo = (wxSerializerInfo*)(pSrzNode->Data());

			if ( pSrzInfo->HasInitializer() )

				(*pSrzInfo->initFn)( pObjNode->Data() );

			pObjNode = pObjNode->Next();
			pSrzNode = pSrzNode->Next();
		}
	}
	else
		// otherwise, nothing's need to be done after storing of objects is proceeded
		mpStm->Flush();
}

// storage methods for basic types

void wxObjectStorage::XchgChar( char& chObj )
{
	if ( mIsLoading ) mpStm->LoadChar( &chObj );
				 else mpStm->StoreChar( chObj );
}

void wxObjectStorage::XchgInt( int& intObj )
{
	if ( mIsLoading ) mpStm->LoadInt( &intObj );
				 else mpStm->StoreInt( intObj );
}

void wxObjectStorage::XchgSizeType( size_t& szObj )
{
	int i = int(szObj);

	if ( mIsLoading ) 
	{
		mpStm->LoadInt( &i );
		szObj = (size_t)i;
	}
	else
		mpStm->StoreInt( i );
}

void wxObjectStorage::XchgLong( long& longObj )
{
	if ( mIsLoading ) mpStm->LoadLong( &longObj );
				 else mpStm->StoreLong( longObj );
}

void wxObjectStorage::XchgBool( bool& boolObj )
{
	// bools are stored as ints

	if ( mIsLoading )
	{
		int bVal = (int)boolObj;
		mpStm->LoadInt( &bVal );
		boolObj = bVal;
	}
	else
		mpStm->StoreInt( (int)boolObj );
}

void wxObjectStorage::XchgUInt  ( unsigned int& uI  )
{
	if ( mIsLoading )
	{
		int uiVal = (int)uI;
		mpStm->LoadInt( &uiVal );
		uI = (unsigned int)uiVal;
	}
	else
		mpStm->StoreInt( (int)uI );
}

void wxObjectStorage::XchgObjList( wxList& objList )
{
	int count = 0;

	if ( mIsLoading )
	{
		XchgInt( count );

		if ( count == 0 ) return;

		objList.Clear();
	}
	else
	{
		count = objList.GetCount();

		XchgInt( count );
	}

	// work-around for assessing operator[] which is protected in wxArrayBase

	if ( mIsLoading )
	
		for( int i = 0; i != count; ++i )
		{
			wxObject* pObj = NULL;

			XchgObjPtr( &pObj );

			objList.Append( pObj );
		}
	else
	{
		wxNode* pNode = objList.First();

		while( pNode )
		{
			wxObject* pObj = pNode->Data();

			XchgObjPtr( &pObj );

			pNode = pNode->Next();
		}
	}
}

void wxObjectStorage::XchgObjArray ( wxBaseArray& objArr )
{
	int count = 0;

	if ( mIsLoading )
	{
		XchgInt( count );

		if ( count == 0 ) return;

		objArr.Clear();
		objArr.Alloc( count );
	}
	else
	{
		count = objArr.GetCount();

		XchgInt( count );
	}

	// work-around for assessing operator[] which is protected in wxArrayBase

	wxArrayLong& longArr = *( (wxArrayLong*) (&objArr) );

	if ( mIsLoading )
	
		for( int i = 0; i != count; ++i )
		{
			wxObject* pObj = NULL;

			XchgObjPtr( &pObj );

			longArr.Add( (long) pObj );
		}
	else
		for( int i = 0; i != count; ++i )
		{
			wxObject* pObj = (wxObject*)longArr[i];

			XchgObjPtr( &pObj );
		}
}

void wxObjectStorage::XchgLongArray( wxBaseArray& longArr )
{
	int count = 0;

	if ( mIsLoading )
	{
		XchgInt( count );

		if ( count == 0 ) return;

		longArr.Clear();
		longArr.Alloc( count );
	}
	else
	{
		count = longArr.GetCount();

		XchgInt( count );
	}

	// work-around for assessing operator[] which is protected in wxArrayBase

	wxArrayLong& realLongArr = *( (wxArrayLong*) (&longArr) );

	if ( mIsLoading )
	
		for( int i = 0; i != count; ++i )
		{
			long l = 0;
			XchgLong( l );

			realLongArr.Add( l );
		}
	else
		for( int i = 0; i != count; ++i )
		
			XchgLong( realLongArr[i] );
}

void wxObjectStorage::XchgDouble( double& doubleObj )
{
	if ( mIsLoading ) mpStm->LoadDouble( &doubleObj );
				 else mpStm->StoreDouble( doubleObj );
}

void wxObjectStorage::XchgCStr( char* pCStrObj )
{
	if ( mIsLoading )
	{
		int len;
		mpStm->LoadInt( &len );
		mpStm->LoadBytes( pCStrObj, len );
	}
	else
	{
		int len = strlen( pCStrObj ) + 1; // include terminating zero
		mpStm->StoreInt( len );
		mpStm->StoreBytes( pCStrObj, len );
	}
}

wxSerializerInfo* wxObjectStorage::FindSrzInfoForClass( wxClassInfo* pInfo )
{
	wxSerializerInfo* pSrz = (wxSerializerInfo*)

		wxSerializerInfo::serInfoHash.Get( (long)pInfo );

	if ( !pSrz )
	{
		// look up recursivelly for serializers for of the base classes

		if ( pInfo->GetBaseClass1() ) 
			
			return FindSrzInfoForClass( pInfo->GetBaseClass1() );
		else
		if ( pInfo->GetBaseClass2() )

			return FindSrzInfoForClass( pInfo->GetBaseClass2() );
		else
		{
			wxASSERT(0); // DBG:: no serializers present for the given class,
						 //       serialization cannot proceed
			return 0;
		}
	}
	else
		return pSrz;
}

bool wxObjectStorage::VersionsMatch( char* v1, char* v2 )
{
	while( *v1 && *v2 )
	{
		if ( *v1 == mMinorMajorSepartorCh ||
			 *v2 == mMinorMajorSepartorCh )

			 // minor versions are ignored 

			 return TRUE;

		if ( toupper(*v1) != toupper(*v2) )

			return FALSE;

		++v1; ++v2;
	}

	return ( *v1 == '\0' && *v2 == '\0' );
}

bool wxObjectStorage::ExchangeObjectInfo( wxClassInfo** ppCInfo, wxSerializerInfo** ppSrz )
{
	char objInfoStr[512]; // FOR NOW:: fixed?

	if ( mIsLoading == FALSE )
	{
		strcpy( objInfoStr, (*ppCInfo)->GetClassName() );

		if ( (*ppSrz)->HasVersion() )
		{
			char separator[2];
			separator[2] = mVerSepartorCh;
			separator[1] = '\0';

			strcat( objInfoStr, separator );

			strcat( objInfoStr, (*ppSrz)->classVersion );
		}

		XchgCStr( objInfoStr );

		return TRUE;
	}

	// otherwise if loading...

	XchgCStr( objInfoStr ); // read string

	(*ppCInfo) = NULL;
	(*ppSrz)   = NULL;

	// formal description of objInfoStr format is following (if '#' and '-' are set
	// as version and minor/major separators): 
	//
	// object objInfoStr = { class_name_str , [version] }
	//
	// version = { '#', simple_version_str | major_and_minor_version_str }
	//
	// simple_version_str = any_literal
	//
	// major_and_minro_version_str = { major_version_str, '-', minro_version_str }
	//
	// major_version_str = any_literal
	//
	// inor_version_str  = any_literal
	//
	// any_literal = "any string not containing '#' and '-' characters"
    //

	char* cur = objInfoStr;

	while( *cur && *cur != mVerSepartorCh ) ++cur;

	char last = *cur;

	if ( last == mVerSepartorCh )

		*cur = '\0';

	(*ppCInfo) = wxClassInfo::FindClass( objInfoStr );

	if ( !(*ppCInfo) ) return FALSE;

	// get the bigining of the chain of serializers for (*ppCInfo)

	wxSerializerInfo* pSrz = FindSrzInfoForClass( (*ppCInfo ) );

	if ( last == mVerSepartorCh ) *cur = last; // restore from temprary "termination"

	// find serializer which matches the version, or the serializer
	// with no version, if version is not attached to the objInfoStr

	if ( *cur == '\0' )
	{
		// there's no version trailing, the "not-versioned"
		// serializer can be present only at the begining of
		// the chain

		if ( pSrz->HasVersion() == FALSE ) 
		{
			(*ppSrz) = pSrz;

			return TRUE;
		}
		else 
			return FALSE;
	}

	++cur; // skip className<->version separator

	// version present, search for matching serializer down the chain

	if ( pSrz->HasVersion() == FALSE ) pSrz = pSrz->nextByVersion;

	while( pSrz )
	{
		if ( VersionsMatch( pSrz->classVersion, cur ) )
		{
			(*ppSrz) = pSrz;

			return TRUE;
		}

		 pSrz = pSrz->nextByVersion;
	}

	return FALSE; // no serializers matching given version found
}

wxSerializerInfo* wxObjectStorage::GetLatestSrzForObj( wxObject* pWxObj )
{
	wxClassInfo* pCInfo = pWxObj->GetClassInfo();

	wxASSERT( pCInfo ); // DBG:: object's class should be dynamic

	// find first serializer for (*pCInfo) in the chain 

	wxSerializerInfo* pSrz = FindSrzInfoForClass( pCInfo );

	wxASSERT( pSrz ); // DBG:: there should be at least one serializer of
					  //       the object's class, at least for it's base classes

	// skip not-versioned serializer at the beginng of the chain if present

	if ( !pSrz->HasVersion() && pSrz->nextByVersion )

		pSrz = pSrz->nextByVersion; // the reminding ones are "vesioned", 
	                                // starting from the highest version

	return pSrz;
}

void wxObjectStorage::DoExchangeObject( wxObject* pInstance, wxSerializerInfo& srzInfo )
{
	if ( mIsLoading )
	
		// put info about already (partially) loaded object (stream-offset <=> object-ptr   )

		mRefHash.Put( (long)mpStm->GetStreamPos(), (wxObject*)pInstance );
	else
		// put info about already (partially) stored object (object-ptr    <=> stream-offset)

		mRefHash.Put( (long)(pInstance), (wxObject*)mpStm->GetStreamPos() );

	if ( mIsLoading )
	{
		mNewObjs.Append( pInstance );
		mSerializersForNewObjs.Append( (wxObject*)&srzInfo );
	}

	// now, perform actual serialization of the object

	(srzInfo.serFn)( pInstance, *this );
}

// storage methods for objects and object-references

void wxObjectStorage::XchgObj( wxObject* pWxObj )
{
	wxClassInfo* pCInfo = pWxObj->GetClassInfo();

	wxASSERT( pCInfo );  // DBG:: if this fails, the object which is passed to
	                     //       XchgObj(..) has not it's class information
						 //       i.e. is not a wxWindows dynamic class

	wxSerializerInfo* pSrz = ( mIsLoading == FALSE ) ? GetLatestSrzForObj( pWxObj )
											 		 : NULL;

	bool success = ExchangeObjectInfo( &pCInfo, &pSrz );

	wxASSERT( success ); // DBG:: all info about object should be present i.e. class 
	                     //       info, serializer which matches the object's version
						 //       (see source of ExchangeObjectInfo() for more info)

	DoExchangeObject( pWxObj, *pSrz );
}

void wxObjectStorage::XchgObjPtr( wxObject** ppWxObj )
{
	if ( mIsLoading )
	{
		char token;
		mpStm->LoadChar( &token );

		if ( token == (char)SOT_NULL_POINTER )
		
			(*ppWxObj) = NULL;
		else
		if ( token == (char)SOT_POINTER_TO_OBJ )
		{
			long ofs; // stream-offset

			mpStm->LoadLong( &ofs );

			wxObject* pObj = (wxObject*) mRefHash.Get( ofs );

			wxASSERT( pObj ); // DBG:: object (at the given stream-offset) 
							  //       must be already loaded

			(*ppWxObj) = pObj;
		}
		else 
		if ( token == (char)SOT_INITIAL_REF )
		{
			long refNo;

			mpStm->LoadLong( &refNo );

			wxASSERT( mInitialRefsCnt >= refNo ); // DBG:: inital refernce should be already added

			// refNo is 1-based
			(*ppWxObj) = mInitialRefs.Nth( refNo-1 )->Data();
		}
		else
		{
			wxASSERT( token == (char)SOT_OBJ_DATA );// DBG:: other types of tokens are
				                                    //       bogous! If this happens you're
													//       probably trying to load the data 
													//       which was stored by defferent. 
													//       perhaps out-dated serializers
													//
													//       Use versioning mechanizm to
													//       privide backwards compatiblity
													//       option, among different versions
													//       stored data-format


			int stmPos = mpStm->GetStreamPos();

			wxClassInfo* pCInfo     = 0;
			wxSerializerInfo* pSrz  = 0;

			bool success = ExchangeObjectInfo( &pCInfo, &pSrz );

			wxASSERT( success ); // DBG:: all info about object should be present
								 //       i.e. class info, serializer which matches the object's 
								 //       version (see source of ExchangeObjectInfo() for more info)

			(*ppWxObj) = pCInfo->CreateObject();

			DoExchangeObject( (*ppWxObj), *pSrz );
		}
	}
	else
	{
		// otherwise if storing the pointer to an object

		// first, check if it's an initial reference

		long refNo = (long)mInitialRefsHash.Get( (long)(*ppWxObj) );

		if ( refNo != 0 )
		{
			mpStm->StoreChar( (char)SOT_INITIAL_REF );
			mpStm->StoreLong( refNo );

			return;
		}

		long streamPos = (long) mRefHash.Get( (long)(*ppWxObj) );

		// check if object is already stored
		if ( streamPos != 0 )
		{
			// store only reference to the object (in the form of stream-offset)

			mpStm->StoreChar( (char)SOT_POINTER_TO_OBJ );
			mpStm->StoreLong( streamPos );
		}
		else
		{
			// otherwise store the entire referenced object

			if ( (*ppWxObj) == NULL )
			{
				mpStm->StoreChar( (char)SOT_NULL_POINTER ); // token
				return;
			}

			mpStm->StoreChar( (char)SOT_OBJ_DATA ); // token

			// store object's info and data
			XchgObj( *ppWxObj );
		}
	}
}

// storage methods for common wxWindows objects

void wxObjectStorage::XchgWxStr( wxString& str )
{
	if ( mIsLoading )
	{
		long len = 0;
		mpStm->LoadLong( &len );

		str = "";
		str.Append( (char)1, len );

		mpStm->LoadBytes( (char*)str.c_str(), len );
	}
	else
		XchgCStr( (char*)str.c_str() );
}

void wxObjectStorage::XchgWxSize( wxSize&   size )
{
	XchgLong( size.x );
	XchgLong( size.y );
}

void wxObjectStorage::XchgWxPoint( wxPoint&  point )
{
	XchgLong( point.x );
	XchgLong( point.y );
}

void wxObjectStorage::XchgWxRect( wxRect&   rect  )
{
	XchgLong( rect.x );
	XchgLong( rect.y );
	XchgLong( rect.width  );
	XchgLong( rect.height );
}

void wxObjectStorage::AddInitialRef( wxObject* pObjRef )
{
	// duplicates are not accepted

	if ( mInitialRefsHash.Get( (long)pObjRef ) != NULL )
	
		return;
		
	++mInitialRefsCnt;

	// NOTE:: reference number is 1-based (zero is "reserved" by wxHashTable)

	mInitialRefs.Append( pObjRef );
	mInitialRefsHash.Put( (long)pObjRef, (wxObject*)mInitialRefsCnt );
}

void wxObjectStorage::ClearInitalRefs()
{
	mInitialRefsCnt = 0;

	mInitialRefsHash.Clear();
	mInitialRefsHash.Clear();
}

/***** Implementation for class wxColourSerializer *****/

IMPLEMENT_SERIALIZER_CLASS( wxColour, 
							wxColourSerializer,
							wxColourSerializer::Serialize,
							NO_CLASS_INIT )

void wxColourSerializer::Serialize( wxObject* pObj, wxObjectStorage& store )
{
	wxColour* pCol = (wxColour*)pObj;

	// slightly optimized

	if ( store.IsLoading() )
	{
		long rgb;
		store.XchgLong( rgb );

		*pCol = wxColour( rgb         & 0xFF,
				  ( rgb >> 8  ) & 0xFF, 
				  ( rgb >> 16 ) & 0xFF );
		
	}
	else
	{
		long rgb = 0;

		unsigned char r = pCol->Red(),g = pCol->Green(), b = pCol->Blue();
		
		rgb = (   long(r)         & 0x0000FF ) | 
			  ( ( long(g) << 8  ) & 0x00FF00 ) |
			  ( ( long(b) << 16 ) & 0xFF0000 );

		store.XchgLong( rgb );
	}
}

/***** Implementation for class wxPenSerializer *****/

IMPLEMENT_SERIALIZER_CLASS( wxPen, 
							wxPenSerializer,
							wxPenSerializer::Serialize,
							NO_CLASS_INIT )

void wxPenSerializer::Serialize( wxObject* pObj, wxObjectStorage& store )
{
	wxPen* pPen = (wxPen*)pObj;

	int      cap;
	wxColour col;
	int      join;
	int      style;
	int      width;

	if ( store.IsLoading() == FALSE )
	{
		cap   = pPen->GetCap();
		col   = pPen->GetColour();
		join  = pPen->GetJoin();
		style = pPen->GetStyle();
		width = pPen->GetWidth();
	}

	store.XchgInt( cap );
	store.XchgObj( (wxObject*) &col );
	store.XchgInt( join );
	store.XchgInt( style );
	store.XchgInt( width );

	if ( store.IsLoading() )
	{
		pPen->SetCap   ( cap );
		pPen->SetColour( col );
		pPen->SetJoin  ( join );
		pPen->SetStyle ( style );
		pPen->SetWidth ( width );
	}
}

/***** Implementation for class wxBrushSerializer *****/

IMPLEMENT_SERIALIZER_CLASS( wxBrush, 
							wxBrushSerializer,
							wxBrushSerializer::Serialize,
							NO_CLASS_INIT )

void wxBrushSerializer::Serialize( wxObject* pObj, wxObjectStorage& store )
{
	wxBrush* pBrush = (wxBrush*)pObj;

	wxColour col;
	int      style;

	if ( store.IsLoading() == FALSE )
	{
		col   = pBrush->GetColour();
		style = pBrush->GetStyle();
	}

	store.XchgObj( (wxObject*) &col );
	store.XchgInt( style );

	if ( store.IsLoading() )
	{
		pBrush->SetColour( col );
		pBrush->SetStyle ( style );
	}
}

/***** Implementation for class wxEvtHandlerSerializer *****/

IMPLEMENT_SERIALIZER_CLASS( wxEvtHandler, 
							wxEvtHandlerSerializer,
							wxEvtHandlerSerializer::Serialize,
							wxEvtHandlerSerializer::Initialize )

void wxEvtHandlerSerializer::Serialize( wxObject* pObj, wxObjectStorage& store )
{
	wxEvtHandler* pHnd = ( wxEvtHandler*) pObj;

	wxEvtHandler* pPrevHnd;
	wxEvtHandler* pNextHnd;

	if ( store.IsLoading() == FALSE )
	{
		// extract properties when storing

		pPrevHnd = pHnd->GetPreviousHandler();
		pNextHnd = pHnd->GetNextHandler();
	}

	// serialize properties

	store.XchgObjPtr( (wxObject**) &pPrevHnd );
	store.XchgObjPtr( (wxObject**) &pNextHnd );

	if ( store.IsLoading() )
	{
		// set properties when loading

		pHnd->SetPreviousHandler( pPrevHnd );
		pHnd->SetNextHandler    ( pNextHnd );
	}
}

void wxEvtHandlerSerializer::Initialize( wxObject* pObj )
{
	wxEvtHandler* pHnd = ( wxEvtHandler*) pObj;

	// if we're on top
	if ( pHnd->GetPreviousHandler() == NULL )
	{
		// then check if we're in the chain which is
		// attached to wxWindow object

		wxEvtHandler* pCur = pHnd->GetNextHandler();

		while( pCur )
		{
			if ( pCur->IsKindOf( CLASSINFO(wxWindow) ) )
			{
				// since we are the the right-most event handler
				// in the chain, then we must be the first
				// receiver of events sent to the window obj. -
				// therefore "make it happen":
				
				((wxWindow*)pCur)->SetEventHandler( pHnd );

				// but if wxWindow is persistant, then why
				// we're setting "manually" the property 
				// which is serialized anyway? 
				//
				// The *PROBLEM* is that, it's not always good idea 
				// to serialize a window (e.g. main frame), instead 
				// they could be referred by "inital-refernces". To 
				// handle the later case, we additionally make sure  
				// that serialized evt. handlers are "glued" to the 
				// window correctly,even if the window is transient 
				// itself

				return;
			}

			// keep on searching for wxWindows down the chain

			pCur = pCur->GetNextHandler();
		}
	}
}

/***** Implementation for class wxWindowSerializer *****/

IMPLEMENT_SERIALIZER_CLASS( wxWindow, 
							wxWindowSerializer,
							wxWindowSerializer::Serialize,
							NO_CLASS_INIT )

void wxWindowSerializer::Serialize( wxObject* pObj, wxObjectStorage& store )
{
	DoSerialize( pObj, store, (wndCreationFn)wxWindowSerializer::CreateWindowFn );
}

void wxWindowSerializer::DoSerialize( wxObject* pObj, wxObjectStorage& store,
									  wndCreationFn creationFn, bool refreshNow )
{
	// wxWindow is a kind of wxEvtHandler - peform serialization of
	// the base class first

	info.SerializeInherited( pObj, store );

	wxWindow* pWnd = (wxWindow*)pObj;

	long          id;
	long          style;
	wxString      name;
	wxPoint       pos;
	wxSize        size;
	wxColour      bkCol;
	wxWindow*     pParent;
	wxList*       pCldLst;
	wxEvtHandler* pEvtHandler;

	wxList		  tmpCldLst;

	if ( store.IsLoading() == FALSE )
	{
		// extract properties from window object

		name  = pWnd->GetName();
		id    = pWnd->GetId();
		style = pWnd->GetWindowStyleFlag();

		// workaround for long/int inconsitency of wxWin2.0a
		int x,y,w,h;
		pWnd->GetPosition( &x, &y  );
		pWnd->GetSize    ( &w, &h );
		bkCol = pWnd->GetBackgroundColour();

		pos.x  = x; pos.y  = y;
		size.x = w; size.y = h;

		pEvtHandler = pWnd->GetEventHandler();
		pParent     = pWnd->GetParent();

#ifdef __HACK_MY_MSDEV40__
		pCldLst     = pWnd->GetChildren();
#else
		pCldLst     = &pWnd->GetChildren();
#endif

	}

	// serialize properties

	store.XchgWxStr ( name   );
	store.XchgLong  ( id     );
	store.XchgLong  ( style  );
	store.XchgLong  ( pos.x  );
	store.XchgLong  ( pos.y  );
	store.XchgLong  ( size.x );
	store.XchgLong  ( size.y );
	store.XchgObj   ( (wxObject* ) &bkCol );
	store.XchgObjPtr( (wxObject**) &pParent );
	store.XchgObjPtr( (wxObject**) &pEvtHandler );

	if ( store.IsLoading() )
	{

		// serialize to on-stack list object, since children will
		// automatically add themselves to parent's list

		pCldLst = &tmpCldLst;

		// first create window (when loading), then serialize it's children

		(*creationFn)( pWnd, pParent, id, pos, size, style, name );

		//pWnd->SetBackgroundColour( bkCol );

		//pWnd->SetBackgroundColour( bkCol );

		if ( refreshNow && 0 ) pWnd->Refresh();
	}

	store.XchgObjList( *pCldLst );
}

void wxWindowSerializer::CreateWindowFn( wxWindow* wnd, wxWindow* parent, const wxWindowID id, 
									     const wxPoint& pos, const wxSize& size, long style , 
									     const wxString& name )
{
	wnd->Create( parent, id, pos, size, style, name );
}

/***** Implementation for class wxTextCtrlSerializer *****/

IMPLEMENT_SERIALIZER_CLASS( wxTextCtrl, 
							wxTextCtrlSerializer,
							wxTextCtrlSerializer::Serialize,
							NO_CLASS_INIT )

void wxTextCtrlSerializer::Serialize( wxObject* pObj, wxObjectStorage& store )
{
	wxTextCtrl* pCtrl = (wxTextCtrl*)pObj;

	wxWindowSerializer::DoSerialize( pObj, store, 
									(wndCreationFn)wxTextCtrlSerializer::CreateTextCtrlWindowFn );

	wxString text;

	if ( store.IsLoading() == FALSE )
	
		text = pCtrl->GetValue();

	store.XchgWxStr( text );

	if ( store.IsLoading() )
	
		pCtrl->SetValue( text );
}

void wxTextCtrlSerializer::CreateTextCtrlWindowFn( wxTextCtrl* wnd, wxWindow* parent, const wxWindowID id, 
												   const wxPoint& pos, const wxSize& size, long style , 
												   const wxString& name )
{
	wnd->Create( parent, id, "", pos, size, style );

	// FIXME:: quick-hack
	wnd->SetBackgroundColour( wxColour(255,255,255) );
}

/***** Implementation for class wxButtonSerializer *****/

IMPLEMENT_SERIALIZER_CLASS( wxButton, 
							wxButtonSerializer,
							wxButtonSerializer::Serialize,
							NO_CLASS_INIT )

void wxButtonSerializer::Serialize( wxObject* pObj, wxObjectStorage& store )
{
	wxButton* pBtn = (wxButton*)pObj;

	wxWindowSerializer::DoSerialize( pObj, store, 
									(wndCreationFn)wxButtonSerializer::CreateButtonWindowFn );

	wxString label;

	if ( store.IsLoading() == FALSE )
	
		label = pBtn->GetLabel();

	store.XchgWxStr( label );

	if ( store.IsLoading() )
	
		pBtn->SetLabel( label );
}

void wxButtonSerializer::CreateButtonWindowFn( wxButton* btn, wxWindow* parent, const wxWindowID id, 
											   const wxPoint& pos, const wxSize& size, long style , 
											   const wxString& name )
{
	btn->Create( parent, id, "", pos, size, style );
}

/***** Implementation for class wxStaticTextSerializer *****/

IMPLEMENT_SERIALIZER_CLASS( wxStaticText, 
							wxStaticTextSerializer,
							wxStaticTextSerializer::Serialize,
							NO_CLASS_INIT )

void wxStaticTextSerializer::Serialize( wxObject* pObj, wxObjectStorage& store )
{
	wxStaticText* pSTxt = (wxStaticText*)pObj;

	wxWindowSerializer::DoSerialize( pObj, store, 
									(wndCreationFn)wxStaticTextSerializer::CreateSTextWindowFn );

	wxString label;

	if ( store.IsLoading() == FALSE )
	
		label = pSTxt->GetLabel();

	store.XchgWxStr( label );

	if ( store.IsLoading() )
	
		pSTxt->SetLabel( label );
}

void wxStaticTextSerializer::CreateSTextWindowFn( wxStaticText* pSTxt, wxWindow* parent, const wxWindowID id, 
												  const wxPoint& pos, const wxSize& size, long style , 
												  const wxString& name )
{
	pSTxt->Create( parent, id, "", pos, size, style );
}

/***** Implementation for class wxScrollBarSerializer *****/

IMPLEMENT_SERIALIZER_CLASS( wxScrollBar, 
							wxScrollBarSerializer,
							wxScrollBarSerializer::Serialize,
							NO_CLASS_INIT )

void wxScrollBarSerializer::Serialize( wxObject* pObj, wxObjectStorage& store )
{
	wxWindowSerializer::DoSerialize( pObj, store, 
									(wndCreationFn)wxScrollBarSerializer::CreateScollBarWindowFn );
}

void wxScrollBarSerializer::CreateScollBarWindowFn( wxScrollBar* sbar, wxWindow* parent, const wxWindowID id, 
													const wxPoint& pos, const wxSize& size, long style , 
													const wxString& name )
{
	sbar->Create( parent, id, pos, size, style );
}

// FIXME:: serialization of tree control causes bunch of assertions on wxGtk

#if 0

/***** Implementation for class wxTreeCtrlSerializer *****/

IMPLEMENT_SERIALIZER_CLASS( wxTreeCtrl, 
							wxTreeCtrlSerializer,
							wxTreeCtrlSerializer::Serialize,
							NO_CLASS_INIT )

static bool get_child_count( wxTreeItemId itemId, wxTreeCtrl* pTree )
{
	long cookie;

	if ( !pTree->ItemHasChildren( itemId ) ) return 0;

	wxTreeItemId curId = pTree->GetFirstChild( itemId, cookie );

	int    cnt = 0;

	do
	{
		++cnt;

		curId = pTree->GetNextChild( itemId, cookie );

	} while( curId );

	return cnt;
}

void wxTreeCtrlSerializer::SerializeBranch( wxTreeItemId parentId, wxTreeCtrl* pTree, 
											wxObjectStorage& store,	wxTreeItemId nextVisId,
											int depth )
{
	wxString text;
	int      childCnt;
	int      img;
	bool     isExpanded;
	bool     isVisible;

	if ( store.IsLoading() )
	{
		store.XchgWxStr( text );
		store.XchgInt  ( childCnt );
		store.XchgInt  ( img );
		store.XchgBool ( isExpanded );
		store.XchgBool ( isVisible );

		wxTreeItemId subBranchId = 
			( depth == 0 ) 
			? pTree->AddRoot( text, img )
			: pTree->AppendItem( parentId, text, img);

		// check if the item was labeled as first-visible

		if ( isVisible )

			nextVisId = subBranchId; 

		while ( childCnt-- )
		
			SerializeBranch( subBranchId, pTree, store, nextVisId, depth+1 );

		if ( isExpanded ) pTree->Expand( subBranchId );
		             else pTree->Collapse( subBranchId );

	}
	else
	{
		// otherwise storing children of the branch 

		text       = pTree->GetItemText( parentId );
		childCnt   = get_child_count( parentId, pTree );
		img        = pTree->GetItemImage( parentId );
		isExpanded = pTree->IsExpanded( parentId );

		if ( parentId == nextVisId )

			isVisible = TRUE;
		else
			isVisible = FALSE;

		store.XchgWxStr( text );
		store.XchgInt  ( childCnt );
		store.XchgInt  ( img );
		store.XchgBool ( isExpanded );
		store.XchgBool ( isVisible );

		long cookie;

		wxTreeItemId curId = pTree->GetFirstChild( parentId, cookie );

		while ( childCnt-- )
		{
			SerializeBranch( curId, pTree, store, nextVisId, -1 );

			curId = pTree->GetNextChild( parentId, cookie );
		}
	}
}

void wxTreeCtrlSerializer::Serialize( wxObject* pObj, wxObjectStorage& store )
{
	// FOR NOW::image id's and image list are not serialized!
	//          it should be provided as a initial reference (IR)
	//          if it presents. Currently only normal image list
	//          for normal items-states is set up

	wxTreeCtrl* pTree = (wxTreeCtrl*)pObj;

	wxWindowSerializer::DoSerialize( pObj, store, 
									(wndCreationFn)wxTreeCtrlSerializer::CreateTreeCtrlWindowFn );

	wxTreeItemId nextVisId = (long)0;
	int          indent = 0;
	int          childCnt;
	wxImageList* pILst;

	if ( store.IsLoading()  )
	{
		store.XchgInt( indent );

		store.XchgObjPtr( (wxObject**) &(pILst) );

		if ( pILst ) 
			
			pTree->SetImageList( pILst );

		store.XchgInt( childCnt );

		while ( childCnt-- )
		
			SerializeBranch( pTree->GetRootItem() , pTree, store, nextVisId, 0 );

		// FIXME:: somehow this is no longer inmplemented in latest wxWin-2.0
		// pTree->ScrollTo( nextVisId );

		pTree->SetIndent( indent );
	}
	else
	{
		indent    = pTree->GetIndent();

		// FIXME:: somehow this is no longer inmplemented in latest wxWin-2.0
		// nextVisId = pTree->GetFirstVisibleItem();

		nextVisId = pTree->GetRootItem();

		pILst = pTree->GetImageList();

		store.XchgInt( indent );

		store.XchgObjPtr( (wxObject**) &(pILst) );

		// otherwise storing children of the branch 

		childCnt = get_child_count( pTree->GetRootItem(), pTree );

		store.XchgInt( childCnt );

		long cookie;
		wxTreeItemId parent = pTree->GetRootItem();
		wxTreeItemId curId  = pTree->GetFirstChild( parent, cookie );

		while ( childCnt-- )
		{
			SerializeBranch( curId, pTree, store, nextVisId, -1 );

			curId = pTree->GetNextChild( parent, cookie );
		}
	}
}

void wxTreeCtrlSerializer::CreateTreeCtrlWindowFn( wxTreeCtrl* tree, wxWindow* parent, const wxWindowID id, 
											   const wxPoint& pos, const wxSize& size, long style , 
											   const wxString& name )
{
	tree->Create( parent, id, pos, size, style );
}

#endif

/***** Implementation for class wxIOStreamWrapper *****/

IMPLEMENT_DYNAMIC_CLASS( wxIOStreamWrapper, wxDataStreamBase )

void wxIOStreamWrapper::Close()
{
	// close previous stream if any
	if ( mpStm )
	{
		mpStm->flush();

		if ( mOwnsStmObject ) 
			
			delete mpStm;

		mOwnsStmObject = FALSE;

		mpStm = NULL;
	}

	mStreamPos = 0;
}

wxIOStreamWrapper::wxIOStreamWrapper()
	: mpStm( NULL ),
	  mOwnsStmObject( FALSE ),
	  mStreamPos(0)
{
	  mIsForInput = TRUE; // just a defaul
}

bool wxIOStreamWrapper::Create( const char* fileName, bool forInput )
{
	Close();

	// FIXME:: if using default value of the last arg, linking breaks complaining
	//         about duplicated symbols 

#ifdef __WXMSW__
	mpStm = new fstream( fileName, 
						 ( ( forInput == FALSE ) ? ios::out : ios::in ) | ios::binary,
						 0
					   );
#else
	mpStm = new fstream( fileName, 
						 ( ( forInput == FALSE ) ? ios::out : ios::in ) | ios::binary  
						 );
#endif

	//((fstream*)mpStm)->close();

	//delete ((fstream*)mpStm);

	mOwnsStmObject = TRUE;

	if ( !Good() )
	{
		Close();
		return FALSE;
	}

	mIsForInput  = forInput;

	return TRUE;
}

wxIOStreamWrapper::wxIOStreamWrapper( iostream& stm, bool forInput )
	: mOwnsStmObject( FALSE )
{
	mpStm = &stm;

	// FIXME:: what about actual stream postion of attached stream?
	mStreamPos = 0;

	mIsForInput = forInput;
}

void wxIOStreamWrapper::Attach( iostream& stm, bool forInput )
{
	Close();

	mOwnsStmObject = FALSE;

	mpStm = &stm;

	// FIXME:: what about actual stream postion of attached stream?
	mStreamPos = 0;

	mIsForInput = forInput;
}

wxIOStreamWrapper::~wxIOStreamWrapper()
{
	Close();
}

bool wxIOStreamWrapper::StoreChar( char ch )
{
	mpStm->write( &ch, sizeof(char) );

	mStreamPos += sizeof(char);

	return Good();
}

bool wxIOStreamWrapper::StoreInt( int i )
{
	mpStm->write( (char*)&i, sizeof(int) );

	mStreamPos += sizeof(int);

	return Good();
}

bool wxIOStreamWrapper::StoreLong( long l )
{
	mpStm->write( (char*)&l, sizeof(long) );

	mStreamPos += sizeof(long);

	return Good();
}

bool wxIOStreamWrapper::StoreDouble( double d )
{
	mpStm->write( (char*)&d, sizeof(double) );

	mStreamPos += sizeof(double);

	return Good();
}

bool wxIOStreamWrapper::StoreBytes( void* bytes, int count )
{
	mpStm->write( (char*)bytes, count );

	mStreamPos += count;

	return Good();
}

bool wxIOStreamWrapper::LoadChar( char* pCh )
{
	mpStm->read( pCh, sizeof(char) );

	mStreamPos += sizeof(char);

	return Good();
}

bool wxIOStreamWrapper::LoadInt( int* pI )
{
	mpStm->read( (char*)pI, sizeof(int) );

	mStreamPos += sizeof(int);

	return Good();
}

bool wxIOStreamWrapper::LoadLong( long* pL ) 
{
	mpStm->read( (char*)pL, sizeof(long) );

	mStreamPos += sizeof(long);

	return Good();
}

bool wxIOStreamWrapper::LoadDouble( double* pD )
{
	mpStm->read( (char*)pD, sizeof(double) );

	mStreamPos += sizeof(double);

	return Good();
}

bool wxIOStreamWrapper::LoadBytes ( void* pBytes, int count )
{
	mpStm->read( (char*)pBytes, count );

	mStreamPos += count;

	return Good();
}

bool wxIOStreamWrapper::Flush() 
{
	mpStm->flush();

	return Good();
}

long wxIOStreamWrapper::GetStreamPos()
{
	return mStreamPos;
}

bool wxIOStreamWrapper::Good()
{
	// FIXME FIXME:: somehow, when using ios::good/ios::bad, linking breaks complaining
	//               about "ios::bad" already defined in this object file...

	return TRUE;
}
