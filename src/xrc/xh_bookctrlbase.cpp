///////////////////////////////////////////////////////////////////////////////
// Name:        src/xrc/xh_bookctrlbase.cpp
// Purpose:     wxBookCtrlXmlHandlerBase implementation
// Author:      Vadim Zeitlin
// Created:     2022-02-24
// Copyright:   (c) 2022 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_XRC && wxUSE_BOOKCTRL

#include "wx/xrc/xh_bookctrlbase.h"

#include "wx/bookctrl.h"

// ============================================================================
// wxBookCtrlXmlHandlerBase implementation
// ============================================================================

wxBookCtrlXmlHandlerBase::PageWithAttrs::PageWithAttrs()
{
    wnd = NULL;
    selected = false;
    imgId =
    bmpId = wxWithImages::NO_IMAGE;
}

int wxBookCtrlXmlHandlerBase::PageWithAttrs::GetImageId() const
{
    return bmpId != wxWithImages::NO_IMAGE ? bmpId : imgId;
}

wxBookCtrlXmlHandlerBase::wxBookCtrlXmlHandlerBase()
                        : m_isInside(false)
{
}

wxBookCtrlXmlHandlerBase::~wxBookCtrlXmlHandlerBase()
{
    // Dtor is trivial but has to be defined here because it implicitly uses
    // PageWithAttrs which is not fully declared in the header.
}

void
wxBookCtrlXmlHandlerBase::DoAddPage(wxBookCtrlBase* book,
                                    size_t WXUNUSED(n),
                                    const PageWithAttrs& page)
{
    book->AddPage(page.wnd, page.label, page.selected, page.GetImageId());
}

void wxBookCtrlXmlHandlerBase::DoCreatePages(wxBookCtrlBase* book)
{
    bool old_ins = m_isInside;
    m_isInside = true;

    wxVector<PageWithAttrs> pagesSave;
    m_bookPages.swap(pagesSave);

    wxVector<wxBitmapBundle> imagesSave;
    m_bookImages.swap(imagesSave);

    wxImageList *imagelist = GetImageList();
    if ( imagelist )
        book->AssignImageList(imagelist);

    CreateChildren(book, true /* only use this handler */);

    if ( !m_bookImages.empty() )
        book->SetImages(m_bookImages);

    for ( size_t i = 0; i < m_bookPages.size(); ++i )
    {
        DoAddPage(book, i, m_bookPages[i]);
    }

    m_bookImages.swap(imagesSave);
    m_bookPages.swap(pagesSave);

    m_isInside = old_ins;
}

wxObject*
wxBookCtrlXmlHandlerBase::DoCreatePage(wxBookCtrlBase* book)
{
    wxXmlNode *n = GetParamNode(wxT("object"));

    if ( !n )
        n = GetParamNode(wxT("object_ref"));

    if (n)
    {
        bool old_ins = m_isInside;
        m_isInside = false;
        wxObject *item = CreateResFromNode(n, book, NULL);
        m_isInside = old_ins;
        wxWindow *wnd = wxDynamicCast(item, wxWindow);

        if (wnd)
        {
            PageWithAttrs currentPage;

            if ( HasParam(wxT("bitmap")) )
            {
                m_bookImages.push_back( GetBitmapBundle(wxT("bitmap"), wxART_OTHER) );
                currentPage.bmpId = m_bookImages.size() - 1;
            }
            else if ( HasParam(wxT("image")) )
            {
                if ( book->GetImageList() )
                {
                    currentPage.imgId = (int)GetLong(wxT("image"));
                }
                else // image without image list?
                {
                    ReportError(n, "image can only be used in conjunction "
                                   "with imagelist");
                }
            }

            currentPage.wnd = wnd;
            currentPage.label = GetText(wxT("label"));
            currentPage.selected = GetBool(wxT("selected"));
            m_bookPages.push_back(currentPage);
        }
        else
        {
            ReportError(n, wxString::Format("%s child must be a window", m_class));
        }
        return wnd;
    }
    else
    {
        ReportError(wxString::Format("%s must have a window child", m_class));
        return NULL;
    }
}

#endif // wxUSE_XRC && wxUSE_BOOKCTRL
