/////////////////////////////////////////////////////////////////////////////
// Name:        htmlcell.cpp
// Purpose:     wxHtmlCell - basic element of HTML output
// Author:      Vaclav Slavik
// RCS-ID:      $Id$
// Copyright:   (c) 1999 Vaclav Slavik
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "htmlcell.h"
#endif

#include "wx/wxprec.h"

#include "wx/defs.h"

#if wxUSE_HTML && wxUSE_STREAMS

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WXPRECOMP
    #include "wx/brush.h"
    #include "wx/colour.h"
    #include "wx/dc.h"
#endif

#include "wx/html/htmlcell.h"
#include "wx/html/htmlwin.h"
#include <stdlib.h>


//-----------------------------------------------------------------------------
// wxHtmlCell
//-----------------------------------------------------------------------------

wxHtmlCell::wxHtmlCell() : wxObject()
{
    m_Next = NULL;
    m_Parent = NULL;
    m_Width = m_Height = m_Descent = 0;
    m_CanLiveOnPagebreak = TRUE;
    m_Link = NULL;
}

wxHtmlCell::~wxHtmlCell()
{
    delete m_Link;
}


void wxHtmlCell::OnMouseClick(wxWindow *parent, int x, int y,
                              const wxMouseEvent& event)
{
    wxHtmlLinkInfo *lnk = GetLink(x, y);
    if (lnk != NULL)
    {
        wxHtmlLinkInfo lnk2(*lnk);
        lnk2.SetEvent(&event);
        lnk2.SetHtmlCell(this);

        // note : this cast is legal because parent is *always* wxHtmlWindow
        wxStaticCast(parent, wxHtmlWindow)->OnLinkClicked(lnk2);
    }
}



// wx 2.5 will use this signature:
//   bool wxHtmlCell::AdjustPagebreak(int *pagebreak, int* WXUNUSED(known_pagebreaks), int WXUNUSED(number_of_pages)) const
//
// Workaround to backport html pagebreaks to 2.4.0:
// Actually, we're passing a pointer to struct wxHtmlKludge, casting
// that pointer to an int* . We don't need to do anything special
// here because that struct's first element is an int* to 'pagebreak'.
// Other struct members can be ignored because they'd be unused anyway.
bool wxHtmlCell::AdjustPagebreak(int *pagebreak) const
{
    if ((!m_CanLiveOnPagebreak) &&
                m_PosY < *pagebreak && m_PosY + m_Height > *pagebreak)
    {
        *pagebreak = m_PosY;
        return TRUE;
    }

    return FALSE;
}



void wxHtmlCell::SetLink(const wxHtmlLinkInfo& link)
{
    if (m_Link) delete m_Link;
    m_Link = NULL;
    if (link.GetHref() != wxEmptyString)
        m_Link = new wxHtmlLinkInfo(link);
}



void wxHtmlCell::Layout(int WXUNUSED(w))
{
    SetPos(0, 0);
}



void wxHtmlCell::GetHorizontalConstraints(int *left, int *right) const
{
    if (left)
        *left = m_PosX;
    if (right)
        *right = m_PosX + m_Width;
}



const wxHtmlCell* wxHtmlCell::Find(int WXUNUSED(condition), const void* WXUNUSED(param)) const
{
    return NULL;
}


wxHtmlCell *wxHtmlCell::FindCellByPos(wxCoord x, wxCoord y) const
{
    if ( x >= 0 && x < m_Width && y >= 0 && y < m_Height )
        return wxConstCast(this, wxHtmlCell);

    return NULL;
}


//-----------------------------------------------------------------------------
// wxHtmlWordCell
//-----------------------------------------------------------------------------

wxHtmlWordCell::wxHtmlWordCell(const wxString& word, wxDC& dc) : wxHtmlCell()
{
    m_Word = word;
    dc.GetTextExtent(m_Word, &m_Width, &m_Height, &m_Descent);
    SetCanLiveOnPagebreak(FALSE);
}



void wxHtmlWordCell::Draw(wxDC& dc, int x, int y, int WXUNUSED(view_y1), int WXUNUSED(view_y2))
{
    dc.DrawText(m_Word, x + m_PosX, y + m_PosY);
}



//-----------------------------------------------------------------------------
// wxHtmlContainerCell
//-----------------------------------------------------------------------------


wxHtmlContainerCell::wxHtmlContainerCell(wxHtmlContainerCell *parent) : wxHtmlCell()
{
    m_Cells = m_LastCell = NULL;
    m_Parent = parent;
    if (m_Parent) m_Parent->InsertCell(this);
    m_AlignHor = wxHTML_ALIGN_LEFT;
    m_AlignVer = wxHTML_ALIGN_BOTTOM;
    m_IndentLeft = m_IndentRight = m_IndentTop = m_IndentBottom = 0;
    m_WidthFloat = 100; m_WidthFloatUnits = wxHTML_UNITS_PERCENT;
    m_UseBkColour = FALSE;
    m_UseBorder = FALSE;
    m_MinHeight = 0;
    m_MinHeightAlign = wxHTML_ALIGN_TOP;
    m_LastLayout = -1;
}

wxHtmlContainerCell::~wxHtmlContainerCell()
{
    wxHtmlCell *cell = m_Cells;
    while ( cell )
    {
        wxHtmlCell *cellNext = cell->GetNext();
        delete cell;
        cell = cellNext;
    }
}



void wxHtmlContainerCell::SetIndent(int i, int what, int units)
{
    int val = (units == wxHTML_UNITS_PIXELS) ? i : -i;
    if (what & wxHTML_INDENT_LEFT) m_IndentLeft = val;
    if (what & wxHTML_INDENT_RIGHT) m_IndentRight = val;
    if (what & wxHTML_INDENT_TOP) m_IndentTop = val;
    if (what & wxHTML_INDENT_BOTTOM) m_IndentBottom = val;
    m_LastLayout = -1;
}



int wxHtmlContainerCell::GetIndent(int ind) const
{
    if (ind & wxHTML_INDENT_LEFT) return m_IndentLeft;
    else if (ind & wxHTML_INDENT_RIGHT) return m_IndentRight;
    else if (ind & wxHTML_INDENT_TOP) return m_IndentTop;
    else if (ind & wxHTML_INDENT_BOTTOM) return m_IndentBottom;
    else return -1; /* BUG! Should not be called... */
}




int wxHtmlContainerCell::GetIndentUnits(int ind) const
{
    bool p = FALSE;
    if (ind & wxHTML_INDENT_LEFT) p = m_IndentLeft < 0;
    else if (ind & wxHTML_INDENT_RIGHT) p = m_IndentRight < 0;
    else if (ind & wxHTML_INDENT_TOP) p = m_IndentTop < 0;
    else if (ind & wxHTML_INDENT_BOTTOM) p = m_IndentBottom < 0;
    if (p) return wxHTML_UNITS_PERCENT;
    else return wxHTML_UNITS_PIXELS;
}



// wx 2.5 will use this signature:
//   bool wxHtmlContainerCell::AdjustPagebreak(int *pagebreak, int* known_pagebreaks, int number_of_pages) const
//
// Workaround to backport html pagebreaks to 2.4.0:
// Actually, we're passing a pointer to struct wxHtmlKludge, casting
// that pointer to an int* . We don't need to do anything special
// here because that struct's first element is an int* to 'pagebreak'.
// Other struct members aren't used here and can be ignored.
bool wxHtmlContainerCell::AdjustPagebreak(int *pagebreak) const
{
    if (!m_CanLiveOnPagebreak)
// wx 2.5 will use this call:
//        return wxHtmlCell::AdjustPagebreak(pagebreak, known_pagebreaks, number_of_pages);
        return wxHtmlCell::AdjustPagebreak(pagebreak);

    else
    {
        wxHtmlCell *c = GetFirstCell();
        bool rt = FALSE;
        int pbrk = *pagebreak - m_PosY;

        // Temporary kludge for backporting html pagebreaks to 2.4.0;
        // remove in 2.4.1 .
        wxHtmlKludge kludge = *(wxHtmlKludge*)pagebreak;
        kludge.pbreak = pbrk;

        while (c)
        {
// wx 2.5 will use this call:
//            if (c->AdjustPagebreak(&pbrk, known_pagebreaks, number_of_pages))
            if (c->AdjustPagebreak((int*)&kludge))
                rt = TRUE;
            c = c->GetNext();
        }
        if (rt)
            {
            pbrk = kludge.pbreak;
            *pagebreak = pbrk + m_PosY;
            }
        return rt;
    }
}



void wxHtmlContainerCell::Layout(int w)
{
    wxHtmlCell::Layout(w);

    if (m_LastLayout == w) return;

    // VS: Any attempt to layout with negative or zero width leads to hell,
    // but we can't ignore such attempts completely, since it sometimes
    // happen (e.g. when trying how small a table can be). The best thing we
    // can do is to set the width of child cells to zero
    if (w < 1)
    {
       m_Width = 0;
       for (wxHtmlCell *cell = m_Cells; cell; cell = cell->GetNext())
            cell->Layout(0);
            // this does two things: it recursively calls this code on all
            // child contrainers and resets children's position to (0,0)
       return;
    }

    wxHtmlCell *cell = m_Cells, *line = m_Cells;
    long xpos = 0, ypos = m_IndentTop;
    int xdelta = 0, ybasicpos = 0, ydiff;
    int s_width, s_indent;
    int ysizeup = 0, ysizedown = 0;
    int MaxLineWidth = 0;
    int xcnt = 0;


    /*

    WIDTH ADJUSTING :

    */

    if (m_WidthFloatUnits == wxHTML_UNITS_PERCENT)
    {
        if (m_WidthFloat < 0) m_Width = (100 + m_WidthFloat) * w / 100;
        else m_Width = m_WidthFloat * w / 100;
    }
    else
    {
        if (m_WidthFloat < 0) m_Width = w + m_WidthFloat;
        else m_Width = m_WidthFloat;
    }

    if (m_Cells)
    {
        int l = (m_IndentLeft < 0) ? (-m_IndentLeft * m_Width / 100) : m_IndentLeft;
        int r = (m_IndentRight < 0) ? (-m_IndentRight * m_Width / 100) : m_IndentRight;
        for (wxHtmlCell *cell = m_Cells; cell; cell = cell->GetNext())
            cell->Layout(m_Width - (l + r));
    }

    /*

    LAYOUTING :

    */

    // adjust indentation:
    s_indent = (m_IndentLeft < 0) ? (-m_IndentLeft * m_Width / 100) : m_IndentLeft;
    s_width = m_Width - s_indent - ((m_IndentRight < 0) ? (-m_IndentRight * m_Width / 100) : m_IndentRight);

    // my own layouting:
    while (cell != NULL)
    {
        switch (m_AlignVer)
        {
            case wxHTML_ALIGN_TOP :      ybasicpos = 0; break;
            case wxHTML_ALIGN_BOTTOM :   ybasicpos = - cell->GetHeight(); break;
            case wxHTML_ALIGN_CENTER :   ybasicpos = - cell->GetHeight() / 2; break;
        }
        ydiff = cell->GetHeight() + ybasicpos;

        if (cell->GetDescent() + ydiff > ysizedown) ysizedown = cell->GetDescent() + ydiff;
        if (ybasicpos + cell->GetDescent() < -ysizeup) ysizeup = - (ybasicpos + cell->GetDescent());

        cell->SetPos(xpos, ybasicpos + cell->GetDescent());
        xpos += cell->GetWidth();
        cell = cell->GetNext();
        xcnt++;

        // force new line if occured:
        if ((cell == NULL) || (xpos + cell->GetWidth() > s_width))
        {
            if (xpos > MaxLineWidth) MaxLineWidth = xpos;
            if (ysizeup < 0) ysizeup = 0;
            if (ysizedown < 0) ysizedown = 0;
            switch (m_AlignHor) {
                case wxHTML_ALIGN_LEFT :
                case wxHTML_ALIGN_JUSTIFY :
                         xdelta = 0;
                         break;
                case wxHTML_ALIGN_RIGHT :
                         xdelta = 0 + (s_width - xpos);
                         break;
                case wxHTML_ALIGN_CENTER :
                         xdelta = 0 + (s_width - xpos) / 2;
                         break;
            }
            if (xdelta < 0) xdelta = 0;
            xdelta += s_indent;

            ypos += ysizeup;

            if (m_AlignHor != wxHTML_ALIGN_JUSTIFY || cell == NULL)
                while (line != cell)
                {
                    line->SetPos(line->GetPosX() + xdelta,
                                   ypos + line->GetPosY());
                    line = line->GetNext();
                }
            else
            {
                int counter = 0;
                int step = (s_width - xpos);
                if (step < 0) step = 0;
                xcnt--;
                if (xcnt > 0) while (line != cell)
                {
                    line->SetPos(line->GetPosX() + s_indent +
                                   (counter++ * step / xcnt),
                                   ypos + line->GetPosY());
                    line = line->GetNext();
                }
                xcnt++;
            }

            ypos += ysizedown;
            xpos = xcnt = 0;
            ysizeup = ysizedown = 0;
            line = cell;
        }
    }

    // setup height & width, depending on container layout:
    m_Height = ypos + (ysizedown + ysizeup) + m_IndentBottom;

    if (m_Height < m_MinHeight)
    {
        if (m_MinHeightAlign != wxHTML_ALIGN_TOP)
        {
            int diff = m_MinHeight - m_Height;
            if (m_MinHeightAlign == wxHTML_ALIGN_CENTER) diff /= 2;
            cell = m_Cells;
            while (cell)
            {
                cell->SetPos(cell->GetPosX(), cell->GetPosY() + diff);
                cell = cell->GetNext();
            }
        }
        m_Height = m_MinHeight;
    }

    MaxLineWidth += s_indent + ((m_IndentRight < 0) ? (-m_IndentRight * m_Width / 100) : m_IndentRight);
    if (m_Width < MaxLineWidth) m_Width = MaxLineWidth;

    m_LastLayout = w;
}


#define mMin(a, b) (((a) < (b)) ? (a) : (b))
#define mMax(a, b) (((a) < (b)) ? (b) : (a))

void wxHtmlContainerCell::Draw(wxDC& dc, int x, int y, int view_y1, int view_y2)
{
    // container visible, draw it:
    if ((y + m_PosY <= view_y2) && (y + m_PosY + m_Height > view_y1))
    {
        if (m_UseBkColour)
        {
            wxBrush myb = wxBrush(m_BkColour, wxSOLID);

            int real_y1 = mMax(y + m_PosY, view_y1);
            int real_y2 = mMin(y + m_PosY + m_Height - 1, view_y2);

            dc.SetBrush(myb);
            dc.SetPen(*wxTRANSPARENT_PEN);
            dc.DrawRectangle(x + m_PosX, real_y1, m_Width, real_y2 - real_y1 + 1);
        }

        if (m_UseBorder)
        {
            wxPen mypen1(m_BorderColour1, 1, wxSOLID);
            wxPen mypen2(m_BorderColour2, 1, wxSOLID);

            dc.SetPen(mypen1);
            dc.DrawLine(x + m_PosX, y + m_PosY, x + m_PosX, y + m_PosY + m_Height - 1);
            dc.DrawLine(x + m_PosX, y + m_PosY, x + m_PosX + m_Width, y + m_PosY);
            dc.SetPen(mypen2);
            dc.DrawLine(x + m_PosX + m_Width - 1, y + m_PosY, x + m_PosX +  m_Width - 1, y + m_PosY + m_Height - 1);
            dc.DrawLine(x + m_PosX, y + m_PosY + m_Height - 1, x + m_PosX + m_Width, y + m_PosY + m_Height - 1);
        }

        if (m_Cells)
        {
            for (wxHtmlCell *cell = m_Cells; cell; cell = cell->GetNext())
                cell->Draw(dc, x + m_PosX, y + m_PosY, view_y1, view_y2);
        }
    }
    // container invisible, just proceed font+color changing:
    else
    {
        if (m_Cells)
        {
            for (wxHtmlCell *cell = m_Cells; cell; cell = cell->GetNext())
                cell->DrawInvisible(dc, x + m_PosX, y + m_PosY);
        }
    }
}



void wxHtmlContainerCell::DrawInvisible(wxDC& dc, int x, int y)
{
    if (m_Cells)
    {
        for (wxHtmlCell *cell = m_Cells; cell; cell = cell->GetNext())
            cell->DrawInvisible(dc, x + m_PosX, y + m_PosY);
    }
}


wxColour wxHtmlContainerCell::GetBackgroundColour()
{
    if (m_UseBkColour)
        return m_BkColour;
    else
        return wxNullColour;
}



wxHtmlLinkInfo *wxHtmlContainerCell::GetLink(int x, int y) const
{
    wxHtmlCell *cell = FindCellByPos(x, y);

    // VZ: I don't know if we should pass absolute or relative coords to
    //     wxHtmlCell::GetLink()? As the base class version just ignores them
    //     anyhow, it hardly matters right now but should still be clarified
    return cell ? cell->GetLink(x, y) : NULL;
}



void wxHtmlContainerCell::InsertCell(wxHtmlCell *f)
{
    if (!m_Cells) m_Cells = m_LastCell = f;
    else
    {
        m_LastCell->SetNext(f);
        m_LastCell = f;
        if (m_LastCell) while (m_LastCell->GetNext()) m_LastCell = m_LastCell->GetNext();
    }
    f->SetParent(this);
    m_LastLayout = -1;
}



void wxHtmlContainerCell::SetAlign(const wxHtmlTag& tag)
{
    if (tag.HasParam(wxT("ALIGN")))
    {
        wxString alg = tag.GetParam(wxT("ALIGN"));
        alg.MakeUpper();
        if (alg == wxT("CENTER"))
            SetAlignHor(wxHTML_ALIGN_CENTER);
        else if (alg == wxT("LEFT"))
            SetAlignHor(wxHTML_ALIGN_LEFT);
        else if (alg == wxT("JUSTIFY"))
            SetAlignHor(wxHTML_ALIGN_JUSTIFY);
        else if (alg == wxT("RIGHT"))
            SetAlignHor(wxHTML_ALIGN_RIGHT);
        m_LastLayout = -1;
    }
}



void wxHtmlContainerCell::SetWidthFloat(const wxHtmlTag& tag, double pixel_scale)
{
    if (tag.HasParam(wxT("WIDTH")))
    {
        int wdi;
        wxString wd = tag.GetParam(wxT("WIDTH"));

        if (wd[wd.Length()-1] == wxT('%'))
        {
            wxSscanf(wd.c_str(), wxT("%i%%"), &wdi);
            SetWidthFloat(wdi, wxHTML_UNITS_PERCENT);
        }
        else
        {
            wxSscanf(wd.c_str(), wxT("%i"), &wdi);
            SetWidthFloat((int)(pixel_scale * (double)wdi), wxHTML_UNITS_PIXELS);
        }
        m_LastLayout = -1;
    }
}



const wxHtmlCell* wxHtmlContainerCell::Find(int condition, const void* param) const
{
    if (m_Cells)
    {
        const wxHtmlCell *r = NULL;

        for (wxHtmlCell *cell = m_Cells; cell; cell = cell->GetNext())
        {
            r = cell->Find(condition, param);
            if (r) return r;
        }
    }
    return NULL;
}


wxHtmlCell *wxHtmlContainerCell::FindCellByPos(wxCoord x, wxCoord y) const
{
    for ( const wxHtmlCell *cell = m_Cells; cell; cell = cell->GetNext() )
    {
        int cx = cell->GetPosX(),
            cy = cell->GetPosY();

        if ( (cx <= x) && (cx + cell->GetWidth() > x) &&
             (cy <= y) && (cy + cell->GetHeight() > y) )
        {
            return cell->FindCellByPos(x - cx, y - cy);
        }
    }

    return NULL;
}


void wxHtmlContainerCell::OnMouseClick(wxWindow *parent, int x, int y, const wxMouseEvent& event)
{
    wxHtmlCell *cell = FindCellByPos(x, y);
    if ( cell )
        cell->OnMouseClick(parent, x, y, event);
}



void wxHtmlContainerCell::GetHorizontalConstraints(int *left, int *right) const
{
    int cleft = m_PosX + m_Width, cright = m_PosX; // worst case
    int l, r;

    for (wxHtmlCell *cell = m_Cells; cell; cell = cell->GetNext())
    {
        cell->GetHorizontalConstraints(&l, &r);
        if (l < cleft)
            cleft = l;
        if (r > cright)
            cright = r;
    }

    cleft -= (m_IndentLeft < 0) ? (-m_IndentLeft * m_Width / 100) : m_IndentLeft;
    cright += (m_IndentRight < 0) ? (-m_IndentRight * m_Width / 100) : m_IndentRight;

    if (left)
        *left = cleft;
    if (right)
        *right = cright;
}





//--------------------------------------------------------------------------------
// wxHtmlColourCell
//--------------------------------------------------------------------------------

void wxHtmlColourCell::Draw(wxDC& dc, int WXUNUSED(x), int WXUNUSED(y), int WXUNUSED(view_y1), int WXUNUSED(view_y2))
{
    if (m_Flags & wxHTML_CLR_FOREGROUND)
        dc.SetTextForeground(m_Colour);
    if (m_Flags & wxHTML_CLR_BACKGROUND)
    {
        dc.SetBackground(wxBrush(m_Colour, wxSOLID));
        dc.SetTextBackground(m_Colour);
    }
}

void wxHtmlColourCell::DrawInvisible(wxDC& dc, int WXUNUSED(x), int WXUNUSED(y))
{
    if (m_Flags & wxHTML_CLR_FOREGROUND)
        dc.SetTextForeground(m_Colour);
    if (m_Flags & wxHTML_CLR_BACKGROUND)
    {
        dc.SetBackground(wxBrush(m_Colour, wxSOLID));
        dc.SetTextBackground(m_Colour);
    }
}




//--------------------------------------------------------------------------------
// wxHtmlFontCell
//--------------------------------------------------------------------------------

void wxHtmlFontCell::Draw(wxDC& dc, int WXUNUSED(x), int WXUNUSED(y), int WXUNUSED(view_y1), int WXUNUSED(view_y2))
{
    dc.SetFont(m_Font);
}

void wxHtmlFontCell::DrawInvisible(wxDC& dc, int WXUNUSED(x), int WXUNUSED(y))
{
    dc.SetFont(m_Font);
}








//--------------------------------------------------------------------------------
// wxHtmlWidgetCell
//--------------------------------------------------------------------------------

wxHtmlWidgetCell::wxHtmlWidgetCell(wxWindow *wnd, int w)
{
    int sx, sy;
    m_Wnd = wnd;
    m_Wnd->GetSize(&sx, &sy);
    m_Width = sx, m_Height = sy;
    m_WidthFloat = w;
}


void wxHtmlWidgetCell::Draw(wxDC& WXUNUSED(dc), int WXUNUSED(x), int WXUNUSED(y), int WXUNUSED(view_y1), int WXUNUSED(view_y2))
{
    int absx = 0, absy = 0, stx, sty;
    wxHtmlCell *c = this;

    while (c)
    {
        absx += c->GetPosX();
        absy += c->GetPosY();
        c = c->GetParent();
    }

    ((wxScrolledWindow*)(m_Wnd->GetParent()))->GetViewStart(&stx, &sty);
    m_Wnd->SetSize(absx - wxHTML_SCROLL_STEP * stx, absy  - wxHTML_SCROLL_STEP * sty, m_Width, m_Height);
}



void wxHtmlWidgetCell::DrawInvisible(wxDC& WXUNUSED(dc), int WXUNUSED(x), int WXUNUSED(y))
{
    int absx = 0, absy = 0, stx, sty;
    wxHtmlCell *c = this;

    while (c)
    {
        absx += c->GetPosX();
        absy += c->GetPosY();
        c = c->GetParent();
    }

    ((wxScrolledWindow*)(m_Wnd->GetParent()))->GetViewStart(&stx, &sty);
    m_Wnd->SetSize(absx - wxHTML_SCROLL_STEP * stx, absy  - wxHTML_SCROLL_STEP * sty, m_Width, m_Height);
}



void wxHtmlWidgetCell::Layout(int w)
{
    if (m_WidthFloat != 0)
    {
        m_Width = (w * m_WidthFloat) / 100;
        m_Wnd->SetSize(m_Width, m_Height);
    }

    wxHtmlCell::Layout(w);
}

#endif
