/////////////////////////////////////////////////////////////////////////////
// Name:        No names yet.
// Purpose:     Contrib. demo
// Author:      Aleksandras Gluchovas
// Modified by:
// Created:     22/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Aleskandars Gluchovas
// Licence:   	GNU General Public License 
/////////////////////////////////////////////////////////////////////////////
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
/////////////////////////////////////////////////////////////////////////////



#ifndef __SOURCEPAINTER_G__
#define __SOURCEPAINTER_G__

#ifndef ASSERT
#define ASSERT(x) if (!(x)) throw
#endif

#include "wxstldefs.h"

#include "markup.h" // import MarkupTagsT definition

// "colored" codes for highlighted blocks

#define RANK_BLACK  0 // common source fragments
#define RANK_BLUE   1 // basic types
#define RANK_RED    2 // reserved words
#define RANK_GREEN  3 // comments

// colored block description format :
// int( ( rank << 16 ) | ( source_range_len ) )

inline int get_src_block_rank( int block )
{
	return (block >> 16) & 0xFFFF;
}

inline int get_src_block_len( int block )
{
	return block & 0xFFFF;
}

// FOR NOW:: no lagnguage-map selection

// source code syntax heighlighter (CPP+JAVA+VB+PASCAL)

class SourcePainter
{
protected:
	string		  mResultStr;
	IntListT  mBlocks;
	bool          mCollectResultsOn;

	// state variables
	bool        mIsInComment;
	bool        mCommentIsMultiline;
public:

	// assembleResultString == TRUE - instructs painter
	// to collect each chunk of srouce passed to ProcessSource(),
	// so that results cann be futher obtained in a single string 
	// instead of vector of block descriptions

	SourcePainter( bool assembleResultString = TRUE );
	virtual ~SourcePainter() {}

	// can be called multiple times (e.g. on each source line)
	virtual void ProcessSource( char* src, int srcLen );

	// method, for manually adjusting state of source painter
	virtual void SetState( bool isInComment,
					       bool commentIsMultiline );

	// reinitializes object - clears results of previouse processing
	virtual void Init( bool assembleResultString = TRUE );

	// generates string of highlighted source for the scipting
	// language given by "tags" argument
	
	virtual void GetResultString(string& result, MarkupTagsT tags);

	// returns vector of block descriptors, see IntListT definition
	// (block descriptors can be used for fast custom hightlighted text generation)

	virtual IntListT& GetBlocks();

	// NOTE:: static method
	// returns if the given word is a reserved word or basic type identifier
	static bool IsKeyword( char* word, int wordLen );
};

#endif
