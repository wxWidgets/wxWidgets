/////////////////////////////////////////////////////////////////////////////
// Name:        No names yet.
// Purpose:     Implementation of C++/Java parser
//              compatible with SourceParserBase interface
// Author:      Aleksandras Gluchovas
// Modified by:
// Created:     22/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Aleskandars Gluchovas
// Licence:       wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __CJPARSESR_G__
#define __CJPARSESR_G__

#include "srcparser.h"

#include <memory.h>
#include <stdlib.h>
#include <stdio.h>

#if wxUSE_IOSTREAMH
    #include <iostream.h>
#else
    #include <iostream>
#endif

// class parses given "memory-resident" Java or C++ source code
// and captures information about classes/attrubutes/methods/
// arguments/etc into structures. Conforms with SourceParserBase
// interface requirements.

class CJSourceParser : public SourceParserBase
{
protected:
    // begining of the full-text area of the source file
    wxChar* mpStart;

    // points to first character after the end
    // of teh full-text area
    wxChar* mpEnd;

    // current "privacy level"
    int   mCurVis;

    // current parsing position int full-text area
    wxChar*  m_cur;

    // about the current class
    bool   mIsVirtual;
    bool   mIsTemplate;
    size_t mNestingLevel;

    // context data for which is currently being collected
    spContext* mpCurCtx;

    int mCurCtxType; // type of the current context

    bool mCommentsOn;
    bool mMacrosOn;

protected:

    void AttachComments( spContext& ctx, char* cur );
    void ParseKeyword( char*& cur );
    bool ParseNameAndRetVal( char*& cur, bool& isAMacro );
    bool ParseArguments( char*& cur );
    void ParseMemberVar( char*& cur );
    void SkipFunction( char*& cur );
    void SkipFunctionBody( char*& cur );
    bool CheckVisibilty( char*& cur );

    void AddClassNode( char*& cur );
    void AddMacroNode( char*& cur );
    void AddEnumNode( char*& cur );
    void AddTypeDefNode( char*& cur );

    void DumpOperationInfo( spOperation& info, const wxString& tab, wxSTD ostream& os );
    void DumpClassHeader( spClass& info, wxSTD ostream& os );
    void DumpClassBody( spClass& info, wxSTD ostream& os );

public:

    // NOTE:: discarding of macros or comments improves performance and
    //          decreases memory usage

    CJSourceParser(bool collectCommnets = 1,
                   bool collectMacros   = 1);

    // returns the root-node of the created context tree
    // (user is responsible for releasing it from the heep)
    // "end" should point to the last (character + 1) of the
    // source text

    virtual spFile* Parse( char* start, char* end );
};

// inline'ed helpers used (just info):
/*
static inline void skip_to_eol( char*& cur );
static inline void skip_eol( char*& cur );
static inline bool skip_to_next_comment_in_the_line( char*& cur );
static void skip_to_prev_line( char*& cur );
static inline void skip_comments( char*& cur );
static inline void clear_commets_queue();
static inline void skip_quoted_string( char*& cur );
static inline bool get_next_token( char*& cur );
static inline void skip_preprocessor_dir( char*& cur );
static void skip_token( char*& cur );
static inline size_t get_token_len( char* tok );
static inline bool cmp_tokens( char* tok1, char* tok2 );
static inline bool cmp_tokens_fast( char* tok1, char* tok2, size_t len );
static inline void skip_tempalate_statement( char*& cur );
static inline void skip_statement( char*& cur );
static inline void skip_token_back( char*& cur );
static inline void skip_next_token_back( char*& cur );
static wxString get_token_str( char* cur );
static size_t skip_block( char*& cur );
static inline bool skip_imp_block( char*& cur );
static bool is_class_token( char*& cur );
inline static bool is_forward_decl( char* cur );
inline static bool is_function( char* cur, bool& isAMacro );
static inline void skip_scope_block( char*& cur );
static void arrange_indirection_tokens_between( wxString& type, wxString& identifier );
static bool is_keyword( char* cur );
static inline void get_string_between( char* start, char* end, wxString* pStr );
static char* set_comment_text( wxString& text, char* start );
*/

#endif
