/////////////////////////////////////////////////////////////////////////////
// Name:        src/xrc/xh_notbk.cpp
// Purpose:     XRC resource for wxNotebook
// Author:      Vaclav Slavik
// Created:     2000/03/21
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_XRC && wxUSE_NOTEBOOK

#include "wx/xrc/xh_notbk.h"

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/sizer.h"
#endif

#include "wx/notebook.h"
#include "wx/imaglist.h"

wxIMPLEMENT_DYNAMIC_CLASS(wxNotebookXmlHandler, wxXmlResourceHandler);

wxNotebookXmlHandler::wxNotebookXmlHandler()
                     :wxXmlResourceHandler(),
                      m_isInside(false),
                      m_notebook(NULL)
{
    XRC_ADD_STYLE(wxBK_DEFAULT);
    XRC_ADD_STYLE(wxBK_LEFT);
    XRC_ADD_STYLE(wxBK_RIGHT);
    XRC_ADD_STYLE(wxBK_TOP);
    XRC_ADD_STYLE(wxBK_BOTTOM);

    // provide the old synonyms for these fields as well
    XRC_ADD_STYLE(wxNB_DEFAULT);
    XRC_ADD_STYLE(wxNB_LEFT);
    XRC_ADD_STYLE(wxNB_RIGHT);
    XRC_ADD_STYLE(wxNB_TOP);
    XRC_ADD_STYLE(wxNB_BOTTOM);

    XRC_ADD_STYLE(wxNB_FIXEDWIDTH);
    XRC_ADD_STYLE(wxNB_MULTILINE);
    XRC_ADD_STYLE(wxNB_NOPAGETHEME);

    AddWindowStyles();
}

wxObject *wxNotebookXmlHandler::DoCreateResource()
{
    if (m_class == wxT("notebookpage"))
    {
        wxXmlNode *n = GetParamNode(wxT("object"));

        if ( !n )
            n = GetParamNode(wxT("object_ref"));

        if (n)
        {
            bool old_ins = m_isInside;
            m_isInside = false;
            wxObject *item = CreateResFromNode(n, m_notebook, NULL);
            m_isInside = old_ins;
            wxWindow *wnd = wxDynamicCast(item, wxWindow);

            if (wnd)
            {
                m_notebook->AddPage(wnd, GetText(wxT("label")),
                                         GetBool(wxT("selected")));
                if ( HasParam(wxT("bitmap")) )
                {
                    m_bookImages.push_back( GetBitmapBundle(wxT("bitmap"), wxART_OTHER) );
                    m_bookImagesIdx.push_back( m_notebook->GetPageCount()-1 );
                }
                else if ( HasParam(wxT("image")) )
                {
                    if ( m_notebook->GetImageList() )
                    {
                        m_notebook->SetPageImage(m_notebook->GetPageCount()-1,
                                                 GetLong(wxT("image")) );
                    }
                    else // image without image list?
                    {
                        ReportError(n, "image can only be used in conjunction "
                                       "with imagelist");
                    }
                }
            }
            else
            {
                ReportError(n, "notebookpage child must be a window");
            }
            return wnd;
        }
        else
        {
            ReportError("notebookpage must have a window child");
            return NULL;
        }
    }

    else
    {
        XRC_MAKE_INSTANCE(nb, wxNotebook)

        nb->Create(m_parentAsWindow,
                   GetID(),
                   GetPosition(), GetSize(),
                   GetStyle(wxT("style")),
                   GetName());

        wxImageList *imagelist = GetImageList();
        if ( imagelist )
            nb->AssignImageList(imagelist);

        SetupWindow(nb);

        wxNotebook *old_par = m_notebook;
        m_notebook = nb;
        bool old_ins = m_isInside;
        m_isInside = true;
        wxVector<wxBitmapBundle> old_images = m_bookImages;
        m_bookImages.clear();
        wxVector<size_t> old_imageIdx = m_bookImagesIdx;
        m_bookImagesIdx.clear();
        CreateChildren(m_notebook, true/*only this handler*/);

        if ( !m_bookImages.empty() )
        {
            m_notebook->SetImages(m_bookImages);
            for ( size_t i = 0; i < m_bookImagesIdx.size(); ++i )
            {
                m_notebook->SetPageImage( m_bookImagesIdx[i], i );
            }
        }

        m_isInside = old_ins;
        m_notebook = old_par;
        m_bookImages = old_images;
        m_bookImagesIdx = old_imageIdx;

        return nb;
    }
}

bool wxNotebookXmlHandler::CanHandle(wxXmlNode *node)
{
    return ((!m_isInside && IsOfClass(node, wxT("wxNotebook"))) ||
            (m_isInside && IsOfClass(node, wxT("notebookpage"))));
}

#endif // wxUSE_XRC && wxUSE_NOTEBOOK
