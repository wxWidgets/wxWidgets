/////////////////////////////////////////////////////////////////////////////
// Name:        No names yet.
// Purpose:     Contrib. demo
// Author:      Aleksandras Gluchovas
// Modified by:
// Created:     27/12/98
// RCS-ID:      $Id$
// Copyright:   (c) Aleskandars Gluchovas
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "ifcontext.h"
#pragma interface
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <malloc.h>
#include <stdio.h>

#include "ifcontext.h"

/***** Implementation for class spInterFileContext *****/

size_t spInterFileContext::GetFileNo( const string& fname )
{
	for( size_t i = 0; i != mFiles.size(); ++i )

		if ( fname == mFiles[i] ) return i;

	wxASSERT(0); // DBG::
	return 0;
}

size_t spInterFileContext::GetFileNoOfContext( spContext& ctx )
{
	spContext* pCtx = ctx.GetEnclosingContext( SP_CTX_FILE );

	// DBG:: outer-file context should be present
	wxASSERT( pCtx && pCtx->GetType() == SP_CTX_FILE ); 

	return GetFileNo( ((spFile*)pCtx)->mFileName );
}

/*** public interface ***/

spInterFileContext::spInterFileContext()
{}

spInterFileContext::~spInterFileContext()
{}

void spInterFileContext::AddFile( const string& fname, const string& content )
{
	mFiles.push_back( fname );
	mContents.push_back( content );
}

void spInterFileContext::RemoveContext( spContext& ctx )
{
	wxASSERT( ctx.PositionIsKnown() ); // DBG:: should be checked by-user code

	size_t fNo = GetFileNoOfContext( ctx );

	mDeletionMarks.push_back( spBookmark( ctx.mSrcOffset, ctx.mContextLength, fNo ) );
}

void spInterFileContext::InsertBookmarkSorted( BookmarkListT& lst, spBookmark& mark )
{
	for( size_t i = 0; i != lst.size(); ++i )
	
		if ( lst[i].mFrom > mark.mFrom )
		{
			lst.insert( &lst[i], mark );
			return;
		}

	lst.push_back( mark );
}		

void spInterFileContext::DoAppendSourceFragment( string& source, 
												 string& result, 
												 size_t  pos, size_t len )
{
	mFiltered.erase( mFiltered.begin(), mFiltered.end() );

	size_t i;

	for( i = 0; i != mDeletionMarks.size(); ++i )
	{
		spBookmark& mark = mDeletionMarks[i];

		if ( mark.mFileNo == mCurFileNo && 
			 mark.mFrom >= pos && mark.mFrom < pos + len )
		
			 InsertBookmarkSorted( mFiltered, mark );
	}

	size_t cur = pos;
	
	for( i = 0; i != mFiltered.size(); ++ i )
	{
		spBookmark& mark = mFiltered[i];

		result.append( source, cur, ( (size_t)mark.mFrom - cur ) );

		cur = size_t( mark.mFrom + mark.mLen );

		if ( cur >= pos + len ) // check if we've overstepped the current source-fragment
		{
			wxASSERT(0); // DBG:: with current imp. this should not happen
			
			cur = pos + len; break;
		}
	}

	result.append( source, cur, ( pos + len ) - cur );
}

void spInterFileContext::GenerateContextBody( spContext& ctx, 
											  string&    source,
											  string&    result, 
											  size_t&    lastSavedPos,
											  size_t&    lastKnownPos )
{
	if ( ctx.PositionIsKnown() )

		lastKnownPos = ctx.mSrcOffset;

	if ( ctx.IsVirtualContext() )
	{
		// add fragment accumulated before this context

		DoAppendSourceFragment( source, result,
							    size_t(lastSavedPos), 
							    size_t(lastKnownPos - lastSavedPos) );

		// add context body

		result += ctx.GetVirtualContextBody();

		lastSavedPos = lastKnownPos;

		if ( ctx.PositionIsKnown() )
		{
			if ( ctx.VitualContextHasChildren() )
			{
				lastKnownPos = ctx.mSrcOffset + ctx.mHeaderLength;

				lastSavedPos = lastKnownPos;
			}
			else
			{
				lastKnownPos = ctx.mSrcOffset + ctx.mContextLength;

				lastSavedPos = lastKnownPos;

				return; // have not children
			}
		}
	}

	MMemberListT& lst = ctx.GetMembers();

	for( size_t i = 0; i != lst.size(); ++i )
	
		GenerateContextBody( *lst[i], source, result, lastSavedPos, lastKnownPos );
	
	if ( ctx.IsVirtualContext() )
	{
		if ( ctx.VitualContextHasChildren() && 

			 ctx.GetFooterOfVirtualContextBody() != "" )
		{
			// append the reminder space after children of the context

			DoAppendSourceFragment( result, source,
									size_t(lastSavedPos), 
									size_t(lastKnownPos - lastSavedPos) );

			// add footer 
			result += ctx.GetFooterOfVirtualContextBody();

			lastKnownPos = ctx.mSrcOffset + ctx.mContextLength;

			lastSavedPos = lastKnownPos;
		}
	}

	if ( ctx.PositionIsKnown() )

		lastKnownPos = ctx.mSrcOffset + ctx.mContextLength;
}

void spInterFileContext::GenrateContents()
{
	MMemberListT& lst = GetMembers();

	for( size_t f = 0; f != lst.size(); ++f )
	{
		string& fname = ((spFile*)lst[f])->mFileName;

		size_t fileNo = GetFileNo( fname );

		string& source = mContents[ fileNo ];

		string result;

		size_t lastKnownPos = 0, // the begining of the file is always "known"
			   lastSavedPos = 0;

		mCurFileNo = fileNo;

		GenerateContextBody( *lst[f], source, result, lastSavedPos, lastKnownPos );

		// the end of file is always known

		lastKnownPos = mContents[ fileNo ].length();

		// append the reminder 

		DoAppendSourceFragment( source, result,
							    size_t(lastSavedPos), 
							    size_t(lastKnownPos - lastSavedPos) );

		// replace original contnet with newly generated one

		mContents[ fileNo ] = result;
	}
}

void spInterFileContext::ParseContents( SourceParserPlugin* pPlugin )
{
	mDeletionMarks.erase( mDeletionMarks.begin(), mDeletionMarks.end() );

	RemoveChildren(); // clean up top-level context

	mParser.SetPlugin( pPlugin );

	for( size_t i = 0; i != mFiles.size(); ++i )
	{
		char* s = (char*)(mContents[i].c_str());

		spFile* pFCtx = mParser.Parse( s, s + mContents[i].length() );

		pFCtx->mFileName = mFiles[i];

		AddMember( pFCtx );
	}
}

void spInterFileContext::WriteToFiles()
{
	for( size_t i = 0; i != mFiles.size(); ++i )
	{
		FILE* fp = fopen( mFiles[i].c_str(), "w+t" );

		if ( int(fp) > 0 )
		{
			fwrite( mContents[i].c_str(), sizeof(char), mContents[i].length(), fp );

			fclose( fp );
		}
	}
}

string spInterFileContext::GetBody( spContext* pCtx )
{
	wxASSERT( pCtx->PositionIsKnown() ); // DBG:: should be checked by-user code

	string& source = mContents[ GetFileNoOfContext( *pCtx ) ];

	return string( source.c_str() + pCtx->mSrcOffset, pCtx->mContextLength );
}

string spInterFileContext::GetHeader( spContext* pCtx )
{
	wxASSERT( pCtx->PositionIsKnown() );   // DBG:: should be checked by-user code

	wxASSERT( pCtx->mHeaderLength != -1 ); // DBG:: -/-

	string& source = mContents[ GetFileNoOfContext( *pCtx ) ];

	return string( source.c_str() + pCtx->mSrcOffset, pCtx->mHeaderLength );
}
