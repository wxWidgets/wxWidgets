///////////////////////////////////////////////////////////////////////////////
// Name:        common/bookctrl.cpp
// Purpose:     wxBookCtrl implementation
// Author:      Vadim Zeitlin
// Modified by:
// Created:     19.08.03
// RCS-ID:      $Id$
// Copyright:   (c) 2003 Vadim Zeitlin <vadim@wxwindows.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "bookctrl.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_BOOKCTRL

#include "wx/imaglist.h"

#include "wx/bookctrl.h"

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// constructors and destructors
// ----------------------------------------------------------------------------

void wxBookCtrl::Init()
{
    m_imageList = NULL;
    m_ownsImageList = false;
}

bool
wxBookCtrl::Create(wxWindow *parent,
                       wxWindowID id,
                       const wxPoint& pos,
                       const wxSize& size,
                       long style,
                       const wxString& name)
{
    return wxControl::Create
                     (
                        parent,
                        id,
                        pos,
                        size,
                        style,
                        wxDefaultValidator,
                        name
                     );
}

wxBookCtrl::~wxBookCtrl()
{
    if ( m_ownsImageList )
    {
        // may be NULL, ok
        delete m_imageList;
    }
}

// ----------------------------------------------------------------------------
// image list
// ----------------------------------------------------------------------------

void wxBookCtrl::SetImageList(wxImageList *imageList)
{
    if ( m_ownsImageList )
    {
        // may be NULL, ok
        delete m_imageList;

        m_ownsImageList = false;
    }

    m_imageList = imageList;
}

void wxBookCtrl::AssignImageList(wxImageList* imageList)
{
    SetImageList(imageList);

    m_ownsImageList = true;
}

// ----------------------------------------------------------------------------
// geometry
// ----------------------------------------------------------------------------

void wxBookCtrl::SetPageSize(const wxSize& size)
{
    SetClientSize(CalcSizeFromPage(size));
}

wxSize wxBookCtrl::DoGetBestSize() const
{
    wxSize bestSize;

    // iterate over all pages, get the largest width and height
    const size_t nCount = m_pages.size();
    for ( size_t nPage = 0; nPage < nCount; nPage++ )
    {
        wxWindow *pPage = m_pages[nPage];
        wxSize childBestSize(pPage->GetBestSize());

        if ( childBestSize.x > bestSize.x )
            bestSize.x = childBestSize.x;

        if ( childBestSize.y > bestSize.y )
            bestSize.y = childBestSize.y;
    }

    // convert display area to window area, adding the size neccessary for the
    // tabs
    wxSize best = CalcSizeFromPage(bestSize);
    CacheBestSize(best);
    return best;
}

// ----------------------------------------------------------------------------
// pages management
// ----------------------------------------------------------------------------

bool
wxBookCtrl::InsertPage(size_t nPage,
                       wxWindow *page,
                       const wxString& WXUNUSED(text),
                       bool WXUNUSED(bSelect),
                       int WXUNUSED(imageId))
{
    wxCHECK_MSG( page, false, _T("NULL page in wxBookCtrl::InsertPage()") );
    wxCHECK_MSG( nPage <= m_pages.size(), false,
                 _T("invalid page index in wxBookCtrl::InsertPage()") );

    m_pages.Insert(page, nPage);
    InvalidateBestSize();

    return true;
}

bool wxBookCtrl::DeletePage(size_t nPage)
{
    wxWindow *page = DoRemovePage(nPage);
    if ( !page )
        return false;

    delete page;

    return true;
}

wxWindow *wxBookCtrl::DoRemovePage(size_t nPage)
{
    wxCHECK_MSG( nPage < m_pages.size(), NULL,
                 _T("invalid page index in wxBookCtrl::DoRemovePage()") );

    wxWindow *pageRemoved = m_pages[nPage];
    m_pages.RemoveAt(nPage);
    InvalidateBestSize();

    return pageRemoved;
}

int wxBookCtrl::GetNextPage(bool forward) const
{
    int nPage;

    int nMax = GetPageCount();
    if ( nMax-- ) // decrement it to get the last valid index
    {
        int nSel = GetSelection();

        // change selection wrapping if it becomes invalid
        nPage = forward ? nSel == nMax ? 0
                                       : nSel + 1
                        : nSel == 0 ? nMax
                                    : nSel - 1;
    }
    else // notebook is empty, no next page
    {
        nPage = -1;
    }

    return nPage;
}

#endif // wxUSE_BOOKCTRL

