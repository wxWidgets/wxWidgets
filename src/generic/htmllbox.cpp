///////////////////////////////////////////////////////////////////////////////
// Name:        generic/htmllbox.cpp
// Purpose:     implementation of wxHtmlListBox
// Author:      Vadim Zeitlin
// Modified by:
// Created:     31.05.03
// RCS-ID:      $Id$
// Copyright:   (c) 2003 Vadim Zeitlin <vadim@wxwindows.org>
// License:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/dcclient.h"
#endif //WX_PRECOMP

#if wxUSE_HTML

#include "wx/htmllbox.h"

#include "wx/html/htmlcell.h"
#include "wx/html/winpars.h"

// this hack forces the linker to always link in m_* files
#include "wx/html/forcelnk.h"
FORCE_WXHTML_MODULES()

// ============================================================================
// private classes
// ============================================================================

// ----------------------------------------------------------------------------
// wxHtmlListBoxCache
// ----------------------------------------------------------------------------

// this class is used by wxHtmlListBox to cache the parsed representation of
// the items to avoid doing it anew each time an item must be drawn
class wxHtmlListBoxCache
{
public:
    wxHtmlListBoxCache()
    {
        for ( size_t n = 0; n < SIZE; n++ )
        {
            m_items[n] = (size_t)-1;
            m_cells[n] = NULL;
        }

        m_next = 0;
    }

    ~wxHtmlListBoxCache()
    {
        for ( size_t n = 0; n < SIZE; n++ )
        {
            delete m_cells[n];
        }
    }

    // completely invalidate the cache
    void Clear()
    {
        for ( size_t n = 0; n < SIZE; n++ )
        {
            m_items[n] = (size_t)-1;
            delete m_cells[n];
            m_cells[n] = NULL;
        }
    }

    // return the cached cell for this index or NULL if none
    wxHtmlCell *Get(size_t item) const
    {
        for ( size_t n = 0; n < SIZE; n++ )
        {
            if ( m_items[n] == item )
                return m_cells[n];
        }

        return NULL;
    }

    // returns true if we already have this item cached
    bool Has(size_t item) const { return Get(item) != NULL; }

    // ensure that the item is cached
    void Store(size_t item, wxHtmlCell *cell)
    {
        delete m_cells[m_next];
        m_cells[m_next] = cell;
        m_items[m_next] = item;

        // advance to the next item wrapping around if there are no more
        if ( ++m_next == SIZE )
            m_next = 0;
    }

private:
    // the max number of the items we cache
    enum { SIZE = 50 };

    // the index of the LRU (oldest) cell
    size_t m_next;

    // the parsed representation of the cached item or NULL
    wxHtmlCell *m_cells[SIZE];

    // the index of the currently cached item (only valid if m_cells != NULL)
    size_t m_items[SIZE];
};

// ----------------------------------------------------------------------------
// wxHtmlListBoxStyle
// ----------------------------------------------------------------------------

// just forward wxDefaultHtmlRenderingStyle callbacks to the main class so that
// they could be overridden by the user code
class wxHtmlListBoxStyle : public wxDefaultHtmlRenderingStyle
{
public:
    wxHtmlListBoxStyle(wxHtmlListBox& hlbox) : m_hlbox(hlbox) { }

    virtual wxColour GetSelectedTextColour(const wxColour& colFg)
    {
        return m_hlbox.GetSelectedTextColour(colFg);
    }

    virtual wxColour GetSelectedTextBgColour(const wxColour& colBg)
    {
        return m_hlbox.GetSelectedTextBgColour(colBg);
    }

private:
    const wxHtmlListBox& m_hlbox;

    DECLARE_NO_COPY_CLASS(wxHtmlListBoxStyle)
};


// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxHtmlListBox, wxVListBox)
    EVT_SIZE(wxHtmlListBox::OnSize)
END_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

IMPLEMENT_ABSTRACT_CLASS(wxHtmlListBox, wxVListBox)


// ----------------------------------------------------------------------------
// wxHtmlListBox creation
// ----------------------------------------------------------------------------

void wxHtmlListBox::Init()
{
    m_htmlParser = NULL;
    m_htmlRendStyle = new wxHtmlListBoxStyle(*this);
    m_cache = new wxHtmlListBoxCache;
}

bool wxHtmlListBox::Create(wxWindow *parent,
                           wxWindowID id,
                           const wxPoint& pos,
                           const wxSize& size,
                           long style,
                           const wxString& name)
{
    return wxVListBox::Create(parent, id, pos, size, style, name);
}

wxHtmlListBox::~wxHtmlListBox()
{
    delete m_cache;

    if ( m_htmlParser )
    {
        delete m_htmlParser->GetDC();
        delete m_htmlParser;
    }

    delete m_htmlRendStyle;
}

// ----------------------------------------------------------------------------
// wxHtmlListBox appearance
// ----------------------------------------------------------------------------

wxColour wxHtmlListBox::GetSelectedTextColour(const wxColour& colFg) const
{
    return m_htmlRendStyle->
                wxDefaultHtmlRenderingStyle::GetSelectedTextColour(colFg);
}

wxColour
wxHtmlListBox::GetSelectedTextBgColour(const wxColour& WXUNUSED(colBg)) const
{
    return GetSelectionBackground();
}

// ----------------------------------------------------------------------------
// wxHtmlListBox items markup
// ----------------------------------------------------------------------------

wxString wxHtmlListBox::OnGetItemMarkup(size_t n) const
{
    // we don't even need to wrap the value returned by OnGetItem() inside
    // "<html><body>" and "</body></html>" because wxHTML can parse it even
    // without these tags
    return OnGetItem(n);
}

// ----------------------------------------------------------------------------
// wxHtmlListBox cache handling
// ----------------------------------------------------------------------------

void wxHtmlListBox::CacheItem(size_t n) const
{
    if ( !m_cache->Has(n) )
    {
        if ( !m_htmlParser )
        {
            wxHtmlListBox *self = wxConstCast(this, wxHtmlListBox);

            self->m_htmlParser = new wxHtmlWinParser;
            m_htmlParser->SetDC(new wxClientDC(self));
            m_htmlParser->SetFS(&self->m_filesystem);

            // use system's default GUI font by default:
            m_htmlParser->SetStandardFonts();
        }

        wxHtmlContainerCell *cell = (wxHtmlContainerCell *)m_htmlParser->
                Parse(OnGetItemMarkup(n));
        wxCHECK_RET( cell, _T("wxHtmlParser::Parse() returned NULL?") );

        cell->Layout(GetClientSize().x - 2*GetMargins().x);

        m_cache->Store(n, cell);
    }
}

void wxHtmlListBox::OnSize(wxSizeEvent& event)
{
    // we need to relayout all the cached cells
    m_cache->Clear();

    event.Skip();
}

void wxHtmlListBox::RefreshAll()
{
    m_cache->Clear();

    wxVListBox::RefreshAll();
}

void wxHtmlListBox::SetItemCount(size_t count)
{
    // the items are going to change, forget the old ones
    m_cache->Clear();

    wxVListBox::SetItemCount(count);
}

// ----------------------------------------------------------------------------
// wxHtmlListBox implementation of wxVListBox pure virtuals
// ----------------------------------------------------------------------------

void wxHtmlListBox::OnDrawItem(wxDC& dc, const wxRect& rect, size_t n) const
{
    CacheItem(n);

    wxHtmlCell *cell = m_cache->Get(n);
    wxCHECK_RET( cell, _T("this cell should be cached!") );

    wxHtmlRenderingInfo htmlRendInfo;

    // draw the selected cell in selected state
    if ( IsSelected(n) )
    {
        wxHtmlSelection htmlSel;
        htmlSel.Set(wxPoint(0, 0), cell, wxPoint(INT_MAX, INT_MAX), cell);
        htmlRendInfo.SetSelection(&htmlSel);
        if ( m_htmlRendStyle )
            htmlRendInfo.SetStyle(m_htmlRendStyle);
        htmlRendInfo.GetState().SetSelectionState(wxHTML_SEL_IN);
    }

    // note that we can't stop drawing exactly at the window boundary as then
    // even the visible cells part could be not drawn, so always draw the
    // entire cell
    cell->Draw(dc, rect.x+2, rect.y+2, 0, INT_MAX, htmlRendInfo);
}

wxCoord wxHtmlListBox::OnMeasureItem(size_t n) const
{
    CacheItem(n);

    wxHtmlCell *cell = m_cache->Get(n);
    wxCHECK_MSG( cell, 0, _T("this cell should be cached!") );

    return cell->GetHeight() + cell->GetDescent() + 4;
}

#endif
