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

#ifndef __SOURCEPAINTER_G__
#define __SOURCEPAINTER_G__

#ifndef ASSERT
#define ASSERT(x) if (!(x)) throw
#endif

#if defined( wxUSE_TEMPLATE_STL )

    #include <vector.h>
    typedef vector<int> SPBlockListT;

    #ifdef WIN32
        #include <bstring.h>
    #else
        #include <strclass.h>
        #include <string.h>
    #endif
#else

    #include "wxstlvec.h"
    #include "wx/string.h"

    typedef WXSTL_VECTOR_SHALLOW_COPY(int) SPBlockListT;

#endif

#include "markup.h" // import MarkupTagsT definition

// "colored" codes for highlighted blocks

#define RANK_BLACK  0 // common source fragments
#define RANK_BLUE   1 // basic types
#define RANK_RED    2 // reserved words
#define RANK_GREEN  3 // comments

// colored block description format :
// int( ( rank << 16 ) | ( source_range_len ) )


// FOR NOW:: no lagnguage-map selection

// source code syntax heighlighter (CPP+JAVA+VB+PASCAL)

class SourcePainter
{
protected:
    wxString        mResultStr;
    SPBlockListT    mBlocks;
    bool            mCollectResultsOn;

    // state variables
    bool            mIsInComment;
    bool            mCommentIsMultiline;
public:

    // assembleResultString == true - instructs painter
    // to collect each chunk of srouce passed to ProcessSource(),
    // so that results cann be futher obtained in a single string
    // instead of vector of block descriptions

    SourcePainter( bool assembleResultString = true );
    virtual ~SourcePainter() {}

    // can be called multiple times (e.g. on each source line)
    void ProcessSource( char* src, int srcLen );

    // method, for manually adjusting state of source painter
    void SetState( bool isInComment,
                   bool commentIsMultiline );

    // reinitializes object - clears results of previouse processing
    void Init( bool assembleResultString = true );

    // generates string of highlighted source for the scipting
    // language given by "tags" argument

    virtual void GetResultString(wxString& result, MarkupTagsT tags);

    // returns vector of block descriptors, see SPBlockListT definition
    // (block descriptors can be used for fast custom highlighted text generation)

    SPBlockListT& GetBlocks();

    // NOTE:: static method
    // returns if the given word is a reserved word or basic type identifier
    static bool IsKeyword( char* word, int wordLen );
};

#endif
