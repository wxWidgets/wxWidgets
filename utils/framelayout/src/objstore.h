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

#ifndef __OBJSTORE_G__
#define __OBJSTORE_G__

#include "wx/object.h"
#include "wx/string.h"
#include "wx/list.h"
#include "wx/hash.h"

#include "wx/window.h"
#include "wx/button.h"
#include "wx/textctrl.h"
#include "wx/treectrl.h"
#include "wx/dynarray.h"

// abstract classes declared

class wxDataStreamBase;
class wxSerializerBase;
class wxObjectStorage;

// classes which implement the above interfaces

class wxPointSerializer;
class wxSizeSerializer;
class wxRectSerializer;
class wxPenSerializer;
class wxBrushSerializer;

class wxObjectListSerializer;

class wxEvtHandlerSerializer;
class wxWindowSerializer;
class wxButtonSerializer;
class wxScrollBarSerializer;
class wxChoiceSerializer;
class wxTextCtrlSerializer;
class wxTreeCtrlSerializer;


class wxIOStreamWrapper;

// prototypes for serialzatoin/initialization functions

typedef void (*wxObjectSerializationFn) (wxObject*, wxObjectStorage& );
typedef void (*wxObjectInitializationFn)(wxObject*);

#define NO_CLASS_VER  NULL
#define NO_CLASS_INIT NULL

/*
 * class conceptually simiar to wxClassInfo, execpt that it's static
 * instances hold information about class-serializers rather then
 * about the classes themselves.
 */

class wxSerializerInfo
{
public:
	char* className;

	wxClassInfo* classInfo;	// link to corresponding class-info object,
	                        // established upon invocation of InitializeSerializers()

	wxObjectSerializationFn  serFn;
	wxObjectInitializationFn initFn;

	char* classVersion;

	static bool alreadyInitialized;
	static wxSerializerInfo* first;
	static wxHashTable serInfoHash; // classInfo <=> serializerInfo

	wxSerializerInfo* next;
	wxSerializerInfo* nextByVersion;

	wxSerializerInfo( char* theClassName, 
					  wxObjectSerializationFn  serializationFun,
					  wxObjectInitializationFn initializationFun,
					  char*  classVersionName
					);

   // looks up for serializers of the base classes (base1 and base2) 
   // of the given object invokes them if present 

   void SerializeInherited ( wxObject* pObj, wxObjectStorage& store );
   void InitializeInherited( wxObject* pObj );

   bool HasVersion() { return classVersion != NO_CLASS_VER; }

   bool HasInitializer()  { return initFn != NO_CLASS_INIT; }

   // static methods

   static void InitializeSerializers(void);

   static wxSerializerInfo* FindSerializer( char* className );
};

/*
 * formal base class for all serializers, implemented as
 * classes with static serialization/initialization methods
 */

class wxSerializerBase {};

// macros for declaring and implementing serializers both as
// classes and as a pair of (serialize/init) functions

#define DECLARE_SERIALIZER_CLASS(serializerName) \
 public:\
  static wxSerializerInfo info;;

#define IMPLEMENT_SERIALIZER_CLASS( name, serializerName, serFn, initFn) \
	wxSerializerInfo 													 \
		serializerName::info( #name, serFn, initFn, NO_CLASS_VER );

#define IMPLEMENT_SERIALIZER_FUNCTIONS( name, serFn, initFn) \
	wxSerializerInfo 										 \
		static __gSerFnInfoFor##name( #name, serFn, initFn, NO_CLASS_VER );

// for serializers, which are dedicated for specific versions of persistant classes
// (further referred as "versioned" serializers)

#define IMPLEMENT_SERIALIZER_CLASS_FOR_VERSION( name, serializerName, serFn, initFn, versionName) \
	wxSerializerInfo 													             \
		serializerName::info( #name, serFn, initFn, #versionName );

#define IMPLEMENT_SERIALIZER_FUNCTIONS_FOR_VERSION( name, serializerName, serFn, initFn, versionName) \
	wxSerializerInfo 													             \
		static __gSerFnInfoFor##name( #name, serFn, initFn, #versionName );

/*
 * defines abstract inferface for data-stream objects,
 * can be implemented as a wrapper class for already
 * existing stream classes
 */

class wxDataStreamBase : public wxObject
{
	DECLARE_ABSTRACT_CLASS( wxDataStreamBase )
protected:
	bool mIsForInput;

public:
	virtual bool StoreChar  ( char   ch ) = 0;
	virtual bool StoreInt   ( int    i  ) = 0;
	virtual bool StoreLong  ( long   l  ) = 0;
	virtual bool StoreDouble( double d  ) = 0;
	virtual bool StoreBytes ( void* bytes, int count ) = 0;

	virtual bool LoadChar  ( char   *pCh ) = 0;
	virtual bool LoadInt   ( int    *pI  ) = 0;
	virtual bool LoadLong  ( long   *pL  ) = 0;
	virtual bool LoadDouble( double *pD  ) = 0;
	virtual bool LoadBytes ( void   *pBytes, int count ) = 0;

	virtual bool Flush() = 0;

	virtual long GetStreamPos() = 0;

	bool IsForInput() { return mIsForInput; }
};

/*
 * class provides stream-based persistance service for
 * classes derivated from wxObject, which are declared
 * as dynamic classes. Relies on the presence of appropriate
 * serializers for objects, which are being stored/loaded.
 */

class wxObjectStorage : public wxObject
{
	DECLARE_DYNAMIC_CLASS( wxObjectStorage )
protected:
	wxDataStreamBase* mpStm;
	bool              mIsLoading;

	wxHashTable       mRefHash;

	wxList            mInitialRefs;
	wxHashTable       mInitialRefsHash;
	long              mInitialRefsCnt;

	wxList            mNewObjs;
	wxList            mSerializersForNewObjs;

	bool              mFinalizePending;

protected:
	wxSerializerBase* FindSerializer( wxObject* pForObj );

	void ClearHashesAndLists();

	virtual bool VersionsMatch( char* v1, char* v2 );

	virtual wxSerializerInfo* FindSrzInfoForClass( wxClassInfo* pInfo );

	void DoExchangeObject( wxObject* pInstance, wxSerializerInfo& srzInfo );

	bool ExchangeObjectInfo( wxClassInfo** ppCInfo, wxSerializerInfo** ppSrz );

	wxSerializerInfo* GetLatestSrzForObj( wxObject* pWxObj );

public:
	// can be changed (with countion!)

	static char       mVerSepartorCh;        // default: '#'
	static char       mMinorMajorSepartorCh; // default: '-'

public:

	wxObjectStorage();

	wxObjectStorage( wxDataStreamBase& stm );

	virtual ~wxObjectStorage();

	// adds initial reference, objects referred by such reference
	// are not serialized when storing. When loading, pointers which
	// refere to these "inital objects" are set up to refere to
	// objects provided in by AddInitailRef() method.
	//
	// NOTE:: initial references should be added always in the
	//        same order, since the seq-# of the reference is used
	//        as an alias to the real object while storing/loading

	void AddInitialRef( wxObject* pObjRef );

	void ClearInitalRefs();

	// init/reinit of object-storage

	void SetDataStream( wxDataStreamBase& stm );

	// performs linkng of in-memory references after loading, or
	// links in-stream references after storing has proceeded

	void Finalize();	

	// storage methods for basic types

	void XchgChar    ( char&   chObj     );
	void XchgInt     ( int&    intObj    );
	void XchgLong    ( long&   longObj   );
	void XchgBool    ( bool&   boolObj   );
	void XchgDouble  ( double& doubleObj );
	void XchgCStr    ( char*   pCStrObj  );
	void XchgUInt    ( unsigned int& uI  );
	void XchgSizeType( size_t& szObj     );

	void XchgObjList  ( wxList&      objList );
	void XchgObjArray ( wxBaseArray& objArr );
	void XchgLongArray( wxBaseArray& longArr );

	// storage methods for objects and pointers to objects

	void XchgObj   ( wxObject*  pWxObj  );
	void XchgObjPtr( wxObject** ppWxObj );

	bool IsLoading() { return mIsLoading; }

	// storage methods for common wxWindows classes,
	// which may or may not be dymaic, therefor use the 
	// below methods instead of XchgObj(..)

	void XchgWxStr  ( wxString& str   );
	void XchgWxSize ( wxSize&   size  );       
	void XchgWxPoint( wxPoint&  point );
	void XchgWxRect ( wxRect&   rect  );
};

/*
 * The below classes provide "curde" serialization for most
 * common wxWindows objects, i.e. they discard the information
 * which may be contained in the subclassed versions of these classes
 * However, more "fine-grainded" serializers could be written
 * to match these subclasses exactly.
 */

class wxColourSerializer : public wxSerializerBase
{
	DECLARE_SERIALIZER_CLASS( wxColourSerializer );

	static void Serialize( wxObject* pObj, wxObjectStorage& store );
};

// NOTE:: currently "stipple" and "dashes" properties of the pen
//        are not serialized

class wxPenSerializer : public wxSerializerBase
{
	DECLARE_SERIALIZER_CLASS( wxPenSerializer );
public:
	static void Serialize( wxObject* pObj, wxObjectStorage& store );
};

// NOTE:: currently "stipple" property of the brush is not serialized

class wxBrushSerializer : public wxSerializerBase
{
	DECLARE_SERIALIZER_CLASS( wxBrushSerializer );

	static void Serialize( wxObject* pObj, wxObjectStorage& store );
};

// serializer for wxList, assuming that the list
// holds derivatives of wxObject. 

class wxObjectListSerializer : public wxSerializerBase
{
	DECLARE_SERIALIZER_CLASS( wxObjectListSerializer );

	static void Serialize( wxObject* pObj, wxObjectStorage& store );
};

// generic serializer for classes derived from wxEvtHandler handler,
// assuming that they do not add any new properties to wxEvtHandler
// or these properties are transient

class wxEvtHandlerSerializer : public wxSerializerBase
{
	DECLARE_SERIALIZER_CLASS( wxEvtHandlerSerializer );
public:
	static void Serialize( wxObject* pObj, wxObjectStorage& store );

	static void Initialize( wxObject* pObj );
};

// serializer for generic wxWindow. Serializes position, size, id,
// reference to parent, list of children, style flags and name string.
// Could be used for serializing wxWindow and generic wxPanel objects.
// Separate serializers have to be written for control classes.

class wxWindowSerializer : public wxEvtHandlerSerializer
{
	DECLARE_SERIALIZER_CLASS( wxWindowSerializer );
public:

	static void Serialize( wxObject* pObj, wxObjectStorage& store );

	// helpers, to ease the creation of serializers for derivatives of wxWindow

	typedef (*wndCreationFn)(wxWindow*, wxWindow*, const wxWindowID, 
							 const wxPoint&, const wxSize&, long, const wxString&  );


	static void DoSerialize( wxObject* pObj, wxObjectStorage& store,
		                     wndCreationFn creationFn, bool refreshNow = TRUE
						   );


	static void CreateWindowFn( wxWindow* wnd, wxWindow* parent, const wxWindowID id, 
							    const wxPoint& pos, const wxSize& size, long style , 
							    const wxString& name );

};

class wxTextCtrlSerializer : public wxWindowSerializer
{
	DECLARE_SERIALIZER_CLASS( wxTextCtrlSerializer );
public:
	static void Serialize( wxObject* pObj, wxObjectStorage& store );

	static void CreateTextCtrlWindowFn( wxTextCtrl* wnd, wxWindow* parent, const wxWindowID id, 
									    const wxPoint& pos, const wxSize& size, long style , 
									    const wxString& name );
};

class wxButtonSerializer : public wxWindowSerializer
{
	DECLARE_SERIALIZER_CLASS( wxButtonSerializer );
public:
	static void Serialize( wxObject* pObj, wxObjectStorage& store );

	static void CreateButtonWindowFn( wxButton* btn, wxWindow* parent, const wxWindowID id, 
									  const wxPoint& pos, const wxSize& size, long style , 
									  const wxString& name );
};

class wxStaticTextSerializer : public wxWindowSerializer
{
	DECLARE_SERIALIZER_CLASS( wxStaticTextSerializer );
public:
	static void Serialize( wxObject* pObj, wxObjectStorage& store );

	static void CreateSTextWindowFn( wxStaticText* pSTxt, wxWindow* parent, const wxWindowID id, 
									 const wxPoint& pos, const wxSize& size, long style , 
									 const wxString& name );
};


class wxScrollBarSerializer : public wxWindowSerializer
{
	DECLARE_SERIALIZER_CLASS( wxScrollBarSerializer );
public:
	static void Serialize( wxObject* pObj, wxObjectStorage& store );

	static void CreateScollBarWindowFn( wxScrollBar* sbar, wxWindow* parent, const wxWindowID id, 
									    const wxPoint& pos, const wxSize& size, long style , 
									    const wxString& name );
};

class wxTreeCtrlSerializer : public wxWindowSerializer
{
	DECLARE_SERIALIZER_CLASS( wxTreeCtrlSerializer );

protected:
	static void SerializeBranch( wxTreeItemId parentId, wxTreeCtrl* pTree, 
								 wxObjectStorage& store, wxTreeItemId nextVisId,
								 int depth );

public:
	static void Serialize( wxObject* pObj, wxObjectStorage& store );

	static void CreateTreeCtrlWindowFn( wxTreeCtrl* tree, wxWindow* parent, const wxWindowID id, 
									    const wxPoint& pos, const wxSize& size, long style , 
									    const wxString& name );
};

// default implementations of interfaces, used by wxObjectStorage class
//
// FOR NOW:: methods do not yet perform byte-swaps for outputting/reading words in
//           machine-independent format. Better solution would be to write wrapper
//           around the "promissed" protable-data-stream class of wxWindows

class wxIOStreamWrapper : public wxDataStreamBase
{ 
	DECLARE_DYNAMIC_CLASS( wxIOStreamWrapper )
protected:
	iostream* mpStm;
	bool      mOwnsStmObject;
	long      mStreamPos;     // precalcualted stream postion,
	                          // assuming that the actual stream object is not
							  // capable of telling postion of current get/put pointer 
							  // (e.g. socket-stream)
	void Close();

public:

	// default constructor
	wxIOStreamWrapper();

	// attach this wrapper to already exiting iostream object

	wxIOStreamWrapper( iostream& stm, bool forInput = TRUE );

	// creates "fstream" object with the given file name in binary mode,
	// returns FALSE, if stream creation failed
	//
	// The created fstream object is "owned" by this wrapper, 
	//  thus it is destored during destruction of this object

	bool Create( const char* fileName, bool forInput = TRUE );

	inline bool CreateForInput( const char* fileName )

		{ return Create( fileName, TRUE ); }

	inline bool CreateForOutput( const char* fileName )

		{ return Create( fileName, FALSE ); }

	// the same as in the second constructor, previousely used
	// stream object is flushed and destroyed (if owned).
	// The attached stream is not "owned" by this wrapper object

	void Attach( iostream& stm, bool forInput = TRUE );

	virtual ~wxIOStreamWrapper();

	virtual bool StoreChar  ( char   ch );
	virtual bool StoreInt   ( int    i  );
	virtual bool StoreLong  ( long   l  );
	virtual bool StoreDouble( double d  );
	virtual bool StoreBytes ( void* bytes, int count );

	virtual bool LoadChar  ( char*   pCh );
	virtual bool LoadInt   ( int*    pI  );
	virtual bool LoadLong  ( long*   pL  );
	virtual bool LoadDouble( double* pD  );
	virtual bool LoadBytes ( void* pBytes, int count );

	virtual bool Flush();

	virtual long GetStreamPos();

	bool Good();
};

#endif
