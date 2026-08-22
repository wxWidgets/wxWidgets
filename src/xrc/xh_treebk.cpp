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
#include "wx/private/windowlifetime.h"
#include "wx/scopeguard.h"

#include "wx/xml/xml.h"

#include "xmlrespriv.h"

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
        const bool factoryOwned =
            wxXRCIsCurrentInstanceFactoryOwned(this);
        const bool ownsBook = m_instance == nullptr || factoryOwned;
        wxTreebook* const tbk =
            m_instance ? wxDynamicCast(m_instance, wxTreebook)
                       : new wxTreebook;
        if ( !tbk )
        {
            if ( factoryOwned )
            {
                wxObject* const wrongInstance = m_instance;
                m_instance = nullptr;
                delete wrongInstance;
            }
            ReportError("provided instance is not a wxTreebook");
            return nullptr;
        }

        const wxWeakRef<wxWindow> weakBook(tbk);
        wxWindow* const parent = m_parentAsWindow;
        const wxWeakRef<wxWindow> weakParent(parent);
        const auto contextIsLive = [&]()
        {
            return wxWeakWindowIsAvailableForCallbacks(weakBook, tbk) &&
                   (!parent ||
                    wxWeakWindowIsAvailableForCallbacks(weakParent, parent));
        };
        const auto createdBookIsLive = [&]()
        {
            return contextIsLive() && tbk->GetParent() == parent;
        };
        const auto discardOwnedBook = [&]()
        {
            if ( ownsBook && weakBook.get() == tbk &&
                    !wxWindowItselfIsUnavailableForCallbacks(tbk) )
            {
                delete tbk;
            }
        };

        const bool hidden = GetBool(wxT("hidden"), false);
        const wxWindowID id = GetID();
        const wxPoint position = GetPosition();
        const wxSize size = GetSize();
        const long style = GetStyle(wxT("style"));
        const wxString name = GetName();
        if ( !contextIsLive() )
        {
            discardOwnedBook();
            return nullptr;
        }

        if ( hidden )
            tbk->Hide();
        if ( !contextIsLive() )
        {
            discardOwnedBook();
            return nullptr;
        }
        const bool created =
            tbk->Create(parent, id, position, size, style, name);
        if ( !created || !createdBookIsLive() )
        {
            discardOwnedBook();
            return nullptr;
        }

        wxTreebook * old_par = m_tbk;
        const wxWeakRef<wxWindow> weakOld(old_par);
        m_tbk = tbk;

        wxArrayTbkPageIndexes old_treeContext = m_treeContext;
        m_treeContext.Clear();

        wxVector<int> parentsSave;
        m_pageParents.swap(parentsSave);

        const wxScopeGuard restoreBookContext = wxMakeGuard(
            [this, old_par, weakOld, &old_treeContext, &parentsSave]()
            {
                m_pageParents.swap(parentsSave);
                m_treeContext = old_treeContext;

                m_tbk = nullptr;
                if ( old_par &&
                        wxWeakWindowIsAvailableForCallbacks(
                            weakOld, old_par) )
                {
                    m_tbk = old_par;
                }
            });
        wxUnusedVar(restoreBookContext);

        if ( !DoCreatePagesSafely(tbk) || !createdBookIsLive() )
        {
            discardOwnedBook();
            return nullptr;
        }

        wxXmlNode *node = GetParamNode("object");
        if ( !createdBookIsLive() )
        {
            discardOwnedBook();
            return nullptr;
        }

        int pageIndex = 0;
        const size_t pageCount = tbk->GetPageCount();
        if ( !createdBookIsLive() )
        {
            discardOwnedBook();
            return nullptr;
        }

        for ( size_t i = 0; i < pageCount; ++i )
        {
            if ( tbk->GetPage(i) )
            {
                if ( !createdBookIsLive() )
                {
                    discardOwnedBook();
                    return nullptr;
                }

                wxXmlNode *child = node->GetChildren();
                while (child)
                {
                    if (child->GetName() == "expanded" && child->GetNodeContent() == "1")
                    {
                        tbk->ExpandNode(pageIndex, true);
                        if ( !createdBookIsLive() )
                        {
                            discardOwnedBook();
                            return nullptr;
                        }
                    }

                    child = child->GetNext();
                }
                pageIndex++;
            }
        }

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
