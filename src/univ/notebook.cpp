/////////////////////////////////////////////////////////////////////////////
// Name:        univ/notebook.cpp
// Purpose:     wxNotebook implementation
// Author:      Vadim Zeitlin
// Modified by:
// Created:     01.02.01
// RCS-ID:      $Id$
// Copyright:   (c) 2001 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "univnotebook.h"
#endif

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_NOTEBOOK

#include "wx/imaglist.h"
#include "wx/notebook.h"

#include "wx/spinbutt.h"

#include "wx/univ/renderer.h"

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

#define IS_VALID_PAGE(nPage) (((nPage) >= 0) && ((nPage) < GetPageCount()))

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

static const size_t INVALID_PAGE = (size_t)-1;

// ============================================================================
// implementation
// ============================================================================

IMPLEMENT_DYNAMIC_CLASS(wxNotebook, wxControl)
IMPLEMENT_DYNAMIC_CLASS(wxNotebookEvent, wxCommandEvent)

// ----------------------------------------------------------------------------
// wxNotebook creation
// ----------------------------------------------------------------------------

void wxNotebook::Init()
{
    m_sel = INVALID_PAGE;

    m_heightTab =
    m_widthMax = 0;

    m_sizePad = wxSize(6, 5); // FIXME: hardcoded

    m_spinbtn = NULL;
}

bool wxNotebook::Create(wxWindow *parent,
                        wxWindowID id,
                        const wxPoint& pos,
                        const wxSize& size,
                        long style,
                        const wxString& name)
{
    if ( !wxControl::Create(parent, id, pos, size, style,
                            wxDefaultValidator, name) )
        return FALSE;

    SetBestSize(size);

    //CreateInputHandler(wxINP_HANDLER_NOTEBOOK);

    return TRUE;
}

// ----------------------------------------------------------------------------
// wxNotebook page titles and images
// ----------------------------------------------------------------------------

wxString wxNotebook::GetPageText(int nPage) const
{
    wxCHECK_MSG( IS_VALID_PAGE(nPage), _T(""), _T("invalid notebook page") );

    return m_titles[nPage];
}

bool wxNotebook::SetPageText(int nPage, const wxString& strText)
{
    wxCHECK_MSG( IS_VALID_PAGE(nPage), FALSE, _T("invalid notebook page") );

    if ( strText != m_titles[nPage] )
    {
        m_accels[nPage] = FindAccelIndex(strText, &m_titles[nPage]);

        if ( FixedSizeTabs() )
        {
            // it's enough to just reresh this one
            RefreshTab(nPage);
        }
        else // var width tabs
        {
            // we need to resize the tab to fit the new string
            ResizeTab(nPage);
        }
    }

    return TRUE;
}

int wxNotebook::GetPageImage(int nPage) const
{
    wxCHECK_MSG( IS_VALID_PAGE(nPage), -1, _T("invalid notebook page") );

    return m_images[nPage];
}

bool wxNotebook::SetPageImage(int nPage, int nImage)
{
    wxCHECK_MSG( IS_VALID_PAGE(nPage), FALSE, _T("invalid notebook page") );

    wxCHECK_MSG( m_imageList && nImage < m_imageList->GetImageCount(), FALSE,
                 _T("invalid image index in SetPageImage()") );

    if ( nImage != m_images[nPage] )
    {
        // if the item didn't have an icon before or, on the contrary, did have
        // it but has lost it now, its size will change - but if the icon just
        // changes, it won't
        bool tabSizeChanges = nImage == -1 || m_images[nPage] == -1;
        m_images[nPage] = nImage;

        if ( tabSizeChanges )
            RefreshAllTabs();
        else
            RefreshTab(nPage);
    }

    return TRUE;
}

wxNotebook::~wxNotebook()
{
}

// ----------------------------------------------------------------------------
// wxNotebook page switching
// ----------------------------------------------------------------------------

int wxNotebook::SetSelection(int nPage)
{
    wxCHECK_MSG( IS_VALID_PAGE(nPage), -1, _T("invalid notebook page") );

    if ( m_sel != -1 )
    {
        RefreshTab(m_sel);

        m_pages[m_sel]->Hide();
    }

    m_sel = nPage;

    if ( m_sel != -1 ) // yes, this is impossible - but test nevertheless
    {
        RefreshTab(m_sel);

        m_pages[m_sel]->Show();
    }

    return m_sel;
}

void wxNotebook::ChangePage(int nPage)
{
    wxNotebookEvent event(wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING, m_windowId);
    event.SetSelection(nPage);
    event.SetOldSelection(m_sel);
    event.SetEventObject(this);
    if ( GetEventHandler()->ProcessEvent(event) && !event.IsAllowed() )
    {
        // program doesn't allow the page change
        return;
    }

    SetSelection(nPage);

    event.SetEventType(wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED);
    GetEventHandler()->ProcessEvent(event);
}

// ----------------------------------------------------------------------------
// wxNotebook pages adding/deleting
// ----------------------------------------------------------------------------

bool wxNotebook::InsertPage(int nPage,
                            wxNotebookPage *pPage,
                            const wxString& strText,
                            bool bSelect,
                            int imageId)
{
    int nPages = GetPageCount();
    wxCHECK_MSG( nPage == nPages || IS_VALID_PAGE(nPage), FALSE,
                 _T("invalid notebook page in InsertPage()") );

    // modify the data
    m_pages.Insert(pPage, nPage);

    wxString label;
    m_accels.Insert(FindAccelIndex(strText, &label), nPage);
    m_titles.Insert(label, nPage);

    m_images.Insert(imageId, nPage);

    // cache the tab geometry here
    wxSize sizeTab = CalcTabSize(nPage);

    if ( sizeTab.y > m_heightTab )
        m_heightTab = sizeTab.y;

    if ( FixedSizeTabs() && sizeTab.x > m_widthMax )
        m_widthMax = sizeTab.x;

    m_widths.Insert(sizeTab.x, nPage);

    // if the tab has just appeared, we have to relayout everything, otherwise
    // it's enough to just redraw the tabs
    if ( nPages == 0 )
    {
        // always select the first tab to have at least some selection
        bSelect = TRUE;

        Relayout();
    }
    else // not the first tab
    {
        RefreshAllTabs();
    }

    if ( bSelect )
    {
        SetSelection(nPage);
    }
    else // pages added to the notebook are initially hidden
    {
        pPage->Hide();
    }

    return TRUE;
}

bool wxNotebook::DeleteAllPages()
{
    if ( !wxNotebookBase::DeleteAllPages() )
        return FALSE;

    Relayout();

    return TRUE;
}

wxNotebookPage *wxNotebook::DoRemovePage(int nPage)
{
    wxCHECK_MSG( IS_VALID_PAGE(nPage), NULL, _T("invalid notebook page") );

    wxNotebookPage *page = m_pages[nPage];
    m_pages.RemoveAt(nPage);
    m_titles.RemoveAt(nPage);
    m_accels.RemoveAt(nPage);
    m_widths.RemoveAt(nPage);
    m_images.RemoveAt(nPage);

    if ( GetPageCount() )
    {
        // some tabs are still left, just redraw them
        RefreshAllTabs();

        m_sel = 0;
    }
    else // no more tabs left
    {
        // have to refresh everything
        Relayout();

        m_sel = INVALID_PAGE;
    }

    return page;
}

// ----------------------------------------------------------------------------
// wxNotebook drawing
// ----------------------------------------------------------------------------

void wxNotebook::RefreshTab(int page)
{
    wxRect r = GetTabRect(page);
    Refresh(TRUE, &r);
}

void wxNotebook::RefreshAllTabs()
{
    wxRect r = GetAllTabsRect();
    Refresh(TRUE, &r);
}

void wxNotebook::DoDrawTab(wxDC& dc, const wxRect& rect, size_t n)
{
    wxBitmap bmp;
    if ( HasImage(n) )
    {
#ifdef __WXMSW__    // FIXME
        wxMemoryDC dc;
        dc.SelectObject(bmp);
        m_imageList->Draw(m_images[n], dc, 0, 0);
#else
        bmp = *m_imageList->GetBitmap(m_images[n]);
#endif
    }

    GetRenderer()->DrawTab
                   (
                     dc,
                     rect,
                     GetTabOrientation(),
                     m_titles[n],
                     bmp,
                     n == m_sel ? wxCONTROL_FOCUSED | wxCONTROL_SELECTED : 0,
                     m_accels[n]
                   );
}

void wxNotebook::DoDraw(wxControlRenderer *renderer)
{
    wxRect rectUpdate = GetUpdateClientRect();

    wxDC& dc = renderer->GetDC();
    dc.SetFont(GetFont());
    dc.SetTextForeground(GetForegroundColour());

    // redraw the border - it's simpler to always do it instead of checking
    // whether this needs to be done
    GetRenderer()->DrawBorder(dc, wxBORDER_RAISED, GetPagePart());

    wxRect rect = GetTabsPart();
    bool isVertical = IsVertical();

    wxRect rectSel;
    size_t count = GetPageCount();
    for ( size_t n = 0; n < count; n++ )
    {
        GetTabSize(n, &rect.width, &rect.height);

        if ( n == m_sel )
        {
            // don't redraw it now as this tab has to be drawn over the other
            // ones
            rectSel = rect;
        }
        else // not selected tab
        {
            if ( rectUpdate.Intersects(rect) )
            {
                DoDrawTab(dc, rect, n);
            }
            //else: doesn't need to be refreshed
        }

        // move the rect to the next tab
        if ( isVertical )
            rect.y += rect.height;
        else
            rect.x += rect.width;
    }

    DoDrawTab(dc, rectSel, m_sel);
}

// ----------------------------------------------------------------------------
// wxNotebook geometry
// ----------------------------------------------------------------------------

bool wxNotebook::IsVertical() const
{
    wxDirection dir = GetTabOrientation();

    return dir == wxLEFT || dir == wxRIGHT;
}

wxDirection wxNotebook::GetTabOrientation() const
{
    long style = GetWindowStyle();
    if ( style & wxNB_BOTTOM )
        return wxBOTTOM;
    else if ( style & wxNB_RIGHT )
        return wxRIGHT;
    else if ( style & wxNB_LEFT )
        return wxLEFT;

    // wxNB_TOP == 0 so we don't have to test for it
    return wxTOP;
}

wxRect wxNotebook::GetTabRect(int page) const
{
    wxRect rect;
    wxCHECK_MSG( IS_VALID_PAGE(page), rect, _T("invalid notebook page") );

    // calc the size of this tab and of the preceding ones
    wxCoord widthThis, widthBefore;
    if ( FixedSizeTabs() )
    {
        widthThis = m_widthMax;
        widthBefore = page*m_widthMax;
    }
    else
    {
        widthBefore = 0;
        for ( int n = 0; n < page; n++ )
        {
            widthBefore += m_widths[n];
        }

        widthThis = m_widths[page];
    }

    rect = GetAllTabsRect();
    if ( IsVertical() )
    {
        rect.y = widthBefore;
        rect.height = widthThis;
    }
    else // horz
    {
        rect.x = widthBefore;
        rect.width = widthThis;
    }

    return rect;
}

wxRect wxNotebook::GetAllTabsRect() const
{
    wxRect rect;

    if ( GetPageCount() )
    {
        const wxSize indent = GetRenderer()->GetTabIndent();
        wxSize size = GetClientSize();

        if ( IsVertical() )
        {
            rect.x = GetTabOrientation() == wxLEFT ? 0 : size.x - m_heightTab;
            rect.width = m_heightTab + indent.x;
            rect.y = 0;
            rect.height = size.y;
        }
        else // horz
        {
            rect.x = 0;
            rect.width = size.x;
            rect.y = GetTabOrientation() == wxTOP ? 0 : size.y - m_heightTab;
            rect.height = m_heightTab + indent.y;
        }
    }
    //else: no pages

    return rect;
}

wxRect wxNotebook::GetTabsPart() const
{
    wxRect rect = GetAllTabsRect();

    const wxSize indent = GetRenderer()->GetTabIndent();
    if ( IsVertical() )
    {
        rect.x += indent.y;
        rect.y += indent.x;
    }
    else // horz
    {
        rect.x += indent.x;
        rect.y += indent.y;
    }

    return rect;
}

void wxNotebook::GetTabSize(int page, wxCoord *w, wxCoord *h) const
{
    wxCHECK_RET( w && h, _T("NULL pointer in GetTabSize") );

    if ( IsVertical() )
    {
        // width and height have inverted meaning
        wxCoord *tmp = w;
        w = h;
        h = tmp;
    }

    // height is always fixed
    *h = m_heightTab;

    // width may also be fixed and be the same for all tabs
    *w = FixedSizeTabs() ? m_widthMax : m_widths[page];
}

void wxNotebook::SetTabSize(const wxSize& sz)
{
    wxCHECK_RET( FixedSizeTabs(), _T("SetTabSize() ignored") );

    if ( IsVertical() )
    {
        m_heightTab = sz.x;
        m_widthMax = sz.y;
    }
    else // horz
    {
        m_widthMax = sz.x;
        m_heightTab = sz.y;
    }
}

wxSize wxNotebook::CalcTabSize(int page) const
{
    // NB: don't use m_widthMax, m_heightTab or m_widths here because this
    //     method is called to calculate them

    wxSize size;

    wxCHECK_MSG( IS_VALID_PAGE(page), size, _T("invalid notebook page") );

    GetTextExtent(m_titles[page], &size.x, &size.y);

    if ( HasImage(page) )
    {
        wxSize sizeImage;
        m_imageList->GetSize(m_images[page], sizeImage.x, sizeImage.y);

        size.x += sizeImage.x + 5; // FIXME: hard coded margin

        if ( sizeImage.y > size.y )
            size.y = sizeImage.y;
    }

    size.x += 2*m_sizePad.x;
    size.y += 2*m_sizePad.y;

    return size;
}

void wxNotebook::ResizeTab(int page)
{
    wxSize sizeTab = CalcTabSize(page);

    // we only need full relayout if the page size changes
    bool needsRelayout = FALSE;

    if ( IsVertical() )
    {
        // swap coordinates
        wxCoord tmp = sizeTab.x;
        sizeTab.x = sizeTab.y;
        sizeTab.y = tmp;
    }

    if ( sizeTab.y > m_heightTab )
    {
        needsRelayout = TRUE;

        m_heightTab = sizeTab.y;
    }

    m_widths[page] = sizeTab.x;

    if ( sizeTab.x > m_widthMax )
        m_widthMax = sizeTab.x;

    if ( needsRelayout )
        Relayout();
    else
        RefreshAllTabs();
}

void wxNotebook::SetPadding(const wxSize& padding)
{
    if ( padding != m_sizePad )
    {
        m_sizePad = padding;

        Relayout();
    }
}

void wxNotebook::Relayout()
{
    wxRect rectPage = GetPageRect();

    size_t count = GetPageCount();
    for ( size_t n = 0; n < count; n++ )
    {
        m_pages[n]->SetSize(rectPage);
    }
}

wxRect wxNotebook::GetPagePart() const
{
    wxRect rectPage = GetClientRect();

    if ( GetPageCount() )
    {
        wxRect rectTabs = GetAllTabsRect();
        wxDirection dir = GetTabOrientation();
        if ( IsVertical() )
        {
            rectPage.width -= rectTabs.width;
            if ( dir == wxLEFT )
                rectPage.x += rectTabs.width;
        }
        else // horz
        {
            rectPage.height -= rectTabs.height;
            if ( dir == wxTOP )
                rectPage.y += rectTabs.height;
        }
    }
    //else: no pages at all

    return rectPage;
}

wxRect wxNotebook::GetPageRect() const
{
    wxRect rect = GetPagePart();

    // leave space for the border
    wxRect rectBorder = GetRenderer()->GetBorderDimensions(wxBORDER_RAISED);

    // FIXME: hardcoded +2!
    rect.Inflate(-(rectBorder.x + rectBorder.width + 2),
                 -(rectBorder.y + rectBorder.height + 2));

    return rect;
}

wxSize wxNotebook::GetSizeForPage(const wxSize& size) const
{
    wxSize sizeNb = size;
    wxRect rect = GetAllTabsRect();
    if ( IsVertical() )
        sizeNb.x += rect.width;
    else
        sizeNb.y += rect.height;

    return sizeNb;
}

void wxNotebook::SetPageSize(const wxSize& size)
{
    SetClientSize(GetSizeForPage(size));
}

// ----------------------------------------------------------------------------
// wxNotebook sizing/moving
// ----------------------------------------------------------------------------

wxSize wxNotebook::DoGetBestClientSize() const
{
    // calculate the max page size
    wxSize size(0, 0);

    size_t count = GetPageCount();
    for ( size_t n = 0; n < count; n++ )
    {
        wxSize sizePage = m_pages[n]->GetSize();

        if ( size.x < sizePage.x )
            size.x = sizePage.x;
        if ( size.y < sizePage.y )
            size.y = sizePage.y;
    }

    return GetSizeForPage(size);
}

void wxNotebook::DoMoveWindow(int x, int y, int width, int height)
{
    // move the spin ctrl
    wxControl::DoMoveWindow(x, y, width, height);
}

void wxNotebook::DoSetSize(int x, int y,
                           int width, int height,
                           int sizeFlags)
{
    wxControl::DoSetSize(x, y, width, height, sizeFlags);

    Relayout();
}

// ----------------------------------------------------------------------------
// wxNotebook input processing
// ----------------------------------------------------------------------------

bool wxNotebook::PerformAction(const wxControlAction& action,
                               long numArg,
                               const wxString& strArg)
{
    if ( action == wxACTION_NOTEBOOK_NEXT )
        ChangePage(GetNextPage(TRUE));
    else if ( action == wxACTION_NOTEBOOK_PREV )
        ChangePage(GetNextPage(FALSE));
    else
        return wxControl::PerformAction(action, numArg, strArg);

    return TRUE;
}

#endif // wxUSE_NOTEBOOK

