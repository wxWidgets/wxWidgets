/////////////////////////////////////////////////////////////////////////////
// Name:        src/xrc/xh_listbk.cpp
// Purpose:     XRC resource for wxListbook
// Author:      Vaclav Slavik
// Created:     2000/03/21
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_XRC && wxUSE_LISTBOOK

#include "wx/xrc/xh_listbk.h"

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/sizer.h"
#endif

#include "wx/listbook.h"
#include "wx/imaglist.h"
#include "wx/private/windowlifetime.h"
#include "wx/scopeguard.h"

#include "xmlrespriv.h"

wxIMPLEMENT_DYNAMIC_CLASS(wxListbookXmlHandler, wxXmlResourceHandler);

wxListbookXmlHandler::wxListbookXmlHandler()
                    : m_listbook(nullptr)
{
    XRC_ADD_STYLE(wxBK_DEFAULT);
    XRC_ADD_STYLE(wxBK_LEFT);
    XRC_ADD_STYLE(wxBK_RIGHT);
    XRC_ADD_STYLE(wxBK_TOP);
    XRC_ADD_STYLE(wxBK_BOTTOM);

    XRC_ADD_STYLE(wxLB_DEFAULT);
    XRC_ADD_STYLE(wxLB_LEFT);
    XRC_ADD_STYLE(wxLB_RIGHT);
    XRC_ADD_STYLE(wxLB_TOP);
    XRC_ADD_STYLE(wxLB_BOTTOM);

    AddWindowStyles();
}

wxObject *wxListbookXmlHandler::DoCreateResource()
{
    if (m_class == wxT("listbookpage"))
    {
        return DoCreatePage(m_listbook);
    }

    else
    {
        const bool factoryOwned =
            wxXRCIsCurrentInstanceFactoryOwned(this);
        const bool ownsBook = m_instance == nullptr || factoryOwned;
        wxListbook* const nb =
            m_instance ? wxDynamicCast(m_instance, wxListbook)
                       : new wxListbook;
        if ( !nb )
        {
            if ( factoryOwned )
            {
                wxObject* const wrongInstance = m_instance;
                m_instance = nullptr;
                delete wrongInstance;
            }
            ReportError("provided instance is not a wxListbook");
            return nullptr;
        }

        const wxWeakRef<wxWindow> weakBook(nb);
        wxWindow* const parent = m_parentAsWindow;
        const wxWeakRef<wxWindow> weakParent(parent);
        const auto contextIsLive = [&]()
        {
            return wxWeakWindowIsAvailableForCallbacks(weakBook, nb) &&
                   (!parent ||
                    wxWeakWindowIsAvailableForCallbacks(weakParent, parent));
        };
        const auto createdBookIsLive = [&]()
        {
            return contextIsLive() && nb->GetParent() == parent;
        };
        const auto discardOwnedBook = [&]()
        {
            if ( ownsBook && weakBook.get() == nb &&
                    !wxWindowItselfIsUnavailableForCallbacks(nb) )
            {
                delete nb;
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
            nb->Hide();
        if ( !contextIsLive() )
        {
            discardOwnedBook();
            return nullptr;
        }
        const bool created =
            nb->Create(parent, id, position, size, style, name);
        if ( !created || !createdBookIsLive() )
        {
            discardOwnedBook();
            return nullptr;
        }

        wxListbook *old_par = m_listbook;
        const wxWeakRef<wxWindow> weakOld(old_par);
        m_listbook = nb;
        wxScopeGuard restoreBook = wxMakeGuard(
            [this, old_par, weakOld]()
            {
                m_listbook = nullptr;
                if ( old_par &&
                        wxWeakWindowIsAvailableForCallbacks(
                            weakOld, old_par) )
                {
                    m_listbook = old_par;
                }
            });
        wxUnusedVar(restoreBook);

        if ( !DoCreatePagesSafely(nb) || !createdBookIsLive() )
        {
            discardOwnedBook();
            return nullptr;
        }

        return nb;
    }
}

bool wxListbookXmlHandler::CanHandle(wxXmlNode *node)
{
    return ((!IsInside() && IsOfClass(node, wxT("wxListbook"))) ||
            (IsInside() && IsOfClass(node, wxT("listbookpage"))));
}

#endif // wxUSE_XRC && wxUSE_LISTBOOK
