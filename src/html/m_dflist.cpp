/////////////////////////////////////////////////////////////////////////////
// Name:        m_dflist.cpp
// Purpose:     wxHtml module for definition lists (DL,DT,DD)
// Author:      Vaclav Slavik
// RCS-ID:      $Id$
// Copyright:   (c) 1999 Vaclav Slavik
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation
#endif

#include "wx/wxprec.h"


#include "wx/defs.h"
#if wxUSE_HTML && wxUSE_STREAMS

#ifdef __BORDLANDC__
#pragma hdrstop
#endif

#ifndef WXPRECOMP
#include "wx/wx.h"
#endif


#include "wx/html/forcelnk.h"
#include "wx/html/m_templ.h"

#include "wx/html/htmlcell.h"

FORCE_LINK_ME(m_dflist)




TAG_HANDLER_BEGIN(DEFLIST, "DL,DT,DD")

    TAG_HANDLER_PROC(tag)
    {
        wxHtmlContainerCell *c;


        if (tag.GetName() == wxT("DL")) {
            if (m_WParser -> GetContainer() -> GetFirstCell() != NULL) {
                m_WParser -> CloseContainer();
                m_WParser -> OpenContainer();
            }
            m_WParser -> GetContainer() -> SetIndent(m_WParser -> GetCharHeight(), wxHTML_INDENT_TOP);

            ParseInner(tag);

            if (m_WParser -> GetContainer() -> GetFirstCell() != NULL) {
                m_WParser -> CloseContainer();
                m_WParser -> OpenContainer();
            }
            m_WParser -> GetContainer() -> SetIndent(m_WParser -> GetCharHeight(), wxHTML_INDENT_TOP);

            return TRUE;
        }
        
        else if (tag.GetName() == wxT("DT")) {
            if (!tag.IsEnding()) {
                m_WParser -> CloseContainer();
                c = m_WParser -> OpenContainer();
                c -> SetAlignHor(wxHTML_ALIGN_LEFT);
                c -> SetMinHeight(m_WParser -> GetCharHeight());
            }
            return FALSE;
        }
        
        else if (!tag.IsEnding()) { // "DD"
            m_WParser -> CloseContainer();
            c = m_WParser -> OpenContainer();
            c -> SetIndent(5 * m_WParser -> GetCharWidth(), wxHTML_INDENT_LEFT);
            return FALSE;
        }
            
        else return FALSE;
    }

TAG_HANDLER_END(DEFLIST)


TAGS_MODULE_BEGIN(DefinitionList)

    TAGS_MODULE_ADD(DEFLIST)

TAGS_MODULE_END(DefinitionList)

#endif
