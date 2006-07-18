/////////////////////////////////////////////////////////////////////////////
// Name:        No names yet.
// Purpose:     Contrib. demo
// Author:      Aleksandras Gluchovas
// Modified by:
// Created:     22/09/98
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

#include "srcparser.h"

/***** Implementation for class spVisitor *****/

void spVisitor::VisitAll( spContext& atContext,
                          bool sortContent
                        )
{
    mSiblingSkipped = false;
    mChildSkipped   = false;
    mContextMask    = SP_CTX_ANY; // FIXME:: should be an arg.

    if ( sortContent && !atContext.IsSorted() )

        atContext.SortMembers();

    mpCurCxt = &atContext; // FIXME:: this is dirty, restoring it each time

    if ( atContext.GetContextType() & mContextMask )

        atContext.AcceptVisitor( *this );

    MMemberListT& members = atContext.GetMembers();

    for( size_t i = 0; i != members.size(); ++i )
    {
        if ( mSiblingSkipped )

            return;

        if ( !mChildSkipped )
        {
            size_t prevSz = members.size();

            // visit members of the context recursivelly
            VisitAll( *members[i], sortContent );

            if ( members.size() != prevSz )

                --i; // current member was removed!

            mChildSkipped = 0;
        }
    }
}

void spVisitor::RemoveCurrentContext()
{
    if ( mpCurCxt->GetParent() )

        mpCurCxt->GetParent()->RemoveChild( mpCurCxt );
}

void spVisitor::SkipSiblings()
{
    mSiblingSkipped = true;
}

void spVisitor::SkipChildren()
{
    mChildSkipped = true;
}

void spVisitor::SetFilter( int contextMask )
{
    mContextMask = contextMask;
}

/***** Implementation for class spComment *****/

bool spComment::IsMultiline()  const
{
    return mIsMultiline;
}

bool spComment::StartsParagraph() const
{
    return mStartsPar;
}

wxString& spComment::GetText()
{
    return m_Text;
}

wxString spComment::GetText() const
{
    return m_Text;
}

/***** Implementation for class spContext *****/

spContext::spContext()

    : m_pParent        ( NULL ),
      mpFirstOccurence( NULL ),
      mAlreadySorted  ( false ),

      mSrcLineNo    (-1),
      mSrcOffset    (-1),
      mContextLength(-1),
      mLastScrLineNo(-1),

      mHeaderLength (-1),
      mFooterLength (-1),

      mFirstCharPos (-1),
      mLastCharPos  (-1),

      mVisibility( SP_VIS_PRIVATE ),

      mIsVirtualContext         ( false ),
      mVirtualContextHasChildren( false ),

      mpUserData( NULL )
{}

void spContext::RemoveChildren()
{
    for( size_t i = 0; i != mMembers.size(); ++i )

        delete mMembers[i];

    mMembers.erase( mMembers.begin(), mMembers.end() );
}

spContext::~spContext()
{
    RemoveChildren();

    for( size_t i = 0; i != mComments.size(); ++i )

        delete mComments[i];
}

bool spContext::IsSorted()
{
    return mAlreadySorted;
}

void spContext::GetContextList( MMemberListT& lst, int contextMask )
{
    for( size_t i = 0; i != mMembers.size(); ++i )
    {
        spContext& member = *mMembers[i];

        if ( member.GetContextType() & contextMask )

            lst.push_back( &member );

        // collect required contexts recursively
        member.GetContextList( lst, contextMask );
    }
}

bool spContext::HasComments()
{
    return ( mComments.size() != 0 );
}

void spContext::RemoveChild( spContext* pChild )
{
    for( size_t i = 0; i != mMembers.size(); ++i )

        if ( mMembers[i] == pChild )
        {
            mMembers.erase( &mMembers[i] );

            delete pChild;
            return;
        }

    // the given child should exist on the parent's list
    wxASSERT( 0 );
}

spContext* spContext::GetEnclosingContext( int mask )
{
    spContext* cur = this->GetParent();

    while ( cur && !(cur->GetContextType() & mask) )

        cur = cur->GetParent();

    return cur;
}

bool spContext::PositionIsKnown()
{
    return ( mSrcOffset != (-1) && mContextLength != (-1) );
}

bool spContext::IsVirtualContext()
{
    return mIsVirtualContext;
}

bool spContext::VitualContextHasChildren()
{
    return mVirtualContextHasChildren;
}

wxString spContext::GetVirtualContextBody()
{
    wxASSERT( mIsVirtualContext );

    return mVirtualContextBody;
}

wxString spContext::GetFooterOfVirtualContextBody()
{
    wxASSERT( mIsVirtualContext );

    return mVittualContextFooter;
}


void spContext::SetVirtualContextBody( const wxString& body,
                                       bool            hasChildren,
                                       const wxString& footer )
{
    mVirtualContextHasChildren = hasChildren;

    mVirtualContextBody   = body;
    mVittualContextFooter = footer;

    // atuomaticllay becomes virtual context

    mIsVirtualContext   = true;
}

wxString spContext::GetBody( spContext* pCtx )
{
    if ( ( pCtx == NULL || pCtx == this ) && mIsVirtualContext )
        return mVirtualContextBody;

    if ( GetParent() )
        return GetParent()->GetBody( ( pCtx != NULL ) ? pCtx : this );
    else
        return wxEmptyString; // source-fragment cannot be found
}

wxString spContext::GetHeader( spContext* pCtx )
{
    if ( GetParent() )
        return GetParent()->GetHeader( ( pCtx != NULL ) ? pCtx : this );
    else
        return wxEmptyString; // source-fragment cannot be found
}

bool spContext::IsFirstOccurence()
{
    return ( mpFirstOccurence != 0 );
}

spContext* spContext::GetFirstOccurence()
{
    // this object should not itself be
    // the first occurence of the context
    wxASSERT( mpFirstOccurence != 0 );

    return mpFirstOccurence;
}

void spContext::AddMember( spContext* pMember )
{
    mMembers.push_back( pMember );

    pMember->m_pParent = this;
}

void spContext::AddComment( spComment* pComment )
{
    mComments.push_back( pComment );
}

MMemberListT& spContext::GetMembers()
{
    return mMembers;
}

spContext* spContext::FindContext( const wxString& identifier,
                                   int   contextType,
                                   bool  searchSubMembers
                                 )
{
    for( size_t i = 0; i != mMembers.size(); ++i )
    {
        spContext& member = *mMembers[i];

        if ( member.GetName() == identifier &&
             ( contextType & member.GetContextType() )
           )

           return &member;

        if ( searchSubMembers )
        {
            spContext* result =
                member.FindContext( identifier, contextType, 1 );

            if ( result ) return result;
        }
    }

    return 0;
}

void spContext::RemoveThisContext()
{
    if ( m_pParent )
        m_pParent->RemoveChild( this );
    else
        // context should have a parent
        wxFAIL_MSG("Context should have a parent");
}

spContext* spContext::GetOutterContext()
{
    return m_pParent;
}

bool spContext::HasOutterContext()
{
    return ( m_pParent != 0 );
}

bool spContext::IsInFile()
{
    return ( GetOutterContext()->GetContextType() == SP_CTX_FILE );
}

bool spContext::IsInNameSpace()
{
    return ( GetOutterContext()->GetContextType() == SP_CTX_NAMESPACE );
}

bool spContext::IsInClass()
{
    return ( GetOutterContext()->GetContextType() == SP_CTX_CLASS );
}

bool spContext::IsInOperation()
{
    return ( GetOutterContext()->GetContextType() == SP_CTX_OPERATION );
}

spClass& spContext::GetClass()
{
    wxASSERT( GetOutterContext()->GetType() == SP_CTX_CLASS );
    return *((spClass*)m_pParent );
}

spFile& spContext::GetFile()
{
    wxASSERT( GetOutterContext()->GetType() == SP_CTX_FILE );
    return *((spFile*)m_pParent );
}

spNameSpace& spContext::GetNameSpace()
{
    wxASSERT( GetOutterContext()->GetType() == SP_CTX_NAMESPACE );
    return *((spNameSpace*)m_pParent );
}

spOperation& spContext::GetOperation()
{
    wxASSERT( GetOutterContext()->GetType() == SP_CTX_OPERATION );
    return *((spOperation*)m_pParent );
}

/***** Implementation for class spClass *****/

void spClass::SortMembers()
{
    // TBD::
}

/***** Implementation for class spOperation *****/

spOperation::spOperation()

    : mHasDefinition( false )
{
    mIsConstant =
    mIsVirtual =
    mHasDefinition = false;
}

wxString spOperation::GetFullName(MarkupTagsT tags)
{
    wxString txt = tags[TAG_BOLD].start + m_RetType;
    txt += _T(" ");
    txt += m_Name;
    txt += _T("( ");
    txt += tags[TAG_BOLD].end;

    for( size_t i = 0; i != mMembers.size(); ++i )
    {
        // DBG::
        wxASSERT( mMembers[i]->GetContextType() == SP_CTX_PARAMETER );

        spParameter& param = *((spParameter*)mMembers[i]);

        if ( i != 0 )
            txt += _T(", ");

        txt += tags[TAG_BOLD].start;

        txt += param.m_Type;

        txt += tags[TAG_BOLD].end;
        txt += tags[TAG_ITALIC].start;

        txt += _T(" ");
        txt += param.m_Name;

        if ( !param.m_InitVal.empty() )
        {
            txt += _T(" = ");
            txt += tags[TAG_BOLD].start;

            txt += param.m_InitVal;

            txt += tags[TAG_BOLD].end;
        }

        txt += tags[TAG_ITALIC].end;;
    }

    txt += tags[TAG_BOLD].start;
    txt += " )";
    txt += tags[TAG_BOLD].end;

    // TBD:: constantness of method

    return txt;
}

/***** Implemenentation for class spPreprocessorLine *****/

wxString spPreprocessorLine::CPP_GetIncludedFileNeme() const
{
    wxASSERT( GetStatementType() == SP_PREP_DEF_INCLUDE_FILE );

    size_t i = 0;

    while( i < m_Line.length() && m_Line[i] != _T('"') && m_Line[i] != _T('<') )

        ++i;

    ++i;

    size_t start = i;

    while( i < m_Line.length() && m_Line[i] != _T('"') && m_Line[i] != _T('>') )

        ++i;

    if ( start < m_Line.length() )
    {
        wxString fname;
        fname.append( m_Line, start, ( i - start ) );

        return fname;
    }
    else
        return wxEmptyString; // syntax error probably
}



/***** Implemenentation for class SourceParserBase *****/

SourceParserBase::SourceParserBase()

    : mpFileBuf( NULL ),
      mFileBufSz( 0 ),

      mpPlugin( NULL )
{}

SourceParserBase::~SourceParserBase()
{
    if ( mpFileBuf ) free( mpFileBuf );

    if ( mpPlugin ) delete mpPlugin;
}

spFile* SourceParserBase::ParseFile( const char* fname )
{
    // FIXME:: the below should not be fixed!

    const size_t MAX_BUF_SIZE = 1024*256;

    if ( !mpFileBuf ) mpFileBuf = (char*)malloc( MAX_BUF_SIZE );

    mFileBufSz = MAX_BUF_SIZE;

    FILE* fp = fopen( fname, "rt" );

    if ( !fp ) return NULL;

    int sz = fread( mpFileBuf, 1, mFileBufSz, fp );

    return Parse( mpFileBuf, mpFileBuf + sz );
}

void SourceParserBase::SetPlugin( SourceParserPlugin* pPlugin )
{
    if ( mpPlugin ) delete mpPlugin;

    mpPlugin = pPlugin;
}

// ===========================================================================
// debug methods
// ===========================================================================

#ifdef __WXDEBUG__

void spContext::Dump(const wxString& indent) const
{
    DumpThis(indent);

    // increase it for the children
    wxString indentChild = indent + "    ";

    for ( MMemberListT::const_iterator i = mMembers.begin();
          i != mMembers.end();
          i++ ) {
        (*i)->Dump(indentChild);
    }
}

void spContext::DumpThis(const wxString& WXUNUSED(indent)) const
{
    wxFAIL_MSG("abstract base class can't be found in parser tree!");
}

void spParameter::DumpThis(const wxString& indent) const
{
    wxLogDebug("%sparam named '%s' of type '%s'",
               indent.c_str(), m_Name.c_str(), m_Type.c_str());
}

void spAttribute::DumpThis(const wxString& indent) const
{
    wxLogDebug("%svariable named '%s' of type '%s'",
               indent.c_str(), m_Name.c_str(), m_Type.c_str());
}

void spOperation::DumpThis(const wxString& indent) const
{
    wxString protection;
    if ( !mScope.empty() ) {
        switch ( mVisibility ) {
            case SP_VIS_PUBLIC:
                protection = "public";
                break;

            case SP_VIS_PROTECTED:
                protection = "protected";
                break;

            case SP_VIS_PRIVATE:
                protection = "private";
                break;

            default:
                wxFAIL_MSG("unknown protection type");
        }
    }
    else {
        protection = "global";
    }

    wxString constStr,virtualStr;
    if(mIsConstant) constStr = _T("const ");
    if(mIsVirtual) virtualStr = _T("virtual ");

    wxLogDebug("%s%s%s%s function named '%s::%s' of type '%s'",
               indent.c_str(),
               constStr.c_str(),
               virtualStr.c_str(),
               protection.c_str(),
               mScope.c_str(), m_Name.c_str(), m_RetType.c_str());
}

void spPreprocessorLine::DumpThis(const wxString& indent) const
{
    wxString kind;
    switch ( mDefType ) {
        case SP_PREP_DEF_DEFINE_SYMBOL:
            kind = "define";
            break;

        case SP_PREP_DEF_REDEFINE_SYMBOL:
            kind = "redefine";
            break;

        case SP_PREP_DEF_INCLUDE_FILE:
            kind.Printf("include (%s)", CPP_GetIncludedFileNeme().c_str());
            break;

        case SP_PREP_DEF_OTHER:
            kind = "other";
            break;

    }

    wxLogDebug("%spreprocessor statement: %s",
               indent.c_str(), kind.c_str());
}

void spClass::DumpThis(const wxString& indent) const
{
    wxString base;
    for ( StrListT::const_iterator i = m_SuperClassNames.begin();
          i != m_SuperClassNames.end();
          i++ ) {
        if ( !base.empty() )
            base += ", ";
        base += *i;
    }

    if ( !base )
        base = "none";

    wxString kind;
    switch ( mClassSubType ) {
        case SP_CLTYPE_CLASS:
            kind = "class";
            break;

        case SP_CLTYPE_TEMPLATE_CLASS:
            kind = "template class";
            break;

        case SP_CLTYPE_STRUCTURE:
            kind = "struc";
            break;

        case SP_CLTYPE_UNION:
            kind = "union";
            break;

        case SP_CLTYPE_INTERFACE:
            kind = "interface";
            break;

        default:
            wxFAIL_MSG("unknown class subtype");
    }

    wxLogDebug("%s%s named '%s' (base classes: %s)",
               indent.c_str(), kind.c_str(),
               m_Name.c_str(), base.c_str());
}

void spEnumeration::DumpThis(const wxString& indent) const
{
    wxLogDebug("%senum named '%s'",
               indent.c_str(), m_Name.c_str());
}

void spTypeDef::DumpThis(const wxString& indent) const
{
    wxLogDebug("%stypedef %s = %s",
               indent.c_str(), m_Name.c_str(), m_OriginalType.c_str());
}

void spFile::DumpThis(const wxString& indent) const
{
    wxLogDebug("%sfile '%s'",
               indent.c_str(), m_FileName.c_str());
}

#endif // __WXDEBUG__
