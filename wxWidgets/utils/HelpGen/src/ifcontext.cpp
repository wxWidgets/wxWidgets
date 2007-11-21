/////////////////////////////////////////////////////////////////////////////
// Name:        No names yet.
// Purpose:     Contrib. demo
// Author:      Aleksandras Gluchovas
// Modified by:
// Created:     27/12/98
// RCS-ID:      $Id$
// Copyright:   (c) Aleskandars Gluchovas
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <stdio.h>

#include "ifcontext.h"

/***** Implementation for class spInterFileContext *****/

size_t spInterFileContext::GetFileNo( const wxString& fname )
{
    for ( size_t i = 0; i != m_Files.size(); ++i )
    {
        if ( fname == m_Files[i] )
            return i;
    }

    wxFAIL_MSG("File not found in array in function spInterFileContext::GetFileNo()");

    return 0;
}

size_t spInterFileContext::GetFileNoOfContext( spContext& ctx )
{
    spContext* pCtx = ctx.GetEnclosingContext( SP_CTX_FILE );

    // DBG:: outer-file context should be present
    wxASSERT( pCtx && pCtx->GetType() == SP_CTX_FILE );

    return GetFileNo( ((spFile*)pCtx)->m_FileName );
}

/*** public interface ***/

spInterFileContext::spInterFileContext()
{}

spInterFileContext::~spInterFileContext()
{}

void spInterFileContext::AddFile( const wxString& fname, const wxString& content )
{
    m_Files.push_back( fname );
    m_Contents.push_back( content );
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

void spInterFileContext::DoAppendSourceFragment( wxString& source,
                                                 wxString& result,
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
//            wxASSERT(0); // DBG:: with current imp. this should not happen
            wxFAIL_MSG("Overstepped the current source fragment in function\nspInterFileContext::DoAppendSourceFragment()");
            cur = pos + len; break;
        }
    }

    result.append( source, cur, ( pos + len ) - cur );
}

void spInterFileContext::GenerateContextBody( spContext& ctx,
                                              wxString&  source,
                                              wxString&  result,
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
    {
        GenerateContextBody( *lst[i], source, result, lastSavedPos, lastKnownPos );
    }

    if ( ctx.IsVirtualContext() )
    {
        if ( ctx.VitualContextHasChildren() && !ctx.GetFooterOfVirtualContextBody().empty() )
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
        wxString& fname = ((spFile*)lst[f])->m_FileName;

        size_t fileNo = GetFileNo( fname );

        wxString& source = m_Contents[ fileNo ];

        wxString result;

        size_t lastKnownPos = 0, // the begining of the file is always "known"
               lastSavedPos = 0;

        mCurFileNo = fileNo;

        GenerateContextBody( *lst[f], source, result, lastSavedPos, lastKnownPos );

        // the end of file is always known

        lastKnownPos = m_Contents[ fileNo ].length();

        // append the reminder

        DoAppendSourceFragment( source, result,
                                size_t(lastSavedPos),
                                size_t(lastKnownPos - lastSavedPos) );

        // replace original contnet with newly generated one

        m_Contents[ fileNo ] = result;
    }
}

void spInterFileContext::ParseContents( SourceParserPlugin* pPlugin )
{
    mDeletionMarks.erase( mDeletionMarks.begin(), mDeletionMarks.end() );

    RemoveChildren(); // clean up top-level context

    mParser.SetPlugin( pPlugin );

    for( size_t i = 0; i != m_Files.size(); ++i )
    {
        wxChar* s = (char*)(m_Contents[i].c_str());

        spFile* pFCtx = mParser.Parse( s, s + m_Contents[i].length() );

        pFCtx->m_FileName = m_Files[i];

        AddMember( pFCtx );
    }
}

void spInterFileContext::WriteToFiles()
{
    for( size_t i = 0; i != m_Files.size(); ++i )
    {
        FILE* fp = fopen( m_Files[i].c_str(), "w+t" );

        if ( fp != NULL )
        {
            fwrite( m_Contents[i].c_str(), sizeof(char), m_Contents[i].length(), fp );

            fclose( fp );
        }
    }
}

wxString spInterFileContext::GetBody( spContext* pCtx )
{
    wxASSERT( pCtx->PositionIsKnown() ); // DBG:: should be checked by-user code

    wxString& source = m_Contents[ GetFileNoOfContext( *pCtx ) ];

    return wxString( source.c_str() + pCtx->mSrcOffset, pCtx->mContextLength );
}

wxString spInterFileContext::GetHeader( spContext* pCtx )
{
    wxASSERT( pCtx->PositionIsKnown() );   // DBG:: should be checked by-user code

    wxASSERT( pCtx->mHeaderLength != -1 ); // DBG:: -/-

    wxString& source = m_Contents[ GetFileNoOfContext( *pCtx ) ];

    return wxString( source.c_str() + pCtx->mSrcOffset, pCtx->mHeaderLength );
}
