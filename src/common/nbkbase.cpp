///////////////////////////////////////////////////////////////////////////////
// Name:        src/common/nbkbase.cpp
// Purpose:     common wxNotebook methods
// Author:      Vadim Zeitlin
// Modified by:
// Created:     02.07.01
// RCS-ID:      $Id$
// Copyright:   (c) 2001 Vadim Zeitlin
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "notebookbase.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_NOTEBOOK

#ifndef WX_PRECOMP
#endif //WX_PRECOMP

#include "wx/imaglist.h"
#include "wx/notebook.h"

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// constructors and destructors
// ----------------------------------------------------------------------------

void wxNotebookBase::Init()
{
    m_imageList = NULL;
    m_ownsImageList = FALSE;
}

wxNotebookBase::~wxNotebookBase()
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

void wxNotebookBase::SetImageList(wxImageList* imageList)
{
    if ( m_ownsImageList )
    {
        // may be NULL, ok
        delete m_imageList;

        m_ownsImageList = FALSE;
    }

    m_imageList = imageList;
}

void wxNotebookBase::AssignImageList(wxImageList* imageList)
{
    SetImageList(imageList);
    m_ownsImageList = TRUE;
}

// ----------------------------------------------------------------------------
// geometry
// ----------------------------------------------------------------------------

wxSize wxNotebookBase::CalcSizeFromPage(const wxSize& sizePage)
{
    // this was just taken from wxNotebookSizer::CalcMin() and is, of
    // course, totally bogus - just like the original code was
    wxSize sizeTotal = sizePage;
    
    // Mac has large notebook borders.

    if ( HasFlag(wxNB_LEFT) || HasFlag(wxNB_RIGHT) )
    {
        sizeTotal.x += 90;
#ifdef __WXMAC__
        sizeTotal.y += 15;
#else
        sizeTotal.y += 10;
#endif
    }
    else
    {
#ifdef __WXMAC__
        sizeTotal.x += 28;   // This is correct for Aqua.
#else
        sizeTotal.x += 10;
#endif
        sizeTotal.y += 40;
    }

    return sizeTotal;
}

// ----------------------------------------------------------------------------
// pages management
// ----------------------------------------------------------------------------

bool wxNotebookBase::DeletePage(int nPage)
{
    wxNotebookPage *page = DoRemovePage(nPage);
    if ( !page )
        return FALSE;

    delete page;

    return TRUE;
}

wxNotebookPage *wxNotebookBase::DoRemovePage(int nPage)
{
    wxCHECK_MSG( nPage >= 0 && (size_t)nPage < m_pages.GetCount(), NULL,
                 _T("invalid page index in wxNotebookBase::DoRemovePage()") );

    wxNotebookPage *pageRemoved = m_pages[nPage];
    m_pages.RemoveAt(nPage);

    return pageRemoved;
}

int wxNotebookBase::GetNextPage(bool forward) const
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

#endif // wxUSE_NOTEBOOK

