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

#include "wx/spinbutt.h"

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

// ----------------------------------------------------------------------------
// wxNotebook creation
// ----------------------------------------------------------------------------

void wxNotebook::Init()
{
    m_sel = INVALID_PAGE;

    m_heightTab =
    m_widthMax = -1;

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
        m_titles[nPage] = strText;

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

    wxCHECK_MSG( m_imageList && nImage < m_imageList.GetImageCount(), FALSE,
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

// ----------------------------------------------------------------------------
// wxNotebook page switching
// ----------------------------------------------------------------------------

int wxNotebook::SetSelection(int nPage)
{
    wxCHECK_MSG( IS_VALID_PAGE(nPage), -1, _T("invalid notebook page") );

    int selOld = m_sel;

    m_sel = nPage;

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
    m_titles.Insert(strText, nPage);
    m_images.Insert(imageId, nPage);

    // cache the tab geometry here
    m_heightTab;
    m_widthMax;
    m_widths[nPage] = ;

    if ( bSelect )
    {
        SetSelection(nPage);
    }

    // if the tab has just appeared, we have to relayout everything, otherwise
    // it's enough to just redraw the tabs
    if ( nPages == 0 )
        Relayout();
    else
        RefreshAllTabs();
}

bool wxNotebook::DeleteAllPages()
{
    if ( !wxNotebookBase::DeleteAllPages() )
        return FALSE;

    Relayout();

    return TRUE;
}

wxNotebookPage *wxNotebook::RemovePage(int nPage)
{
    wxCHECK_MSG( IS_VALID_PAGE(nPage), NULL, _T("invalid notebook page") );

    wxNotebookPage *page = m_pages[nPage];
    m_pages.RemoveAt(nPage);
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
}

void wxNotebook::RefreshAllTabs()
{
}

void wxNotebook::DoDraw(wxControlRenderer *renderer)
{
    wxRect rect = GetAllTabsRect();
    wxDirection dir = GetTabOrientation();
    bool isVertical = IsVertical();

    size_t count = GetPageCount();
    for ( size_t n = 0; n < count; n++ )
    {
        GetTabSize(n, &rect.width, &rect.height);

        wxBitmap *bmp = m_imageList ? m_imageList->GetBitmap(m_images[n])
                                    : NULL;
        renderer->DrawTab(dir, rect, m_titles[n], bmp,
                          n == m_sel ? wxCONTROL_FOCUSED : 0);

        // move the rect to the next tab
        if ( isVertical )
            rect.y += rect.height;
        else
            rect.x += rect.width;
    }
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

wxRect wxNotebook::GetAllTabsRect() const
{
    wxRect rect;
    wxSize size = GetClientSize();

    if ( IsVertical() )
    {
        rect.x = GetDirection() == wxLEFT ? 0 : size.x - m_heightTab;
        rect.width = m_heightTab;
        rect.y = 0;
        rect.height = size.y;
    }
    else // horz
    {
        rect.x = 0;
        rect.width = size.x;
        rect.y = GetDirection() == wxTOP ? 0 : size.y - m_heightTab;
        rect.height = m_heightTab;
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
    wxSize size;

    wxCHECK_MSG( IS_VALID_PAGE(nPage), size, _T("invalid notebook page") );

    GetTextExtent(m_titles[page], &size.x, &size.y);

    if ( HasImage(page) )
    {
        wxSize sizeImage;
        m_imageList->GetSize(m_images[page], &sizeImage.x, &sizeImage.y);

        size.x += sizeImage.x + 5; // FIXME: hard coded margin

        if ( sizeImage.y > size.y )
            size.y = sizeImage.y;
    }

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
}

wxSize wxNotebook::GetSizeForPage(const wxSize& size) const
{
    wxSize sizeNb = size;
    wxRect rect = GetAllTabsRect();
    if ( IsVertical() )
        sizeNb.width += rect.width;
    else
        sizeNb.height += rect.height;

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
}

void wxNotebook::DoSetSize(int x, int y,
                           int width, int height,
                           int sizeFlags)
{
    wxControl::DoSetSize(x, y, width, height, sizeFlags);
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

