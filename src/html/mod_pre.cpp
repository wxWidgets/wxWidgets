/////////////////////////////////////////////////////////////////////////////
// Name:        mod_pre.cpp
// Purpose:     wxHtml module for <PRE> ... </PRE> tag (code citation)
// Author:      Vaclav Slavik
// Copyright:   (c) 1999 Vaclav Slavik
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/defs.h"
#if wxUSE_HTML

#include <wx/html/forcelink.h>
#include <wx/html/mod_templ.h>

#include <wx/html/htmlcell.h>
#include <wx/tokenzr.h>

FORCE_LINK_ME(mod_pre)


//-----------------------------------------------------------------------------
// wxHtmlCodeCell
//-----------------------------------------------------------------------------

class wxHtmlPRECell : public wxHtmlCell
{
    private:
        wxString** m_Text;
                // list of wxString objects.
        int m_LinesCnt;
                // number of lines
        int m_LineHeight;
                // height of single line of text

    public:
        wxHtmlPRECell(const wxString& s, wxDC& dc);
        ~wxHtmlPRECell();
        void Draw(wxDC& dc, int x, int y, int view_y1, int view_y2);
};


wxHtmlPRECell::wxHtmlPRECell(const wxString& s, wxDC& dc) : wxHtmlCell()
{
    wxStringTokenizer tokenizer(s, "\n");
    wxString tmp;
    long int x, z;
    int i;

    m_LineHeight = dc.GetCharHeight();
    m_LinesCnt = 0;
    m_Text = NULL;
    m_Width = m_Height = 0;

    i = 0;
#if (wxVERSION_NUMBER < 2100)
    while (tokenizer.HasMoreToken()) {
#else
    while (tokenizer.HasMoreTokens()) {
#endif
        if (i % 10 == 0) m_Text = (wxString**) realloc(m_Text, sizeof(wxString*) * (i + 10));
        tmp = tokenizer.NextToken();
        tmp.Replace("&nbsp;", " ", TRUE);
        tmp.Replace("&quot;", "\"", TRUE);
        tmp.Replace("&lt;", "<", TRUE);
        tmp.Replace("&gt;", ">", TRUE);
        tmp.Replace("&amp;", "&", TRUE);
        tmp.Replace("\t", "        ", TRUE);
        tmp.Replace("\r", "", TRUE);
        m_Text[i++] = new wxString(tmp);

        dc.GetTextExtent(tmp, &x, &z, &z);
        if (x > m_Width) m_Width = x;
        m_Height += m_LineHeight;
        m_LinesCnt++;
    }
}



wxHtmlPRECell::~wxHtmlPRECell()
{
    for (int i = 0; i < m_LinesCnt; i++) delete m_Text[i];
    free(m_Text);
}


void wxHtmlPRECell::Draw(wxDC& dc, int x, int y, int view_y1, int view_y2)
{
    for (int i = 0; i < m_LinesCnt; i++)
        dc.DrawText(*(m_Text[i]), x + m_PosX, y + m_PosY + m_LineHeight * i);

    wxHtmlCell::Draw(dc, x, y, view_y1, view_y2);
}




//-----------------------------------------------------------------------------
// The list handler:
//-----------------------------------------------------------------------------


TAG_HANDLER_BEGIN(PRE, "PRE")

    TAG_HANDLER_PROC(tag)
    {
        wxHtmlContainerCell *c;

        int fixed = m_WParser -> GetFontFixed(),
            italic = m_WParser -> GetFontItalic(),
            underlined = m_WParser -> GetFontUnderlined(),
            bold = m_WParser -> GetFontBold(),
            fsize = m_WParser -> GetFontSize();

        m_WParser -> CloseContainer();
        c = m_WParser -> OpenContainer();
        c -> SetAlignHor(HTML_ALIGN_LEFT);
        c -> SetIndent(m_WParser -> GetCharHeight(), HTML_INDENT_VERTICAL);

        m_WParser -> SetFontUnderlined(FALSE);
        m_WParser -> SetFontBold(FALSE);
        m_WParser -> SetFontItalic(FALSE);
        m_WParser -> SetFontFixed(TRUE);
        m_WParser -> SetFontSize(0);
        c -> InsertCell(new wxHtmlFontCell(m_WParser -> CreateCurrentFont()));

        {
            wxString cit;
            cit = m_WParser -> GetSource() -> Mid(tag.GetBeginPos(), tag.GetEndPos1() - tag.GetBeginPos());
            c -> InsertCell(new wxHtmlPRECell(cit, *(m_WParser -> GetDC())));
        }

        m_WParser -> SetFontUnderlined(underlined);
        m_WParser -> SetFontBold(bold);
        m_WParser -> SetFontItalic(italic);
        m_WParser -> SetFontFixed(fixed);
        m_WParser -> SetFontSize(fsize);
        c -> InsertCell(new wxHtmlFontCell(m_WParser -> CreateCurrentFont()));

        m_WParser -> CloseContainer();
        m_WParser -> OpenContainer();
        return TRUE;
    }

TAG_HANDLER_END(PRE)





TAGS_MODULE_BEGIN(Pre)

    TAGS_MODULE_ADD(PRE)

TAGS_MODULE_END(Pre)

#endif
