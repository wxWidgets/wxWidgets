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
#include "wx/private/windowlifetime.h"
#include "wx/scopeguard.h"

#include "wx/xml/xml.h"

#include "xmlrespriv.h"

wxIMPLEMENT_DYNAMIC_CLASS(wxToolbookXmlHandler, wxXmlResourceHandler);

wxToolbookXmlHandler::wxToolbookXmlHandler()
                    : m_toolbook(nullptr)
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
        return DoCreatePage(m_toolbook);
    }

    else
    {
        const bool factoryOwned =
            wxXRCIsCurrentInstanceFactoryOwned(this);
        const bool ownsBook = m_instance == nullptr || factoryOwned;
        wxToolbook* const nb =
            m_instance ? wxDynamicCast(m_instance, wxToolbook)
                       : new wxToolbook;
        if ( !nb )
        {
            if ( factoryOwned )
            {
                wxObject* const wrongInstance = m_instance;
                m_instance = nullptr;
                delete wrongInstance;
            }
            ReportError("provided instance is not a wxToolbook");
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

        wxToolbook *old_par = m_toolbook;
        const wxWeakRef<wxWindow> weakOld(old_par);
        m_toolbook = nb;
        wxScopeGuard restoreBook = wxMakeGuard(
            [this, old_par, weakOld]()
            {
                m_toolbook = nullptr;
                if ( old_par &&
                        wxWeakWindowIsAvailableForCallbacks(
                            weakOld, old_par) )
                {
                    m_toolbook = old_par;
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

bool wxToolbookXmlHandler::CanHandle(wxXmlNode *node)
{
    return ((!IsInside() && IsOfClass(node, wxT("wxToolbook"))) ||
            (IsInside() && IsOfClass(node, wxT("toolbookpage"))));
}

#endif // wxUSE_XRC && wxUSE_TOOLBOOK
