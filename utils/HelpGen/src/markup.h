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

#ifndef __MARKUP_G__
#define __MARKUP_G__

// indicies for the MarkupTagsT array

#define TAG_BOLD        0
#define TAG_ITALIC		1

#define TAG_FIXED_FONT  2
#define TAG_BLACK_FONT	3
#define TAG_RED_FONT	4
#define TAG_GREEN_FONT	5
#define TAG_BLUE_FONT	6

#define TAG_PARAGRAPH	7
#define TAG_NEW_LINE	8
#define TAG_HEADING_1	9
#define TAG_HEADING_2	10
#define TAG_HEADING_3   11

#define TAG_ITEM_LIST   12
#define TAG_LIST_ITEM   13 

struct TagStructT
{
	char* start; // tag that starts style
	char* end;   // tag that finishes style
};

// tag array
typedef TagStructT* MarkupTagsT;

// returns array of TagStructT with tag strings for HTML

MarkupTagsT get_HTML_markup_tags();

// MarkupTagsT get_PostScript_markup_tags();
// MarkupTagsT get_Latex_markup_tags();

#endif
