/////////////////////////////////////////////////////////////////////////////
// Name:        src/xrc/xh_treebk.cpp
// Purpose:     XRC resource handler for wxTreebook
// Author:      Evgeniy Tarassov
// Created:     2005/09/28
// Copyright:   (c) 2005 TT-Solutions <vadim@tt-solutions.com>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_XRC && wxUSE_TREEBOOK

#include "wx/xrc/xh_treebk.h"

#ifndef WX_PRECOMP
    #include "wx/log.h"
#endif

#include "wx/treebook.h"
#include "wx/imaglist.h"

#include "wx/xml/xml.h"

wxIMPLEMENT_DYNAMIC_CLASS(wxTreebookXmlHandler, wxXmlResourceHandler);

wxTreebookXmlHandler::wxTreebookXmlHandler()
                    : m_tbk(nullptr)
{
    XRC_ADD_STYLE(wxBK_DEFAULT);
    XRC_ADD_STYLE(wxBK_TOP);
    XRC_ADD_STYLE(wxBK_BOTTOM);
    XRC_ADD_STYLE(wxBK_LEFT);
    XRC_ADD_STYLE(wxBK_RIGHT);

    AddWindowStyles();
}

bool wxTreebookXmlHandler::CanHandle(wxXmlNode *node)
{
    return ((!IsInside() && IsOfClass(node, wxT("wxTreebook"))) ||
            (IsInside() && IsOfClass(node, wxT("treebookpage"))));
}


void
wxTreebookXmlHandler::DoAddPage(wxBookCtrlBase* book,
                                size_t n,
                                const PageWithAttrs& page)
{
    const int parent = m_pageParents.at(n);

    const int imgId = page.GetImageId();

    // This is a bit ugly, but as we know that we only call DoCreatePages()
    // with a wxTreebook, this cast here is always safe.
    wxTreebook* const tbk = static_cast<wxTreebook*>(book);

    if ( parent == -1 )
    {
        tbk->AddPage(page.wnd, page.label, page.selected, imgId);
    }
    else
    {
        tbk->InsertSubPage(parent, page.wnd, page.label, page.selected, imgId);
    }
}

wxObject *wxTreebookXmlHandler::DoCreateResource()
{
    if (m_class == wxT("wxTreebook"))
    {
        XRC_MAKE_INSTANCE(tbk, wxTreebook)

        tbk->Create(m_parentAsWindow,
                    GetID(),
                    GetPosition(), GetSize(),
                    GetStyle(wxT("style")),
                    GetName());

        wxTreebook * old_par = m_tbk;
        m_tbk = tbk;

        wxArrayTbkPageIndexes old_treeContext = m_treeContext;
        m_treeContext.Clear();

        wxVector<int> parentsSave;
        m_pageParents.swap(parentsSave);

        DoCreatePages(m_tbk);

        wxXmlNode *node = GetParamNode("object");
        int pageIndex = 0;
        for (unsigned int i = 0; i < m_tbk->GetPageCount(); i++)
        {
            if ( m_tbk->GetPage(i) )
            {
                wxXmlNode *child = node->GetChildren();
                while (child)
                {
                    if (child->GetName() == "expanded" && child->GetNodeContent() == "1")
                        m_tbk->ExpandNode(pageIndex, true);

                    child = child->GetNext();
                }
                pageIndex++;
            }
        }

        m_treeContext = old_treeContext;
        m_tbk = old_par;

        m_pageParents.swap(parentsSave);

        return tbk;
    }

//    else ( m_class == wxT("treebookpage") )


    size_t depth = GetLong( wxT("depth") );

    if ( depth > m_treeContext.GetCount() )
    {
        ReportParamError("depth", "invalid depth");
        return nullptr;
    }

    wxObject* const page = DoCreatePage(m_tbk);
    if ( !page )
    {
        // Error was already reported by DoCreatePage().
        return nullptr;
    }

    // Determine the index of the parent page to use.
    if( depth < m_treeContext.GetCount() )
        m_treeContext.RemoveAt(depth, m_treeContext.GetCount() - depth );
    if( depth != 0)
    {
        m_pageParents.push_back(m_treeContext.Item(depth - 1));
    }
    else
    {
        m_pageParents.push_back(-1);
    }

    m_treeContext.Add(m_pageParents.size() - 1);

    return page;
}

#endif // wxUSE_XRC && wxUSE_TREEBOOK
