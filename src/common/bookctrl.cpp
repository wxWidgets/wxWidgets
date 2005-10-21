///////////////////////////////////////////////////////////////////////////////
// Name:        src/common/bookctrl.cpp
// Purpose:     wxBookCtrlBase implementation
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
// event table
// ----------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxBookCtrlBase, wxControl)

BEGIN_EVENT_TABLE(wxBookCtrlBase, wxControl)
    EVT_SIZE(wxBookCtrlBase::OnSize)
END_EVENT_TABLE()

// ----------------------------------------------------------------------------
// constructors and destructors
// ----------------------------------------------------------------------------

void wxBookCtrlBase::Init()
{
    m_bookctrl = NULL;
    m_imageList = NULL;
    m_ownsImageList = false;

#if defined(__WXWINCE__)
    m_internalBorder = 1;
#else
    m_internalBorder = 5;
#endif
}

bool
wxBookCtrlBase::Create(wxWindow *parent,
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

wxBookCtrlBase::~wxBookCtrlBase()
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

void wxBookCtrlBase::SetImageList(wxImageList *imageList)
{
    if ( m_ownsImageList )
    {
        // may be NULL, ok
        delete m_imageList;

        m_ownsImageList = false;
    }

    m_imageList = imageList;
}

void wxBookCtrlBase::AssignImageList(wxImageList* imageList)
{
    SetImageList(imageList);

    m_ownsImageList = true;
}

// ----------------------------------------------------------------------------
// geometry
// ----------------------------------------------------------------------------

void wxBookCtrlBase::SetPageSize(const wxSize& size)
{
    SetClientSize(CalcSizeFromPage(size));
}

wxSize wxBookCtrlBase::DoGetBestSize() const
{
    wxSize bestSize;

    // iterate over all pages, get the largest width and height
    const size_t nCount = m_pages.size();
    for ( size_t nPage = 0; nPage < nCount; nPage++ )
    {
        const wxWindow * const pPage = m_pages[nPage];
        if( pPage )
        {
            wxSize childBestSize(pPage->GetBestSize());

            if ( childBestSize.x > bestSize.x )
                bestSize.x = childBestSize.x;

            if ( childBestSize.y > bestSize.y )
                bestSize.y = childBestSize.y;
        }
    }

    // convert display area to window area, adding the size necessary for the
    // tabs
    wxSize best = CalcSizeFromPage(bestSize);
    CacheBestSize(best);
    return best;
}

// ----------------------------------------------------------------------------
// pages management
// ----------------------------------------------------------------------------

bool
wxBookCtrlBase::InsertPage(size_t nPage,
                           wxWindow *page,
                           const wxString& WXUNUSED(text),
                           bool WXUNUSED(bSelect),
                           int WXUNUSED(imageId))
{
    wxCHECK_MSG( page || AllowNullPage(), false,
                 _T("NULL page in wxBookCtrlBase::InsertPage()") );
    wxCHECK_MSG( nPage <= m_pages.size(), false,
                 _T("invalid page index in wxBookCtrlBase::InsertPage()") );

    m_pages.Insert(page, nPage);
    InvalidateBestSize();

    return true;
}

bool wxBookCtrlBase::DeletePage(size_t nPage)
{
    wxWindow *page = DoRemovePage(nPage);
    if ( !(page || AllowNullPage()) )
        return false;

    // delete NULL is harmless
    delete page;

    return true;
}

wxWindow *wxBookCtrlBase::DoRemovePage(size_t nPage)
{
    wxCHECK_MSG( nPage < m_pages.size(), NULL,
                 _T("invalid page index in wxBookCtrlBase::DoRemovePage()") );

    wxWindow *pageRemoved = m_pages[nPage];
    m_pages.RemoveAt(nPage);
    InvalidateBestSize();

    return pageRemoved;
}

int wxBookCtrlBase::GetNextPage(bool forward) const
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
        nPage = wxNOT_FOUND;
    }

    return nPage;
}

wxRect wxBookCtrlBase::GetPageRect() const
{
    const wxSize size = GetControllerSize();

    wxPoint pt;
    wxRect rectPage(pt, GetClientSize());
    switch ( GetWindowStyle() & wxBK_ALIGN_MASK )
    {
        default:
            wxFAIL_MSG( _T("unexpected alignment") );
            // fall through

        case wxBK_TOP:
            rectPage.y = size.y + GetInternalBorder();
            // fall through

        case wxBK_BOTTOM:
            rectPage.height -= size.y + GetInternalBorder();
            break;

        case wxBK_LEFT:
            rectPage.x = size.x + GetInternalBorder();
            // fall through

        case wxBK_RIGHT:
            rectPage.width -= size.x + GetInternalBorder();
            break;
    }

    return rectPage;
}

void wxBookCtrlBase::OnSize(wxSizeEvent& event)
{
    event.Skip();

    if ( !m_bookctrl )
    {
        // we're not fully created yet or OnSize() should be hidden by derived class
        return;
    }

    // resize controller and the page area to fit inside our new size
    const wxSize sizeClient( GetClientSize() ),
                 sizeBorder( m_bookctrl->GetSize() - m_bookctrl->GetClientSize() ),
                 sizeCtrl( GetControllerSize() );

    m_bookctrl->SetClientSize( sizeCtrl.x - sizeBorder.x, sizeCtrl.y - sizeBorder.y );

    const wxSize sizeNew = m_bookctrl->GetSize();
    wxPoint posCtrl;
    switch ( GetWindowStyle() & wxBK_ALIGN_MASK )
    {
        default:
            wxFAIL_MSG( _T("unexpected alignment") );
            // fall through

        case wxBK_TOP:
        case wxBK_LEFT:
            // posCtrl is already ok
            break;

        case wxBK_BOTTOM:
            posCtrl.y = sizeClient.y - sizeNew.y;
            break;

        case wxBK_RIGHT:
            posCtrl.x = sizeClient.x - sizeNew.x;
            break;
    }

    if ( m_bookctrl->GetPosition() != posCtrl )
        m_bookctrl->Move(posCtrl);

    // resize the currently shown page
    if (GetSelection() != wxNOT_FOUND )
    {
        wxWindow *page = m_pages[GetSelection()];
        wxCHECK_RET( page, _T("NULL page?") );
        page->SetSize(GetPageRect());
    }
}

wxSize wxBookCtrlBase::GetControllerSize() const
{
    if(!m_bookctrl)
        return wxSize(0,0);

    const wxSize sizeClient = GetClientSize(),
                 sizeBorder = m_bookctrl->GetSize() - m_bookctrl->GetClientSize(),
                 sizeCtrl = m_bookctrl->GetBestSize() + sizeBorder;

    wxSize size;

    if ( IsVertical() )
    {
        size.x = sizeClient.x;
        size.y = sizeCtrl.y;
    }
    else // left/right aligned
    {
        size.x = sizeCtrl.x;
        size.y = sizeClient.y;
    }

    return size;
}

#endif // wxUSE_BOOKCTRL
