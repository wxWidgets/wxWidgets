/////////////////////////////////////////////////////////////////////////////
// Name:        mod_layout.cpp
// Purpose:     wxHtml module for basic paragraphs/layout handling
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
#if wxUSE_HTML
#ifdef __BORDLANDC__
#pragma hdrstop
#endif

#ifndef WXPRECOMP
#include "wx/wx.h"
#endif


#include "wx/html/forcelnk.h"
#include "wx/html/m_templ.h"

#include "wx/html/htmlwin.h"

FORCE_LINK_ME(mod_layout)


TAG_HANDLER_BEGIN(P, "P")

    TAG_HANDLER_PROC(tag)
    {
        if (m_WParser -> GetContainer() -> GetFirstCell() != NULL) {
            m_WParser -> CloseContainer();
            m_WParser -> OpenContainer();
    }
        m_WParser -> GetContainer() -> SetIndent(m_WParser -> GetCharHeight(), wxHTML_INDENT_TOP);
        m_WParser -> GetContainer() -> SetAlign(tag);
        return FALSE;
    }

TAG_HANDLER_END(P)



TAG_HANDLER_BEGIN(BR, "BR")

    TAG_HANDLER_PROC(tag)
    {
        int al = m_WParser -> GetContainer() -> GetAlignHor();
        wxHtmlContainerCell *c;
    
        m_WParser -> CloseContainer();
        c = m_WParser -> OpenContainer();
        c -> SetAlignHor(al);
        c -> SetAlign(tag);
        return FALSE;
    }

TAG_HANDLER_END(BR)



TAG_HANDLER_BEGIN(CENTER, "CENTER")

    TAG_HANDLER_PROC(tag)
    {
        int old = m_WParser -> GetAlign();
        wxHtmlContainerCell *c = m_WParser -> GetContainer();

        m_WParser -> SetAlign(wxHTML_ALIGN_CENTER);
        if (c -> GetFirstCell() != NULL) {
            m_WParser -> CloseContainer();
            m_WParser -> OpenContainer();
        }
        else
            c -> SetAlignHor(wxHTML_ALIGN_CENTER);

        if (tag.HasEnding()) {
            ParseInner(tag);

            m_WParser -> SetAlign(old);
            if (c -> GetFirstCell() != NULL) {
                m_WParser -> CloseContainer();
                m_WParser -> OpenContainer();
            }
            else
                c -> SetAlignHor(old);

            return TRUE;
        }
        else return FALSE;
    }

TAG_HANDLER_END(CENTER)



TAG_HANDLER_BEGIN(DIV, "DIV")

    TAG_HANDLER_PROC(tag)
    {
        int old = m_WParser -> GetAlign();
        wxHtmlContainerCell *c = m_WParser -> GetContainer();
        if (c -> GetFirstCell() != NULL) {
            m_WParser -> CloseContainer();
            m_WParser -> OpenContainer();
            c = m_WParser -> GetContainer();
            c -> SetAlign(tag);
            m_WParser -> SetAlign(c -> GetAlignHor());
        }
        else {
            c -> SetAlign(tag);
        m_WParser -> SetAlign(c -> GetAlignHor());
        }

        ParseInner(tag);

        m_WParser -> SetAlign(old);
        if (c -> GetFirstCell() != NULL) {
            m_WParser -> CloseContainer();
            m_WParser -> OpenContainer();
        }
        else
            c -> SetAlignHor(old);

        return TRUE;
    }

TAG_HANDLER_END(DIV)




TAG_HANDLER_BEGIN(TITLE, "TITLE")

    TAG_HANDLER_PROC(tag)
    {
        if (m_WParser -> GetWindow()) {
            wxHtmlWindow *wfr = (wxHtmlWindow*)(m_WParser -> GetWindow());
            if (wfr) {
                wxString title = "";
                wxString *src = m_WParser -> GetSource();

                for (int i = tag.GetBeginPos(); i < tag.GetEndPos1(); i++) title += (*src)[(unsigned int) i];
                wfr -> SetTitle(title);
            }
        }
        return TRUE;
    }

TAG_HANDLER_END(TITLE)




TAG_HANDLER_BEGIN(BODY, "BODY")

    TAG_HANDLER_PROC(tag)
    {
        unsigned long tmp;
        wxColour clr;

        if (tag.HasParam(wxT("TEXT"))) {
            if (tag.ScanParam(wxT("TEXT"), wxT("#%lX"), &tmp) == 1) {
                clr = wxColour((tmp & 0xFF0000) >> 16 , (tmp & 0x00FF00) >> 8, (tmp & 0x0000FF));
                m_WParser -> SetActualColor(clr);
                m_WParser -> GetContainer() -> InsertCell(new wxHtmlColourCell(clr));
            }
	}

        if (tag.HasParam(wxT("LINK"))) {
            if (tag.ScanParam(wxT("LINK"), wxT("#%lX"), &tmp) == 1) {
                clr = wxColour((tmp & 0xFF0000) >> 16 , (tmp & 0x00FF00) >> 8, (tmp & 0x0000FF));
                m_WParser -> SetLinkColor(clr);
	    }
        }

        if (tag.HasParam(wxT("BGCOLOR"))) {
            if (tag.ScanParam(wxT("BGCOLOR"), wxT("#%lX"), &tmp) == 1) {
                clr = wxColour((tmp & 0xFF0000) >> 16 , (tmp & 0x00FF00) >> 8, (tmp & 0x0000FF));
                m_WParser -> GetContainer() -> InsertCell(new wxHtmlColourCell(clr, wxHTML_CLR_BACKGROUND));
                if (m_WParser -> GetWindow() != NULL)
                    m_WParser -> GetWindow() -> SetBackgroundColour(clr);
	    }
        }
        return FALSE;
    }

TAG_HANDLER_END(BODY)



TAG_HANDLER_BEGIN(BLOCKQUOTE, "BLOCKQUOTE")

    TAG_HANDLER_PROC(tag)
    {
        wxHtmlContainerCell *c;
    
        m_WParser -> CloseContainer();
        c = m_WParser -> OpenContainer();
        if (c -> GetAlignHor() == wxHTML_ALIGN_RIGHT)
            c -> SetIndent(5 * m_WParser -> GetCharWidth(), wxHTML_INDENT_RIGHT);
        else
            c -> SetIndent(5 * m_WParser -> GetCharWidth(), wxHTML_INDENT_LEFT);
        c -> SetIndent(m_WParser -> GetCharHeight(), wxHTML_INDENT_TOP);
        m_WParser -> OpenContainer();
        ParseInner(tag);
        c = m_WParser -> CloseContainer();
        c -> SetIndent(m_WParser -> GetCharHeight(), wxHTML_INDENT_BOTTOM);
        m_WParser -> CloseContainer();
        m_WParser -> OpenContainer();
        return TRUE;
    }

TAG_HANDLER_END(BLOCKQUOTE)






TAGS_MODULE_BEGIN(Layout)

    TAGS_MODULE_ADD(P)
    TAGS_MODULE_ADD(BR)
    TAGS_MODULE_ADD(CENTER)
    TAGS_MODULE_ADD(DIV)
    TAGS_MODULE_ADD(TITLE)
    TAGS_MODULE_ADD(BODY)
    TAGS_MODULE_ADD(BLOCKQUOTE)

TAGS_MODULE_END(Layout)

#endif
