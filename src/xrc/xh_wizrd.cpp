/////////////////////////////////////////////////////////////////////////////
// Name:        src/xrc/xh_wizrd.cpp
// Purpose:     XRC resource for wxWizard
// Author:      Vaclav Slavik
// Created:     2003/03/01
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_XRC && wxUSE_WIZARDDLG

#include "wx/xrc/xh_wizrd.h"

#ifndef WX_PRECOMP
    #include "wx/log.h"
#endif

#include "wx/wizard.h"
#include "wx/private/windowlifetime.h"
#include "wx/scopeguard.h"
#include "wx/xml/xml.h"

#include "xmlrespriv.h"

#include <unordered_map>

namespace
{

bool IsLiveWindow(const wxWindowRef& weakWindow,
                  const wxWindow* expected)
{
    return wxWeakWindowIsAvailableForCallbacks(weakWindow, expected);
}

wxWizardPage* FindLiveWizardPageChildByObjectAddress(
    wxWizard* wizard,
    const wxObject* address)
{
    if ( !wizard || !address )
        return nullptr;

    const wxWindowRef weakWizard(wizard);
    if ( !IsLiveWindow(weakWizard, wizard) )
        return nullptr;

    // CreateResFromNode() is an application callback boundary: its returned
    // pointer can already be stale. Compare it only with wxObject addresses
    // obtained from the current, weak-validated child list; cast the live child
    // rather than the untrusted return value.
    for ( wxWindowList::compatibility_iterator node =
              wizard->GetChildren().GetFirst();
          node;
          node = node->GetNext() )
    {
        wxWindow* const child = node->GetData();
        const wxWindowRef weakChild(child);
        if ( !IsLiveWindow(weakWizard, wizard) ||
             !IsLiveWindow(weakChild, child) ||
             child->GetParent() != wizard )
        {
            return nullptr;
        }

        if ( static_cast<wxObject*>(child) == address )
            return wxDynamicCast(child, wxWizardPage);
    }

    return nullptr;
}

unsigned long NextGeneration(unsigned long& generation)
{
    // Zero is reserved for "no active context".
    if ( ++generation == 0 )
        ++generation;

    return generation;
}

struct WizardXmlRuntimeState
{
    wxWindowRef wizardLifetime;
    wxWindowRef lastSimplePageLifetime;
    unsigned long contextGeneration = 0;
    unsigned long pageOperationGeneration = 0;
    unsigned long nextContextGeneration = 0;
};

using WizardXmlRuntimeStates =
    std::unordered_map<const wxWizardXmlHandler*, WizardXmlRuntimeState>;

WizardXmlRuntimeStates& GetWizardXmlRuntimeStates()
{
    // The public handler layout is ABI-stable. Deliberately leak this small
    // sidecar table to avoid static-destruction ordering with tracked windows;
    // a reused handler address is reset by its constructor.
    static WizardXmlRuntimeStates* const states =
        new WizardXmlRuntimeStates;
    return *states;
}

WizardXmlRuntimeState& WizardXmlState(wxWizardXmlHandler* handler)
{
    return GetWizardXmlRuntimeStates()[handler];
}

} // anonymous namespace

wxIMPLEMENT_DYNAMIC_CLASS(wxWizardXmlHandler, wxXmlResourceHandler);

wxWizardXmlHandler::wxWizardXmlHandler()
    : wxXmlResourceHandler(),
      m_wizard(nullptr),
      m_lastSimplePage(nullptr)
{
    WizardXmlRuntimeState& state = WizardXmlState(this);
    state.wizardLifetime.Release();
    state.lastSimplePageLifetime.Release();
    state.contextGeneration = 0;
    state.pageOperationGeneration = 0;
    state.nextContextGeneration = 0;

    XRC_ADD_STYLE(wxSTAY_ON_TOP);
    XRC_ADD_STYLE(wxCAPTION);
    XRC_ADD_STYLE(wxDEFAULT_DIALOG_STYLE);
    XRC_ADD_STYLE(wxSYSTEM_MENU);
    XRC_ADD_STYLE(wxRESIZE_BORDER);
    XRC_ADD_STYLE(wxCLOSE_BOX);
    XRC_ADD_STYLE(wxDIALOG_NO_PARENT);

    XRC_ADD_STYLE(wxTAB_TRAVERSAL);
    XRC_ADD_STYLE(wxWS_EX_VALIDATE_RECURSIVELY);
    XRC_ADD_STYLE(wxDIALOG_EX_METAL);
    XRC_ADD_STYLE(wxMAXIMIZE_BOX);
    XRC_ADD_STYLE(wxMINIMIZE_BOX);
    XRC_ADD_STYLE(wxFRAME_SHAPED);
    XRC_ADD_STYLE(wxDIALOG_EX_CONTEXTHELP);

    XRC_ADD_STYLE(wxWIZARD_EX_HELPBUTTON);
    AddWindowStyles();

    // bitmap placement flags
    XRC_ADD_STYLE(wxWIZARD_VALIGN_TOP);
    XRC_ADD_STYLE(wxWIZARD_VALIGN_CENTRE);
    XRC_ADD_STYLE(wxWIZARD_VALIGN_BOTTOM);
    XRC_ADD_STYLE(wxWIZARD_HALIGN_LEFT);
    XRC_ADD_STYLE(wxWIZARD_HALIGN_CENTRE);
    XRC_ADD_STYLE(wxWIZARD_HALIGN_RIGHT);
    XRC_ADD_STYLE(wxWIZARD_TILE);
}

wxWizardXmlHandler::~wxWizardXmlHandler()
{
    GetWizardXmlRuntimeStates().erase(this);
}

wxObject *wxWizardXmlHandler::DoCreateResource()
{
    if (m_class == wxT("wxWizard"))
    {
        // A caller-supplied m_instance remains caller-owned, while an instance
        // produced by an XRC subclass factory belongs to this resource load.
        // Only resource-owned objects may be deleted when creation fails.
        const bool factoryOwned =
            wxXRCIsCurrentInstanceFactoryOwned(this);
        const bool ownsWizard = m_instance == nullptr || factoryOwned;
        wxWizard* wiz = m_instance
                            ? wxDynamicCast(m_instance, wxWizard)
                            : new wxWizard;
        if ( !wiz )
        {
            if ( factoryOwned )
            {
                wxObject* const wrongInstance = m_instance;
                m_instance = nullptr;
                delete wrongInstance;
            }
            ReportError("provided instance is not a wxWizard");
            return nullptr;
        }

        const wxWindowRef weakWizard(wiz);
        wxWindow* const parent = m_parentAsWindow;
        const wxWindowRef weakParent(parent);

        const auto parentIsLive = [&weakParent, parent]()
        {
            return !parent || IsLiveWindow(weakParent, parent);
        };
        const auto wizardIsLive = [&weakWizard, wiz, &parentIsLive, parent]()
        {
            return IsLiveWindow(weakWizard, wiz) && parentIsLive() &&
                   wiz->GetParent() == parent;
        };
        const auto failWizard = [this, ownsWizard, wiz, &weakWizard]
                               (const wxString& message) -> wxObject*
        {
            ReportError(message);
            if ( ownsWizard && IsLiveWindow(weakWizard, wiz) )
                delete wiz;
            return nullptr;
        };

        // Read the resource first: once a virtual window operation begins, a
        // nested XRC load may temporarily replace all handler input state.
        const long extraStyle = GetStyle(wxT("exstyle"), 0);
        const int id = GetID();
        const wxString title = GetText(wxT("title"));
        const wxBitmapBundle bitmap = GetBitmapBundle();
        const wxPoint position = GetPosition();
        const long windowStyle =
            GetStyle(wxT("style"), wxDEFAULT_DIALOG_STYLE);
        const bool hidden = GetBool(wxT("hidden"), false);
        const int border = GetLong("border", -1);
        const int placement = GetStyle("bitmap-placement", 0);
        const int minimumBitmapWidth = placement > 0
                                           ? GetLong("bitmap-minwidth", -1)
                                           : -1;
        const wxColour bitmapBackground = placement > 0
                                              ? GetColour("bitmap-bg")
                                              : wxColour();

        if ( !parentIsLive() || !IsLiveWindow(weakWizard, wiz) )
            return failWizard("wizard or its parent was destroyed before creation");

        if ( extraStyle != 0 )
        {
            wiz->SetExtraStyle(extraStyle);
            if ( !parentIsLive() || !IsLiveWindow(weakWizard, wiz) )
                return failWizard("wizard was destroyed while setting its extra style");
        }

        const bool created = wiz->Create(parent,
                                         id,
                                         title,
                                         bitmap,
                                         position,
                                         windowStyle);
        if ( !created || !wizardIsLive() )
            return failWizard("could not create wizard");

        if ( hidden )
        {
            wiz->Hide();
            if ( !wizardIsLive() )
                return failWizard("wizard was destroyed while hiding it");
        }

        if ( border > 0 )
        {
            wiz->SetBorder(border);
            if ( !wizardIsLive() )
                return failWizard("wizard was destroyed while setting its border");
        }

        if ( placement > 0 )
        {
            wiz->SetBitmapPlacement(placement);
            if ( !wizardIsLive() )
                return failWizard("wizard was destroyed while setting bitmap placement");

            // These options are meaningful only when bitmap placement is set.
            if ( minimumBitmapWidth > 0 )
                wiz->SetMinimumBitmapWidth(minimumBitmapWidth);
            if ( bitmapBackground.IsOk() )
                wiz->SetBitmapBackgroundColour(bitmapBackground);

            if ( !wizardIsLive() )
                return failWizard("wizard was destroyed while setting bitmap properties");
        }

        SetupWindow(wiz);
        if ( !wizardIsLive() )
            return failWizard("wizard was destroyed while applying XRC properties");

        // Install an isolated page-chain context. A nested wizard restores this
        // exact state when it returns, but it must never restore a pointer whose
        // weak identity disappeared while the nested load was in progress.
        wxWizard* const oldWizard = m_wizard;
        const wxWindowRef oldWizardLifetime(WizardXmlState(this).wizardLifetime);
        wxWizardPageSimple* const oldLastSimplePage = m_lastSimplePage;
        const wxWindowRef oldLastSimplePageLifetime(
            WizardXmlState(this).lastSimplePageLifetime);
        const unsigned long oldContextGeneration = WizardXmlState(this).contextGeneration;
        const unsigned long oldPageOperationGeneration =
            WizardXmlState(this).pageOperationGeneration;

        const wxScopeGuard restoreContext = wxMakeGuard(
            [this, oldWizard, oldWizardLifetime,
             oldLastSimplePage, oldLastSimplePageLifetime,
             oldContextGeneration, oldPageOperationGeneration]()
            {
                m_wizard = nullptr;
                WizardXmlState(this).wizardLifetime.Release();
                m_lastSimplePage = nullptr;
                WizardXmlState(this).lastSimplePageLifetime.Release();
                WizardXmlState(this).contextGeneration = 0;
                WizardXmlState(this).pageOperationGeneration = 0;

                if ( !IsLiveWindow(oldWizardLifetime, oldWizard) )
                    return;

                m_wizard = oldWizard;
                WizardXmlState(this).wizardLifetime = oldWizardLifetime;
                WizardXmlState(this).contextGeneration = oldContextGeneration;
                WizardXmlState(this).pageOperationGeneration = oldPageOperationGeneration;

                if ( oldLastSimplePage &&
                     IsLiveWindow(oldLastSimplePageLifetime,
                                  oldLastSimplePage) &&
                     oldLastSimplePage->GetParent() == oldWizard )
                {
                    m_lastSimplePage = oldLastSimplePage;
                    WizardXmlState(this).lastSimplePageLifetime = oldLastSimplePageLifetime;
                }
                else if ( oldLastSimplePage )
                {
                    // Invalidate a page transaction in the restored context if
                    // its chain tail disappeared during the nested load.
                    NextGeneration(WizardXmlState(this).pageOperationGeneration);
                }
            });
        wxUnusedVar(restoreContext);

        m_wizard = wiz;
        WizardXmlState(this).wizardLifetime = wxWindowRef(wiz);
        m_lastSimplePage = nullptr;
        WizardXmlState(this).lastSimplePageLifetime.Release();
        WizardXmlState(this).contextGeneration = NextGeneration(WizardXmlState(this).nextContextGeneration);
        WizardXmlState(this).pageOperationGeneration = 0;
        const unsigned long contextGeneration = WizardXmlState(this).contextGeneration;

        // Create direct children one at a time so destruction by one page can
        // never leave CreateChildren() passing a dead parent to the next one.
        // Validate the returned type to retain the old "this handler only"
        // contract while still allowing object_ref nodes to be resolved first.
        for ( wxXmlNode* child = m_node->GetChildren(); child; )
        {
            wxXmlNode* const next = child->GetNext();
            if ( IsObjectNode(child) )
            {
                wxObject* const createdChild =
                    CreateResFromNode(child, wiz, nullptr);

                if ( WizardXmlState(this).contextGeneration != contextGeneration ||
                     m_wizard != wiz ||
                     !IsLiveWindow(WizardXmlState(this).wizardLifetime, wiz) ||
                     !wizardIsLive() )
                {
                    return failWizard(
                        "wizard was destroyed or replaced while creating its pages");
                }

                wxWizardPage* const createdPage =
                    FindLiveWizardPageChildByObjectAddress(wiz, createdChild);
                if ( !createdPage ||
                     createdPage->GetParent() != wiz )
                {
                    return failWizard("could not create a direct wizard page");
                }
            }

            child = next;
        }

        if ( m_lastSimplePage &&
             (!IsLiveWindow(WizardXmlState(this).lastSimplePageLifetime,
                            m_lastSimplePage) ||
              m_lastSimplePage->GetParent() != wiz) )
        {
            return failWizard("wizard page chain became invalid during XRC creation");
        }

        return wiz;
    }
    else
    {
        wxWizard* const wizard = m_wizard;
        if ( WizardXmlState(this).contextGeneration == 0 ||
             !IsLiveWindow(WizardXmlState(this).wizardLifetime, wizard) ||
             m_parentAsWindow != wizard )
        {
            ReportError("wizard page must be a direct child of a live wizard");
            return nullptr;
        }

        const wxWindowRef weakWizard(wizard);
        const unsigned long contextGeneration = WizardXmlState(this).contextGeneration;
        const unsigned long pageOperationGeneration =
            NextGeneration(WizardXmlState(this).pageOperationGeneration);

        wxWizardPageSimple* const previousSimplePage = m_lastSimplePage;
        const wxWindowRef weakPreviousSimplePage(WizardXmlState(this).lastSimplePageLifetime);
        if ( previousSimplePage &&
             (!IsLiveWindow(weakPreviousSimplePage, previousSimplePage) ||
              previousSimplePage->GetParent() != wizard) )
        {
            m_lastSimplePage = nullptr;
            WizardXmlState(this).lastSimplePageLifetime.Release();
            ReportError("previous simple wizard page is no longer live");
            return nullptr;
        }

        const bool isSimplePage = m_class == wxT("wxWizardPageSimple");
        const bool factoryOwned =
            wxXRCIsCurrentInstanceFactoryOwned(this);
        const bool ownsPage =
            factoryOwned || (isSimplePage && m_instance == nullptr);
        wxWizardPageSimple* simplePage = nullptr;
        wxWizardPage* page = nullptr;

        if ( isSimplePage )
        {
            simplePage = m_instance
                             ? wxDynamicCast(m_instance, wxWizardPageSimple)
                             : new wxWizardPageSimple;
            if ( !simplePage )
            {
                if ( factoryOwned )
                {
                    wxObject* const wrongInstance = m_instance;
                    m_instance = nullptr;
                    delete wrongInstance;
                }
                ReportError("provided instance is not a wxWizardPageSimple");
                return nullptr;
            }

            page = simplePage;
        }
        else /*if (m_class == wxT("wxWizardPage"))*/
        {
            if ( !m_instance )
            {
                ReportError("wxWizardPage is abstract class and must be subclassed");
                return nullptr;
            }

            page = wxDynamicCast(m_instance, wxWizardPage);
            if ( !page )
            {
                if ( factoryOwned )
                {
                    wxObject* const wrongInstance = m_instance;
                    m_instance = nullptr;
                    delete wrongInstance;
                }
                ReportError("provided instance is not a wxWizardPage");
                return nullptr;
            }
        }

        const wxWindowRef weakPage(page);
        const auto contextIsValid =
            [this, wizard, &weakWizard,
             contextGeneration, pageOperationGeneration]()
            {
                return WizardXmlState(this).contextGeneration == contextGeneration &&
                       WizardXmlState(this).pageOperationGeneration == pageOperationGeneration &&
                       m_wizard == wizard &&
                       IsLiveWindow(WizardXmlState(this).wizardLifetime, wizard) &&
                       IsLiveWindow(weakWizard, wizard);
            };
        const auto pageIsValid =
            [page, &weakPage, wizard, &contextIsValid]()
            {
                return contextIsValid() &&
                       IsLiveWindow(weakPage, page) &&
                       page->GetParent() == wizard;
            };
        const auto previousPageIsUnchanged =
            [this, previousSimplePage, &weakPreviousSimplePage, wizard]()
            {
                if ( !previousSimplePage )
                {
                    return m_lastSimplePage == nullptr &&
                           WizardXmlState(this).lastSimplePageLifetime.get() == nullptr;
                }

                return m_lastSimplePage == previousSimplePage &&
                       IsLiveWindow(WizardXmlState(this).lastSimplePageLifetime,
                                    previousSimplePage) &&
                       IsLiveWindow(weakPreviousSimplePage,
                                    previousSimplePage) &&
                       previousSimplePage->GetParent() == wizard;
            };
        const auto failPage = [this, ownsPage, page, &weakPage]
                              (const wxString& message) -> wxObject*
        {
            ReportError(message);
            if ( ownsPage && IsLiveWindow(weakPage, page) )
                delete page;
            return nullptr;
        };

        const wxBitmapBundle bitmap = GetBitmapBundle();
        const wxString name = GetName();
        const int id = GetID();

        if ( !contextIsValid() || !IsLiveWindow(weakPage, page) )
            return failPage("wizard page or parent was destroyed before creation");

        const bool created = simplePage
                                 ? simplePage->Create(wizard,
                                                      nullptr,
                                                      nullptr,
                                                      bitmap)
                                 : page->Create(wizard, bitmap);
        if ( !created || !pageIsValid() || !previousPageIsUnchanged() )
            return failPage("could not create wizard page");

        page->SetName(name);
        if ( !pageIsValid() || !previousPageIsUnchanged() )
            return failPage("wizard page was destroyed while setting its name");

        page->SetId(id);
        if ( !pageIsValid() || !previousPageIsUnchanged() )
            return failPage("wizard page was destroyed while setting its id");

        SetupWindow(page);
        if ( !pageIsValid() || !previousPageIsUnchanged() )
            return failPage("wizard page was destroyed while applying XRC properties");

        // As above, stop immediately if a child handler destroys/reparents the
        // page instead of passing that stale parent to a following child.
        for ( wxXmlNode* child = m_node->GetChildren(); child; )
        {
            wxXmlNode* const next = child->GetNext();
            if ( IsObjectNode(child) )
            {
                wxObject* const createdChild =
                    CreateResFromNode(child, page, nullptr);

                if ( !pageIsValid() || !previousPageIsUnchanged() )
                {
                    return failPage(
                        "wizard page was destroyed or replaced while creating its children");
                }

                if ( !createdChild )
                    return failPage("could not create wizard page child");
            }

            child = next;
        }

        // Publish the chain only after every callback boundary succeeded. This
        // means rollback is just deletion of a handler-owned page: no previous
        // page can retain a link to a failed transaction.
        if ( simplePage )
        {
            if ( previousSimplePage )
                wxWizardPageSimple::Chain(previousSimplePage, simplePage);

            m_lastSimplePage = simplePage;
            WizardXmlState(this).lastSimplePageLifetime = wxWindowRef(simplePage);
        }

        return page;
    }
}

bool wxWizardXmlHandler::CanHandle(wxXmlNode *node)
{
    const bool hasLiveWizard =
        WizardXmlState(this).contextGeneration != 0 && m_wizard &&
        IsLiveWindow(WizardXmlState(this).wizardLifetime, m_wizard);

    return IsOfClass(node, wxT("wxWizard")) ||
           (hasLiveWizard &&
                (IsOfClass(node, wxT("wxWizardPage")) ||
                 IsOfClass(node, wxT("wxWizardPageSimple")))
           );
}

#endif // wxUSE_XRC && wxUSE_WIZARDDLG
