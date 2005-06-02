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

#if defined( wxUSE_TEMPLATE_STL )
#  include <map>
#else
#  include "wxstlac.h"
#endif

#include "sourcepainter.h"

const int MAX_KEYWORD_LEN = 16;

struct KeywordT
{
    char  keyWord[MAX_KEYWORD_LEN];
    int   rank;
};

// source fragment ranks :

// 0 - nomral text
// 1 - basic types
// 2 - reserved words

// multil-language keywords map

static KeywordT __gKeyWords[] =
{
    { "for", 1 },
    { "FOR", 1 },
    { "For", 1 },

    { "next", 1 },
    { "Next", 1 },
    { "NEXT", 1 },

    { "if", 1 },
    { "If", 1 },
    { "IF", 1 },

    { "then", 1 },
    { "Then", 1 },
    { "THEN", 1 },

    { "else", 1 },
    { "Else", 1 },
    { "ELSE", 1 },

    { "do", 1 },
    { "Do", 1 },
    { "DO", 1 },


    { "break", 1 },
    { "Break", 1 },
    { "BREAK", 1 },

    { "continue", 1 },

    { "goto", 1 },
    { "Goto", 1 },
    { "GOTO", 1 },

    { "switch",  1 },
    { "default", 1 },
    { "case",    1 },

    { "repeat", 1 },
    { "Repeat", 1 },
    { "REPEAT", 1 },

    { "until", 1 },
    { "Until", 1 },
    { "UNTIL", 1 },

    { "return", 1 },
    { "Return", 1 },
    { "RETURN", 1 },

    { "unit",      1 },
    { "Unit",      1 },
    { "UNIT",      1 },

    { "procedure", 1 },
    { "Procedure", 1 },
    { "PROCEDURE", 1 },

    { "function", 1 },
    { "Function", 1 },
    { "FUNCTION", 1 },

    { "begin", 1 },
    { "Begin", 1 },
    { "BEGIN", 1 },

    { "End", 1 },
    { "END", 1 },

    ////////////////////////////////////////////////////

    { "enum",     1 },
    { "static",   1 },
    { "const",    1 },
    { "mutable",  1 },
    { "volatile", 1 },
    { "__asm",    1 },
    { "asm",      1 },

    { "typeid",   1 },
    { "sizeof",   1 },
    { "typeof",   1 },


    { "native",   1 },

    { "#include", 1 },
    { "#define",  1 },
    { "#def",     1 },
    { "#undef",   1 },
    { "#ifdef",   1 },
    { "#ifndef",  1 },
    { "#if",      1 },
    { "#endif",   1 },
    { "#elif",    1 },
    { "#else",    1 },
    { "#pragma",  1 },
    { "#line",    1 },

    { "package",  1 },
    { "import",   1 },
    { "export",   1 },

    ////////////////////////////////////////////////////

    { "dynamic_cast",     1 },
    { "const_cast",       1 },

    //////// some hacks for VB /////////

    { "sub", 1 },
    { "Sub", 1 },
    { "SUB", 1 },
    { "as",  1 },
    { "As",  1 },
    { "AS",  1 },

    /////// data types ///////

    { "int" ,    1 },
    { "integer", 1 },
    { "Integer", 1 },
    { "INTEGER", 1 },

    { "real", 1 },
    { "Real", 1 },
    { "REAL", 1 },

    { "float", 1 },
    { "Float", 1 },
    { "FLOAT", 1 },

    { "char",  1 },
    { "Char",  1 },
    { "CHAR",  1 },

    { "register",   1 },

    { "string", 1 },
    { "String", 1 },
    { "STRING", 1 },

    { "array", 1 },
    { "Array", 1 },
    { "ARRAY", 1 },

    { "packed", 1 },
    { "Packed", 1 },
    { "PACKED", 1 },

    { "property", 1 },
    { "Property", 1 },
    { "PROPERTY", 1 },

    { "unsigned", 1 },

    { "long",   1 },
    { "double", 1 },
    { "short",  1 },
    { "bool",   1 },

    { "longint", 1 },
    { "Longint", 1 },
    { "LONGINT", 1 },

    { "extended", 1 },
    { "Extended", 1 },
    { "EXTENTED", 1 },

    { "pointer", 1 },
    { "Pointer", 1 },
    { "POINTER", 1 },

    { "and",     1 },
    { "And",     1 },
    { "AND",     1 },
    { "or",      1 },
    { "Or",      1 },
    { "OR",      1 },
    { "xor",     1 },
    { "Xor",     1 },
    { "XOR",     1 },

    { "void",       1 },
    { "__stdcall",  1 },
    { "__declspec",    1 },
    { "extern",     1 },
    { "stdcall",    1 },
    { "dllimport",  1 },
    { "dllexport",  1 },
    { "__cdecl",    1 },
    { "cdecl",      1 },
    { "template",   1 },
    { "typedef",    1 },
    { "naked",      1 },

    { "try",        1 },
    { "catch",      1 },
    { "throw",      2 }, // C++
    { "throws",     1 }, // Java


    { "finalize",   1 },

    // "STL-suport"

    { "size_t",     1 },
    { "NPOS",       1 },
    { "vector",     1 },
    { "list",       1 },
    { "map",        1 },
    { "multimap",   1 },

    { "external", 1 },
    { "External", 1 },
    { "EXTERNAL", 1 },

    //////////// meta-information //////////////

    { "virtual", 2 },
    { "Virtual", 2 },

    { "override", 2 },
    { "Override", 2 },

    { "class", 2 },
    { "Class", 2 },
    { "CLASS", 2 },

    { "struct", 2 },
    { "union",  2 },

    { "record", 2 },
    { "Record", 2 },
    { "RECORD", 2 },

    { "form",     1 },
    { "Form",     1 },
    { "FORM",     1 },

    { "namespace", 2 },

    { "interface" , 2 },
    { "abstract",   2 },

    { "Interface" , 2 },
    { "INTERFACE" , 2 },

    { "implementation", 2 },
    { "Implementation", 2 },
    { "IMPLEMENTATION", 2 },

    { "label", 2 },
    { "Label", 2 },
    { "LABEL", 2 },

    { "implements", 2 },

    { "public",    2 },
    { "private",   2 },
    { "protected", 2 },

    { "this", 2 },
    { "This", 2 },
    { "THIS", 2 },

    { "new", 2 },
    { "New", 2 },
    { "NEW", 2 },

    { "delete", 2 },
    { "inline", 2 },

    { "operator",  2 },

    { "Inherited", 2 },
    { "Inherited", 2 },

    { "final", 2 },
    { "implements", 2 },
    { "super", 2 },

    // even more...
    { "java",      2 },
    { "Java",      2 },
    { "JAVA",      2 },
    { "delphi",    2 },
    { "Delphi",    2 },
    { "SmallTalk", 2 },
    { "Smalltalk", 2 },
    { "smalltalk", 2 },
    { "assembler", 2 },
    { "Assembler", 2 },
    { "Basic",     2 },
    { "BASIC",     2 },
    { "basic",     2 },
    { "CORBA",     2 },
    { "COBOL",     2 },
    { "ADA",       2 },
    { "LISP",      2 },

    // just for fun...
    { "life",        2 },
    { "sucks",       2 },
    { "rules",       2 },
    { "Quake",       2 },
    { "QuakeWorld",  2 },
    { "[ag_slammer]",2 },
    { "Aleksandras", 2 },
    { "Gluchovas"  , 2 },
    { "Alex",        2 },
    { "alex",        2 },
    { "aleks",       2 },
    { "aleksas",     3 },
    { "AlexSoft",    2 },
    { "Alexsoft",    2 },
    { "SpringSky",   2 },
    { "SK_Team",     2 },
    { "soften",      2 },
    { "UB40",        2 },
    { "U96",         2 }
};

struct less_c_str
{
    inline bool operator()( char* x, char* y) const
    {     return ( strcmp( x,y ) < 0 );
    }
};

#if defined( wxUSE_TEMPLATE_STL )

    typedef map< char*, char*, less_c_str > KeywordMapT;

#else

    typedef char* CharPtrT;
    typedef WXSTL_MAP( CharPtrT, CharPtrT ,less_c_str) KeywordMapT;

#endif

static KeywordMapT __gMultiLangMap;
static int         __gMapReady = 0;

void check_keyword_map( int WXUNUSED(keywordMapNr) )
{
    if ( !__gMapReady )
    {
        __gMapReady = 1;

        // "make sure" the address of the first member of non-polimorphic class
        // coinsides with the address of the instance

/*
        KeywordT dummy;

        if ( (char*)& dummy != &dummy.keyWord[0] )
            throw;
*/

        int size = sizeof(__gKeyWords) / sizeof( KeywordT );

        for( int i = 0; i != size; ++i )

            __gMultiLangMap.insert(
                    KeywordMapT::value_type( (char*)&__gKeyWords[i],
                                             (char*)&__gKeyWords[i]
                                           )
                                  );
    }
}

int get_rank( char* start, char* end )
{
    // FIXME:: what if end is no longer leagal adress?

    char tmp = *end;
    *end = '\0'; // put temporary terminator

    KeywordMapT::iterator i;

    if (  (i = __gMultiLangMap.find( start ) ) != __gMultiLangMap.end() )
    {
        KeywordT* pKey = (KeywordT*)(*i).second;

        *end = tmp;

        return pKey->rank;
    }
    else
    {
        *end = tmp;
        return 0;
    }
}

static inline void store_range( SPBlockListT& results, int rank, int range_len )
{
    if ( !range_len ) return;

    results.push_back ( ( rank << 16 ) | ( range_len ) );
}


#define STORE_RANGE  store_range( results, cur_rank, cur_range_len ); \
                   cur_rank = cur_range_len = 0;                    \
                     wxUnusedVar( cur_rank );

#define NEXT_CHAR cur_range_len++; \
                  ++cur;           \
                  continue;

static inline int is_alpha( char ch )
{
    return ( (( ch >= '_' ) && ( ch <= 'z' )) ||
             (( ch >= 'A' ) && ( ch <= 'Z' ))
           );
}

  // _       .          .
  // Ziema atEjo netikEtai

static void heighlight_syntax( char* str,   int strLen,
                               SPBlockListT& results, bool& isComment )
{
    bool isMultiline = false;
    char* cur = str;
    char* end = str + strLen;

    int cur_rank      = ( isComment == 1 ) ? RANK_GREEN : RANK_BLACK;
    int cur_range_len = 0;

    while (    cur != end )
    {
        int has_next = ( cur+1 != end );

        if ( isComment )
        {
            if ( *cur == '*' )
                if ( has_next && *(cur+1) == '/' )
                {
                     // turn off multiline comment mode
                     cur           += 2;
                     cur_range_len += 2;
                     isComment      = 0;
                     isMultiline    = 0;
                     STORE_RANGE;

                     continue;
                }

            ++cur_range_len;
            ++cur;
            continue;
        }

        /*
        if ( *cur == 10 )
            if ( isComment )
               if ( isMultiline )
        {
             cur_rank      = RANK_GREEN;
             cur_range_len = end - cur;
             STORE_RANGE;
             isComment = 0;
             isMultiline = 0;
             continue;
        }*/

        if ( *cur == '/' )
        {
            if ( has_next )
            {
                 if ( *(cur+1) == '/' )
                 {
                     STORE_RANGE;

                     char* eol = cur;
                     while ( eol < end && *eol != 10 )
                         ++eol;

                     cur_rank      = RANK_GREEN;
                     cur_range_len = eol - cur;
                     cur = eol;
                     STORE_RANGE;

                      continue;
                 }

                 if ( *(cur+1) == '*' )
                 {
                     STORE_RANGE;
                     cur_rank      = RANK_GREEN;
                     cur_range_len = 2;
                     isComment     = 1;
                     cur          += 2;
                     isMultiline   = 1;
                     continue;
                 }
            }

            NEXT_CHAR;
        }

        if (  ( is_alpha( *cur ) || *(cur) == '#' )
              && has_next
           )
        {
            if ( is_alpha( *(cur+1) ) )
            {
                char* start = cur;
                cur += 2;

                while ( cur != end && is_alpha(*cur) ) ++cur;

                int wordRank;

                if ( (wordRank = get_rank( start, cur )) > 0 )
                {
                    STORE_RANGE;

                    store_range( results, wordRank, int(cur-start) );
                    cur_rank = cur_range_len = 0;
                    continue;
                }

                cur_range_len += ( cur-start );
                continue;
            }
            else
                NEXT_CHAR;
        }

        NEXT_CHAR;
    }

    if ( cur_range_len > 0 )  STORE_RANGE;

      wxUnusedVar(isMultiline);
}

/***** Implementation for class SourcePainter ******/

SourcePainter::SourcePainter( bool assembleResultString )
    : mCollectResultsOn( assembleResultString ),
      mIsInComment( false ),
      mCommentIsMultiline( false )
{
    check_keyword_map(0);
}

void SourcePainter::ProcessSource( char* src, int srcLen )
{
    // TBD:: multilne state...

    heighlight_syntax( src, srcLen, mBlocks, mIsInComment );

    if ( mCollectResultsOn )

        mResultStr += wxString( src, srcLen );
}

void SourcePainter::SetState( bool isInComment,
                              bool commentIsMultiline )
{
    mIsInComment        = isInComment;
    mCommentIsMultiline = commentIsMultiline;
}

void SourcePainter::Init(bool assembleResultString)
{
    mIsInComment        = 0;
    mCommentIsMultiline = 0;
    mCollectResultsOn   = assembleResultString;

    mResultStr = wxEmptyString;

    mBlocks.erase( mBlocks.begin(), mBlocks.end() );
}

static int rank_tags_map[] =
{
    TAG_BLACK_FONT,
    TAG_BLUE_FONT,
    TAG_RED_FONT,
    TAG_GREEN_FONT
};

void SourcePainter::GetResultString(wxString& result, MarkupTagsT tags)
{
    // this method works, only if results of processing
    // are collected
    // ASSERT( mCollectResultsOn );
    result = wxEmptyString;

    unsigned pos = 0;

    for( size_t i = 0; i != mBlocks.size(); ++i )
    {
        int desc = mBlocks[i];

        unsigned len  = desc & 0xFFFF;
        int rank = (desc >> 16) & 0xFFFF;

        result += tags[ rank_tags_map[rank] ].start;

        for( unsigned n = 0; n != len; ++n )
        {
            result += mResultStr[(unsigned int)(pos+n)];
        }

        pos += len;

        result += tags[ rank_tags_map[rank] ].end;
    }
}

SPBlockListT& SourcePainter::GetBlocks()
{
    return mBlocks;
}

bool SourcePainter::IsKeyword( char* word, int wordLen )
{
    check_keyword_map(0);

    int rank = get_rank( word, word + wordLen );

    return (  rank == RANK_BLUE || rank == RANK_RED );
}
