/////////////////////////////////////////////////////////////////////////////
// Name:        m_templ.h
// Purpose:     Modules template file
// Author:      Vaclav Slavik
// RCS-ID:      $Id$
// Copyright:   (c) Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/*

DESCRIPTION:
This is set of macros for easier writing of tag handlers. How to use it?
See mod_fonts.cpp for example...

Attention! This is quite strange C++ bastard. Before using it,
I STRONGLY recommend reading and understanding these macros!!

*/


#ifndef _WX_M_TEMPL_H_
#define _WX_M_TEMPL_H_

#include "wx/defs.h"
#if wxUSE_HTML


#ifdef __GNUG__
#pragma interface
#pragma implementation
#endif

#include <wx/wxprec.h>

#ifdef __BORDLANDC__
#pragma hdrstop
#endif

#ifndef WXPRECOMP
#include <wx/wx.h>
#endif




#include "wx/html/winpars.h"


#define TAG_HANDLER_BEGIN(name,tags)                                      \
    class HTML_Handler_##name : public wxHtmlWinTagHandler                \
    {                                                                     \
        public:                                                           \
            wxString GetSupportedTags() {return tags;}



#define TAG_HANDLER_VARS                                                  \
        private:

#define TAG_HANDLER_CONSTR(name)                                                \
        public:                                                           \
        HTML_Handler_##name () : wxHtmlWinTagHandler()


#define TAG_HANDLER_PROC(varib)                                           \
        public:                                                           \
            bool HandleTag(const wxHtmlTag& varib)



#define TAG_HANDLER_END(name)                                             \
    };




#define TAGS_MODULE_BEGIN(name)                                           \
    class HTML_Module##name : public wxHtmlTagsModule                     \
    {                                                                     \
        DECLARE_DYNAMIC_CLASS(HTML_Module##name )                         \
        public:                                                           \
            void FillHandlersTable(wxHtmlWinParser *parser)               \
                {




#define TAGS_MODULE_ADD(handler)                                          \
                    parser->AddTagHandler(new HTML_Handler_##handler);




#define TAGS_MODULE_END(name)                                             \
                }                                                         \
    };                                                                    \
    IMPLEMENT_DYNAMIC_CLASS(HTML_Module##name , wxHtmlTagsModule)



#endif
#endif
