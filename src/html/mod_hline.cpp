/////////////////////////////////////////////////////////////////////////////
// Name:        mod_hline.cpp
// Purpose:     wxHtml module for horizontal line (HR tag)
// Author:      Vaclav Slavik
// Copyright:   (c) 1999 Vaclav Slavik
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation
#endif

#include <wx/wxprec.h>


#include "wx/defs.h"
#if wxUSE_HTML
#ifdef __BORDLANDC__
#pragma hdrstop
#endif

#ifndef WXPRECOMP
#include <wx/wx.h>
#endif



#include <wx/html/forcelink.h>
#include <wx/html/mod_templ.h>

#include <wx/html/htmlcell.h>

FORCE_LINK_ME(mod_hline)


//-----------------------------------------------------------------------------
// wxHtmlLineCell
//-----------------------------------------------------------------------------

class wxHtmlLineCell : public wxHtmlCell
{
    public:
        wxHtmlLineCell(int size) : wxHtmlCell() {m_Height = size;}
        void Draw(wxDC& dc, int x, int y, int view_y1, int view_y2);
        void Layout(int w) {m_Width = w; if (m_Next) m_Next -> Layout(w);}
};


void wxHtmlLineCell::Draw(wxDC& dc, int x, int y, int view_y1, int view_y2)
{
    wxBrush mybrush("BLACK", wxSOLID);
    wxPen mypen("BLACK", 1, wxSOLID);
    dc.SetBrush(mybrush);
    dc.SetPen(mypen);
    dc.DrawRectangle(x + m_PosX, y + m_PosY, m_Width, m_Height);
    wxHtmlCell::Draw(dc, x, y, view_y1, view_y2);
}




//-----------------------------------------------------------------------------
// The list handler:
//-----------------------------------------------------------------------------


TAG_HANDLER_BEGIN(HR, "HR")

    TAG_HANDLER_PROC(tag)
    {
        wxHtmlContainerCell *c;
        int sz;

        m_WParser -> CloseContainer();
        c = m_WParser -> OpenContainer();

        c -> SetIndent(m_WParser -> GetCharHeight(), HTML_INDENT_VERTICAL);
        c -> SetAlignHor(HTML_ALIGN_CENTER);
        c -> SetAlign(tag);
        c -> SetWidthFloat(tag);
        if (tag.HasParam("SIZE")) tag.ScanParam("SIZE", "%i", &sz);
        else sz = 1;
        c -> InsertCell(new wxHtmlLineCell(sz));

        m_WParser -> CloseContainer();
        m_WParser -> OpenContainer();

        return FALSE;
    }

TAG_HANDLER_END(HR)





TAGS_MODULE_BEGIN(HLine)

    TAGS_MODULE_ADD(HR)

TAGS_MODULE_END(HLine)

#endif
