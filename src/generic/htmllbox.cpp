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
#endif //WX_PRECOMP

#include "wx/htmllbox.h"

#include "wx/html/htmlcell.h"
#include "wx/html/winpars.h"

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// this class is used by wxHtmlListBox to cache the parsed representation of
// the items to avoid doing it anew each time an item must be drawn
//
// TODO: extend the class to cache more than item
class wxHtmlListBoxCache
{
public:
    wxHtmlListBoxCache() { m_cell = NULL; }
    ~wxHtmlListBoxCache() { delete m_cell; }

    // returns true if we already have this item cached
    bool Has(size_t n) const { return m_cell && n == m_item; }

    // ensure that the item is cached
    void Store(size_t n, wxHtmlCell *cell)
    {
        m_item = n;

        delete m_cell;
        m_cell = cell;
    }

    // return the cached cell for this index or NULL if none
    wxHtmlCell *Get(size_t n) const
    {
        // we could be reading uninitialized m_item here but the code is still
        // correct
        return n == m_item ? m_cell : NULL;
    }

private:
    // the parsed representation of the cached item or NULL
    wxHtmlCell *m_cell;

    // the index of the currently cached item (only valid if m_cell != NULL)
    size_t m_item;
};

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxHtmlListBox creation
// ----------------------------------------------------------------------------

void wxHtmlListBox::Init()
{
    m_htmlParser = NULL;
    m_cache = new wxHtmlListBoxCache;
}

bool wxHtmlListBox::Create(wxWindow *parent,
                           wxWindowID id,
                           const wxPoint& pos,
                           const wxSize& size,
                           size_t countItems,
                           long style,
                           const wxString& name)
{
    return wxVListBox::Create(parent, id, pos, size, countItems, style, name);
}

wxHtmlListBox::~wxHtmlListBox()
{
    delete m_cache;
    if ( m_htmlParser )
    {
        delete m_htmlParser->GetDC();
        delete m_htmlParser;
    }
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

void wxHtmlListBox::CacheItem(size_t n) const
{
    if ( !m_cache->Has(n) )
    {
        if ( !m_htmlParser )
        {
            wxHtmlListBox *self = wxConstCast(this, wxHtmlListBox);

            self->m_htmlParser = new wxHtmlWinParser;
            m_htmlParser->SetDC(new wxClientDC(self));
        }

        wxHtmlContainerCell *cell = (wxHtmlContainerCell *)m_htmlParser->
                Parse(OnGetItemMarkup(n));
        wxCHECK_RET( cell, _T("wxHtmlParser::Parse() returned NULL?") );

        cell->Layout(GetClientSize().x);

        m_cache->Store(n, cell);
    }
}

// ----------------------------------------------------------------------------
// wxHtmlListBox implementation of wxVListBox pure virtuals
// ----------------------------------------------------------------------------

void wxHtmlListBox::OnDrawItem(wxDC& dc, const wxRect& rect, size_t n) const
{
    CacheItem(n);

    wxHtmlCell *cell = m_cache->Get(n);
    wxCHECK_RET( cell, _T("this cell should be cached!") );

    // draw the selected cell in selected state
    if ( IsSelected(n) )
    {
        wxHtmlSelection htmlSel;
        htmlSel.Set(wxPoint(0, 0), cell, wxPoint(INT_MAX, INT_MAX), cell);
        wxHtmlRenderingState htmlRendState(&htmlSel);
        htmlRendState.SetSelectionState(wxHTML_SEL_IN);
        cell->Draw(dc, rect.x, rect.y, 0, INT_MAX, htmlRendState);
    }
    else
    {
        // note that we can't stop drawing exactly at the window boundary as then
        // even the visible cells part could be not drawn, so always draw the
        // entire cell
        wxHtmlRenderingState htmlRendState(NULL);
        cell->Draw(dc, rect.x, rect.y, 0, INT_MAX, htmlRendState);
    }
}

wxCoord wxHtmlListBox::OnMeasureItem(size_t n) const
{
    CacheItem(n);

    wxHtmlCell *cell = m_cache->Get(n);
    wxCHECK_MSG( cell, 0, _T("this cell should be cached!") );

    return cell->GetHeight() + cell->GetDescent();
}

