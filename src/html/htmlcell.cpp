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
#include "wx/settings.h"
#include "wx/module.h"

#include <stdlib.h>

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------

static wxCursor *gs_cursorLink = NULL;
static wxCursor *gs_cursorText = NULL;


//-----------------------------------------------------------------------------
// Helper classes
//-----------------------------------------------------------------------------

void wxHtmlSelection::Set(const wxPoint& fromPos, const wxHtmlCell *fromCell,
                          const wxPoint& toPos, const wxHtmlCell *toCell)
{
    m_fromCell = fromCell;
    m_toCell = toCell;
    m_fromPos = fromPos;
    m_toPos = toPos;
}

void wxHtmlSelection::Set(const wxHtmlCell *fromCell, const wxHtmlCell *toCell)
{
    wxPoint p1 = fromCell ? fromCell->GetAbsPos() : wxDefaultPosition;
    wxPoint p2 = toCell ? toCell->GetAbsPos() : wxDefaultPosition;
    if ( toCell )
    {
        p2.x += toCell->GetWidth()-1;
        p2.y += toCell->GetHeight()-1;
    }
    Set(p1, fromCell, p2, toCell);
}

wxColour wxDefaultHtmlRenderingStyle::GetSelectedTextColour(
                                        const wxColour& clr)
{
    return wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT);
}

wxColour wxDefaultHtmlRenderingStyle::GetSelectedTextBgColour(
                                        const wxColour& WXUNUSED(clr))
{
    return wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT);
}


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


wxCursor wxHtmlCell::GetCursor() const
{
    if ( GetLink() )
    {
        if ( !gs_cursorLink )
            gs_cursorLink = new wxCursor(wxCURSOR_HAND);
        return *gs_cursorLink;
    }
    else
        return *wxSTANDARD_CURSOR;
}


bool wxHtmlCell::AdjustPagebreak(int *pagebreak, int* WXUNUSED(known_pagebreaks), int WXUNUSED(number_of_pages)) const
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


wxHtmlCell *wxHtmlCell::FindCellByPos(wxCoord x, wxCoord y,
                                      unsigned flags) const
{
    if ( x >= 0 && x < m_Width && y >= 0 && y < m_Height )
    {
        return wxConstCast(this, wxHtmlCell);
    }
    else
    {
        if ((flags & wxHTML_FIND_NEAREST_AFTER) &&
                (y < 0 || (y == 0 && x <= 0)))
            return wxConstCast(this, wxHtmlCell);
        else if ((flags & wxHTML_FIND_NEAREST_BEFORE) &&
                (y > m_Height-1 || (y == m_Height-1 && x >= m_Width)))
            return wxConstCast(this, wxHtmlCell);
        else
            return NULL;
    }
}


wxPoint wxHtmlCell::GetAbsPos() const
{
    wxPoint p(m_PosX, m_PosY);
    for (wxHtmlCell *parent = m_Parent; parent; parent = parent->m_Parent)
    {
        p.x += parent->m_PosX;
        p.y += parent->m_PosY;
    }
    return p;
}

unsigned wxHtmlCell::GetDepth() const
{
    unsigned d = 0;
    for (wxHtmlCell *p = m_Parent; p; p = p->m_Parent)
        d++;
    return d;
}

bool wxHtmlCell::IsBefore(wxHtmlCell *cell) const
{
    const wxHtmlCell *c1 = this;
    const wxHtmlCell *c2 = cell;
    unsigned d1 = GetDepth();
    unsigned d2 = cell->GetDepth();

    if ( d1 > d2 )
        for (; d1 != d2; d1-- )
            c1 = c1->m_Parent;
    else if ( d1 < d2 )
        for (; d1 != d2; d2-- )
            c2 = c2->m_Parent;

    if ( cell == this )
        return true;

    while ( c1 && c2 )
    {
        if ( c1->m_Parent == c2->m_Parent )
        {
            while ( c1 )
            {
                if ( c1 == c2 )
                    return true;
                c1 = c1->GetNext();
            }
            return false;
        }
        else
        {
            c1 = c1->m_Parent;
            c2 = c2->m_Parent;
        }
    }

    wxFAIL_MSG(_T("Cells are in different trees"));
    return false;
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


// Splits m_Word into up to three parts according to selection, returns
// substring before, in and after selection and the points (in relative coords)
// where s2 and s3 start:
void wxHtmlWordCell::Split(wxDC& dc,
                           const wxPoint& selFrom, const wxPoint& selTo,
                           unsigned& pos1, unsigned& pos2) const
{
    wxPoint pt1 = (selFrom == wxDefaultPosition) ?
                   wxDefaultPosition : selFrom - GetAbsPos();
    wxPoint pt2 = (selTo == wxDefaultPosition) ?
                   wxPoint(m_Width, -1) : selTo - GetAbsPos();

    wxCoord charW, charH;
    unsigned len = m_Word.length();
    unsigned i = 0;
    pos1 = 0;

    // before selection:
    while ( pt1.x > 0 && i < len )
    {
        dc.GetTextExtent(m_Word[i], &charW, &charH);
        pt1.x -= charW;
        if ( pt1.x >= 0 )
        {
            pos1 += charW;
            i++;
        }
    }

    // in selection:
    unsigned j = i;
    pos2 = pos1;
    pt2.x -= pos2;
    while ( pt2.x > 0 && j < len )
    {
        dc.GetTextExtent(m_Word[j], &charW, &charH);
        pt2.x -= charW;
        if ( pt2.x >= 0 )
        {
            pos2 += charW;
            j++;
        }
    }

    pos1 = i;
    pos2 = j;
}

void wxHtmlWordCell::SetSelectionPrivPos(wxDC& dc, wxHtmlSelection *s) const
{
    unsigned p1, p2;

    Split(dc,
          this == s->GetFromCell() ? s->GetFromPos() : wxDefaultPosition,
          this == s->GetToCell() ? s->GetToPos() : wxDefaultPosition,
          p1, p2);

    wxPoint p(0, m_Word.length());

    if ( this == s->GetFromCell() )
        p.x = p1; // selection starts here
    if ( this == s->GetToCell() )
        p.y = p2; // selection ends here

    if ( this == s->GetFromCell() )
        s->SetFromPrivPos(p);
    if ( this == s->GetToCell() )
        s->SetToPrivPos(p);
}


static void SwitchSelState(wxDC& dc, wxHtmlRenderingInfo& info,
                           bool toSelection)
{
    wxColour fg = info.GetState().GetFgColour();
    wxColour bg = info.GetState().GetBgColour();

    if ( toSelection )
    {
        dc.SetBackgroundMode(wxSOLID);
        dc.SetTextForeground(info.GetStyle().GetSelectedTextColour(fg));
        dc.SetTextBackground(info.GetStyle().GetSelectedTextBgColour(bg));
        dc.SetBackground(wxBrush(info.GetStyle().GetSelectedTextBgColour(bg),
                                 wxSOLID));
    }
    else
    {
        dc.SetBackgroundMode(wxTRANSPARENT);
        dc.SetTextForeground(fg);
        dc.SetTextBackground(bg);
        dc.SetBackground(wxBrush(bg, wxSOLID));
    }
}


void wxHtmlWordCell::Draw(wxDC& dc, int x, int y,
                          int WXUNUSED(view_y1), int WXUNUSED(view_y2),
                          wxHtmlRenderingInfo& info)
{
#if 0 // useful for debugging
    dc.DrawRectangle(x+m_PosX,y+m_PosY,m_Width,m_Height);
#endif

    bool drawSelectionAfterCell = false;
    
    if ( info.GetState().GetSelectionState() == wxHTML_SEL_CHANGING )
    {
        // Selection changing, we must draw the word piecewise:
        wxHtmlSelection *s = info.GetSelection();
        wxString txt;
        int w, h;
        int ofs = 0;

        wxPoint priv = (this == s->GetFromCell()) ?
                           s->GetFromPrivPos() : s->GetToPrivPos();

        // NB: this is quite a hack: in order to compute selection boundaries
        //     (in word's characters) we must know current font, which is only
        //     possible inside rendering code. Therefore we update the
        //     information here and store it in wxHtmlSelection so that
        //     ConvertToText can use it later:
        if ( priv == wxDefaultPosition )
        {
            SetSelectionPrivPos(dc, s);
            priv = (this == s->GetFromCell()) ?
                    s->GetFromPrivPos() : s->GetToPrivPos();
        }

        int part1 = priv.x;
        int part2 = priv.y;

        if ( part1 > 0 )
        {
            txt = m_Word.Mid(0, part1);
            dc.DrawText(txt, x + m_PosX, y + m_PosY);
            dc.GetTextExtent(txt, &w, &h);
            ofs += w;
        }

        SwitchSelState(dc, info, true);

        txt = m_Word.Mid(part1, part2-part1);
        dc.DrawText(txt, ofs + x + m_PosX, y + m_PosY);

        if ( (size_t)part2 < m_Word.length() )
        {
            dc.GetTextExtent(txt, &w, &h);
            ofs += w;
            SwitchSelState(dc, info, false);
            txt = m_Word.Mid(part2);
            dc.DrawText(txt, ofs + x + m_PosX, y + m_PosY);
        }
        else
            drawSelectionAfterCell = true;
    }
    else
    {
        wxHtmlSelectionState selstate = info.GetState().GetSelectionState();
        // Not changing selection state, draw the word in single mode:
        if ( selstate != wxHTML_SEL_OUT &&
             dc.GetBackgroundMode() != wxSOLID )
        {
            SwitchSelState(dc, info, true);
        }
        else if ( selstate == wxHTML_SEL_OUT &&
                  dc.GetBackgroundMode() == wxSOLID )
        {
            SwitchSelState(dc, info, false);
        }
        dc.DrawText(m_Word, x + m_PosX, y + m_PosY);
        drawSelectionAfterCell = (selstate != wxHTML_SEL_OUT);
    }

    // NB: If the text is justified then there is usually some free space
    //     between adjacent cells and drawing the selection only onto cells
    //     would result in ugly unselected spaces. The code below detects
    //     this special case and renders the selection *outside* the sell,
    //     too.
    if ( m_Parent->GetAlignHor() == wxHTML_ALIGN_JUSTIFY &&
         drawSelectionAfterCell )
    {
        wxHtmlCell *nextCell = m_Next;
        while ( nextCell && nextCell->IsFormattingCell() )
            nextCell = nextCell->GetNext();
        if ( nextCell )
        {
            int nextX = nextCell->GetPosX();
            if ( m_PosX + m_Width < nextX )
            {
                dc.SetBrush(dc.GetBackground());
                dc.SetPen(*wxTRANSPARENT_PEN);
                dc.DrawRectangle(x + m_PosX + m_Width, y + m_PosY,
                                 nextX - m_PosX - m_Width, m_Height);
            }
        }
    }
}


wxString wxHtmlWordCell::ConvertToText(wxHtmlSelection *s) const
{
    if ( s && (this == s->GetFromCell() || this == s->GetToCell()) )
    {
        wxPoint priv = this == s->GetFromCell() ? s->GetFromPrivPos()
                                                : s->GetToPrivPos();

        // VZ: we may be called before we had a chance to re-render ourselves
        //     and in this case GetFrom/ToPrivPos() is not set yet -- assume
        //     that this only happens in case of a double/triple click (which
        //     seems to be the case now) and so it makes sense to select the
        //     entire contents of the cell in this case
        //
        // TODO: but this really needs to be fixed in some better way later...
        if ( priv != wxDefaultPosition )
        {
            int part1 = priv.x;
            int part2 = priv.y;
            return m_Word.Mid(part1, part2-part1);
        }
        //else: return the whole word below
    }

    return m_Word;
}

wxCursor wxHtmlWordCell::GetCursor() const
{
    if ( !GetLink() )
    {
        if ( !gs_cursorText )
            gs_cursorText = new wxCursor(wxCURSOR_IBEAM);
        return *gs_cursorText;
    }
    else
        return wxHtmlCell::GetCursor();
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



bool wxHtmlContainerCell::AdjustPagebreak(int *pagebreak, int* known_pagebreaks, int number_of_pages) const
{
    if (!m_CanLiveOnPagebreak)
        return wxHtmlCell::AdjustPagebreak(pagebreak, known_pagebreaks, number_of_pages);

    else
    {
        wxHtmlCell *c = GetFirstChild();
        bool rt = FALSE;
        int pbrk = *pagebreak - m_PosY;

        while (c)
        {
            if (c->AdjustPagebreak(&pbrk, known_pagebreaks, number_of_pages))
                rt = TRUE;
            c = c->GetNext();
        }
        if (rt)
            *pagebreak = pbrk + m_PosY;
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

void wxHtmlContainerCell::UpdateRenderingStatePre(wxHtmlRenderingInfo& info,
                                                  wxHtmlCell *cell) const
{
    wxHtmlSelection *s = info.GetSelection();
    if (!s) return;
    if (s->GetFromCell() == cell || s->GetToCell() == cell)
    {
        info.GetState().SetSelectionState(wxHTML_SEL_CHANGING);
    }
}

void wxHtmlContainerCell::UpdateRenderingStatePost(wxHtmlRenderingInfo& info,
                                                   wxHtmlCell *cell) const
{
    wxHtmlSelection *s = info.GetSelection();
    if (!s) return;
    if (s->GetToCell() == cell)
        info.GetState().SetSelectionState(wxHTML_SEL_OUT);
    else if (s->GetFromCell() == cell)
        info.GetState().SetSelectionState(wxHTML_SEL_IN);
}

#define mMin(a, b) (((a) < (b)) ? (a) : (b))
#define mMax(a, b) (((a) < (b)) ? (b) : (a))

void wxHtmlContainerCell::Draw(wxDC& dc, int x, int y, int view_y1, int view_y2,
                               wxHtmlRenderingInfo& info)
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
            // draw container's contents:
            for (wxHtmlCell *cell = m_Cells; cell; cell = cell->GetNext())
            {
                UpdateRenderingStatePre(info, cell);
                cell->Draw(dc,
                           x + m_PosX, y + m_PosY, view_y1, view_y2,
                           info);
                UpdateRenderingStatePost(info, cell);
            }
        }
    }
    // container invisible, just proceed font+color changing:
    else
    {
        DrawInvisible(dc, x, y, info);
    }
}



void wxHtmlContainerCell::DrawInvisible(wxDC& dc, int x, int y,
                                        wxHtmlRenderingInfo& info)
{
    if (m_Cells)
    {
        for (wxHtmlCell *cell = m_Cells; cell; cell = cell->GetNext())
        {
            UpdateRenderingStatePre(info, cell);
            cell->DrawInvisible(dc, x + m_PosX, y + m_PosY, info);
            UpdateRenderingStatePost(info, cell);
        }
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


wxHtmlCell *wxHtmlContainerCell::FindCellByPos(wxCoord x, wxCoord y,
                                               unsigned flags) const
{
    if ( flags & wxHTML_FIND_EXACT )
    {
        for ( const wxHtmlCell *cell = m_Cells; cell; cell = cell->GetNext() )
        {
            int cx = cell->GetPosX(),
                cy = cell->GetPosY();

            if ( (cx <= x) && (cx + cell->GetWidth() > x) &&
                 (cy <= y) && (cy + cell->GetHeight() > y) )
            {
                return cell->FindCellByPos(x - cx, y - cy, flags);
            }
        }
    }
    else if ( flags & wxHTML_FIND_NEAREST_AFTER )
    {
        wxHtmlCell *c;
        int y2;
        for ( const wxHtmlCell *cell = m_Cells; cell; cell = cell->GetNext() )
        {
            y2 = cell->GetPosY() + cell->GetHeight() - 1;
            if (y2 < y || (y2 == y && cell->GetPosX()+cell->GetWidth()-1 < x))
                continue;
            c = cell->FindCellByPos(x - cell->GetPosX(), y - cell->GetPosY(),
                                    flags);
            if (c) return c;
        }
    }
    else if ( flags & wxHTML_FIND_NEAREST_BEFORE )
    {
        wxHtmlCell *c2, *c = NULL;
        for ( const wxHtmlCell *cell = m_Cells; cell; cell = cell->GetNext() )
        {
            if (cell->GetPosY() > y ||
                    (cell->GetPosY() == y && cell->GetPosX() > x))
                break;
            c2 = cell->FindCellByPos(x - cell->GetPosX(), y - cell->GetPosY(),
                                     flags);
            if (c2)
                c = c2;
        }
        if (c) return c;
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


wxHtmlCell *wxHtmlContainerCell::GetFirstTerminal() const
{
    if ( m_Cells )
    {
        wxHtmlCell *c2;
        for (wxHtmlCell *c = m_Cells; c; c = c->GetNext())
        {
            c2 = c->GetFirstTerminal();
            if ( c2 )
                return c2;
        }
    }
    return NULL;
}

wxHtmlCell *wxHtmlContainerCell::GetLastTerminal() const
{
    if ( m_Cells )
    {
        // most common case first:
        wxHtmlCell *c = m_LastCell->GetLastTerminal();
        if ( c )
            return c;

        wxHtmlCell *c2 = NULL;
        for (c = m_Cells; c; c = c->GetNext())
            c2 = c->GetLastTerminal();
        return c2;
    }
    else
        return NULL;
}




// --------------------------------------------------------------------------
// wxHtmlColourCell
// --------------------------------------------------------------------------

void wxHtmlColourCell::Draw(wxDC& dc,
                            int x, int y,
                            int WXUNUSED(view_y1), int WXUNUSED(view_y2),
                            wxHtmlRenderingInfo& info)
{
    DrawInvisible(dc, x, y, info);
}

void wxHtmlColourCell::DrawInvisible(wxDC& dc,
                                     int WXUNUSED(x), int WXUNUSED(y),
                                     wxHtmlRenderingInfo& info)
{
    wxHtmlRenderingState& state = info.GetState();
    if (m_Flags & wxHTML_CLR_FOREGROUND)
    {
        state.SetFgColour(m_Colour);
        if (state.GetSelectionState() != wxHTML_SEL_IN)
            dc.SetTextForeground(m_Colour);
        else
            dc.SetTextForeground(
                    info.GetStyle().GetSelectedTextColour(m_Colour));
    }
    if (m_Flags & wxHTML_CLR_BACKGROUND)
    {
        state.SetBgColour(m_Colour);
        if (state.GetSelectionState() != wxHTML_SEL_IN)
        {
            dc.SetTextBackground(m_Colour);
            dc.SetBackground(wxBrush(m_Colour, wxSOLID));
        }
        else
        {
            wxColour c = info.GetStyle().GetSelectedTextBgColour(m_Colour);
            dc.SetTextBackground(c);
            dc.SetBackground(wxBrush(c, wxSOLID));
        }
    }
}




// ---------------------------------------------------------------------------
// wxHtmlFontCell
// ---------------------------------------------------------------------------

void wxHtmlFontCell::Draw(wxDC& dc,
                          int WXUNUSED(x), int WXUNUSED(y),
                          int WXUNUSED(view_y1), int WXUNUSED(view_y2),
                          wxHtmlRenderingInfo& WXUNUSED(info))
{
    dc.SetFont(m_Font);
}

void wxHtmlFontCell::DrawInvisible(wxDC& dc, int WXUNUSED(x), int WXUNUSED(y),
                                   wxHtmlRenderingInfo& WXUNUSED(info))
{
    dc.SetFont(m_Font);
}








// ---------------------------------------------------------------------------
// wxHtmlWidgetCell
// ---------------------------------------------------------------------------

wxHtmlWidgetCell::wxHtmlWidgetCell(wxWindow *wnd, int w)
{
    int sx, sy;
    m_Wnd = wnd;
    m_Wnd->GetSize(&sx, &sy);
    m_Width = sx, m_Height = sy;
    m_WidthFloat = w;
}


void wxHtmlWidgetCell::Draw(wxDC& WXUNUSED(dc),
                            int WXUNUSED(x), int WXUNUSED(y),
                            int WXUNUSED(view_y1), int WXUNUSED(view_y2),
                            wxHtmlRenderingInfo& WXUNUSED(info))
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



void wxHtmlWidgetCell::DrawInvisible(wxDC& WXUNUSED(dc),
                                     int WXUNUSED(x), int WXUNUSED(y),
                                     wxHtmlRenderingInfo& WXUNUSED(info))
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



// ----------------------------------------------------------------------------
// wxHtmlTerminalCellsInterator
// ----------------------------------------------------------------------------

const wxHtmlCell* wxHtmlTerminalCellsInterator::operator++()
{
    if ( !m_pos )
        return NULL;

    do
    {
        if ( m_pos == m_to )
        {
            m_pos = NULL;
            return NULL;
        }

        if ( m_pos->GetNext() )
            m_pos = m_pos->GetNext();
        else
        {
            // we must go up the hierarchy until we reach container where this
            // is not the last child, and then go down to first terminal cell:
            while ( m_pos->GetNext() == NULL )
            {
                m_pos = m_pos->GetParent();
                if ( !m_pos )
                    return NULL;
            }
            m_pos = m_pos->GetNext();
        }
        while ( m_pos->GetFirstChild() != NULL )
            m_pos = m_pos->GetFirstChild();
    } while ( !m_pos->IsTerminalCell() );

    return m_pos;
}







//-----------------------------------------------------------------------------
// Cleanup
//-----------------------------------------------------------------------------

class wxHtmlCellModule: public wxModule
{
DECLARE_DYNAMIC_CLASS(wxHtmlCellModule)
public:
    wxHtmlCellModule() : wxModule() {}
    bool OnInit() { return true; }
    void OnExit()
    {
        wxDELETE(gs_cursorLink);
        wxDELETE(gs_cursorText);
    }
};

IMPLEMENT_DYNAMIC_CLASS(wxHtmlCellModule, wxModule)

#endif
