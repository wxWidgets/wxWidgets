/////////////////////////////////////////////////////////////////////////////
// Name:        src/xrc/xh_propdlg.cpp
// Purpose:     XRC resource handler for wxPropertySheetDialog
// Author:      Sander Berents
// Created:     2007/07/12
// Copyright:   (c) 2007 Sander Berents
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_XRC && wxUSE_BOOKCTRL

#include "wx/xrc/xh_propdlg.h"

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/sizer.h"
    #include "wx/frame.h"
#endif

#include "wx/bookctrl.h"
#include "wx/propdlg.h"
#include "wx/imaglist.h"
#include "wx/private/windowlifetime.h"
#include "wx/scopeguard.h"

#include "xmlrespriv.h"

#include <vector>

namespace
{

struct PropertySheetXrcContext
{
    wxPropertySheetDialogXmlHandler* handler;
    wxPropertySheetDialog* dialog;
    wxWeakRef<wxWindow> dialogLifetime;
    wxWindow* dialogParent;
    wxWeakRef<wxWindow> dialogParentLifetime;
    wxSizer* topSizer;
    wxSizer* innerSizer;
    wxBookCtrlBase* book;
    wxWeakRef<wxWindow> bookLifetime;
};

thread_local std::vector<PropertySheetXrcContext*>
    gs_propertySheetXrcContexts;

constexpr size_t wxMAX_PROPERTY_SHEET_XRC_CONTEXT_DEPTH = 64;

wxSizer* FindLiveDirectChildSizerByAddress(wxSizer* owner,
                                           const wxSizer* address)
{
    if ( !owner || !address )
        return nullptr;

    for ( wxSizerItem* const item : owner->GetChildren() )
    {
        wxSizer* const child = item->GetSizer();
        if ( child == address )
            return child;
    }

    return nullptr;
}

size_t CountExactWindowItems(wxSizer* sizer, const wxWindow* window)
{
    size_t count = 0;
    if ( sizer && window )
    {
        for ( wxSizerItem* const item : sizer->GetChildren() )
        {
            if ( item->GetWindow() == window )
                ++count;
        }
    }
    return count;
}

PropertySheetXrcContext* FindPropertySheetXrcContext(
    wxPropertySheetDialogXmlHandler* handler)
{
    for ( std::vector<PropertySheetXrcContext*>::reverse_iterator it =
              gs_propertySheetXrcContexts.rbegin();
          it != gs_propertySheetXrcContexts.rend();
          ++it )
    {
        if ( (*it)->handler == handler )
            return *it;
    }

    return nullptr;
}

bool IsPropertySheetXrcContextAvailable(
    const PropertySheetXrcContext& context)
{
    if ( !wxWeakWindowIsAvailableForCallbacks(
             context.dialogLifetime, context.dialog) )
    {
        return false;
    }

    if ( context.dialog->GetParent() != context.dialogParent ||
         (context.dialogParent &&
          !wxWeakWindowIsAvailableForCallbacks(
              context.dialogParentLifetime, context.dialogParent)) ||
         !context.topSizer || !context.innerSizer ||
         context.dialog->GetSizer() != context.topSizer ||
         context.dialog->GetInnerSizer() != context.innerSizer ||
         FindLiveDirectChildSizerByAddress(context.topSizer,
                                            context.innerSizer) !=
             context.innerSizer )
    {
        return false;
    }

    // Resolve the book through wxPropertySheetDialog's weak sidecar before
    // inspecting the captured address. This clears a historical raw member
    // after destruction/reparent and prevents an ABA match.
    wxBookCtrlBase* const currentBook = context.dialog->GetBookCtrl();
    return currentBook == context.book &&
           wxWeakWindowIsAvailableForCallbacks(
               context.bookLifetime, context.book) &&
           context.book->GetParent() == context.dialog &&
           context.book->GetContainingSizer() == context.innerSizer &&
           CountExactWindowItems(context.innerSizer, context.book) == 1;
}

} // anonymous namespace

wxIMPLEMENT_DYNAMIC_CLASS(wxPropertySheetDialogXmlHandler, wxXmlResourceHandler);

wxPropertySheetDialogXmlHandler::wxPropertySheetDialogXmlHandler()
                               : m_dialog(nullptr)
{
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

    AddWindowStyles();
}

wxObject *wxPropertySheetDialogXmlHandler::DoCreateResource()
{
    if (m_class == wxT("propertysheetpage"))
    {
        PropertySheetXrcContext* const context =
            FindPropertySheetXrcContext(this);
        if ( !context || !IsPropertySheetXrcContextAvailable(*context) )
        {
            if ( context )
                MarkCurrentPageCreationFailed(context->book);
            ReportError("property sheet page has no live creation context");
            return nullptr;
        }

        wxObject* const page = DoCreatePage(context->book);
        if ( !IsPropertySheetXrcContextAvailable(*context) )
        {
            // DoCreatePage() may already have published this page to the
            // surrounding book transaction. Explicit invalidation makes the
            // outer DoCreatePagesSafely() roll it back instead of committing
            // a page into a book the property sheet no longer owns.
            MarkCurrentPageCreationFailed(context->book);
            return nullptr;
        }

        return page;
    }

    else
    {
        const bool factoryOwned =
            wxXRCIsCurrentInstanceFactoryOwned(this);
        const bool ownsDialog = m_instance == nullptr || factoryOwned;
        wxPropertySheetDialog* const dlg =
            m_instance
                ? wxDynamicCast(m_instance, wxPropertySheetDialog)
                : new wxPropertySheetDialog;
        if ( !dlg )
        {
            if ( factoryOwned )
            {
                wxObject* const wrongInstance = m_instance;
                m_instance = nullptr;
                delete wrongInstance;
            }
            ReportError("provided instance is not a wxPropertySheetDialog");
            return nullptr;
        }

        const wxWeakRef<wxWindow> weakDialog(dlg);
        wxWindow* const parent = m_parentAsWindow;
        const wxWeakRef<wxWindow> weakParent(parent);
        wxWindow* const initialDialogParent = dlg->GetParent();
        const wxWeakRef<wxWindow> weakInitialDialogParent(initialDialogParent);
        wxWindow* expectedCreateParent = nullptr;
        wxWeakRef<wxWindow> weakExpectedCreateParent;
        bool createAttempted = false;
        const auto isPreCreateContextAvailable =
            [dlg, parent, weakDialog, weakParent,
             initialDialogParent, weakInitialDialogParent]()
            {
                return wxWeakWindowIsAvailableForCallbacks(weakDialog, dlg) &&
                       dlg->GetParent() == initialDialogParent &&
                       (!initialDialogParent ||
                        wxWeakWindowIsAvailableForCallbacks(
                            weakInitialDialogParent, initialDialogParent)) &&
                       (!parent ||
                        wxWeakWindowIsAvailableForCallbacks(weakParent,
                                                            parent));
            };
        const auto discardOwnedDialog =
            [&, dlg, ownsDialog, weakDialog, initialDialogParent,
             weakInitialDialogParent]()
            {
                if ( !ownsDialog || weakDialog.get() != dlg ||
                     wxWindowItselfIsUnavailableForCallbacks(dlg) )
                {
                    return;
                }

                wxWindow* const currentParent = dlg->GetParent();
                const bool stillInitiallyOwned =
                    currentParent == initialDialogParent &&
                    (!initialDialogParent ||
                     wxWeakWindowIsAvailableForCallbacks(
                         weakInitialDialogParent, initialDialogParent));
                const bool stillCreateOwned =
                    createAttempted && currentParent == expectedCreateParent &&
                    (!expectedCreateParent ||
                     wxWeakWindowIsAvailableForCallbacks(
                         weakExpectedCreateParent, expectedCreateParent));
                if ( stillInitiallyOwned || stillCreateOwned )
                    delete dlg;
            };

        // Attribute access can enter application log/art handlers. Snapshot
        // the complete Create() argument list and validate after every read;
        // never evaluate another handler callback as part of a raw dlg call.
        const bool hidden = GetBool(wxT("hidden"), false);
        if ( !isPreCreateContextAvailable() )
        {
            discardOwnedDialog();
            return nullptr;
        }

        const wxWindowID id = GetID();
        if ( !isPreCreateContextAvailable() )
        {
            discardOwnedDialog();
            return nullptr;
        }
        const wxString title = GetText(wxT("title"));
        if ( !isPreCreateContextAvailable() )
        {
            discardOwnedDialog();
            return nullptr;
        }
        const wxPoint position = GetPosition();
        if ( !isPreCreateContextAvailable() )
        {
            discardOwnedDialog();
            return nullptr;
        }
        const wxSize size = GetSize();
        if ( !isPreCreateContextAvailable() )
        {
            discardOwnedDialog();
            return nullptr;
        }
        const long style = GetStyle();
        if ( !isPreCreateContextAvailable() )
        {
            discardOwnedDialog();
            return nullptr;
        }
        const wxString name = GetName();
        if ( !isPreCreateContextAvailable() )
        {
            discardOwnedDialog();
            return nullptr;
        }

        expectedCreateParent = dlg->GetParentForModalDialog(parent, style);
        weakExpectedCreateParent =
            wxWeakRef<wxWindow>(expectedCreateParent);
        if ( (expectedCreateParent &&
              !wxWeakWindowIsAvailableForCallbacks(
                  weakExpectedCreateParent, expectedCreateParent)) ||
             !isPreCreateContextAvailable() )
        {
            discardOwnedDialog();
            return nullptr;
        }

        createAttempted = true;
        const bool created = dlg->Create(parent, id, title, position, size,
                                         style, name);
        if ( !created ||
             !wxWeakWindowIsAvailableForCallbacks(weakDialog, dlg) ||
             dlg->GetParent() != expectedCreateParent ||
             (expectedCreateParent &&
              !wxWeakWindowIsAvailableForCallbacks(
                  weakExpectedCreateParent, expectedCreateParent)) )
        {
            ReportError("could not create property sheet dialog");
            discardOwnedDialog();
            return nullptr;
        }

        wxWindow* const dialogParent = dlg->GetParent();
        const wxWeakRef<wxWindow> weakDialogParent(dialogParent);
        wxSizer* const topSizer = dlg->GetSizer();
        wxSizer* const innerSizer = dlg->GetInnerSizer();
        wxBookCtrlBase* const book = dlg->GetBookCtrl();
        const wxWeakRef<wxWindow> weakBook(book);
        PropertySheetXrcContext context {
            this, dlg, weakDialog,
            dialogParent, weakDialogParent,
            topSizer, innerSizer,
            book, weakBook
        };
        const auto isPropertySheetContextAvailable = [&context]()
        {
            return IsPropertySheetXrcContextAvailable(context);
        };

        if ( !book || !isPropertySheetContextAvailable() )
        {
            ReportError("property sheet dialog has no live book control");
            discardOwnedDialog();
            return nullptr;
        }

        if ( hidden )
        {
            dlg->Hide();
            if ( !isPropertySheetContextAvailable() )
            {
                discardOwnedDialog();
                return nullptr;
            }
        }

        const bool hasIcon = HasParam(wxT("icon"));
        if ( !isPropertySheetContextAvailable() )
        {
            discardOwnedDialog();
            return nullptr;
        }
        if ( hasIcon )
        {
            const auto icons =
                GetIconBundle(wxT("icon"), wxART_FRAME_ICON);
            if ( !isPropertySheetContextAvailable() )
            {
                discardOwnedDialog();
                return nullptr;
            }

            dlg->SetIcons(icons);
            if ( !isPropertySheetContextAvailable() )
            {
                discardOwnedDialog();
                return nullptr;
            }
        }

        SetupWindow(dlg);

        if ( !isPropertySheetContextAvailable() )
        {
            discardOwnedDialog();
            return nullptr;
        }

        bool pagesCreated = false;
        {
            if ( gs_propertySheetXrcContexts.size() >=
                    wxMAX_PROPERTY_SHEET_XRC_CONTEXT_DEPTH )
            {
                ReportError("property sheet XRC nesting limit exceeded");
                discardOwnedDialog();
                return nullptr;
            }

            wxPropertySheetDialog *old_par = m_dialog;
            const wxWeakRef<wxWindow> oldDialogLifetime(old_par);
            m_dialog = dlg;

            gs_propertySheetXrcContexts.push_back(&context);
            wxScopeGuard restoreContext = wxMakeGuard(
                [this, old_par, oldDialogLifetime, &context]()
                {
                    wxUnusedVar(context);
                    wxASSERT( !gs_propertySheetXrcContexts.empty() &&
                              gs_propertySheetXrcContexts.back() == &context );
                    gs_propertySheetXrcContexts.pop_back();
                    m_dialog = nullptr;
                    if ( old_par &&
                         wxWeakWindowIsAvailableForCallbacks(
                             oldDialogLifetime, old_par) )
                    {
                        m_dialog = old_par;
                    }
                });
            wxUnusedVar(restoreContext);

            pagesCreated = DoCreatePagesSafely(book);
        }

        if ( !pagesCreated || !isPropertySheetContextAvailable() )
        {
            discardOwnedDialog();
            return nullptr;
        }

        const wxString buttons = GetText(wxT("buttons"));
        if ( !isPropertySheetContextAvailable() )
        {
            discardOwnedDialog();
            return nullptr;
        }
        if (!buttons.IsEmpty())
        {
            int flags = 0;
            if (buttons.Find(wxT("wxOK"))         != wxNOT_FOUND) flags |= wxOK;
            if (buttons.Find(wxT("wxCANCEL"))     != wxNOT_FOUND) flags |= wxCANCEL;
            if (buttons.Find(wxT("wxYES"))        != wxNOT_FOUND) flags |= wxYES;
            if (buttons.Find(wxT("wxNO"))         != wxNOT_FOUND) flags |= wxNO;
            if (buttons.Find(wxT("wxAPPLY"))      != wxNOT_FOUND) flags |= wxAPPLY;
            if (buttons.Find(wxT("wxHELP"))       != wxNOT_FOUND) flags |= wxHELP;
            if (buttons.Find(wxT("wxCLOSE"))      != wxNOT_FOUND) flags |= wxCLOSE;
            if (buttons.Find(wxT("wxNO_DEFAULT")) != wxNOT_FOUND) flags |= wxNO_DEFAULT;
            dlg->CreateButtons(flags);
            if ( !isPropertySheetContextAvailable() )
            {
                discardOwnedDialog();
                return nullptr;
            }
        }

        dlg->LayoutDialog(0);
        if ( !isPropertySheetContextAvailable() )
        {
            discardOwnedDialog();
            return nullptr;
        }

        const bool centered = GetBool(wxT("centered"), false);
        if ( !isPropertySheetContextAvailable() )
        {
            discardOwnedDialog();
            return nullptr;
        }
        if ( centered )
        {
            dlg->Centre();
            if ( !isPropertySheetContextAvailable() )
            {
                discardOwnedDialog();
                return nullptr;
            }
        }

        return dlg;
    }
}

bool wxPropertySheetDialogXmlHandler::CanHandle(wxXmlNode *node)
{
    return ((!IsInside() && IsOfClass(node, wxT("wxPropertySheetDialog"))) ||
            (IsInside() && IsOfClass(node, wxT("propertysheetpage"))));
}

#endif // wxUSE_XRC && wxUSE_BOOKCTRL
