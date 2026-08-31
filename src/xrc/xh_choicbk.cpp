/////////////////////////////////////////////////////////////////////////////
// Name:        src/xrc/xh_choicbk.cpp
// Purpose:     XRC resource for wxChoicebook
// Author:      Vaclav Slavik
// Created:     2000/03/21
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_XRC && wxUSE_CHOICEBOOK

#include "wx/xrc/xh_choicbk.h"

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/sizer.h"
#endif

#include "wx/choicebk.h"
#include "wx/imaglist.h"
#include "wx/private/windowlifetime.h"
#include "wx/scopeguard.h"

#include "xmlrespriv.h"

wxIMPLEMENT_DYNAMIC_CLASS(wxChoicebookXmlHandler, wxXmlResourceHandler);

wxChoicebookXmlHandler::wxChoicebookXmlHandler()
                      : m_choicebook(nullptr)
{
    XRC_ADD_STYLE(wxBK_DEFAULT);
    XRC_ADD_STYLE(wxBK_LEFT);
    XRC_ADD_STYLE(wxBK_RIGHT);
    XRC_ADD_STYLE(wxBK_TOP);
    XRC_ADD_STYLE(wxBK_BOTTOM);

    XRC_ADD_STYLE(wxCHB_DEFAULT);
    XRC_ADD_STYLE(wxCHB_LEFT);
    XRC_ADD_STYLE(wxCHB_RIGHT);
    XRC_ADD_STYLE(wxCHB_TOP);
    XRC_ADD_STYLE(wxCHB_BOTTOM);

    AddWindowStyles();
}

wxObject *wxChoicebookXmlHandler::DoCreateResource()
{
    if (m_class == wxT("choicebookpage"))
    {
        return DoCreatePage(m_choicebook);
    }

    else
    {
        const bool factoryOwned =
            wxXRCIsCurrentInstanceFactoryOwned(this);
        const bool ownsBook = m_instance == nullptr || factoryOwned;
        wxChoicebook* const nb =
            m_instance ? wxDynamicCast(m_instance, wxChoicebook)
                       : new wxChoicebook;
        if ( !nb )
        {
            if ( factoryOwned )
            {
                wxObject* const wrongInstance = m_instance;
                m_instance = nullptr;
                delete wrongInstance;
            }
            ReportError("provided instance is not a wxChoicebook");
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

        wxChoicebook *old_par = m_choicebook;
        const wxWeakRef<wxWindow> weakOld(old_par);
        m_choicebook = nb;
        wxScopeGuard restoreBook = wxMakeGuard(
            [this, old_par, weakOld]()
            {
                m_choicebook = nullptr;
                if ( old_par &&
                        wxWeakWindowIsAvailableForCallbacks(
                            weakOld, old_par) )
                {
                    m_choicebook = old_par;
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

bool wxChoicebookXmlHandler::CanHandle(wxXmlNode *node)
{
    return ((!IsInside() && IsOfClass(node, wxT("wxChoicebook"))) ||
            (IsInside() && IsOfClass(node, wxT("choicebookpage"))));
}

#endif // wxUSE_XRC && wxUSE_CHOICEBOOK
