/////////////////////////////////////////////////////////////////////////////
// Name:        mod_links.cpp
// Purpose:     wxHtml module for links & anchors
// Author:      Vaclav Slavik
// RCS-ID:      $Id$
// Copyright:   (c) 1999 Vaclav Slavik
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation
#endif

#include <wx/wxprec.h>

#if wxUSE_HTML

#ifdef __BORDLANDC__
#pragma hdrstop
#endif

#ifndef WXPRECOMP
#include <wx/wx.h>
#endif


#include "wx/html/forcelnk.h"
#include "wx/html/m_templ.h"


FORCE_LINK_ME(mod_links)


class wxHtmlAnchorCell : public wxHtmlCell
{
    private:
        wxString m_AnchorName;

    public:
        wxHtmlAnchorCell(const wxString& name) : wxHtmlCell() {m_AnchorName = name;}
        virtual const wxHtmlCell* Find(int condition, const void* param) const
        {
            if ((condition == HTML_COND_ISANCHOR) && (m_AnchorName == (*((const wxString*)param))))
                return this;
            else
                return wxHtmlCell::Find(condition, param);
        }
};



TAG_HANDLER_BEGIN(A, "A")

    TAG_HANDLER_PROC(tag)
    {
        if (tag.HasParam("NAME")) {
            m_WParser -> GetContainer() -> InsertCell(new wxHtmlAnchorCell(tag.GetParam("NAME")));
        }

        if (tag.HasParam("HREF")) {
            wxString oldlnk = m_WParser -> GetLink();
            wxColour oldclr = m_WParser -> GetActualColor();
            int oldund = m_WParser -> GetFontUnderlined();

            m_WParser -> SetActualColor(m_WParser -> GetLinkColor());
            m_WParser -> GetContainer() -> InsertCell(new wxHtmlColourCell(m_WParser -> GetLinkColor()));
            m_WParser -> SetFontUnderlined(TRUE);
            m_WParser -> GetContainer() -> InsertCell(new wxHtmlFontCell(m_WParser -> CreateCurrentFont()));
            m_WParser -> SetLink(tag.GetParam("HREF"));

            ParseInner(tag);

            m_WParser -> SetLink(oldlnk);
            m_WParser -> SetFontUnderlined(oldund);
            m_WParser -> GetContainer() -> InsertCell(new wxHtmlFontCell(m_WParser -> CreateCurrentFont()));
            m_WParser -> SetActualColor(oldclr);
            m_WParser -> GetContainer() -> InsertCell(new wxHtmlColourCell(oldclr));

            return TRUE;
        }
        else return FALSE;
    }

TAG_HANDLER_END(A)



TAGS_MODULE_BEGIN(Links)

    TAGS_MODULE_ADD(A)

TAGS_MODULE_END(Links)


#endif
