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

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <malloc.h>
#include <string.h>
#include <memory.h>

#include <stdio.h>   // import sprintf() (for doubles)
#include <stdlib.h>  // import atoi()    (for integers)

#include "scriptbinder.h"

// helper functions

static size_t log2(size_t nr)
{
	size_t tmp = 0;
	while (nr >= 2 ) 
	{
		nr /= 2;
		++tmp;
	}

	return tmp;
}

/***** Implementation for class ScriptStream *****/

ScriptStream::ScriptStream()
	: mpBuf(0),
	  mSize(0),
	  mCapacity(0)
{}

ScriptStream::~ScriptStream()
{
	if ( mpBuf ) delete mpBuf;
}

void ScriptStream::WriteBytes( const void* srcBuf, size_t count )
{
	if ( !count ) return;

	// increase the capacity if necessary
	if ( mSize + count > mCapacity )
	{
			mCapacity = 
				(  0x2 << (log2( mSize + count ) + 1 ) );

			if ( mCapacity < 128 ) mCapacity = 128;

			char* oldBuf = mpBuf;

			mpBuf = new char[mCapacity];

			if ( oldBuf )
			{
				memcpy( mpBuf, oldBuf, mSize );
				delete oldBuf;
			}
	}

	// append new data
	memcpy( &mpBuf[mSize], srcBuf, count );

	mSize += count;
}

ScriptStream& ScriptStream::operator<<( const char* str )
{
	WriteBytes( str, strlen( str ) );

	return *this;
}

ScriptStream& ScriptStream::operator<<( const string& str )
{
	if ( str.length() < 512 )
	{
		char buf[512];
		size_t len = str.length();

		for( size_t i = 0; i != len; ++i )
			buf[i] = str[i];

		WriteBytes( buf, len );
	}
	else
		WriteBytes( str.c_str(), str.length() );

	return *this;
}

ScriptStream& ScriptStream::operator<<( char ch )
{
	WriteBytes( &ch, 1 );

	return *this;
}

void ScriptStream::endl()
{
	char ch = '\n';
	WriteBytes( &ch, 1 );
}

/***** Implementation for class ScriptTemplate *****/

ScriptTemplate::ScriptTemplate( const string& templateText )
{
	string tmp = templateText;

	mTText = (char*)malloc( tmp.length() + 1 );

	strcpy( mTText, tmp.c_str() );
}

ScriptTemplate::~ScriptTemplate()
{
	for( size_t i = 0; i != mVars.size(); ++i )

		delete mVars[i];

	free( mTText );
}

bool ScriptTemplate::HasVar( const char* name )
{
	for( size_t i = 0; i != mVars.size(); ++i ) 

		if ( strcmp( mVars[i]->mName, name ) == 0 )

			return 1;

	return 0;
}

void ScriptTemplate::AddStringVar ( const char* name, int ofs )
{
	mVars.push_back( new TVarInfo( name, ofs, TVAR_STRING ) );
}

void ScriptTemplate::AddIntegerVar( const char* name, int ofs )
{
	mVars.push_back( new TVarInfo( name, ofs, TVAR_INTEGER ) );
}

void ScriptTemplate::AddDoubleVar ( const char* name, int ofs )
{
	mVars.push_back( new TVarInfo( name, ofs, TVAR_DOUBLE ) );
}

void ScriptTemplate::AddObjectRefArray( const char*     name,
										int			    ofsRefToFirstObj,
										int			    ofsObjSizeInt,
										int			    ofsObjRefTempl
									  )
{
	TArrayInfo* pInfo = new TArrayInfo( name );

	mVars.push_back( pInfo );

	pInfo->mRefOfs         = ofsRefToFirstObj;
	pInfo->mSizeIntOfs     = ofsObjSizeInt;
	pInfo->mObjRefTemplOfs = ofsObjRefTempl;
}

inline void ScriptTemplate::PrintVar( TVarInfo*   pInfo, 
								    void*       dataObj, 
									ScriptStream& stm )
{
	char buf[128];

	switch ( pInfo->mType )
	{
		case TVAR_INTEGER :
			{
				sprintf(buf, "%d",*( (int*) ((char*)dataObj + pInfo->mOfs) ) );
				
				stm.WriteBytes( buf, strlen(buf ) );
				break;
			}

		case TVAR_STRING :
			{
				string& str = *( (string*) ((char*)dataObj+pInfo->mOfs) );

				const char* cs = str.c_str();
#ifdef DEBUG_WEIRED_OFFSETS
				cout << "DBG:: cs address is " << (int)cs << endl;
				cout << "DBG:: str address is " << (int)(&str) << endl;
				cout << "DBG:: dataObj points to " << (int)dataObj << endl;
				cout << "DBG:: pInfo->mOfs value is " << (int)pInfo->mOfs << endl;
				cout << "DBG:: d+pInfo->mOfs is " << (int)((char*)dataObj + pInfo->mOfs) << endl;
				cout << "DBG:: pInfo->mName is " << pInfo->mName << endl;
				cout << "DBG:: pInfo->mType is " << pInfo->mType << endl;
				cout << "DBG:: end of dump. " << endl;

				cout << "DBG:: cs value is " << endl << cs << endl;
#endif
				stm.WriteBytes( cs, strlen(cs) );
				break;
			}

		case TVAR_DOUBLE :
			{
				sprintf( buf, "%f", 
					     *( (double*)( (char*)dataObj+pInfo->mOfs)  ) );

				stm.WriteBytes( buf, strlen(buf ) );
				break;
			}

		case TVAR_REF_ARRAY :
			{
				TArrayInfo& info = *((TArrayInfo*)pInfo);

				int sz = *((int*) ( (char*)dataObj+info.mSizeIntOfs ));
				if ( !sz ) 
				{
					// DBG::
					int u = 0;
					++u;
					break;
				}

				int*   array = *((int**)( (char*)dataObj+info.mRefOfs ));
	
				ScriptTemplate* pRefTempl;

				for( int i = 0; i != sz; ++i )
				{
					pRefTempl = 
						*((ScriptTemplate**)((char*)(array[i])+info.mObjRefTemplOfs));

					pRefTempl->PrintScript( (void*)array[i], stm );
				}

				break;
			}

		default : break;
	}
}

void ScriptTemplate::PrintScript( void* dataObj, ScriptStream& stm )
{
	char* cur   = mTText;

	// template parsing loop
	do
	{
		char* start = cur;

		while( *cur != '\0' && *cur != '$' ) ++cur;

		// flush text collected between variables
		stm.WriteBytes( start, cur - start );

		if ( *cur == '\0' ) break;

		cur += 2; // skip to the name of the var

		start = cur;

		while( *cur != ')' ) ++cur;

		// put terminating zero temorarely

		*cur = '\0';

		// look up variable

		size_t sz = mVars.size();
		bool found = 0;

		for( size_t i = 0; i != sz; ++i ) 
		{
			if ( strcmp( mVars[i]->mName, start ) == 0 )
			{
				PrintVar( mVars[i], dataObj, stm );

				*cur = ')';	// remove terminating zero
				++cur;
				found = 1;
				break;
			}
		}

		// variable referred by template script is not
		// registered to this tempalte object
		ASSERT( found ); 

	} while(1);
}

/***** implementation for class ScriptSection *****/

int ScriptSection::mIdCounter = 0;

ScriptSection::ScriptSection( const string&   name,
						      const string&   body,
							  ScriptTemplate* pSectionTemplate,
							  ScriptTemplate* pReferenceTemplate,
							  bool            autoHide,
							  bool            sorted
							)
	: mpParent ( NULL ),

	  mName    ( name ),
	  mBody    ( body ),

	  mAutoHide( autoHide ),
	  mSortOn  ( sorted ),

	  mpSectTempl( pSectionTemplate ),
	  mpRefTempl ( pReferenceTemplate ),

	  mRefCount( 0 ),
	  mArrSize( 0 )
{
	// generate GUID

	char buf[32];
	sprintf( buf, "%d", ++mIdCounter );
	mId = buf;
}

ScriptSection::~ScriptSection()
{
	SectListT lst = mSubsections;

	while( mSubsections.size() )

		mSubsections[0]->RemoveRef();

	for( size_t i = 0; i != mReferences.size(); ++i )

		mReferences[i]->RemoveRef();
}

void ScriptSection::AddRef()
{
	++mRefCount;
}

void ScriptSection::RemoveRef()
{
	if ( !mRefCount || !(--mRefCount) )
	{
		if (mpParent)
		{
			// remove ourselves from parent's list

			SectListT& lst = mpParent->mSubsections;
			for( size_t i = 0; i != lst.size(); ++i )

				if ( lst[i] == this )
				{
					lst.erase( &lst[i] );
					break;
				}
		}

		delete this;
	}
}

ScriptSection* ScriptSection::GetSubsection( const char* name )
{
	// FOR NOW:: fixed section name length
	char buf[128];

	size_t cur = 0;

	while( name[cur] && name[cur] != '/' )
	{
		buf[cur] = name[cur];
		++cur;
	}

	ASSERT( cur < sizeof(buf) );

	buf[cur] = '\0';

	size_t sz = mSubsections.size();

	for( size_t i = 0; i != sz; ++i )
	{
		// DBG::
		//ScriptSection& sect = *mSubsections[i];

		if ( mSubsections[i]->mName == buf )
		{
			if ( name[cur] == '/' )

				// search recursivelly
				return mSubsections[i]->GetSubsection( &name[cur+1] );
			else
				return mSubsections[i];
		}
	}

	return 0;
}

void ScriptSection::AddSection( ScriptSection* pSection,
							    bool addToReferencesToo
							  )
{
	mSubsections.push_back( pSection );

	pSection->AddRef();

	// can add section to multiple containers
	ASSERT( pSection->mpParent == 0 );

	pSection->mpParent = this;

	if ( addToReferencesToo )

		AddReference( pSection );
}

void ScriptSection::AddReference( ScriptSection* pReferredSection )
{
	mReferences.push_back( pReferredSection );

	pReferredSection->AddRef();

	// set up mandatory fields used by ScriptTemplate
	mArrSize  = mReferences.size();
	if ( mArrSize )
		mRefFirst = (void*)&mReferences[0];
}

SectListT& ScriptSection::GetSubsections()
{
	return mSubsections;
}

// static method:
void ScriptSection::RegisterTemplate( ScriptTemplate& sectionTempalte )
{
	int nameOfs, bodyOfs, idOfs,
		arrRefOfs, arrSizeOfs, refTemplOfs;

	// obtaining offsets of member vars
	
	GET_VAR_OFS( ScriptSection, mName,     &nameOfs    )
	GET_VAR_OFS( ScriptSection, mBody,     &bodyOfs    )
	GET_VAR_OFS( ScriptSection, mId,       &idOfs      )
	GET_VAR_OFS( ScriptSection, mRefFirst, &arrRefOfs  )
	GET_VAR_OFS( ScriptSection, mArrSize,  &arrSizeOfs )

	GET_VAR_OFS( ScriptSection, mpRefTempl, &refTemplOfs )

	// registering member variables with given script template

	sectionTempalte.AddStringVar( "NAME", nameOfs );
	sectionTempalte.AddStringVar( "BODY", bodyOfs );
	sectionTempalte.AddStringVar( "ID",   idOfs   );

	sectionTempalte.AddObjectRefArray( "REFLIST",
									   arrRefOfs, arrSizeOfs, refTemplOfs );
}

void ScriptSection::Print( ScriptStream& stm )
{
	// TBD:: sorting

	// print out this content first
	if ( mpSectTempl )

		mpSectTempl->PrintScript( this, stm );

	// attach contents subsections at the end of this content

	for( size_t i = 0; i != mSubsections.size(); ++i )

		mSubsections[i]->Print( stm );
}

void ScriptSection::DoRemoveEmptySections(int& nRemoved, SectListT& removedLst)
{
	for( size_t i = 0; i != mSubsections.size(); ++i )
	{
		ScriptSection& sect = *mSubsections[i];

		sect.DoRemoveEmptySections( nRemoved, removedLst );

		if (sect.mAutoHide )
			
			if ( sect.mReferences.size() == 0 )
			{
				bool found = 0;
				for( size_t k = 0; k != removedLst.size(); ++k )

					if ( removedLst[k] == &sect )
					{
						found = 1;
						break;
					}
				
				if ( !found )
				{
					removedLst.push_back( &sect );
					++nRemoved;

					delete &sect;
					--i;
				}
			}
	}
}

void ScriptSection::DoRemoveDeadLinks( SectListT& removedLst)
{
	size_t dsz = removedLst.size();

	for( size_t i = 0; i != mSubsections.size(); ++i )
	{
		mSubsections[i]->DoRemoveDeadLinks( removedLst );
	}

	for( size_t n = 0; n != mReferences.size(); ++n )
	{
		for( size_t k = 0; k != dsz; ++k )

			if ( removedLst[k] == mReferences[n] )
			{
				mReferences.erase( &mReferences[n] );
				--n;

				// set up mandatory fields used by ScriptTemplate
				mArrSize  = mReferences.size();
				if ( mArrSize )
					mRefFirst = (void*)&mReferences[0];

				break;
			}
	}
}


void ScriptSection::RemoveEmptySections()
{
	// FIXME:: this is very_very_very slow alg.! +"doesn't work"

	int nRemoved = 0;

	do
	{
		SectListT removedLst;
		nRemoved = 0;

		DoRemoveEmptySections( nRemoved, removedLst );

		DoRemoveDeadLinks( removedLst );
	}
	while( nRemoved );
}

/***** Iimplementation for class DocGeneratorBase *****/

bool DocGeneratorBase::SaveDocument( const char*    fname, 
								     const char*    fopenOptions,
								     ScriptSection* pFromSection
								   )
{
	FILE* fp = fopen( fname, fopenOptions );

	if ( (int)fp == -1 ) return 0;

	ScriptStream stm;

	// check if derived class agrees about saving it
	if ( !OnSaveDocument( stm ) ) return 0;

	if ( pFromSection )

		pFromSection->Print( stm );
	else
	{
		ScriptSection* pTopSect = GetTopSection();
		ASSERT( pTopSect );
		pTopSect->Print( stm );
	}

	size_t nWrite = fwrite( stm.GetBuf(), 1, stm.GetBufSize(), fp );

	if ( nWrite != stm.GetBufSize() ) return 0;

	fclose( fp );

	return 1;

	// that^s it
}
