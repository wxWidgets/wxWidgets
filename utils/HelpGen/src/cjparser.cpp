/////////////////////////////////////////////////////////////////////////////
// Name:        No names yet.
// Purpose:     Contrib. demo
// Author:      Aleksandras Gluchovas
// Modified by:
// Created:     22/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Aleskandars Gluchovas
// Licence:       wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "acell.h"
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

#include "cjparser.h"

#if defined( wxUSE_TEMPLATE_STL )

    #include <map>

#else

    #include "wxstlac.h"

#endif


/***** Implementation for class SJParser *****/

// statics used by inline'ed C helper-functions
static char* _gSrcStart = 0;
static char* _gSrcEnd   = 0;
static char* _gLastSuppresedComment = 0;
static int   _gLineNo      = 0;

// FOR NOW:: comments queue is static
#define MAX_CQ_ENTRIES 128
static char* _gCommentsQueue[MAX_CQ_ENTRIES];
static int    _gCQSize = 0;

/***** keyword map related structures *****/

struct less_c_str
{
    inline bool operator()( char* x, char* y) const
    {     return ( strcmp( x,y ) < 0 );
    }
};

//WXSTL_MAP(CharPtrT,CharPtrT, LESS_THEN_FUNCTOR(CharPtrT));

#if defined( wxUSE_TEMPLATE_STL )

    typedef map< char*, char*, less_c_str > KeywordMapT;

#else

    typedef char* CharPtrT;
    typedef WXSTL_MAP( CharPtrT, CharPtrT ,less_c_str) KeywordMapT;

#endif

static KeywordMapT __gMultiLangMap;
static int         __gMapReady = 0;

static char* __gKeyWords[] =
{
    "public",
    "protected",
    "private",

    "class",
    "struct",
    "union",
    "enum",
    "interface",

    "package",
    "import",

    "typedef",
    "template",
    "friend",
    "const",
    "volatile",
    "mutable",
    "virtual",
    "inline",
    "static",
    "register",

    "final",
    "abstract",
    "native",

    "__stdcall",
    "extern",

    0
};

static void check_keyword_map()
{
    if ( !__gMapReady )
    {
        __gMapReady = 1;

        // "make sure" the address of the first member of non-polimorphic class
        // coinsides with the address of the instance

        char** keyword = __gKeyWords;

        while ( (*keyword) != 0 )
        {
            __gMultiLangMap.insert(
                KeywordMapT::value_type( *keyword, *keyword )
            );

            ++keyword;
        }
    }
}

/***** helper functions *****/

static inline void skip_to_eol( char*& cur )
{
    while( *(cur) != 10 && *cur != 13 && cur < _gSrcEnd) ++cur;
}

static inline void skip_eol( char*& cur )
{
    if ( *cur == 13 )

        cur += 2;
    else
        cur += 1;

    ++_gLineNo;
}

static inline bool skip_to_next_comment_in_the_line( char*& cur )
{
    do
    {
        while( cur < _gSrcEnd  &&
               *cur != 10 &&
               *cur != 13 &&
               *cur != '/'
             ) ++cur;

        if ( cur == _gSrcEnd ) return FALSE;

        if ( *cur == '/' )
        {
            if ( (*(cur+1) == '*') ||
                 (*(cur+1) == '/') ) return TRUE;
            else
            {
                ++cur;
                continue;
            }
        }

        return FALSE;

    } while(1);
}

inline static void store_line_no( int& toVar )
{
    toVar = _gLineNo;
}

inline static void restore_line_no( int storedLineNo )
{
    _gLineNo = storedLineNo;
}

inline static int get_line_no()
{
    return _gLineNo;
}

static void skip_to_prev_line( char*& cur )
{
    while( cur >= _gSrcStart  &&
           *cur != 10 &&
           *cur != 13
           ) --cur;

    // NOTE:: '\n' is 13,10 for DOS
    //        '\n' is 10 for UNIX

    // NOTE1: '\n' symbol is not used here,
    //        to provide possibility of loading
    //        file as binary

    --cur;
    if ( *cur == 10 )
    {
        ++cur;
        return;
    }

    if ( *cur == 13 ) --cur;

    while( cur >= _gSrcStart  &&
           *cur != 10 &&
           *cur != 13
           ) --cur;

    ++cur; // move to the first character in the line
}

static inline void skip_comments( char*& cur )
{
    ++cur; // skip '/' token

    if ( *cur != '/' && *cur != '*' ) return;

    // first, store position of the comment into the queue
    // (which further will be attached to the next context
    //  found)

    if ( cur-1 != _gLastSuppresedComment )
    {
        if ( _gCQSize == MAX_CQ_ENTRIES )
        {
            size_t i = MAX_CQ_ENTRIES-1;

            while( i != 0 )
            {
                _gCommentsQueue[i-1] = _gCommentsQueue[i];
                --i;
            }

            --_gCQSize ;
        }

        _gCommentsQueue[_gCQSize++] = cur-1;
    }

    // if signle-line comment, skip it now
    if ( *cur == '/' )
    {
        skip_to_eol( cur );
        skip_eol( cur );
        return;
    }

    size_t level = 1;

    // check for multiline comment (handle nested multiline comments!)

    int line_len = 0;

    ++cur;
    ++cur;
    do
    {
        // TBD:: check eof cond.

        // detect and remove vertical columns of '*''s

        while ( *cur != '/' && cur < _gSrcEnd )
        {
            switch (*cur)
            {
                case '*' :
                    {
                        if ( *(cur+1) != '/' )
                        {
                            if ( line_len == 1 )

                                *cur = ' ';
                        }

                        break;
                    }

                case 13 : line_len = 0; break;
                case 10 : { line_len = 0; ++_gLineNo; } break;

                default : ++line_len;
            }

            ++cur;
        }

        if ( cur >= _gSrcEnd  ) return;

        ++cur;

        if ( *(cur-2) == '*' )
        {
            --level;
            if ( level == 0 )
                break;
        }
        else
        if ( *cur == '*' )
        {
            ++cur;
            ++cur;

            ++level;
        }

    } while(1);
}

static inline void clear_commets_queue()
{
    _gCQSize = 0;
}

static inline void skip_quoted_string( char*& cur )
{
    ++cur; // skip first quote '"'

    // check if quote wasn't prefixed
    if ( *(cur-2) == '\\' )
        return;

    do
    {
        while ( *cur != '"' && cur < _gSrcEnd )
        {
            if ( *cur == 10 ) ++_gLineNo;
            ++cur;
        }

        if ( cur >= _gSrcEnd ) return;

        ++cur; // skip the last quote

        // check if it wasn't prefixed

        if ( *(cur-2) != '\\' )
            break;

    } while (1);
}

// skips subsequent white space and comments
// (return false if the end of source code reached)

static inline bool get_next_token( char*& cur )
{
    for( ; cur < _gSrcEnd; ++cur )
    {
        switch( *(cur) )
        {
            case ' ' : continue;
            case '\t': continue;
            case 13  : continue;

            case 10  : { ++_gLineNo;continue; }

            case '/' : skip_comments( cur );
                       --cur;
                       continue;

            default : break;
        };

        break;
    }

    if ( cur >= _gSrcEnd )

        return FALSE;
    else
        return TRUE;
}

static inline void skip_preprocessor_dir( char*& cur )
{
    do
    {
        skip_to_eol(cur);

        if ( *(cur-1) != '\\' )
            break;

        if ( cur < _gSrcEnd )
            skip_eol( cur );
        else
            break;

    } while(1);
}

static void skip_token( char*& cur )
{
    if ( *cur == '"' )
    {
        skip_quoted_string( cur );
        return;
    }

    if ( *cur == ',' ||
         *cur == ';' ||
         *cur == ')' ||
         *cur == '('
       )
    {
        ++cur;
        return;
    }

    // special case of "!=", "<=", ... 2 character composite tokens
    if ( *cur == '<' ||
         *cur == '>' ||
         *cur == '=' ||
         *cur == '!'
       )
    {
        cur++;
        if ( *cur == '=' )
            cur++;

        return;
    }

    ++cur; // leading character is always skipped

    for( ; cur < _gSrcEnd ; ++cur )
    {
        switch ( *cur )
        {
            case ' ' : break;
            case '\t': break;
            case 13  : break;
            case 10  : break;
            case ',' : break;
            case ';' : break;
            case '<' : break;
            case '>' : break;

            // FIXME:: QUICK-HACK:: to treat scope resolution
            //         tokens are a part of the string - e.g. SomeSpace::SubName would
            //         become one token

            case ':' : if ( *(cur+1) == ':' )
                       {
                           ++cur;
                           continue;
                       }

                       break;
            case '=' : break;
            case '(' : break;
            case ')' : break;
            case '{' : break;
            case '}' : break;

            default : continue;
        };
        break;
    }
}

static inline size_t get_token_len( char* tok )
{
    char* start = tok;

    skip_token( tok );

    return size_t( tok - start );
}

// returns true, if given tokens are equel

static inline bool cmp_tokens( char* tok1, char* tok2 )
{
    // NOTE:: the case one token includes
    //        other in it's entirely is not handled

    size_t len = get_token_len( tok1 );

    // assuming that tokens are non-zero length

    do
    {
        if ( *(tok1++) != *(tok2++) )
            return FALSE;

        --len;

    } while ( --len );

    return TRUE;
}

static inline bool cmp_tokens_fast( char* tok1, char* tok2, size_t len )
{
    do
    {
        if ( *(tok1++) != *(tok2++) )
            return FALSE;

    } while ( --len );

    return TRUE;
}

static inline void skip_tempalate_statement( char*& cur )
{
    size_t level = 0;

    // go one level deeper
    while( *cur != '<' && cur < _gSrcEnd )
    {
        if (*cur == 10 ) ++_gLineNo;
        ++cur;
    }

    // FIXME:: template should be checked statement for
    //         comments inside of it

    do
    {
        if ( *cur == '<' )
            ++level;
        else
            --level;

        ++cur; // skip '<' or '>' token

        if ( level == 0 )
            return;

        while( *cur != '<' && *cur != '>' && cur < _gSrcEnd )
        {
            if (*cur == 10 ) ++_gLineNo;
            ++cur;
        }

    } while (1);
}

static inline void skip_statement( char*& cur )
{
    for( ; cur < _gSrcEnd; ++cur )

        switch (*cur)
        {
            case  ';' : ++cur; // skip statement-terminator token
                        return;

            case  '"' : skip_quoted_string(cur);
                        --cur;
                        continue;

            case  10  : ++_gLineNo;

                        continue;
            case  '/' : skip_comments( cur );
                        --cur;
                        continue;
            default : continue;
        }
}

// "reversed" versions of skip_token() and get_next_token()

static inline void skip_token_back( char*& cur )
{
    // FIXME:: now, when moving backwards, neither strings nor
    //         comment blocks are checked

    --cur; // skip to the trailing character

    if ( *cur == ',' ||
         *cur == ')' ||
         *cur == '('
       )
       return;


    for( ; cur < _gSrcEnd ; --cur )
    {
        switch ( *cur )
        {
            case ' ' : break;
            case '\t': break;
            case 13  : break;
            case 10  : break;
            case ',' : break;
            case '(' : break;

            default : continue;
        };

        break;
    }

    ++cur; // get to the leading character of the token
}

static inline void skip_next_token_back( char*& cur )
{
    --cur; // skip leading character of the current token

    if ( *cur == ',' ||
         *cur == ')' ||
         *cur == '('
       )
    {
       ++cur;
       return;
    }

    for( ; cur < _gSrcEnd; --cur )
    {
        switch ( *cur )
        {
            case ' ' : continue;
            case '\t': continue;
            case 13  : continue;
            case 10  : continue;
            case ',' : continue;
            case '(' : continue;

            default : break;
        };

        break;
    }

    ++cur; // position after the trailing charcter of the prev token
}

static string get_token_str( char* cur )
{
    return string( cur, get_token_len( cur ) );
}

// skips token or whole expression which may have
// nested  expressions between '(' ')' brackets.
//
// Upon return, the cursor points to the terminating bracket ')',
//
// Return value is the size of the block

static size_t skip_block( char*& cur )
{
    size_t level = 0; // nesting level

    char* start = cur;

    // NOTE:: assumed that block not necessarely starts
    //        with bracket rightaway

    if ( *cur == '(' )
    {
        ++level;
    }

    do
    {
        skip_token( cur );

        char* savedPos = cur;
        int tmpLnNo;
        store_line_no( tmpLnNo );

        get_next_token( cur );

        if ( cur >= _gSrcEnd ) return 0;

        if ( *cur == '(' )
        {
            ++level;
        }
        else
        if ( *cur == ')' )
        {
            if ( level == 0 )
            {
                cur = savedPos;
                restore_line_no( tmpLnNo );

                return size_t(cur-start);
            }

            --level;

            if ( level == 0 )
            {
                ++cur;

                // QUICK-HACK::to easily handle function prototypes ,
                // it works, besause theoretically there should
                // be no cast-expressions in non-implementation
                // scope (e.g. "time( (long*)(ptr+1) )" should not
                // appear in the declarations, thus it is most likelly
                // for the ")(" fragment to be within a function
                // prototype in the declarations scope

                if ( *cur == '(' )
                {
                    ++level;
                    continue;
                }

                else return size_t(cur-start);
            }
        }
        else
        {
            if ( level == 0 )
            {
                cur = savedPos;
                restore_line_no( tmpLnNo );

                return size_t(cur-start);
            }
        }

    } while(1);
}

// returns 0, if end of source reached
static inline bool skip_imp_block( char*& cur )
{
    while( *cur != '{' && cur < _gSrcEnd )
    {
        skip_token( cur );
        if ( !get_next_token( cur ) ) return FALSE;
    }

    while( *cur != '}' && cur < _gSrcEnd )
    {
        skip_token( cur );
        if ( !get_next_token( cur ) ) return FALSE;
    }

    ++cur;

    return TRUE;
}

static bool is_class_token( char*& cur )
{
    // FIXME:: the below mess should be cleaned in it's entirely

    if ( *cur == 'i' )
        if ( *(cur+1) == 'n' )

            return cmp_tokens_fast( cur, "interface", 9 );

    if ( *cur == 'c' )
        if ( *(cur+1) == 'l' )

            return cmp_tokens_fast( cur, "class", 5 );

    if ( *cur == 's' )
        if ( *(cur+1) == 't' )

            return cmp_tokens_fast( cur, "struct", 6 );

    if ( *cur == 'u' )
        if ( *(cur+1) == 'n' )

            return cmp_tokens_fast( cur, "union", 5 );

    return FALSE;
}

inline static bool is_forward_decl( char* cur )
{
    do
    {
        switch( *cur )
        {
            case ':' : return FALSE;
            case '{' : return FALSE;
            case '(' : return FALSE;

            case ';' : return TRUE;

            default : break;
        };

        ++cur;

    } while (cur < _gSrcEnd); // prevent running out of bounds

    return FALSE;
}

inline static bool is_function( char* cur, bool& isAMacro )
{
    isAMacro = FALSE;

    int tmpLnNo;
    store_line_no( tmpLnNo );

    // NOTE:: comments and quoted strings are not checked here

    // first,check for "single-line hanginging macros" like:
    // ___UNICODE
    //

    char* eol = cur;
    skip_to_eol( eol );

    skip_token( cur );
    get_next_token( cur );

    if ( cur > eol )
    {
        isAMacro = TRUE;
        restore_line_no( tmpLnNo );

        return TRUE;
    }

    // it's not a macro, go to the begining of arg. list

    do
    {
        // if bracket found, it's a function or a begining
        // of some macro
        if ( *cur == '(' )
        {
            restore_line_no( tmpLnNo );
            return TRUE;
        }

        // end of statement found without any brackets in it
        // - it cannot be a function

        if ( *cur == ';' )
        {
            restore_line_no( tmpLnNo );
            return FALSE;
        }

        ++cur;

    } while( cur < _gSrcEnd);

    isAMacro = 1;
    restore_line_no( tmpLnNo );

    return FALSE;
}

// upon return the cursor is positioned after the
// terminating curly brace

static inline void skip_scope_block( char*& cur )
{
    size_t level = 0;

    for( ; cur < _gSrcEnd ; ++cur )

        switch( *cur )
        {
            case '/' : skip_comments( cur );
                       --cur;
                       continue;
            case '"' : skip_quoted_string( cur );
                       --cur;
                       continue;

            case '{' : ++level;
                       continue;

            case '}'  :--level;
                       if ( level == 0 )
                       {
                           ++cur; // skip final closing curly brace
                           return;
                       }

            case 10 : ++_gLineNo; continue;

            default : continue;
        };
}

// moves tokens like '*' '**', '***', '&' from the name
// to the type

static void arrange_indirection_tokens_between( string& type,
                                                string& identifier )
{
    // TBD:: FIXME:: return value of operators !

    while ( identifier[0] == '*' ||
            identifier[0] == '&'
          )
    {
        type += identifier[0];
        identifier.erase(0,1);

        if ( !identifier.length() ) return;
    }
}


// the only function where multi-lang keyword map is accessed

static bool is_keyword( char* cur )
{
    size_t len = get_token_len( cur );

    // put a terminating zero after the given token
    char tmp = *(cur + len);
    *(cur+len) = '\0';

    KeywordMapT::iterator i;

    i = __gMultiLangMap.find( cur );

    // restore original character suppresed by terminating zero
    *(cur + len) = tmp;

    return i == __gMultiLangMap.end() ? false : true;
}

static inline void get_string_between( char* start, char* end,
                                       string* pStr )
{
    char saved = *end;

    *end  = '\0';
    *pStr = start;
    *end  = saved;
}

static char* set_comment_text( string& text, char* start )
{
    char* end = start;

    // to avoid poluting the queue with this comment
    _gLastSuppresedComment = start;

    skip_comments( end );

    if ( *(end-1) == '/' )
        end -= 2;

    start += 2;

    // skip multiple leading '/''s or '*''s
    while( *start == '/' && start < end ) ++start;
    while( *start == '*' && start < end ) ++start;

    get_string_between( start, end, &text );

    return end;
}

/***** Implementation for class CJSourceParser *****/

CJSourceParser::CJSourceParser( bool collectCommnets, bool collectMacros )
    : mpStart(0),
      mpEnd(0),
      mpCurCtx( 0 ),
      mCommentsOn( collectCommnets ),
      mMacrosOn  ( collectMacros )
{
    check_keyword_map();
}

spFile* CJSourceParser::Parse( char* start, char* end )
{
    // set up state variables
    mCurVis       = SP_VIS_PRIVATE;

    spFile* pTopCtx = new spFile();
    mpCurCtx        = pTopCtx;

    mIsVirtual    = 0;
    mIsTemplate   = 0;
    mNestingLevel = 0;

    cur = start;

    mpStart = start;
    mpEnd   = end;

    _gSrcEnd   = mpEnd; // let all the C-functions "smell" the end of file
    _gSrcStart = start;

    _gLineNo   = 0;

    clear_commets_queue();

    // main parsing loop

    do
    {
        if ( !get_next_token( cur ) )
            // end of source reached
            return pTopCtx;

        if ( memcmp( cur, "ScriptSection( const string&",
                     strlen( "ScriptSection( const string&" )
                   ) == 0
            )
        {
            int o = 0;
            ++o;
        }

        switch (*cur)
        {
            case '#' :
                {
                    AddMacroNode( cur );
                    continue;
                }

            case ':' :
                {
                    skip_token( cur );
                    continue;
                }

            case ';' :
                {
                    skip_token( cur );
                    continue;
                }

            case ')' :
                {
                    skip_token( cur );
                    continue;
                }

            case '=' :
                {
                    skip_token( cur );
                    continue;
                }

            default: break;
        }

        // 'const' is a part of the return type, not a keyword here
        if ( strncmp(cur, "const", 5) != 0 && is_keyword( cur ) )
        {
            // parses, token, if token identifies
            // the container context (e.g. class/namespace)
            // the corresponding context object is created
            // and set as current context

            ParseKeyword( cur );
            continue;
        }

        if ( *cur >= '0' && *cur <= '9' )
        {
            skip_token( cur );
            continue;
        }

        if ( *cur == '}' )
        {
            if ( mCurCtxType != SP_CTX_CLASS )
            {
                // FOR NOW:: disable the below assertion

                // DBG:: unexpected closing-bracket found
                //ASSERT(0);

                skip_token( cur ); // just skip it
                continue;
            }

            if ( mpCurCtx->GetType() == SP_CTX_CLASS )
            {
                int curOfs = ( (cur+1) - _gSrcStart );

                mpCurCtx->mContextLength = ( curOfs - mpCurCtx->mSrcOffset );
            }

            --mNestingLevel;

            // terminate operation/class/namespace context
            // TBD:: check if it's really this type of context

            wxASSERT( mpCurCtx );
            mpCurCtx = mpCurCtx->GetOutterContext();
            wxASSERT( mpCurCtx );

            if ( mNestingLevel == 0 )
            {

                mCurCtxType = SP_CTX_FILE;

                // not-nested class delclaration finished,
                // rest template flag in any case
                mIsTemplate = 0;
            }

            skip_token( cur );
            continue;
        }

        bool isAMacro = 0;

        if ( is_function( cur, isAMacro ) )
        {
            if ( isAMacro )
            {
                skip_token( cur );
                continue;
            }

            char* savedPos = cur;

            int tmpLnNo;
            store_line_no( tmpLnNo );

            isAMacro = FALSE;

            if ( !ParseNameAndRetVal( cur, isAMacro ) )
            {
                if ( !isAMacro )
                {
                    cur = savedPos;
                    SkipFunction( cur );
                }
                continue;
            }

            if ( !ParseArguments( cur ) )
            {
                // failure while parsing arguments,
                // remove enclosing operation context

                spContext* pFailed = mpCurCtx;
                mpCurCtx = mpCurCtx->GetOutterContext();
                mpCurCtx->RemoveChild( pFailed );

                skip_to_eol( cur );
                //cur = savedPos;
            }
            else
            {
                // otherwise, successfully close operation context:

                clear_commets_queue();

                SkipFunctionBody( cur );

                mpCurCtx = mpCurCtx->GetOutterContext();

                // DBG::
                wxASSERT( mpCurCtx );

            }
        }
        else // otherwise it's declaration of a variable;
        {
            // now, the cursor point to the end of statement (';' token)

            if ( mCurCtxType != SP_CTX_CLASS )
            {
                // non-class members are ignored

                skip_token( cur ); // skip the end of statement
                continue;
            }

            ParseMemberVar( cur );
        }

    } while( 1 );
}

void CJSourceParser::AttachComments( spContext& ctx, char* cur )
{
    if ( !mCommentsOn ) return;

    MCommentListT& lst = ctx.GetCommentList();

    char* prevComEnd = 0;

    int tmpLnNo;
    store_line_no( tmpLnNo );

    // attach comments which were found before the given context

    for( int i = 0; i != _gCQSize; ++i )
    {
        spComment* pComment = new spComment();
        lst.push_back( pComment );

        // find the end of comment
        char* start = _gCommentsQueue[i];

        pComment->mIsMultiline = ( *(start+1) == '*' );

        // first comment in the queue and multiline
        // comments are always treated as a begining
        // of the new paragraph in the comment text

        if ( i == 0 )

            pComment->mStartsPar = TRUE;
        else
        if ( pComment->mIsMultiline )

            pComment->mStartsPar = TRUE;
        else
        {
            // find out wheather there is a new-line
            // between to adjecent comments


            char* prevLine = start;
            skip_to_prev_line(prevLine);

            if ( prevLine >= prevComEnd )

                pComment->mStartsPar = TRUE;
            else
                pComment->mStartsPar = FALSE;
        }

        prevComEnd = set_comment_text( pComment->mText, start );
    }


    // attach comments which are at the end of the line
    // of the given context (if any)

    if ( skip_to_next_comment_in_the_line( cur ) )
    {
        spComment* pComment = new spComment();
        lst.push_back( pComment );

        set_comment_text( pComment->mText, cur );

        pComment->mStartsPar = 1;
        pComment->mIsMultiline = ( *(cur+1) == '*' );

        // mark this comment, so that it would not
        // get in the comments list of the next context
        _gLastSuppresedComment = cur;
    }

    restore_line_no( tmpLnNo );

    clear_commets_queue();
}

void CJSourceParser::AddMacroNode( char*& cur )
{
    char* start = cur;

    int lineNo = get_line_no();

    skip_preprocessor_dir( cur );

    int tmpLnNo;
    store_line_no( tmpLnNo );

    if ( !mMacrosOn ) return;

    spPreprocessorLine* pPL = new spPreprocessorLine();
    pPL->mSrcLineNo = lineNo;

    AttachComments( *pPL, cur );

    get_string_between( start, cur, &pPL->mLine );

    ++start; // skip '#'
    get_next_token( start );

    pPL->mDefType = SP_PREP_DEF_OTHER;

    // if we found a definition or redefinition,
    // determine the type exactly and assign
    // a name to the context

    if ( *start == 'd' )
    {
        if ( cmp_tokens_fast( start, "define", 6 ) )
        {
            char* tok = start+6;

            get_next_token( tok );

            pPL->mName = get_token_str( tok );

            skip_token( tok );
            get_next_token( tok);


            if ( tok > cur )
                pPL->mDefType = SP_PREP_DEF_DEFINE_SYMBOL;
            else
                pPL->mDefType = SP_PREP_DEF_REDEFINE_SYMBOL;
        }
    }
    else if ( *start == 'i' )
    {
        if ( cmp_tokens_fast( start, "include", 7 ) )
        {
            pPL->mDefType = SP_PREP_DEF_INCLUDE_FILE;
        }
        else if ( *++start == 'f' )
        {
            // either "#if" or "#ifdef"
            cur = start;
            skip_token( cur );
            get_next_token( cur );

            string condition = get_token_str( cur );

            // currently, everything except '0' is true
            if ( condition == "0" ) {
                // skip until the following else or enif
                while ( cur < _gSrcEnd ) {
                    skip_to_eol( cur );
                    skip_eol( cur );

                    get_next_token( cur );
                    if ( *cur++ == '#' && *cur == 'e' )
                        break;
                }
            }

            // TODO parse the condition...
        }
    }
    else if ( cmp_tokens_fast( start, "else", 4 ) )
    {
        // skip until "#endif"
        while ( cur < _gSrcEnd ) {
            skip_to_eol( cur );
            skip_eol( cur );

            get_next_token( cur );
            if ( *cur++ == '#' && cmp_tokens_fast( cur, "endif", 5 ) )
                break;
        }
    }

    mpCurCtx->AddMember( pPL );

    skip_to_eol( cur );
    skip_eol( cur );

    restore_line_no( tmpLnNo );

    clear_commets_queue();
}

void CJSourceParser::ParseKeyword( char*& cur )
{
    // analyze token, which identifies the begining of a new context

    if ( CheckVisibilty( cur ) )
    {
        skip_token( cur );
        return;
    }

    if ( is_class_token( cur ) )
    {
        if ( is_forward_decl( cur ) )
        {
            // forward declarations are ignored;
            skip_token( cur );
            return;
        }

        if ( mNestingLevel == 0 )
        {
            // change context form global class context
            mCurCtxType = SP_CTX_CLASS;
        }

        ++mNestingLevel;

        // add information about new class (name, inheritance, etc)
        AddClassNode( cur );

        // the default visiblity for class members is 'private'
        mCurVis = SP_VIS_PRIVATE;

        return;
    }

    size_t len = get_token_len( cur );

    if ( cmp_tokens_fast( cur, "typedef", len  ) )
    {
        skip_token(cur);
        get_next_token(cur);

        if ( cmp_tokens_fast( cur, "struct", len ) ||
             cmp_tokens_fast( cur, "union",  len ) ||
             cmp_tokens_fast( cur, "class",  len )
           )
        {
            if ( mNestingLevel == 0 )
            {
                // change context form global class context
                mCurCtxType = SP_CTX_CLASS;
            }

            ++mNestingLevel;

            // add information about new class (name, inheritance, etc)
            AddClassNode( cur );

            // the default visiblity for class members is 'private'
            mCurVis = SP_VIS_PRIVATE;

            return;

            // FOR NOW:: typedef struct, etc are also ignored
            //skip_scope_block( cur );
        }

        if ( cmp_tokens_fast( cur, "enum", len  ) )
        {
            AddEnumNode( cur );
            return;
        }

        AddTypeDefNode( cur );

        return;
    }

    if ( cmp_tokens_fast( cur, "enum", len ) )
    {
        AddEnumNode( cur );
        return;
    }

    if ( cmp_tokens_fast( cur, "extern", len ) )
    {
        // extern's are ignored (both extern "C" and extern vars)
        while ( *cur != '{' &&
                *cur != ';' )
        {
            skip_token( cur );
            get_next_token( cur );
        }
        return;

    }
    if ( cmp_tokens_fast( cur, "enum", len ) )
    {
        // enumeration blocks are ignored

        skip_scope_block( cur );

        get_next_token( cur );
        skip_token( cur ); // skip ';' token;
        return;
    }

    if ( cmp_tokens_fast( cur, "package", len  ) )
    {
        // packages are ignored
        skip_statement( cur );
        return;
    };

    if ( cmp_tokens_fast( cur, "import", len  ) )
    {
        // import statements are ignored
        skip_statement( cur );
        return;
    }

    if ( cmp_tokens_fast( cur, "virtual", len  ) )
    {
        // probably the virtual method is in front of us;
        mIsVirtual = 1;
        skip_token( cur );
        return;
    }

    if ( cmp_tokens_fast( cur, "template", len  ) )
    {
        mIsTemplate = 1;
        skip_tempalate_statement( cur );
        return;
    }

    if ( cmp_tokens_fast( cur, "friend", len  ) )
    {
        skip_statement( cur );
        return;
    }

    // ingnore "unsigificant" tokens (i.e. which do not
    // affect the current parsing context)

    skip_token( cur );
}

bool CJSourceParser::ParseNameAndRetVal( char*& cur, bool& isAMacro )
{
    isAMacro = FALSE;

    // FOR NOW:: all functions in the global
    //           scope are ignored

    int lineNo = get_line_no();

    char* start = cur;

    bool isVirtual = false;
    while( *cur != '(' )
    {
        if ( get_token_str( cur ) == "virtual" )
            isVirtual = true;

        skip_token( cur );
        if ( !get_next_token( cur ) ) return FALSE;
    }

    char* bracketPos = cur;
    char* savedPos   = cur + 1;

    int tmpLnNo;
    store_line_no( tmpLnNo );

    // skip gap between function name and start of paramters list
    while ( *(cur-1) == ' ' )
        --cur;

    // check if it's not a macro, and let plugin handle it, if so

    if ( mpPlugin )
    {
        skip_token_back( cur );

        char* tmp = cur;

        if ( mpPlugin->CanUnderstandContext( tmp, _gSrcEnd, mpCurCtx ) )
        {
            cur = tmp;

            mpPlugin->ParseContext( _gSrcStart, cur, _gSrcEnd, mpCurCtx );

            isAMacro = TRUE;

            return FALSE;
        }
    }

    spOperation* pOp = new spOperation();

    pOp->mSrcLineNo    = lineNo;
    pOp->mSrcOffset    = int( start - _gSrcStart );
    pOp->mHeaderLength = int( bracketPos - start );
    if ( mpCurCtx->GetContextType() == SP_CTX_CLASS )
        pOp->mScope = mpCurCtx->mName;

    mpCurCtx->AddMember( pOp );
    pOp->mVisibility = mCurVis;
    pOp->mIsVirtual = isVirtual;

    // add comments about operation
    AttachComments( *pOp, cur );

    // go backwards to method name
    skip_token_back( cur );

    pOp->mName = get_token_str( cur );

    // checker whether it's not an operator
    char chFirst = *pOp->mName.c_str();
    if ( !isalpha(chFirst) && chFirst != '_' && chFirst != '~' ) {
        // skip 'operator'
        skip_next_token_back( cur );
        skip_token_back( cur );

        string lastToken = get_token_str( cur );
        if ( lastToken == "operator" ) {
            lastToken += pOp->mName;
            pOp->mName = lastToken;
        }
        else {
            // ok, it wasn't an operator after all
            skip_token( cur );
        }
    }
    else if ( pOp->mName == "operator" ) {
        skip_token( cur );
        get_next_token( cur );
        string oper = get_token_str( cur );

        pOp->mName += oper;
    }

    // go backwards to method return type
    skip_next_token_back( cur );

    if ( cur >= start )
    {
        string rettype = string( start, size_t( cur-start ) );
        rettype.Replace("WXDLLEXPORT ", ""); // FIXME just for now...
        pOp->mRetType = rettype;
    }

    arrange_indirection_tokens_between( pOp->mRetType, pOp->mName );

    cur = savedPos;
    restore_line_no( tmpLnNo );

    // now, enter operation context
    mpCurCtx = pOp;

    return TRUE;
}

bool CJSourceParser::ParseArguments( char*& cur )
{
    // DANGER-MACROS::

    // now cursor position is right after the first opening bracket
    // of the function declaration

    char* blocks    [16]; // used exclusivelly for iterative "lean out"
                          // of macros and misc. not-obviouse grammar
                          // (dirty,, but we cannot do it very nice,
                          //  we're not preprocessor-free C/C++ code)
    int   blockSizes[16];

    do
    {
        size_t blocksSkipped = 0;

        get_next_token( cur );

        bool first_blk = 1;

        while( *cur != ')' && *cur != ',' )
        {
            blocks[blocksSkipped] = cur;

            if ( first_blk )
            {
                char* prev = cur;
                skip_token( cur );

                blockSizes[blocksSkipped] = size_t(cur-prev);

                first_blk = 0;
            }
            else
                blockSizes[blocksSkipped] = skip_block( cur );

            get_next_token( cur );
            ++blocksSkipped;
        }


        if ( blocksSkipped == 1 )
        {
            // check if the empty arg. list stressed with "void" inside
            if ( cmp_tokens_fast( blocks[0] , "void", 4 ) )
            {
                cur++;  // skip ')'

                break;
            }

            // FIXME:: TBD:: K&R-style function declarations!

            // if only one block enclosed, than it's probably
            // some macro, there should be at least two blocks,
            // one for argument type and another for it's identifier
            return FALSE;
        }

        if ( blocksSkipped == 0 )
        {
            if ( *cur == 10 ) ++_gLineNo;
            ++cur; // skip ')'

            break; // function without paramters
        }

        // we should be in the operation context now
        spOperation* pOp = (spOperation*)mpCurCtx;

        spParameter* pPar = new spParameter();

        pOp->AddMember( pPar );
        // FOR NOW:: line number is not exact if argument list is mutiline
        pPar->mSrcLineNo = get_line_no();

        size_t nameBlock = blocksSkipped - 1;
        size_t typeBlock = nameBlock - 1;

        // check if default values present
        if ( *blocks[typeBlock] == '=' )
        {
            // expressions like "int = 5" are ignored,
            // since name for paramters is required
            if ( blocksSkipped == 3 )
            {
                if ( *cur == ')' )
                {
                    ++cur;
                    break;
                }
            else
                continue;
            }

            pPar->mInitVal = string( blocks[nameBlock], blockSizes[nameBlock] );

            nameBlock = nameBlock - 2; // skip '=' token and default value block
            typeBlock = nameBlock - 1;
        }

        // attach comments about the parameter
        AttachComments( *pPar, blocks[nameBlock] );

        // retrieve argument name
        pPar->mName = string( blocks[nameBlock], blockSizes[nameBlock] );

        // retreive argument type

        size_t len = blockSizes[ typeBlock ];
        len = size_t ( (blocks[ typeBlock ] + len) - blocks[ 0 ] );

        pPar->mType = string( blocks[0], len );

        arrange_indirection_tokens_between( pPar->mType, pPar->mName );

        if ( *cur == ')' )
        {
            ++cur;
            break;
        }

        ++cur; // skip comma
        get_next_token(cur);

    } while(1);

    // skip possible whitespace between ')' and following "const"
    while ( isspace(*cur) )
        cur++;

    // check if it was really a function not a macro,
    // if so, than it should be terminated with semicolon ';'
    // or opening implemenetaton bracket '{'

    char* tok = cur;

    int tmpLnNo;
    store_line_no( tmpLnNo );

    do
    {
        if ( *tok == '{' || *tok == ';' )
        {
            restore_line_no(tmpLnNo);
            return TRUE;
        }

        // check for unexpected tokens
        if ( *tok == '=' || *tok == '0' )
        {
            skip_token(tok);
            if ( !get_next_token(tok) ) return FALSE;
            continue;
        }

        if ( *tok == '}' ) return FALSE;

        // if initialization list found
        if ( *tok == ':' )
        {
            restore_line_no(tmpLnNo);
            return TRUE;
        }

        if ( cmp_tokens_fast( tok, "const", 5 ) )
        {
            ((spOperation*)mpCurCtx)->mIsConstant = true;

            skip_token(tok);
            if ( !get_next_token(tok) ) return FALSE;
            continue;
        }

        if ( CheckVisibilty( tok ) ) return FALSE;

        // if next context found
        if ( is_keyword( tok ) ) return FALSE;

        skip_token(tok);
        if ( !get_next_token(tok) ) return FALSE;

    } while(1);

    return TRUE;
}

void CJSourceParser::ParseMemberVar( char*& cur )
{
    MMemberListT& members = mpCurCtx->GetMembers();

    bool firstMember = 1;

    size_t first = 0;

    string type;

    // jump to the end of statement
    // and start collecting same-type varibles
    // back-to-front towards the type identifier

    skip_statement( cur );
    char* savedPos = cur;

    int tmpLnNo;
    store_line_no( tmpLnNo );

    --cur; // rewind back to ';'

    do
    {
        spAttribute* pAttr = new spAttribute();
        // FOR NOW:: line not is not exact, if member declaration is multiline
        pAttr->mSrcLineNo = get_line_no();

        mpCurCtx->AddMember( pAttr );
        pAttr->mVisibility = mCurVis;

        pAttr->mIsConstant = 0;

        if ( firstMember )
        {
            firstMember = 0;
            first = members.size() - 1;;
        }

        skip_token_back( cur );

        // attach comments about the attribute
        AttachComments( *pAttr, cur );

        pAttr->mName = get_token_str( cur );

        // guessing that this going to be variable type
        skip_next_token_back( cur );
        skip_token_back( cur );

        pAttr->mType = get_token_str( cur );

        // if comma, than variable list continues
        // otherwise the variable type reached - stop

        if ( *cur == '=' )
        {
            // yes, we've mistaken, it was not a identifier,
            // but it's default value
            pAttr->mInitVal =
                pAttr->mName;

            // skip default value and '=' symbol
            skip_next_token_back( cur );
            skip_token_back( cur );

            pAttr->mName = get_token_str( cur );

            skip_next_token_back( cur );
            skip_token_back( cur );
        }

        if ( *cur != ',' )
        {
            type = get_token_str( cur );
            break;
        }

    } while(1);

    first = 0;

    // set up types for all collected (same-type) attributes;
    while ( first != members.size() - 1 )
    {
        spAttribute* pAttr = members[first++]->CastToAttribute();
        if ( !pAttr )
            continue;

        if ( !pAttr->mType )
            pAttr->mType = type;
        pAttr->mVisibility = mCurVis;

        if ( !!pAttr->mName )
            arrange_indirection_tokens_between( pAttr->mType, pAttr->mName );
    }

    cur = savedPos;
    restore_line_no( tmpLnNo );

    clear_commets_queue();


}

void CJSourceParser::SkipFunction( char*& cur )
{
    while ( *cur != '(' && cur < _gSrcEnd )
    {
        if (*cur == 10 ) ++_gLineNo;
        ++cur;
    }

    skip_next_token_back( cur ); // go back and skip function identifier
    skip_token_back( cur );      // go back and skip return type

    skip_block( cur );           // now, go ahead and skip whole declaration

    SkipFunctionBody( cur );

}

void CJSourceParser::SkipFunctionBody( char*& cur )
{
    // FIXME:: check for comments and quoted stirngs here

    bool hasDefinition = FALSE;

    while( *cur != '{' && *cur != ';' )
    {
        if (*cur == 10 ) ++_gLineNo;
        ++cur;
    }

    if ( *cur == ';' )
    {
        ++cur;
    }
    else
    {
        hasDefinition = TRUE;

        skip_scope_block( cur ); // skip the whole imp.
    }

    if ( mpCurCtx->GetType() == SP_CTX_OPERATION )
    {
        spOperation& op = *((spOperation*)mpCurCtx);

        int curOfs = int ( cur - _gSrcStart );

        op.mContextLength = curOfs - mpCurCtx->mSrcOffset;

        op.mHasDefinition = hasDefinition;

        // separate scope resolution token from the name of operation

        for( size_t i = 0; i != op.mName.length(); ++i )
        {
            if ( op.mName[i] == ':' && op.mName[i+1] == ':' )
            {
                string unscoped( op.mName, i+2, op.mName.length() - ( i + 2 ) );

                op.mScope = string( op.mName, 0, i );

                op.mName = unscoped;

                break;
            }
        }
    }
}

bool CJSourceParser::CheckVisibilty( char*& cur )
{
    size_t len = get_token_len( cur );

    if ( cmp_tokens_fast( cur, "public:", len ) )
    {
        mCurVis = SP_VIS_PUBLIC;
        return TRUE;
    }

    if ( cmp_tokens_fast( cur, "protected:", len ) )
    {
        mCurVis = SP_VIS_PROTECTED;
        return TRUE;
    }

    if ( cmp_tokens_fast( cur, "private:", len ) )
    {
        mCurVis = SP_VIS_PRIVATE;
        return TRUE;
    }

    return FALSE;
}

void CJSourceParser::AddClassNode( char*& cur )
{
    char* ctxStart = cur;

    string classkeyword = get_token_str( cur );

    skip_token( cur ); // skip 'class' keyword
    if ( !get_next_token( cur ) ) return;

    // in C++
    if ( *cur == ':' )
    {
        skip_token( cur );
        get_next_token( cur );
    }

    // by default all class members are private
    mCurVis = SP_VIS_PRIVATE;

    spClass* pClass = new spClass();
    if ( classkeyword == "class" )
        pClass->mClassSubType = SP_CLTYPE_CLASS;
    else if ( classkeyword == "struct" ) {
        pClass->mClassSubType = SP_CLTYPE_STRUCTURE;

        mCurVis = SP_VIS_PUBLIC;
    }
    else if ( classkeyword == "union" ) {
        pClass->mClassSubType = SP_CLTYPE_UNION;

        mCurVis = SP_VIS_PUBLIC;
    }
    else if ( classkeyword == "interface" )
        pClass->mClassSubType = SP_CLTYPE_INTERFACE;
    else {
        pClass->mClassSubType = SP_CLTYPE_INVALID;

        wxFAIL_MSG("unknown class keyword");
    }

    mpCurCtx->AddMember( pClass );

    // attach comments about the class
    AttachComments( *pClass, cur );

    pClass->mSrcLineNo = get_line_no();

    pClass->mSrcOffset = int( ctxStart - _gSrcStart );

    char* nameTok = cur;
    pClass->mName = get_token_str( cur );

    bool isDerived = 0;

    // DANGER-MACROS::

    do
    {
        skip_token( cur );
        if ( !get_next_token( cur ) ) return;

        if ( *cur == ':' )
        {
            isDerived = 1;

            char* tok = cur;

            int tmpLn;
            store_line_no( tmpLn );

            skip_next_token_back( tok );
            skip_token_back( tok );

            restore_line_no( tmpLn );

            // class name should precend ':' colon, thus
            // the one which was captured before was
            // proablty something else (like __dllexport MyClass : ... )

            if ( nameTok != tok )
            {
                pClass->mName = get_token_str( tok );
            }

        }

        if ( *cur == '{' )
            break;

        if ( *cur == ',' )
            continue;

        size_t len = get_token_len( cur );

        // skip neglectable C++ modifieres
        if ( cmp_tokens_fast( cur, "public", len ) )
            continue;

        if ( cmp_tokens_fast( cur, "protected", len ) )
            continue;

        if ( cmp_tokens_fast( cur, "private", len ) )
            continue;

        if ( cmp_tokens_fast( cur, "virtual", len ) )
            continue;

        // skip neglectable JAVA modifieres

        if ( cmp_tokens_fast( cur, "extends", len ) )
        {
            isDerived = 1;
            continue;
        }

        if ( cmp_tokens_fast( cur, "implements", len ) )
        {
            isDerived = 1;
            continue;
        }

        // all we need to know is superclass or interface

        char* tok = cur;
        int tmpLn;
        store_line_no( tmpLn );

        skip_token(tok);
        get_next_token(tok);

        restore_line_no( tmpLn );

        if ( *tok != ':' && *cur != ':' )

            pClass->mSuperClassNames.push_back( string( cur, len ) );

    } while(1);

    if ( !isDerived )
    {
        int tmpLn;
        store_line_no( tmpLn );

        while ( pClass->mSuperClassNames.size() )

            pClass->mSuperClassNames.erase( &pClass->mSuperClassNames[0] );

        char* tok = cur;

        // some non-obviouse token was following "class" keyword -
        // we've confused it with class name - thus now we're reverting this mistake

        skip_next_token_back( tok );
        skip_token_back( tok );

        pClass->mName = get_token_str( tok );

        restore_line_no( tmpLn );
    }


    ++cur; // skip opening curly brace

    pClass->mHeaderLength = ( cur - ctxStart );

    // now, enter the class context
    mpCurCtx = pClass;

    clear_commets_queue();
}

void CJSourceParser::AddEnumNode( char*& cur )
{
    // now the cursor is at "enum" keyword
    char* start = cur;

    spEnumeration* pEnum = new spEnumeration();
    mpCurCtx->AddMember( pEnum );

    pEnum->mSrcLineNo = get_line_no();


    AttachComments( *pEnum, cur );

    skip_token( cur );
    if ( !get_next_token( cur ) ) return;

    // check if enumeration has got it's identifier
    if ( *cur != '{' )
    {
        pEnum->mName = get_token_str( cur );
    }

    if ( !skip_imp_block( cur ) ) return;

    get_string_between( start, cur, &pEnum->mEnumContent );

    if ( get_next_token(cur) )
    {
        // check if the identifier if after the {...} block
        if ( *cur != ';' )

            pEnum->mName = get_token_str( cur );
    }

    clear_commets_queue();
}

void CJSourceParser::AddTypeDefNode( char*& cur )
{
    // now the cursor at the token next to "typedef" keyword

    if ( !get_next_token(cur) ) return;

    char* start = cur;

    spTypeDef* pTDef = new spTypeDef();
    mpCurCtx->AddMember( pTDef );

    pTDef->mSrcLineNo = get_line_no();

    AttachComments( *pTDef, cur );

    skip_statement( cur );

    int tmpLnNo;
    store_line_no( tmpLnNo );

    char* tok = cur-1;
    skip_next_token_back( tok );

    char* nameEnd = tok;

    skip_token_back( tok );

    char* nameStart = tok;

    skip_next_token_back( tok );

    char* typeEnd = tok;

    // check if it's function prototype
    if ( *nameStart == ')' )
    {
        typeEnd = nameStart+1;

        // skip argument list
        while ( *nameStart != '(' ) --nameStart;

        // skip to function type definition
        while ( *nameStart != ')' ) --nameStart;

        skip_next_token_back( nameStart );

        nameEnd = nameStart;

        skip_token_back( nameStart );

        if ( *nameStart == '*' ) ++nameStart;
    }

    get_string_between( start, typeEnd, &pTDef->mOriginalType );

    get_string_between( nameStart, nameEnd, &pTDef->mName );

    clear_commets_queue();

    restore_line_no( tmpLnNo );
}
