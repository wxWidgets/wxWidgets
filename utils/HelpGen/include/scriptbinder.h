/////////////////////////////////////////////////////////////////////////////
// Name:        No names yet.
// Purpose:     Contrib. demo
// Author:      Aleksandras Gluchovas
// Modified by:
// Created:     22/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Aleskandars Gluchovas
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __SCRIPTBINDER_G__
#define __SCRIPTBINDER_G__

#if defined( wxUSE_TEMPLATE_STL )

	#include <vector>

	#ifdef WIN32
		#include <bstring.h>
	#else
		#include <strclass.h>
		#include <string.h>
	#endif

#else

	#include "wxstlvec.h"
	#include "wx/string.h"
	
	// FIXME:: dirty!
	typedef wxString string;

#endif

#ifndef ASSERT
// assert yourself
#define ASSERT(x) if (!(x) ) throw;
#endif

#include "markup.h"

// just another portable stream class...

class ScriptStream
{
protected:
	char*  mpBuf;
	size_t mSize;
	size_t mCapacity;
public:
	ScriptStream();
	~ScriptStream();

	void WriteBytes( const void* srcBuf, size_t count );

	ScriptStream& operator<<( const char* str );
	ScriptStream& operator<<( const string& str );
	ScriptStream& operator<<( char ch );

	void endl();

	inline char*  GetBuf() { return mpBuf; }
	inline size_t GetBufSize() { return mSize; }

	// clears current contents of the stream
	void Reset() { mSize = 0; }
};


class ScriptTemplate;

// used internally by ScriptTemplate

enum TEMPLATE_VARIABLE_TYPES
{
	TVAR_INTEGER,
	TVAR_STRING,
	TVAR_DOUBLE,
	TVAR_REF_ARRAY
};

// helper structures used only by ScriptTemplate

struct TVarInfo
{
public:
	const char* 		mName;
	int			mType;
	int			mOfs;

	TVarInfo( const char* name, int ofs, int varType )
		: mName(name),
		  mType( varType ),
		  mOfs( ofs )
	{}
};

struct TArrayInfo : public TVarInfo
{
public:
	int mRefOfs;
	int mSizeIntOfs;
	int mObjRefTemplOfs;

	TArrayInfo( const char* name )
		: TVarInfo( name, 0, TVAR_REF_ARRAY )
	{}
};

// stores offset of the given member (of the given class)
// to (*pOfs), though the use of template classes would have 
// solved this problem in much clearer fashion

// FOR NOW:: obtaining physical offset of class member
//           does not appeare to be protable across compilers?
// FIXME::   +/- 1 problem

#ifdef __UNIX__
	#define WEIRD_OFFSET 1
#else
	#define WEIRD_OFFSET 0

#endif

#define GET_VAR_OFS( className, varName, pOfs )          \
	{													 \
		int* className::* varPtr;						 \
		varPtr = (int* className::*)&className::varName; \
														 \
		(*pOfs) = int(*(int*)&varPtr)-WEIRD_OFFSET;					 \
	}

class ScriptSection;

#if defined( wxUSE_TEMPLATE_STL )

	typedef vector<TVarInfo*> TVarListT;

	// container class for sections
	typedef vector<ScriptSection*> SectListT;

#else

	typedef TVarInfo*      TVarInfoPtrT;
	typedef ScriptSection* ScriptSectionPtrT;

	typedef WXSTL_VECTOR_SHALLOW_COPY(TVarInfoPtrT) TVarListT;

	// container class for sections
	typedef WXSTL_VECTOR_SHALLOW_COPY(ScriptSectionPtrT) SectListT;

#endif

// class performs preprocessing of arbitrary scripts,
// replaces identifiers enclosed in $(..) tag, whith
// values of the corresponding class member variables

class ScriptTemplate
{
protected:
	// do not use string object here - parsing of
	// C string can be much faster (in debug v.)
	char*     mTText; 
				      

	TVarListT mVars;

	inline void PrintVar( TVarInfo*  pInfo, 
						  void*       dataObj, 
						  ScriptStream& stm );
	
public:
	ScriptTemplate( const string& templateText );
	virtual ~ScriptTemplate();
	
	bool HasVar( const char* name );

	// Member variables registration methods.

	// NOTE:: GET_VAR_OFS() macro should be used
	// to get offset of the class member (see #define above)
	void AddStringVar ( const char* name, int ofs );
	void AddIntegerVar( const char* name, int ofs );
	void AddDoubleVar ( const char* name, int ofs );

	void AddObjectRefArray( const char*     name,
							int			    ofsRefToFirstObj,
		                    int			    ofsObjSizeInt,
							int			    ofsObjRefTempl
						  );

	// reads the script, replaces $(..) tags with values
	// of registered members of dataObj object, and outputs
	// the result to given text stream

	void PrintScript( void* dataObj, ScriptStream& stm );
};

class ScriptSection;

// class manages section and aggregated sections of
// inter-linked documents

class ScriptSection
{
protected:

	// NOTE:: "$(NAME)", $(ID), "$(BODY)" and "$(REFLIST)" aree 
	//        reseved template variables of ScriptSection 

	// the below there members are registered to ScriptTemplate,
	// GUID within the section tree (numeric)

	ScriptSection*  mpParent;
	string          mId;   // $(ID)      
	string          mName; // $(NAME)      
	string          mBody; // $(BODY)

	// NULL, if this section is not aggregated anywhere

	SectListT       mSubsections; // aggregated sectons
	SectListT       mReferences;  // registered as $(REFLIST)

	bool            mAutoHide;  // see autoHide arg, in constructor
	bool            mSortOn;    // TRUE, if sort subsectons by naem

	// tempalte for this section
	ScriptTemplate* mpSectTempl;

	// template used for links (or references) to this section
	ScriptTemplate* mpRefTempl;
	
	// do not call destructor of this object,
	// call RemoveRef() instead
	int             mRefCount;

	static int      mIdCounter;  // generator of GUIDs

	// fields registered and used by ScriptTemplate object
	void*           mRefFirst;
	int             mArrSize;

protected:
	virtual void AddRef();
	virtual void RemoveRef();
	void DoRemoveEmptySections(int& nRemoved, SectListT& removedLst);
	void DoRemoveDeadLinks( SectListT& removedLst);

public:

	// NOTE:: pass NULL to certain template, if your sure
	//        this kind of template will never be used,
	//        e.g. if section is contained but never referrenced,
	//        then pReferenceTemplate can be NULL

	// if autoHide option is TRUE, the section will be automatically
	// collapsed (not shown) if it doesn't contain any references
	// to other sections (e.g. could be usefull for autoamically 
	// hiding empty index-sections). 

	ScriptSection( const string&   name = "",
				   const string&   body = "",
				   ScriptTemplate* pSectionTemplate   = NULL,
				   ScriptTemplate* pReferenceTemplate = NULL,          	           
				   bool            autoHide           = FALSE,
				   bool            sorted             = FALSE
				 );

	// calls RemoveRef() to all aggreagated sections first,
	// then to all referenced section - this way all
	// sections (even not aggregated ones) become "garbage-collected"

	// NOTE:: do not call destructor directlly, call RemoveRef()
	//        instead
	virtual ~ScriptSection();


	// if addToReferencesToo is TRUE, section is aggregated and
	// also added to reference list of this section

	void AddSection( ScriptSection* pSection, bool addToReferencesToo = FALSE );

	// add cross-reference to this given section
	void AddReference( ScriptSection* pReferredSection );

	// subsection may be given of variable depth level,
	// e.g. "publications/reviews/software"

	ScriptSection* GetSubsection( const char* name );

	// returns list aggregated sections
	SectListT& GetSubsections();

	// binds reserved template names ( $(..) ) to member 
	// vairalbes in the ScriptSection class, should be called 
	// to initialize each user-code provided script template

	static void RegisterTemplate( ScriptTemplate& sectionTempalte );

	// prints out section tree to the stream, starting from
	// this section as a root node
	virtual void Print( ScriptStream& stm );	

	// searches empty sections which has autoHide == TRUE,
	// and colapses them (this method should be called )
	// on the root-section of the sections tree

	// NOTE:: does not work properly, yet!
	void RemoveEmptySections();
};

// base class for documnetation generators
// (allows user code set up target script type,
//  independently of documentation type)

class DocGeneratorBase
{
protected:
	MarkupTagsT    mTags;

	// override this method to do some post processing
	// after generation of document, or even write some
	// data into output stream, before the section tree
	// is flushed into it.

	// return FALSE, if something has gone wrong and
	// document cannot be saved now

	virtual bool OnSaveDocument( ScriptStream& stm ) 
		{ return 1; }

	// override this method to provide reference to
	// the top section of the document (used as default
	// starting section when saving a document)

	virtual ScriptSection* GetTopSection() 
		{ return 0; }

public:

	DocGeneratorBase() 
		: mTags(0) // no defaul script
	{}

	// dectrouctors of polymorphic classes SHOULD be virtual
	virtual ~DocGeneratorBase() {}

	// returns tags, being used for specific target script
	MarkupTagsT GetScriptMarkupTags() { return mTags; }

	// sets tag array for specific script

	// NOTE:: Why virtual? since approach with MarkupTagsT is 
	//        "flowless" only in theory. Overriding this method
	//        allows document generators to check the type of the
	//        target script, and perhaps make some modifications
	//        to generator's tamplates, to match the specific script

	virtual void SetScriptMarkupTags( MarkupTagsT tags )
		{ mTags = tags; }

	// seves document to file starting from the root-node of
	// the document (provided by GetTopSection() method),
	// or from "pFromSection" if it's not NULL.

	// fopenOptions arg. is string passed to fopen() method,
	// returns TRUE, if saving was successfull

	virtual bool SaveDocument( const char*    fname, 
							   const char*    fopenOptions = "w",
							   ScriptSection* pFromSection = NULL
							   );
		     
};

#endif
