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

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "markup.h"

static TagStructT htmlTags[] =
{
	{ "<b>","</b>" }, // 0
	{ "<i>","</i>" }, // 1
	{ "<pre>","</pre>" }, // 2
	{ "<font color=\"#000000\">","</font>" }, // 3
	{ "<font color=\"#8F0000\">","</font>" }, // 4
	{ "<font color=\"#008F00\">","</font>" }, // 5
	{ "<font color=\"#0000CF\">","</font>" }, // 6
	{ "<p>","</p>" }, // 7
	{ "<br>","" }, // 8
	{ "<h1>","</h1>" }, // 9
	{ "<h2>","</h2>" }, // 10
	{ "<h3>","</h3>" }, // 11
	{ "<ul>","</ul>" }, // 12
	{ "<li>","</li>" }, // 13
};

MarkupTagsT get_HTML_markup_tags()
{
	return htmlTags;
}
