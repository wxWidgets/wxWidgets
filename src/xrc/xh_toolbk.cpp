/////////////////////////////////////////////////////////////////////////////
// Name:        src/xrc/xh_toolbk.cpp
// Purpose:     XRC resource for wxToolbook
// Author:      Andrea Zanellato
// Created:     2009/12/12
// Copyright:   (c) 2010 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_XRC && wxUSE_TOOLBOOK

#include "wx/xrc/xh_toolbk.h"

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/sizer.h"
#endif

#include "wx/toolbook.h"
#include "wx/imaglist.h"

#include "wx/xml/xml.h"

wxIMPLEMENT_DYNAMIC_CLASS(wxToolbookXmlHandler, wxXmlResourceHandler);

wxToolbookXmlHandler::wxToolbookXmlHandler()
                     :wxXmlResourceHandler(),
                      m_isInside(false),
                      m_toolbook(NULL)
{
    XRC_ADD_STYLE(wxBK_DEFAULT);
    XRC_ADD_STYLE(wxBK_TOP);
    XRC_ADD_STYLE(wxBK_BOTTOM);
    XRC_ADD_STYLE(wxBK_LEFT);
    XRC_ADD_STYLE(wxBK_RIGHT);

    XRC_ADD_STYLE(wxTBK_BUTTONBAR);
    XRC_ADD_STYLE(wxTBK_HORZ_LAYOUT);

    AddWindowStyles();
}

wxObject *wxToolbookXmlHandler::DoCreateResource()
{
    if (m_class == wxT("toolbookpage"))
    {
        wxXmlNode *n = GetParamNode(wxT("object"));

        if ( !n )
            n = GetParamNode(wxT("object_ref"));

        if (n)
        {
            bool old_ins = m_isInside;
            m_isInside = false;
            wxObject *item = CreateResFromNode(n, m_toolbook, NULL);
            m_isInside = old_ins;
            wxWindow *wnd = wxDynamicCast(item, wxWindow);

            if (wnd)
            {
                newPage currentPage;

                if ( HasParam(wxT("bitmap")) )
                {
                    m_bookImages.push_back( GetBitmapBundle(wxT("bitmap"), wxART_OTHER) );
                    currentPage.bmpId = m_bookImages.size() - 1;
                }
                else if ( HasParam(wxT("image")) )
                {
                    if ( m_toolbook->GetImageList() )
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
                ReportError(n, "toolbookpage child must be a window");
            }
            return wnd;
        }
        else
        {
            ReportError("toolbookpage must have a window child");
            return NULL;
        }
    }

    else
    {
        XRC_MAKE_INSTANCE(nb, wxToolbook)

        nb->Create( m_parentAsWindow,
                    GetID(),
                    GetPosition(), GetSize(),
                    GetStyle(wxT("style")),
                    GetName() );

        wxImageList *imagelist = GetImageList();
        if ( imagelist )
            nb->AssignImageList(imagelist);

        wxToolbook *old_par = m_toolbook;
        m_toolbook = nb;
        bool old_ins = m_isInside;
        m_isInside = true;
        wxVector<newPage> old_pages = m_bookPages;
        m_bookPages.clear();
        wxVector<wxBitmapBundle> old_images = m_bookImages;
        m_bookImages.clear();
        CreateChildren(m_toolbook, true/*only this handler*/);

        if ( !m_bookImages.empty() )
        {
            m_toolbook->SetImages(m_bookImages);
        }
        for ( size_t i = 0; i < m_bookPages.size(); ++i )
        {
            const newPage& currentPage = m_bookPages.at(i);
            int imgId = currentPage.bmpId;
            if ( imgId == -1 )
            {
                imgId = currentPage.imgId;
            }
            m_toolbook->AddPage(currentPage.wnd, currentPage.label,
                        currentPage.selected, imgId );
        }

        m_isInside = old_ins;
        m_toolbook = old_par;
        m_bookPages = old_pages;
        m_bookImages = old_images;

        return nb;
    }
}

bool wxToolbookXmlHandler::CanHandle(wxXmlNode *node)
{
    return ((!m_isInside && IsOfClass(node, wxT("wxToolbook"))) ||
            (m_isInside && IsOfClass(node, wxT("toolbookpage"))));
}

#endif // wxUSE_XRC && wxUSE_TOOLBOOK
