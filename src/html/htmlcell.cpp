/////////////////////////////////////////////////////////////////////////////
// Name:        src/html/htmlcell.cpp
// Purpose:     wxHtmlCell - basic element of HTML output
// Author:      Vaclav Slavik
// Copyright:   (c) 1999 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_HTML && wxUSE_STREAMS

#ifndef WX_PRECOMP
    #include "wx/dynarray.h"
    #include "wx/brush.h"
    #include "wx/colour.h"
    #include "wx/dc.h"
    #include "wx/settings.h"
    #include "wx/module.h"
    #include "wx/wxcrtvararg.h"
#endif

#include "wx/html/htmlcell.h"
#include "wx/html/htmlwin.h"

#include <stdlib.h>

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
        p2.x += toCell->GetWidth();
        p2.y += toCell->GetHeight();
    }
    Set(p1, fromCell, p2, toCell);
}

wxColour
wxDefaultHtmlRenderingStyle::
GetSelectedTextColour(const wxColour& WXUNUSED(clr))
{
    return wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT);
}

wxColour
wxDefaultHtmlRenderingStyle::
GetSelectedTextBgColour(const wxColour& WXUNUSED(clr))
{
    // By default we use the fixed standard selection colour, but if we're
    // associated with a window use the colour appropriate for the window
    // state, i.e. grey out selection when it's not in focus.

    return wxSystemSettings::GetColour(!m_wnd || m_wnd->HasFocus() ?
        wxSYS_COLOUR_HIGHLIGHT : wxSYS_COLOUR_BTNSHADOW);
}


//-----------------------------------------------------------------------------
// wxHtmlCell
//-----------------------------------------------------------------------------

wxIMPLEMENT_ABSTRACT_CLASS(wxHtmlCell, wxObject);

wxHtmlCell::wxHtmlCell() : wxObject()
{
    m_Next = NULL;
    m_Parent = NULL;
    m_Width = m_Height = m_Descent = 0;
    m_ScriptMode = wxHTML_SCRIPT_NORMAL;        // <sub> or <sup> mode
    m_ScriptBaseline = 0;                       // <sub> or <sup> baseline
    m_CanLiveOnPagebreak = true;
    m_Link = NULL;
}

wxHtmlCell::~wxHtmlCell()
{
    delete m_Link;
}

// Update the descent value when we are in a <sub> or <sup>.
// prevbase is the parent base
void wxHtmlCell::SetScriptMode(wxHtmlScriptMode mode, long previousBase)
{
    m_ScriptMode = mode;

    if (mode == wxHTML_SCRIPT_SUP)
        m_ScriptBaseline = previousBase - (m_Height + 1) / 2;
    else if (mode == wxHTML_SCRIPT_SUB)
        m_ScriptBaseline = previousBase + (m_Height + 1) / 6;
    else
        m_ScriptBaseline = 0;

    m_Descent += m_ScriptBaseline;
}

bool wxHtmlCell::ProcessMouseClick(wxHtmlWindowInterface *window,
                                   const wxPoint& pos,
                                   const wxMouseEvent& event)
{
    wxCHECK_MSG( window, false, wxT("window interface must be provided") );

    wxHtmlLinkInfo *lnk = GetLink(pos.x, pos.y);
    bool retval = false;

    if (lnk)
    {
        wxHtmlLinkInfo lnk2(*lnk);
        lnk2.SetEvent(&event);
        lnk2.SetHtmlCell(this);

        window->OnHTMLLinkClicked(lnk2);
        retval = true;
    }

    return retval;
}

wxCursor
wxHtmlCell::GetMouseCursor(wxHtmlWindowInterface* WXUNUSED(window)) const
{
    // This is never called directly, only from GetMouseCursorAt() and we
    // return an invalid cursor by default to let it delegate to the window.
    return wxNullCursor;
}

wxCursor
wxHtmlCell::GetMouseCursorAt(wxHtmlWindowInterface *window,
                             const wxPoint& relPos) const
{
    const wxCursor curCell = GetMouseCursor(window);
    if ( curCell.IsOk() )
      return curCell;

    if ( GetLink(relPos.x, relPos.y) )
    {
        return window->GetHTMLCursor(wxHtmlWindowInterface::HTMLCursor_Link);
    }
    else
    {
        return window->GetHTMLCursor(wxHtmlWindowInterface::HTMLCursor_Default);
    }
}


bool
wxHtmlCell::AdjustPagebreak(int *pagebreak, int pageHeight) const
{
    // Notice that we always break the cells bigger than the page height here
    // as otherwise we wouldn't be able to break them at all.
    if ( m_Height <= pageHeight &&
            (!m_CanLiveOnPagebreak &&
                m_PosY < *pagebreak && m_PosY + m_Height > *pagebreak) )
    {
        *pagebreak = m_PosY;
        return true;
    }

    return false;
}



void wxHtmlCell::SetLink(const wxHtmlLinkInfo& link)
{
    wxDELETE(m_Link);
    if (!link.GetHref().empty())
        m_Link = new wxHtmlLinkInfo(link);
}


void wxHtmlCell::Layout(int WXUNUSED(w))
{
    SetPos(0, 0);
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
                (y < 0 || (y < 0+m_Height && x < 0+m_Width)))
            return wxConstCast(this, wxHtmlCell);
        else if ((flags & wxHTML_FIND_NEAREST_BEFORE) &&
                (y >= 0+m_Height || (y >= 0 && x >= 0)))
            return wxConstCast(this, wxHtmlCell);
        else
            return NULL;
    }
}


wxPoint wxHtmlCell::GetAbsPos(const wxHtmlCell *rootCell) const
{
    wxPoint p(m_PosX, m_PosY);
    for (const wxHtmlCell *parent = m_Parent; parent && parent != rootCell;
         parent = parent->m_Parent)
    {
        p.x += parent->m_PosX;
        p.y += parent->m_PosY;
    }
    return p;
}

wxRect wxHtmlCell::GetRect(const wxHtmlCell* rootCell) const
{
    return wxRect(GetAbsPos(rootCell), wxSize(m_Width, m_Height));
}

wxHtmlCell *wxHtmlCell::GetRootCell() const
{
    wxHtmlCell *c = wxConstCast(this, wxHtmlCell);
    while ( c->m_Parent )
        c = c->m_Parent;
    return c;
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

    wxFAIL_MSG(wxT("Cells are in different trees"));
    return false;
}

wxString wxHtmlCell::GetDescription() const
{
    return GetClassInfo()->GetClassName();
}

wxString wxHtmlCell::Dump(int indent) const
{
    wxString s(' ', indent);
    s += wxString::Format("%s(%p) at (%d, %d) %dx%d",
                          GetDescription(), this,
                          m_PosX, m_PosY, GetMaxTotalWidth(), m_Height);
    if ( !m_id.empty() )
        s += wxString::Format(" [id=%s]", m_id);

    return s;
}

//-----------------------------------------------------------------------------
// wxHtmlWordCell
//-----------------------------------------------------------------------------

wxIMPLEMENT_ABSTRACT_CLASS(wxHtmlWordCell, wxHtmlCell);

wxHtmlWordCell::wxHtmlWordCell(const wxString& word, const wxDC& dc) : wxHtmlCell()
    , m_Word(word)
{
    wxCoord w, h, d;
    dc.GetTextExtent(m_Word, &w, &h, &d);
    m_Width = w;
    m_Height = h;
    m_Descent = d;
    SetCanLiveOnPagebreak(false);
    m_allowLinebreak = true;
}

void wxHtmlWordCell::SetPreviousWord(wxHtmlWordCell *cell)
{
    if ( cell && m_Parent == cell->m_Parent &&
         !wxIsspace(cell->m_Word.Last()) && !wxIsspace(m_Word[0u]) )
    {
        m_allowLinebreak = false;
    }
}

// Splits m_Word into up to three parts according to selection, returns
// substring before, in and after selection and the points (in relative coords)
// where s2 and s3 start:
void wxHtmlWordCell::Split(const wxDC& dc,
                           const wxPoint& selFrom, const wxPoint& selTo,
                           unsigned& pos1, unsigned& pos2,
                           unsigned& ext1, unsigned& ext2) const
{
    wxPoint pt1 = (selFrom == wxDefaultPosition) ?
                   wxDefaultPosition : selFrom - GetAbsPos();
    wxPoint pt2 = (selTo == wxDefaultPosition) ?
                   wxPoint(m_Width, wxDefaultCoord) : selTo - GetAbsPos();

    // if the selection is entirely within this cell, make sure pt1 < pt2 in
    // order to make the rest of this function simpler:
    if ( selFrom != wxDefaultPosition && selTo != wxDefaultPosition &&
         selFrom.x > selTo.x )
    {
        wxPoint tmp = pt1;
        pt1 = pt2;
        pt2 = tmp;
    }

    unsigned len = m_Word.length();
    unsigned i = 0;
    pos1 = 0;

    // adjust for cases when the start/end position is completely
    // outside the cell:
    if ( pt1.y < 0 )
        pt1.x = 0;
    if ( pt2.y >= m_Height )
        pt2.x = m_Width;

    // before selection:
    // (include character under caret only if in first half of width)
    wxArrayInt widths ;
    dc.GetPartialTextExtents(m_Word,widths) ;
    while( i < len && pt1.x >= widths[i] )
        i++ ;
    if ( i < len )
    {
        int charW = (i > 0) ? widths[i] - widths[i-1] : widths[i];
        if ( widths[i] - pt1.x < charW/2 )
            i++;
    }

    // in selection:
    // (include character under caret only if in first half of width)
    unsigned j = i;
    while( j < len && pt2.x >= widths[j] )
        j++ ;
    if ( j < len )
    {
        int charW = (j > 0) ? widths[j] - widths[j-1] : widths[j];
        if ( widths[j] - pt2.x < charW/2 )
            j++;
    }

    pos1 = i;
    pos2 = j;

    wxASSERT( pos2 >= pos1 );

    ext1 = pos1 == 0 ? 0 : (pos1 < widths.size() ? widths[pos1-1] : widths.Last());
    ext2 = pos2 == 0 ? 0 : (pos2 < widths.size() ? widths[pos2-1] : widths.Last());
}

void wxHtmlWordCell::SetSelectionPrivPos(const wxDC& dc, wxHtmlSelection *s) const
{
    unsigned p1, p2, ext1, ext2;

    Split(dc,
          this == s->GetFromCell() ? s->GetFromPos() : wxDefaultPosition,
          this == s->GetToCell() ? s->GetToPos() : wxDefaultPosition,
          p1, p2, ext1, ext2);

    if ( this == s->GetFromCell() )
    {
        s->SetFromCharacterPos(p1); // selection starts here
        s->SetExtentBeforeSelection(ext1);
    }
    if ( this == s->GetToCell() )
    {
        s->SetToCharacterPos(p2); // selection ends here
        s->SetExtentBeforeSelectionEnd(ext2);
    }
}


static void SwitchSelState(wxDC& dc, wxHtmlRenderingInfo& info,
                           bool toSelection)
{
    wxColour fg = info.GetState().GetFgColour();
    wxColour bg = info.GetState().GetBgColour();

    if ( toSelection )
    {
        dc.SetBackgroundMode(wxBRUSHSTYLE_SOLID);
        dc.SetTextForeground(info.GetStyle().GetSelectedTextColour(fg));
        dc.SetTextBackground(info.GetStyle().GetSelectedTextBgColour(bg));
        dc.SetBackground(info.GetStyle().GetSelectedTextBgColour(bg));
    }
    else
    {
        const int mode = info.GetState().GetBgMode();
        dc.SetBackgroundMode(mode);
        dc.SetTextForeground(fg);
        dc.SetTextBackground(bg);
        if ( mode != wxBRUSHSTYLE_TRANSPARENT )
            dc.SetBackground(bg);
    }
}


void wxHtmlWordCell::Draw(wxDC& dc, int x, int y,
                          int WXUNUSED(view_y1), int WXUNUSED(view_y2),
                          wxHtmlRenderingInfo& info)
{
#if 0 // useful for debugging
    dc.SetPen(*wxBLACK_PEN);
    dc.DrawRectangle(x+m_PosX,y+m_PosY,m_Width /* VZ: +1? */ ,m_Height);
#endif

    bool drawSelectionAfterCell = false;

    if ( info.GetState().GetSelectionState() == wxHTML_SEL_CHANGING )
    {
        // Selection changing, we must draw the word piecewise:
        wxHtmlSelection *s = info.GetSelection();
        wxString txt;
        int ofs = 0;

        // NB: this is quite a hack: in order to compute selection boundaries
        //     (in word's characters) we must know current font, which is only
        //     possible inside rendering code. Therefore we update the
        //     information here and store it in wxHtmlSelection so that
        //     ConvertToText can use it later:
        if ( !s->AreFromToCharacterPosSet () )
        {
            SetSelectionPrivPos(dc, s);
        }

        int part1 = s->GetFromCell()==this ? s->GetFromCharacterPos() : 0;
        int part2 = s->GetToCell()==this   ? s->GetToCharacterPos()   : m_Word.Length();

        if ( part1 > 0 )
        {
            txt = m_Word.Mid(0, part1);
            dc.DrawText(txt, x + m_PosX, y + m_PosY);
            ofs += s->GetExtentBeforeSelection();
        }

        SwitchSelState(dc, info, true);

        txt = m_Word.Mid(part1, part2-part1);
        dc.DrawText(txt, ofs + x + m_PosX, y + m_PosY);

        if ( (size_t)part2 < m_Word.length() )
        {
            SwitchSelState(dc, info, false);
            txt = m_Word.Mid(part2);
            dc.DrawText(txt, x + m_PosX + s->GetExtentBeforeSelectionEnd(), y + m_PosY);
        }
        else
            drawSelectionAfterCell = true;
    }
    else
    {
        wxHtmlSelectionState selstate = info.GetState().GetSelectionState();
        // Not changing selection state, draw the word in single mode:
        SwitchSelState(dc, info, selstate != wxHTML_SEL_OUT);

        // This is a quite horrible hack but it fixes a nasty user-visible
        // problem: when drawing underlined text, which is common in wxHTML as
        // all links are underlined, there is a 1 pixel gap between the
        // underlines because we draw separate words in separate DrawText()
        // calls. The right thing to do would be to draw all of them appearing
        // on the same line at once (this would probably be more efficient as
        // well), but this doesn't seem simple to do, so instead we just draw
        // an extra space at a negative offset to ensure that the underline
        // spans the previous pixel and so overlaps the one from the previous
        // word, if any.
        const bool prevUnderlined = info.WasPreviousUnderlined();
        const bool thisUnderlined = dc.GetFont().GetUnderlined();
        if ( prevUnderlined && thisUnderlined )
        {
            dc.DrawText(wxS(" "), x + m_PosX - 1, y + m_PosY);
        }
        info.SetCurrentUnderlined(thisUnderlined);

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

wxCursor wxHtmlWordCell::GetMouseCursor(wxHtmlWindowInterface *window) const
{
    if ( !GetLink() )
    {
        return window->GetHTMLCursor(wxHtmlWindowInterface::HTMLCursor_Text);
    }
    else
    {
        return wxHtmlCell::GetMouseCursor(window);
    }
}

wxString wxHtmlWordCell::ConvertToText(wxHtmlSelection *s) const
{
    if ( s && (this == s->GetFromCell() || this == s->GetToCell()) )
    {
        // VZ: we may be called before we had a chance to re-render ourselves
        //     and in this case GetFrom/ToPrivPos() is not set yet -- assume
        //     that this only happens in case of a double/triple click (which
        //     seems to be the case now) and so it makes sense to select the
        //     entire contents of the cell in this case
        //
        // TODO: but this really needs to be fixed in some better way later...
        if ( s->AreFromToCharacterPosSet() )
        {
            const int part1 = s->GetFromCell()==this ? s->GetFromCharacterPos() : 0;
            const int part2 = s->GetToCell()==this   ? s->GetToCharacterPos()   : m_Word.Length();
            if ( part1 == part2 )
                return wxEmptyString;
            return GetPartAsText(part1, part2);
        }
        //else: return the whole word below
    }

    return GetAllAsText();
}

wxString wxHtmlWordWithTabsCell::GetAllAsText() const
{
    return m_wordOrig;
}

wxString wxHtmlWordWithTabsCell::GetPartAsText(int begin, int end) const
{
    // NB: The 'begin' and 'end' positions are in the _displayed_ text
    //     (stored in m_Word) and not in the text with tabs that should
    //     be copied to clipboard (m_wordOrig).
    //
    // NB: Because selection is performed on displayed text, it's possible
    //     to select e.g. "half of TAB character" -- IOW, 'begin' and 'end'
    //     may be in the middle of TAB character expansion into ' 's. In this
    //     case, we copy the TAB character to clipboard once.

    wxASSERT( begin < end );

    const unsigned SPACES_PER_TAB = 8;

    wxString sel;

    int pos = 0;
    wxString::const_iterator i = m_wordOrig.begin();

    // find the beginning of text to copy:
    for ( ; pos < begin; ++i )
    {
        if ( *i == '\t' )
        {
            pos += 8 - (m_linepos + pos) % SPACES_PER_TAB;
            if ( pos >= begin )
            {
                sel += '\t';
            }
        }
        else
        {
            ++pos;
        }
    }

    // copy the content until we reach 'end':
    for ( ; pos < end; ++i )
    {
        const wxChar c = *i;
        sel += c;

        if ( c == '\t' )
            pos += 8 - (m_linepos + pos) % SPACES_PER_TAB;
        else
            ++pos;
    }

    return sel;
}

wxString wxHtmlWordCell::GetDescription() const
{
    wxString s;
    s = wxString::Format("wxHtmlWordCell(%s)", m_Word);
    if ( !m_allowLinebreak )
        s += " no line break";

    return s;
}


//-----------------------------------------------------------------------------
// wxHtmlContainerCell
//-----------------------------------------------------------------------------

wxIMPLEMENT_ABSTRACT_CLASS(wxHtmlContainerCell, wxHtmlCell);

wxHtmlContainerCell::wxHtmlContainerCell(wxHtmlContainerCell *parent) : wxHtmlCell()
{
    m_Cells = m_LastCell = NULL;
    m_Parent = parent;
    m_MaxTotalWidth = 0;
    if (m_Parent) m_Parent->InsertCell(this);
    m_AlignHor = wxHTML_ALIGN_LEFT;
    m_AlignVer = wxHTML_ALIGN_BOTTOM;
    m_IndentLeft = m_IndentRight = m_IndentTop = m_IndentBottom = 0;
    m_WidthFloat = 100; m_WidthFloatUnits = wxHTML_UNITS_PERCENT;
    m_Border = 0;
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
    bool p = false;
    if (ind & wxHTML_INDENT_LEFT) p = m_IndentLeft < 0;
    else if (ind & wxHTML_INDENT_RIGHT) p = m_IndentRight < 0;
    else if (ind & wxHTML_INDENT_TOP) p = m_IndentTop < 0;
    else if (ind & wxHTML_INDENT_BOTTOM) p = m_IndentBottom < 0;
    if (p) return wxHTML_UNITS_PERCENT;
    else return wxHTML_UNITS_PIXELS;
}


bool
wxHtmlContainerCell::AdjustPagebreak(int *pagebreak, int pageHeight) const
{
    if (!m_CanLiveOnPagebreak)
        return wxHtmlCell::AdjustPagebreak(pagebreak, pageHeight);

    bool rt = false;
    int pbrk = *pagebreak - m_PosY;

    for ( wxHtmlCell *c = GetFirstChild(); c; c = c->GetNext() )
    {
        if (c->AdjustPagebreak(&pbrk, pageHeight))
            rt = true;
    }
    if (rt)
        *pagebreak = pbrk + m_PosY;
    return rt;
}


void wxHtmlContainerCell::Layout(int w)
{
    wxHtmlCell::Layout(w);

    if (m_LastLayout == w)
        return;
    m_LastLayout = w;

    // VS: Any attempt to layout with negative or zero width leads to hell,
    // but we can't ignore such attempts completely, since it sometimes
    // happen (e.g. when trying how small a table can be), so use at least one
    // pixel width, this will at least give us the correct height sometimes.
    if (w < 1)
        w = 1;

    wxHtmlCell *nextCell;
    long xpos = 0, ypos = m_IndentTop;
    int xdelta = 0, ybasicpos = 0;
    int s_width, s_indent;
    int ysizeup = 0, ysizedown = 0;
    int MaxLineWidth = 0;
    int curLineWidth = 0;
    m_MaxTotalWidth = 0;


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

    LAYOUT :

    */

    // adjust indentation:
    s_indent = (m_IndentLeft < 0) ? (-m_IndentLeft * m_Width / 100) : m_IndentLeft;
    s_width = m_Width - s_indent - ((m_IndentRight < 0) ? (-m_IndentRight * m_Width / 100) : m_IndentRight);

    // my own layout:
    wxHtmlCell *cell = m_Cells,
               *line = m_Cells;
    while (cell != NULL)
    {
        switch (m_AlignVer)
        {
            case wxHTML_ALIGN_TOP :      ybasicpos = 0; break;
            case wxHTML_ALIGN_BOTTOM :   ybasicpos = - cell->GetHeight(); break;
            case wxHTML_ALIGN_CENTER :   ybasicpos = - cell->GetHeight() / 2; break;
        }
        int ydiff;
        ydiff = cell->GetHeight() + ybasicpos;

        if (cell->GetDescent() + ydiff > ysizedown) ysizedown = cell->GetDescent() + ydiff;
        if (ybasicpos + cell->GetDescent() < -ysizeup) ysizeup = - (ybasicpos + cell->GetDescent());

        // layout nonbreakable run of cells:
        cell->SetPos(xpos, ybasicpos + cell->GetDescent());
        xpos += cell->GetWidth();
        if (!cell->IsTerminalCell())
        {
            // Container cell indicates new line
            if (curLineWidth > m_MaxTotalWidth)
                m_MaxTotalWidth = curLineWidth;

            if (cell->GetMaxTotalWidth() > m_MaxTotalWidth)
                m_MaxTotalWidth = cell->GetMaxTotalWidth();

            curLineWidth = 0;
        }
        else
            // Normal cell, add maximum cell width to line width
            curLineWidth += cell->GetMaxTotalWidth();

        cell = cell->GetNext();

        // compute length of the next word that would be added:
        int nextWordWidth;
        nextWordWidth = 0;
        if (cell)
        {
            nextCell = cell;
            do
            {
                nextWordWidth += nextCell->GetWidth();
                nextCell = nextCell->GetNext();
            } while (nextCell && !nextCell->IsLinebreakAllowed());
        }

        // force new line if occurred:
        if ((cell == NULL) ||
            (xpos + nextWordWidth > s_width && cell->IsLinebreakAllowed()))
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
            {
                while (line != cell)
                {
                    line->SetPos(line->GetPosX() + xdelta,
                                   ypos + line->GetPosY());
                    line = line->GetNext();
                }
            }
            else // align == justify
            {
                // we have to distribute the extra horz space between the cells
                // on this line

                // an added complication is that some cells have fixed size and
                // shouldn't get any increment (it so happens that these cells
                // also don't allow line break on them which provides with an
                // easy way to test for this) -- and neither should the cells
                // adjacent to them as this could result in a visible space
                // between two cells separated by, e.g. font change, cell which
                // is wrong

                int step = s_width - xpos;
                if ( step > 0 )
                {
                    // first count the cells which will get extra space
                    int total = -1;

                    const wxHtmlCell *c;
                    if ( line != cell )
                    {
                        for ( c = line; c != cell; c = c->GetNext() )
                        {
                            if ( c->IsLinebreakAllowed() )
                            {
                                total++;
                            }
                        }
                    }

                    // and now extra space to those cells which merit it
                    if ( total )
                    {
                        // first visible cell on line is not moved:
                        while (line !=cell && !line->IsLinebreakAllowed())
                        {
                            line->SetPos(line->GetPosX() + s_indent,
                                         line->GetPosY() + ypos);
                            line = line->GetNext();
                        }

                        if (line != cell)
                        {
                            line->SetPos(line->GetPosX() + s_indent,
                                         line->GetPosY() + ypos);

                            line = line->GetNext();
                        }

                        for ( int n = 0; line != cell; line = line->GetNext() )
                        {
                            if ( line->IsLinebreakAllowed() )
                            {
                                // offset the next cell relative to this one
                                // thus increasing our size
                                n++;
                            }

                            line->SetPos(line->GetPosX() + s_indent +
                                           ((n * step) / total),
                                           line->GetPosY() + ypos);
                        }
                    }
                    else
                    {
                        // this will cause the code to enter "else branch" below:
                        step = 0;
                    }
                }
                // else branch:
                if ( step <= 0 ) // no extra space to distribute
                {
                    // just set the indent properly
                    while (line != cell)
                    {
                        line->SetPos(line->GetPosX() + s_indent,
                                     line->GetPosY() + ypos);
                        line = line->GetNext();
                    }
                }
            }

            ypos += ysizedown;
            xpos = 0;
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

    if (curLineWidth > m_MaxTotalWidth)
        m_MaxTotalWidth = curLineWidth;

    m_MaxTotalWidth += s_indent + ((m_IndentRight < 0) ? (-m_IndentRight * m_Width / 100) : m_IndentRight);
    MaxLineWidth += s_indent + ((m_IndentRight < 0) ? (-m_IndentRight * m_Width / 100) : m_IndentRight);
    if (m_Width < MaxLineWidth) m_Width = MaxLineWidth;
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
#if 0 // useful for debugging
    dc.SetPen(*wxRED_PEN);
    dc.DrawRectangle(x+m_PosX,y+m_PosY,m_Width,m_Height);
#endif

    int xlocal = x + m_PosX;
    int ylocal = y + m_PosY;

    if (m_BkColour.IsOk())
    {
        wxBrush myb = wxBrush(m_BkColour, wxBRUSHSTYLE_SOLID);

        int real_y1 = mMax(ylocal, view_y1);
        int real_y2 = mMin(ylocal + m_Height - 1, view_y2);

        dc.SetBrush(myb);
        dc.SetPen(*wxTRANSPARENT_PEN);
        dc.DrawRectangle(xlocal, real_y1, m_Width, real_y2 - real_y1 + 1);
    }

    if (m_Border == 1)
    {
        // draw thin border using lines
        wxPen mypen1(m_BorderColour1, 1, wxPENSTYLE_SOLID);
        wxPen mypen2(m_BorderColour2, 1, wxPENSTYLE_SOLID);

        dc.SetPen(mypen1);
        dc.DrawLine(xlocal, ylocal, xlocal, ylocal + m_Height - 1);
        dc.DrawLine(xlocal, ylocal, xlocal + m_Width, ylocal);
        dc.SetPen(mypen2);
        dc.DrawLine(xlocal + m_Width - 1, ylocal, xlocal +  m_Width - 1, ylocal + m_Height - 1);
        dc.DrawLine(xlocal, ylocal + m_Height - 1, xlocal + m_Width, ylocal + m_Height - 1);
    }
    else if (m_Border> 0)
    {
        wxBrush mybrush1(m_BorderColour1, wxBRUSHSTYLE_SOLID);
        wxBrush mybrush2(m_BorderColour2, wxBRUSHSTYLE_SOLID);

        // draw upper left corner
        // 0---------------5
        // |              /
        // | 3-----------4
        // | |
        // | 2
        // |/
        // 1

        wxPoint poly[6];
        poly[0].x =m_PosX; poly[0].y = m_PosY ;
        poly[1].x =m_PosX; poly[1].y = m_PosY + m_Height;
        poly[2].x =m_PosX + m_Border; poly[2].y = poly[1].y - m_Border;
        poly[3].x =poly[2].x ; poly[3].y = m_PosY + m_Border;
        poly[4].x =m_PosX + m_Width - m_Border; poly[4].y = poly[3].y;
        poly[5].x =m_PosX + m_Width; poly[5].y = m_PosY;

        dc.SetBrush(mybrush1);
        dc.SetPen(*wxTRANSPARENT_PEN);
        dc.DrawPolygon(6, poly, x, y);

        // draw lower right corner reusing point 1,2,4 and 5
        //                 5
        //                /|
        //               4 |
        //               | |
        //   2-----------3 |
        //  /              |
        // 1---------------0
        dc.SetBrush(mybrush2);
        poly[0].x = poly[5].x; poly[0].y = poly[1].y;
        poly[3].x = poly[4].x; poly[3].y = poly[2].y;
        dc.DrawPolygon(6, poly, x, y);

        // smooth color transition like firefox
        wxColour borderMediumColour(
            (m_BorderColour1.Red() + m_BorderColour2.Red()) /2 ,
            (m_BorderColour1.Green() + m_BorderColour2.Green()) /2 ,
            (m_BorderColour1.Blue() + m_BorderColour2.Blue()) /2
            );
        wxPen mypen3(borderMediumColour, 1, wxPENSTYLE_SOLID);
        dc.SetPen(mypen3);
        dc.DrawLines(2, &poly[1], x, y - 1); // between 1 and 2
        dc.DrawLines(2, &poly[4], x, y - 1); // between 4 and 5
    }
    if (m_Cells)
    {
        // draw container's contents:
        for (wxHtmlCell *cell = m_Cells; cell; cell = cell->GetNext())
        {

            // optimize drawing: don't render off-screen content:
            if ((ylocal + cell->GetPosY() <= view_y2) &&
                (ylocal + cell->GetPosY() + cell->GetHeight() > view_y1))
            {
                // the cell is visible, draw it:
                UpdateRenderingStatePre(info, cell);
                cell->Draw(dc,
                           xlocal, ylocal, view_y1, view_y2,
                           info);
                UpdateRenderingStatePost(info, cell);
            }
            else
            {
                // the cell is off-screen, proceed with font+color+etc.
                // changes only:
                cell->DrawInvisible(dc, xlocal, ylocal, info);
            }
        }
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
    return m_BkColour;
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



void wxHtmlContainerCell::Detach(wxHtmlCell *cell)
{
    wxHtmlCell* const firstChild = GetFirstChild();
    if ( cell == firstChild )
    {
        m_Cells = cell->GetNext();
        if ( m_LastCell == cell )
            m_LastCell = NULL;
    }
    else // Not the first child.
    {
        for ( wxHtmlCell* prev = firstChild;; )
        {
            wxHtmlCell* const next = prev->GetNext();

            // We can't reach the end of the children list without finding this
            // cell, normally.
            wxCHECK_RET( next,  "Detaching cell which is not our child" );

            if ( cell == next )
            {
                prev->SetNext(cell->GetNext());
                if ( m_LastCell == cell )
                    m_LastCell = prev;
                break;
            }

            prev = next;
        }
    }

    cell->SetParent(NULL);
    cell->SetNext(NULL);
}



void wxHtmlContainerCell::SetAlign(const wxHtmlTag& tag)
{
    wxString alg;
    if (tag.GetParamAsString(wxT("ALIGN"), &alg))
    {
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
    int wdi;
    bool wpercent;
    if (tag.GetParamAsIntOrPercent(wxT("WIDTH"), &wdi, wpercent))
    {
        if (wpercent)
        {
            SetWidthFloat(wdi, wxHTML_UNITS_PERCENT);
        }
        else
        {
            SetWidthFloat((int)(pixel_scale * (double)wdi), wxHTML_UNITS_PIXELS);
        }
        m_LastLayout = -1;
    }
}



const wxHtmlCell* wxHtmlContainerCell::Find(int condition, const void* param) const
{
    if (m_Cells)
    {
        for (wxHtmlCell *cell = m_Cells; cell; cell = cell->GetNext())
        {
            const wxHtmlCell *r = cell->Find(condition, param);
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
        for ( const wxHtmlCell *cell = m_Cells; cell; cell = cell->GetNext() )
        {
            if ( cell->IsFormattingCell() )
                continue;
            int cellY = cell->GetPosY();
            if (!( y < cellY || (y < cellY + cell->GetHeight() &&
                                 x < cell->GetPosX() + cell->GetWidth()) ))
                continue;

            c = cell->FindCellByPos(x - cell->GetPosX(), y - cellY, flags);
            if (c) return c;
        }
    }
    else if ( flags & wxHTML_FIND_NEAREST_BEFORE )
    {
        wxHtmlCell *c2, *c = NULL;
        for ( const wxHtmlCell *cell = m_Cells; cell; cell = cell->GetNext() )
        {
            if ( cell->IsFormattingCell() )
                continue;
            int cellY = cell->GetPosY();
            if (!( cellY + cell->GetHeight() <= y ||
                   (y >= cellY && x >= cell->GetPosX()) ))
                break;
            c2 = cell->FindCellByPos(x - cell->GetPosX(), y - cellY, flags);
            if (c2)
                c = c2;
        }
        if (c) return c;
    }

    return NULL;
}


bool wxHtmlContainerCell::ProcessMouseClick(wxHtmlWindowInterface *window,
                                            const wxPoint& pos,
                                            const wxMouseEvent& event)
{
    bool retval = false;
    wxHtmlCell *cell = FindCellByPos(pos.x, pos.y);
    if ( cell )
        retval = cell->ProcessMouseClick(window, pos, event);

    return retval;
}


wxHtmlCell *wxHtmlContainerCell::GetFirstTerminal() const
{
    if ( m_Cells )
    {
        for (wxHtmlCell *c = m_Cells; c; c = c->GetNext())
        {
            wxHtmlCell *c2;
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

        wxHtmlCell *ctmp;
        wxHtmlCell *c2 = NULL;
        for (c = m_Cells; c; c = c->GetNext())
        {
            ctmp = c->GetLastTerminal();
            if ( ctmp )
                c2 = ctmp;
        }
        return c2;
    }
    else
        return NULL;
}


static bool IsEmptyContainer(wxHtmlContainerCell *cell)
{
    for ( wxHtmlCell *c = cell->GetFirstChild(); c; c = c->GetNext() )
    {
        if ( !c->IsTerminalCell() || !c->IsFormattingCell() )
            return false;
    }
    return true;
}

void wxHtmlContainerCell::RemoveExtraSpacing(bool top, bool bottom)
{
    if ( top )
        SetIndent(0, wxHTML_INDENT_TOP);
    if ( bottom )
        SetIndent(0, wxHTML_INDENT_BOTTOM);

    if ( m_Cells )
    {
        wxHtmlCell *c;
        wxHtmlContainerCell *cont;
        if ( top )
        {
            for ( c = m_Cells; c; c = c->GetNext() )
            {
                if ( c->IsTerminalCell() )
                {
                    if ( !c->IsFormattingCell() )
                        break;
                }
                else
                {
                    cont = (wxHtmlContainerCell*)c;
                    if ( IsEmptyContainer(cont) )
                    {
                        cont->SetIndent(0, wxHTML_INDENT_VERTICAL);
                    }
                    else
                    {
                        cont->RemoveExtraSpacing(true, false);
                        break;
                    }
                }
            }
        }

        if ( bottom )
        {
            wxArrayPtrVoid arr;
            for ( c = m_Cells; c; c = c->GetNext() )
                arr.Add((void*)c);

            for ( int i = arr.GetCount() - 1; i >= 0; i--)
            {
                c = (wxHtmlCell*)arr[i];
                if ( c->IsTerminalCell() )
                {
                    if ( !c->IsFormattingCell() )
                        break;
                }
                else
                {
                    cont = (wxHtmlContainerCell*)c;
                    if ( IsEmptyContainer(cont) )
                    {
                        cont->SetIndent(0, wxHTML_INDENT_VERTICAL);
                    }
                    else
                    {
                        cont->RemoveExtraSpacing(false, true);
                        break;
                    }
                }
            }
        }
    }
}

wxString wxHtmlContainerCell::Dump(int indent) const
{
    wxString s = wxHtmlCell::Dump(indent);

    for ( wxHtmlCell* c = m_Cells; c; c = c->GetNext() )
        s << "\n" << c->Dump(indent + 4);

    return s;
}



// --------------------------------------------------------------------------
// wxHtmlColourCell
// --------------------------------------------------------------------------

wxIMPLEMENT_ABSTRACT_CLASS(wxHtmlColourCell, wxHtmlCell);

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
        state.SetBgMode(wxBRUSHSTYLE_SOLID);
        const wxColour c = state.GetSelectionState() == wxHTML_SEL_IN
                         ? info.GetStyle().GetSelectedTextBgColour(m_Colour)
                         : m_Colour;
        dc.SetTextBackground(c);
        dc.SetBackground(c);
        dc.SetBackgroundMode(wxBRUSHSTYLE_SOLID);
    }
    if (m_Flags & wxHTML_CLR_TRANSPARENT_BACKGROUND)
    {
        state.SetBgColour(m_Colour);
        state.SetBgMode(wxBRUSHSTYLE_TRANSPARENT);
        const wxColour c = state.GetSelectionState() == wxHTML_SEL_IN
                         ? info.GetStyle().GetSelectedTextBgColour(m_Colour)
                         : m_Colour;
        dc.SetTextBackground(c);
        dc.SetBackgroundMode(wxBRUSHSTYLE_TRANSPARENT);
    }
}

wxString wxHtmlColourCell::GetDescription() const
{
    return wxString::Format("wxHtmlColourCell(%s)", m_Colour.GetAsString());
}



// ---------------------------------------------------------------------------
// wxHtmlFontCell
// ---------------------------------------------------------------------------

wxIMPLEMENT_ABSTRACT_CLASS(wxHtmlFontCell, wxHtmlCell);

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


wxString wxHtmlFontCell::GetDescription() const
{
    return wxString::Format("wxHtmlFontCell(%s)", m_Font.GetNativeFontInfoUserDesc());
}






// ---------------------------------------------------------------------------
// wxHtmlWidgetCell
// ---------------------------------------------------------------------------

wxIMPLEMENT_ABSTRACT_CLASS(wxHtmlWidgetCell, wxHtmlCell);

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

    wxScrolledWindow *scrolwin =
        wxDynamicCast(m_Wnd->GetParent(), wxScrolledWindow);
    wxCHECK_RET( scrolwin,
                 wxT("widget cells can only be placed in wxHtmlWindow") );

    scrolwin->GetViewStart(&stx, &sty);
    m_Wnd->SetSize(absx - wxHTML_SCROLL_STEP * stx,
                   absy  - wxHTML_SCROLL_STEP * sty,
                   m_Width, m_Height);
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

#endif
