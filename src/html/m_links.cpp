/////////////////////////////////////////////////////////////////////////////
// Name:        src/html/m_links.cpp
// Purpose:     wxHtml module for links & anchors
// Author:      Vaclav Slavik
// Copyright:   (c) 1999 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"


#if wxUSE_HTML && wxUSE_STREAMS

#ifndef WX_PRECOMP
#endif

#include "wx/html/forcelnk.h"
#include "wx/html/m_templ.h"
#include "wx/html/styleparams.h"


FORCE_LINK_ME(m_links)


class wxHtmlAnchorCell : public wxHtmlCell
{
private:
    wxString m_AnchorName;

public:
    wxHtmlAnchorCell(const wxString& name) : wxHtmlCell()
        , m_AnchorName(name) { }
    void Draw(wxDC& WXUNUSED(dc),
              int WXUNUSED(x), int WXUNUSED(y),
              int WXUNUSED(view_y1), int WXUNUSED(view_y2),
              wxHtmlRenderingInfo& WXUNUSED(info)) override {}

    virtual const wxHtmlCell* Find(int condition, const void* param) const override
    {
        if ((condition == wxHTML_COND_ISANCHOR) &&
            (m_AnchorName == (*((const wxString*)param))))
        {
            return this;
        }
        else
        {
            return wxHtmlCell::Find(condition, param);
        }
    }

    wxDECLARE_NO_COPY_CLASS(wxHtmlAnchorCell);
};



TAG_HANDLER_BEGIN(A, "A")
    TAG_HANDLER_CONSTR(A) { }

    TAG_HANDLER_PROC(tag)
    {
        wxString name;
        if (tag.GetParamAsString(wxT("NAME"), &name))
        {
            m_WParser->GetContainer()->InsertCell(new wxHtmlAnchorCell(name));
        }

        wxString href;
        if (tag.GetParamAsString(wxT("HREF"), &href))
        {
            wxHtmlLinkInfo oldlnk = m_WParser->GetLink();
            wxColour oldclr = m_WParser->GetActualColor();
            wxColour oldbackclr = m_WParser->GetActualBackgroundColor();
            int oldbackmode = m_WParser->GetActualBackgroundMode();
            int oldsize = m_WParser->GetFontSize();
            int oldbold = m_WParser->GetFontBold();
            int olditalic = m_WParser->GetFontItalic();
            int oldund = m_WParser->GetFontUnderlined();
            wxString oldfontface = m_WParser->GetFontFace();
            wxString target = tag.GetParam( wxT("TARGET") );

            // set default styles, might get overridden by ApplyStyle
            m_WParser->SetActualColor(m_WParser->GetLinkColor());
            m_WParser->GetContainer()->InsertCell(new wxHtmlColourCell(m_WParser->GetLinkColor()));
            m_WParser->SetFontUnderlined(true);
            m_WParser->GetContainer()->InsertCell(new wxHtmlFontCell(m_WParser->CreateCurrentFont()));
            m_WParser->SetLink(wxHtmlLinkInfo(href, target));

            // Load any style parameters
            wxHtmlStyleParams styleParams(tag);
            ApplyStyle(styleParams);

            ParseInner(tag);

            m_WParser->SetLink(oldlnk);
            m_WParser->SetFontSize(oldsize);
            m_WParser->SetFontBold(oldbold);
            m_WParser->SetFontFace(oldfontface);
            m_WParser->SetFontItalic(olditalic);
            m_WParser->SetFontUnderlined(oldund);
            m_WParser->GetContainer()->InsertCell(new wxHtmlFontCell(m_WParser->CreateCurrentFont()));
            m_WParser->SetActualColor(oldclr);
            m_WParser->GetContainer()->InsertCell(new wxHtmlColourCell(oldclr));

            if (oldbackmode != m_WParser->GetActualBackgroundMode() ||
                oldbackclr != m_WParser->GetActualBackgroundColor())
            {
               m_WParser->SetActualBackgroundMode(oldbackmode);
               m_WParser->SetActualBackgroundColor(oldbackclr);
               m_WParser->GetContainer()->InsertCell(
                   new wxHtmlColourCell(oldbackclr, oldbackmode == wxBRUSHSTYLE_TRANSPARENT ? wxHTML_CLR_TRANSPARENT_BACKGROUND : wxHTML_CLR_BACKGROUND));
            }

            return true;
        }
        else return false;
    }

TAG_HANDLER_END(A)



TAGS_MODULE_BEGIN(Links)

    TAGS_MODULE_ADD(A)

TAGS_MODULE_END(Links)


#endif
