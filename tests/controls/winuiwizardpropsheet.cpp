///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/winuiwizardpropsheet.cpp
// Purpose:     Contracts for WinUI generic wizard/property/preferences fallbacks
// Author:      wxWidgets development team
// Created:     2026-08-02
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"
#include "waitfor.h"

#if wxUSE_BUTTON && \
    (wxUSE_BOOKCTRL || wxUSE_WIZARDDLG || wxUSE_PREFERENCES_EDITOR)

#include "wx/app.h"
#include "wx/button.h"
#include "wx/dialog.h"
#include "wx/frame.h"
#include "wx/gbsizer.h"
#include "wx/log.h"
#include "wx/notebook.h"
#include "wx/panel.h"
#include "wx/private/windowlifetime.h"
#include "wx/scrolwin.h"
#include "wx/sizer.h"
#include "wx/stattext.h"
#include "wx/weakref.h"

#if wxUSE_BOOKCTRL
    #include "wx/bookctrl.h"
    #include "wx/propdlg.h"
    #if wxUSE_CHOICEBOOK
        #include "wx/choicebk.h"
    #endif
    #if wxUSE_LISTBOOK
        #include "wx/listbook.h"
    #endif
    #if wxUSE_TOOLBOOK
        #include "wx/toolbook.h"
    #endif
    #if wxUSE_TREEBOOK
        #include "wx/treebook.h"
    #endif
#endif

#if wxUSE_XRC
    #include "wx/sstream.h"
    #include "wx/xml/xml.h"
    #include "wx/xrc/xmlres.h"
#endif

#if wxUSE_WIZARDDLG
    #include "wx/wizard.h"
#endif

#if wxUSE_PREFERENCES_EDITOR
    #include "wx/preferences.h"
#endif

#if defined(__WXWINUI__) && wxUSE_WINUI3
    #include "wx/winui/private/tlwhostmsw.h"
#endif

#include <memory>
#include <functional>
#include <vector>

namespace
{

wxPanel* CreateSizedPage(wxWindow* parent,
                         const wxString& label,
                         const wxSize& minSize = wxSize(120, 70))
{
    wxPanel* const page = new wxPanel(parent);
    wxBoxSizer* const sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(new wxStaticText(page, wxID_ANY, label),
               wxSizerFlags().Border());
    page->SetSizer(sizer);
    page->SetMinSize(minSize);
    return page;
}

void SendButtonCommand(wxWindow* window, int id)
{
    wxButton* const button = wxDynamicCast(window->FindWindow(id), wxButton);
    REQUIRE(button);

    wxCommandEvent event(wxEVT_BUTTON, id);
    event.SetEventObject(button);
    button->Command(event);
}

wxDialog* FindDialogWithTitle(const wxString& title)
{
    for ( wxWindowList::compatibility_iterator node =
              wxTopLevelWindows.GetFirst();
          node;
          node = node->GetNext() )
    {
        wxDialog* const dialog = wxDynamicCast(node->GetData(), wxDialog);
        if ( dialog && !dialog->IsBeingDeleted() &&
             dialog->GetTitle() == title )
        {
            return dialog;
        }
    }

    return nullptr;
}

size_t CountDialogsWithTitle(const wxString& title)
{
    size_t count = 0;
    for ( wxWindowList::compatibility_iterator node =
              wxTopLevelWindows.GetFirst();
          node;
          node = node->GetNext() )
    {
        wxDialog* const dialog = wxDynamicCast(node->GetData(), wxDialog);
        if ( dialog && !dialog->IsBeingDeleted() &&
             dialog->GetTitle() == title )
        {
            ++count;
        }
    }
    return count;
}

template <typename T>
T* FindDescendantOfType(wxWindow* parent)
{
    if ( T* const match = wxDynamicCast(parent, T) )
        return match;

    for ( wxWindowList::compatibility_iterator node =
              parent->GetChildren().GetFirst();
          node;
          node = node->GetNext() )
    {
        if ( T* const match =
                 FindDescendantOfType<T>(node->GetData()) )
        {
            return match;
        }
    }

    return nullptr;
}

bool CommandDialogButton(wxDialog* dialog, int id)
{
    wxButton* const button =
        wxDynamicCast(dialog ? dialog->FindWindow(id) : nullptr, wxButton);
    if ( !button )
        return false;

    wxCommandEvent event(wxEVT_BUTTON, id);
    event.SetEventObject(button);
    button->Command(event);
    return true;
}

#if wxUSE_PREFERENCES_EDITOR && !defined(wxHAS_PREF_EDITOR_MODELESS)

void AbortAnyModalDialog()
{
    for ( wxWindowList::compatibility_iterator node =
              wxTopLevelWindows.GetFirst();
          node;
          node = node->GetNext() )
    {
        wxDialog* const dialog = wxDynamicCast(node->GetData(), wxDialog);
        if ( dialog && dialog->IsModal() )
        {
            dialog->EndModal(wxID_CANCEL);
            return;
        }
    }
}

#endif // wxUSE_PREFERENCES_EDITOR && !wxHAS_PREF_EDITOR_MODELESS

#if wxUSE_WIZARDDLG

class TrackingWizardPage final : public wxWizardPageSimple
{
public:
    explicit TrackingWizardPage(wxWizard* wizard,
                                const wxString& label = "Wizard page")
        : wxWizardPageSimple(wizard)
    {
        wxBoxSizer* const sizer = new wxBoxSizer(wxVERTICAL);
        sizer->Add(new wxStaticText(this, wxID_ANY, label),
                   wxSizerFlags().Border());
        SetSizer(sizer);
        SetMinSize(wxSize(180, 100));
    }

    bool Validate() override
    {
        ++validateCalls;
        if ( onValidate )
            onValidate();
        return allowValidation;
    }

    bool TransferDataToWindow() override
    {
        ++transferToCalls;
        if ( onTransferTo )
            onTransferTo();
        return allowTransferTo;
    }

    bool TransferDataFromWindow() override
    {
        ++transferFromCalls;
        if ( onTransferFrom )
            onTransferFrom();
        return allowTransferFrom;
    }

    bool allowValidation = true;
    bool allowTransferTo = true;
    bool allowTransferFrom = true;
    int validateCalls = 0;
    int transferToCalls = 0;
    int transferFromCalls = 0;
    std::function<void()> onValidate;
    std::function<void()> onTransferTo;
    std::function<void()> onTransferFrom;
};

class ProtectedStateWizard final : public wxWizard
{
public:
    ProtectedStateWizard(wxWindow* parent, const wxString& title)
        : wxWizard(parent, wxID_ANY, title)
    {
    }

    void PublishCurrentPageDirectly(wxWizardPage* page) { m_page = page; }
    void PublishFirstPageDirectly(wxWizardPage* page) { m_firstpage = page; }
};

class DestructiveDynamicBindWizardPage final : public wxWizardPageSimple
{
public:
    DestructiveDynamicBindWizardPage(wxWizard* wizard, int* bindCalls)
        : wxWizardPageSimple(wizard), m_bindCalls(bindCalls)
    {
        SetSizer(new wxBoxSizer(wxVERTICAL));
    }

protected:
    bool OnDynamicBind(wxDynamicEventTableEntry&) override
    {
        ++*m_bindCalls;
        delete this;
        return false;
    }

private:
    int* const m_bindCalls;
};

class DestroyWizardOnReparentPanel final : public wxPanel
{
public:
    DestroyWizardOnReparentPanel(wxWindow* parent,
                                 wxWizard* wizard,
                                 int* reparentCalls)
        : wxPanel(parent),
          m_wizard(wizard),
          m_reparentCalls(reparentCalls)
    {
    }

    bool Reparent(wxWindowBase* newParent) override
    {
        const bool result = wxPanel::Reparent(newParent);
        ++*m_reparentCalls;
        if ( !m_destroyRequested )
        {
            m_destroyRequested = true;
            if ( m_wizard )
                m_wizard->Destroy();
        }
        return result;
    }

private:
    wxWeakRef<wxWizard> m_wizard;
    int* const m_reparentCalls;
    bool m_destroyRequested { false };
};

// wxDialogLayoutAdapter::CanDoLayoutAdaptation() deliberately depends on the
// current display geometry.  Destructive callback tests must not enter a real
// modal loop merely because the test desktop happens to have enough room, so
// force only this decision and preserve the complete production path through
// wxWizard::DoWizardLayout() and wxWizard::DoLayoutAdaptation().
class ForcedLayoutAdaptationWizard final : public wxWizard
{
public:
    ForcedLayoutAdaptationWizard(wxWindow* parent,
                                 wxWindowID id,
                                 const wxString& title)
        : wxWizard(parent, id, title)
    {
    }

protected:
    bool CanDoLayoutAdaptation() override
    {
        return true;
    }
};

class RejectWizardReparentPanel final : public wxPanel
{
public:
    RejectWizardReparentPanel(wxWindow* parent, int* reparentCalls)
        : wxPanel(parent), m_reparentCalls(reparentCalls)
    {
    }

    bool Reparent(wxWindowBase* WXUNUSED(newParent)) override
    {
        ++*m_reparentCalls;
        return false;
    }

private:
    int* const m_reparentCalls;
};

class ReplaceWizardSizerOnReparentPanel final : public wxPanel
{
public:
    ReplaceWizardSizerOnReparentPanel(wxWizardPage* page,
                                      wxSizer** replacementSizer,
                                      int* reparentCalls)
        : wxPanel(page),
          m_page(page),
          m_replacementSizer(replacementSizer),
          m_reparentCalls(reparentCalls)
    {
    }

    bool Reparent(wxWindowBase* newParent) override
    {
        ++*m_reparentCalls;
        if ( *m_reparentCalls == 1 )
        {
            wxWizardPage* const page = m_page.get();
            if ( page )
            {
                *m_replacementSizer = new wxBoxSizer(wxVERTICAL);
                // Deliberately delete the original sizer while the layout
                // adapter is inside the virtual Reparent() boundary.
                page->SetSizer(*m_replacementSizer, true);
            }
        }

        return wxPanel::Reparent(newParent);
    }

private:
    wxWeakRef<wxWizardPage> m_page;
    wxSizer** const m_replacementSizer;
    int* const m_reparentCalls;
};

class NavigateOnGetNextWizardPage final : public wxWizardPageSimple
{
public:
    explicit NavigateOnGetNextWizardPage(wxWizard* wizard)
        : wxWizardPageSimple(wizard), m_wizard(wizard)
    {
    }

    void SetNavigationTarget(wxWizardPage* target)
    {
        m_target = target;
        SetNext(target);
    }

    wxWizardPage* GetNext() const override
    {
        wxWizard* const wizard = m_wizard.get();
        wxWizardPage* const target = m_target.get();
        if ( !m_navigated && wizard && target )
        {
            m_navigated = true;
            (void)wizard->ShowPage(target);
        }
        return target;
    }

private:
    wxWeakRef<wxWizard> m_wizard;
    wxWeakRef<wxWizardPage> m_target;
    mutable bool m_navigated { false };
};

class NavigateDuringRollbackPanel final : public wxPanel
{
public:
    NavigateDuringRollbackPanel(wxWindow* parent,
                                wxWizard* wizard,
                                wxWizardPage* target,
                                int* reparentCalls)
        : wxPanel(parent),
          m_wizard(wizard),
          m_target(target),
          m_reparentCalls(reparentCalls)
    {
    }

    bool Reparent(wxWindowBase* newParent) override
    {
        const bool moved = wxPanel::Reparent(newParent);
        ++*m_reparentCalls;
        if ( moved && *m_reparentCalls == 2 )
        {
            wxWizard* const wizard = m_wizard.get();
            wxWizardPage* const target = m_target.get();
            if ( wizard && target )
                (void)wizard->ShowPage(target);
        }
        return moved;
    }

private:
    wxWeakRef<wxWizard> m_wizard;
    wxWeakRef<wxWizardPage> m_target;
    int* const m_reparentCalls;
};

class DeleteTargetOnGetNextWizardPage final : public wxWizardPageSimple
{
public:
    explicit DeleteTargetOnGetNextWizardPage(wxWizard* wizard)
        : wxWizardPageSimple(wizard)
    {
    }

    void SetDeletionTarget(wxWizardPage* target)
    {
        m_target = target;
    }

    wxWizardPage* GetNext() const override
    {
        wxWizardPage* const target = m_target;
        m_target = nullptr;
        delete target;
        return target;
    }

private:
    mutable wxWizardPage* m_target { nullptr };
};

class RefuseWizardRollbackPanel final : public wxPanel
{
public:
    RefuseWizardRollbackPanel(wxWindow* parent, int* reparentCalls)
        : wxPanel(parent), m_originalParent(parent),
          m_reparentCalls(reparentCalls)
    {
    }

    bool Reparent(wxWindowBase* newParent) override
    {
        ++*m_reparentCalls;
        if ( newParent == m_originalParent )
            return false;
        return wxPanel::Reparent(newParent);
    }

private:
    wxWindow* const m_originalParent;
    int* const m_reparentCalls;
};

class NavigateAndRefuseWizardRollbackPanel final : public wxPanel
{
public:
    NavigateAndRefuseWizardRollbackPanel(wxWindow* parent,
                                         wxWizard* wizard,
                                         wxWizardPage* target,
                                         int* reparentCalls)
        : wxPanel(parent),
          m_originalParent(parent),
          m_wizard(wizard),
          m_target(target),
          m_reparentCalls(reparentCalls)
    {
    }

    bool Reparent(wxWindowBase* newParent) override
    {
        ++*m_reparentCalls;
        if ( newParent == m_originalParent )
            return false;

        const bool moved = wxPanel::Reparent(newParent);
        if ( moved && *m_reparentCalls == 1 )
        {
            wxWizard* const wizard = m_wizard.get();
            wxWizardPage* const target = m_target.get();
            if ( wizard && target )
                (void)wizard->ShowPage(target);
        }
        return moved;
    }

private:
    wxWindow* const m_originalParent;
    wxWeakRef<wxWizard> m_wizard;
    wxWeakRef<wxWizardPage> m_target;
    int* const m_reparentCalls;
};

void AddWizardPage(wxWizard* wizard, wxWizardPage* page)
{
    wizard->GetPageAreaSizer()->Add(page);
}

#if wxUSE_XRC

class SelfDeletingWizard final : public wxWizard
{
public:
    explicit SelfDeletingWizard(bool* destroyed)
        : m_destroyed(destroyed)
    {
    }

    ~SelfDeletingWizard() override
    {
        *m_destroyed = true;
    }

    void DoCreateControls() override
    {
        delete this;
    }

private:
    bool* const m_destroyed;
};

class SelfDeletingWizardPage final : public wxWizardPageSimple
{
public:
    SelfDeletingWizardPage() { ++ms_constructed; }
    ~SelfDeletingWizardPage() override { ++ms_destroyed; }

    bool SetBackgroundColour(const wxColour&) override
    {
        delete this;
        return true;
    }

    static void ResetCounters()
    {
        ms_constructed = 0;
        ms_destroyed = 0;
    }

    static int GetConstructedCount() { return ms_constructed; }
    static int GetDestroyedCount() { return ms_destroyed; }

private:
    static int ms_constructed;
    static int ms_destroyed;

    wxDECLARE_DYNAMIC_CLASS(SelfDeletingWizardPage);
};

int SelfDeletingWizardPage::ms_constructed = 0;
int SelfDeletingWizardPage::ms_destroyed = 0;

wxIMPLEMENT_DYNAMIC_CLASS(SelfDeletingWizardPage, wxWizardPageSimple);

class DestroyingOwnerWizardPage final : public wxWizardPageSimple
{
public:
    DestroyingOwnerWizardPage() { ++ms_constructed; }
    ~DestroyingOwnerWizardPage() override { ++ms_destroyed; }

    bool SetBackgroundColour(const wxColour& colour) override
    {
        const bool result = wxWizardPageSimple::SetBackgroundColour(colour);
        if ( wxWindow* const owner = GetParent() )
        {
#if defined(__WXWINUI__) && wxUSE_WINUI3
            if ( ms_retainedGuardStorage )
            {
                ms_retainedGuardStorage->reset(
                    new wxWinUITLWHostWindowEventGuard(owner));
            }
#endif
            owner->Destroy();
        }
        return result;
    }

    bool SetForegroundColour(const wxColour& colour) override
    {
        ++ms_foregroundCalls;
        return wxWizardPageSimple::SetForegroundColour(colour);
    }

    static void ResetCounters()
    {
        ms_constructed = 0;
        ms_destroyed = 0;
        ms_foregroundCalls = 0;
    }

    static int GetConstructedCount() { return ms_constructed; }
    static int GetDestroyedCount() { return ms_destroyed; }
    static int GetForegroundCalls() { return ms_foregroundCalls; }
#if defined(__WXWINUI__) && wxUSE_WINUI3
    static void SetRetainedGuardStorage(
        std::unique_ptr<wxWinUITLWHostWindowEventGuard>* storage)
    {
        ms_retainedGuardStorage = storage;
    }
#endif

private:
    static int ms_constructed;
    static int ms_destroyed;
    static int ms_foregroundCalls;
#if defined(__WXWINUI__) && wxUSE_WINUI3
    static std::unique_ptr<wxWinUITLWHostWindowEventGuard>*
        ms_retainedGuardStorage;
#endif

    wxDECLARE_DYNAMIC_CLASS(DestroyingOwnerWizardPage);
};

int DestroyingOwnerWizardPage::ms_constructed = 0;
int DestroyingOwnerWizardPage::ms_destroyed = 0;
int DestroyingOwnerWizardPage::ms_foregroundCalls = 0;
#if defined(__WXWINUI__) && wxUSE_WINUI3
std::unique_ptr<wxWinUITLWHostWindowEventGuard>*
    DestroyingOwnerWizardPage::ms_retainedGuardStorage = nullptr;
#endif

wxIMPLEMENT_DYNAMIC_CLASS(DestroyingOwnerWizardPage, wxWizardPageSimple);

class NestedLoadingWizardPage final : public wxWizardPageSimple
{
public:
    bool SetBackgroundColour(const wxColour& colour) override
    {
        const bool result = wxWizardPageSimple::SetBackgroundColour(colour);
        if ( !ms_loadingNested && !ms_nestedWizard )
        {
            ms_loadingNested = true;
            ms_nestedWizard = wxDynamicCast(
                wxXmlResource::Get()->LoadObject(
                    ms_parent,
                    "phase013NestedWizard",
                    "wxWizard"),
                wxWizard);
            ms_loadingNested = false;
        }
        return result;
    }

    bool SetForegroundColour(const wxColour& colour) override
    {
        ++ms_foregroundCalls;
        return wxWizardPageSimple::SetForegroundColour(colour);
    }

    static void Reset(wxWindow* parent)
    {
        ms_parent = parent;
        ms_nestedWizard = nullptr;
        ms_loadingNested = false;
        ms_foregroundCalls = 0;
    }

    static wxWizard* TakeNestedWizard()
    {
        wxWizard* const wizard = ms_nestedWizard;
        ms_nestedWizard = nullptr;
        return wizard;
    }

    static int GetForegroundCalls() { return ms_foregroundCalls; }

private:
    static wxWindow* ms_parent;
    static wxWizard* ms_nestedWizard;
    static bool ms_loadingNested;
    static int ms_foregroundCalls;

    wxDECLARE_DYNAMIC_CLASS(NestedLoadingWizardPage);
};

wxWindow* NestedLoadingWizardPage::ms_parent = nullptr;
wxWizard* NestedLoadingWizardPage::ms_nestedWizard = nullptr;
bool NestedLoadingWizardPage::ms_loadingNested = false;
int NestedLoadingWizardPage::ms_foregroundCalls = 0;

wxIMPLEMENT_DYNAMIC_CLASS(NestedLoadingWizardPage, wxWizardPageSimple);

class XrcOwnedWizard final : public wxWizard
{
public:
    XrcOwnedWizard() { ++ms_constructed; }
    ~XrcOwnedWizard() override { ++ms_destroyed; }

    static void ResetCounters()
    {
        ms_constructed = 0;
        ms_destroyed = 0;
    }

    static int GetConstructedCount() { return ms_constructed; }
    static int GetDestroyedCount() { return ms_destroyed; }

private:
    static int ms_constructed;
    static int ms_destroyed;

    wxDECLARE_DYNAMIC_CLASS(XrcOwnedWizard);
};

int XrcOwnedWizard::ms_constructed = 0;
int XrcOwnedWizard::ms_destroyed = 0;

wxIMPLEMENT_DYNAMIC_CLASS(XrcOwnedWizard, wxWizard);

class XrcOwnedWizardPage final : public wxWizardPage
{
public:
    XrcOwnedWizardPage() { ++ms_constructed; }
    ~XrcOwnedWizardPage() override { ++ms_destroyed; }

    wxWizardPage* GetPrev() const override { return nullptr; }
    wxWizardPage* GetNext() const override { return nullptr; }

    bool SetBackgroundColour(const wxColour& colour) override
    {
        const bool result = wxWizardPage::SetBackgroundColour(colour);
        if ( ms_foreignParent )
            Reparent(ms_foreignParent);
        return result;
    }

    static void Reset(wxWindow* foreignParent)
    {
        ms_foreignParent = foreignParent;
        ms_constructed = 0;
        ms_destroyed = 0;
    }

    static void ClearForeignParent() { ms_foreignParent = nullptr; }
    static int GetConstructedCount() { return ms_constructed; }
    static int GetDestroyedCount() { return ms_destroyed; }

private:
    static wxWindow* ms_foreignParent;
    static int ms_constructed;
    static int ms_destroyed;

    wxDECLARE_DYNAMIC_CLASS(XrcOwnedWizardPage);
};

wxWindow* XrcOwnedWizardPage::ms_foreignParent = nullptr;
int XrcOwnedWizardPage::ms_constructed = 0;
int XrcOwnedWizardPage::ms_destroyed = 0;

wxIMPLEMENT_DYNAMIC_CLASS(XrcOwnedWizardPage, wxWizardPage);

class WrongXrcWizardObject final : public wxObject
{
public:
    WrongXrcWizardObject() { ++ms_constructed; }
    ~WrongXrcWizardObject() override { ++ms_destroyed; }

    static void ResetCounters()
    {
        ms_constructed = 0;
        ms_destroyed = 0;
    }

    static int GetConstructedCount() { return ms_constructed; }
    static int GetDestroyedCount() { return ms_destroyed; }

private:
    static int ms_constructed;
    static int ms_destroyed;

    wxDECLARE_DYNAMIC_CLASS(WrongXrcWizardObject);
};

int WrongXrcWizardObject::ms_constructed = 0;
int WrongXrcWizardObject::ms_destroyed = 0;

wxIMPLEMENT_DYNAMIC_CLASS(WrongXrcWizardObject, wxObject);

class DeletedWizardChildXmlHandler final : public wxXmlResourceHandler
{
public:
    wxObject* DoCreateResource() override
    {
        wxWizard* const wizard = wxDynamicCast(m_parentAsWindow, wxWizard);
        if ( !wizard )
            return nullptr;

        wxWizardPageSimple* const page = new wxWizardPageSimple(wizard);
        wxObject* const staleAddress = page;
        delete page;
        return staleAddress;
    }

    bool CanHandle(wxXmlNode* node) override
    {
        return IsOfClass(node, "phase013DeletedWizardChild");
    }
};

#endif // wxUSE_XRC

#endif // wxUSE_WIZARDDLG

#if wxUSE_XRC
class XrcUnloadGuard final
{
public:
    XrcUnloadGuard(wxXmlResource* resource, const wxString& name)
        : m_resource(resource), m_name(name)
    {
    }

    ~XrcUnloadGuard()
    {
        m_resource->Unload(m_name);
    }

private:
    wxXmlResource* const m_resource;
    const wxString m_name;
};
#endif

#if wxUSE_BOOKCTRL

size_t CountSizerWindowItemsForTest(wxSizer* sizer,
                                    const wxWindow* window)
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

size_t CountSizerChildItemsForTest(wxSizer* owner, const wxSizer* child)
{
    size_t count = 0;
    if ( owner && child )
    {
        for ( wxSizerItem* const item : owner->GetChildren() )
        {
            if ( item->GetSizer() == child )
                ++count;
        }
    }
    return count;
}

size_t CountDescendantsWithId(wxWindow* parent, wxWindowID id)
{
    size_t count = 0;
    for ( wxWindow* const child : parent->GetChildren() )
    {
        if ( child->GetId() == id )
            ++count;
        count += CountDescendantsWithId(child, id);
    }
    return count;
}

std::vector<wxWindow*> GetDirectChildOrder(wxWindow* parent)
{
    std::vector<wxWindow*> order;
    order.reserve(parent->GetChildren().GetCount());
    for ( wxWindow* const child : parent->GetChildren() )
        order.push_back(child);
    return order;
}

class TrackingSizerUserData final : public wxObject
{
public:
    explicit TrackingSizerUserData(int* destroyed) : m_destroyed(destroyed) { }

    ~TrackingSizerUserData() override
    {
        ++*m_destroyed;
    }

private:
    int* const m_destroyed;
};

#if wxUSE_XRC
class DestroyingXrcBookPage final : public wxPanel
{
public:
    DestroyingXrcBookPage() { ++ms_constructed; }
    ~DestroyingXrcBookPage() override { ++ms_destroyed; }

    bool SetBackgroundColour(const wxColour& colour) override
    {
        const bool result = wxPanel::SetBackgroundColour(colour);
        wxWindow* const book = GetParent();
        wxWindow* const dialog = book ? wxGetTopLevelParent(book) : nullptr;
        if ( dialog )
        {
#if defined(__WXWINUI__) && wxUSE_WINUI3
            if ( ms_retainedGuardStorage )
            {
                ms_retainedGuardStorage->reset(
                    new wxWinUITLWHostWindowEventGuard(dialog));
            }
#endif
            dialog->Destroy();
        }
        return result;
    }

    bool SetForegroundColour(const wxColour& colour) override
    {
        ++ms_foregroundCalls;
        return wxPanel::SetForegroundColour(colour);
    }

    static void Reset()
    {
        ms_constructed = 0;
        ms_destroyed = 0;
        ms_foregroundCalls = 0;
    }

    static int GetConstructedCount() { return ms_constructed; }
    static int GetDestroyedCount() { return ms_destroyed; }
    static int GetForegroundCalls() { return ms_foregroundCalls; }

#if defined(__WXWINUI__) && wxUSE_WINUI3
    static void SetRetainedGuardStorage(
        std::unique_ptr<wxWinUITLWHostWindowEventGuard>* storage)
    {
        ms_retainedGuardStorage = storage;
    }
#endif

private:
    static int ms_constructed;
    static int ms_destroyed;
    static int ms_foregroundCalls;
#if defined(__WXWINUI__) && wxUSE_WINUI3
    static std::unique_ptr<wxWinUITLWHostWindowEventGuard>*
        ms_retainedGuardStorage;
#endif

    wxDECLARE_DYNAMIC_CLASS(DestroyingXrcBookPage);
};

int DestroyingXrcBookPage::ms_constructed = 0;
int DestroyingXrcBookPage::ms_destroyed = 0;
int DestroyingXrcBookPage::ms_foregroundCalls = 0;
#if defined(__WXWINUI__) && wxUSE_WINUI3
std::unique_ptr<wxWinUITLWHostWindowEventGuard>*
    DestroyingXrcBookPage::ms_retainedGuardStorage = nullptr;
#endif

wxIMPLEMENT_DYNAMIC_CLASS(DestroyingXrcBookPage, wxPanel);

class ReplacingXrcPropertySheetBookPage final : public wxPanel
{
public:
    ReplacingXrcPropertySheetBookPage() { ++ms_constructed; }
    ~ReplacingXrcPropertySheetBookPage() override { ++ms_destroyed; }

    bool SetBackgroundColour(const wxColour& colour) override
    {
        const bool result = wxPanel::SetBackgroundColour(colour);
        if ( m_replaced )
            return result;

        m_replaced = true;
        wxBookCtrlBase* const book =
            wxDynamicCast(GetParent(), wxBookCtrlBase);
        wxPropertySheetDialog* const dialog = wxDynamicCast(
            book ? book->GetParent() : nullptr,
            wxPropertySheetDialog);
        if ( book && ms_bookStorage )
            *ms_bookStorage = book;
        if ( dialog )
            dialog->SetBookCtrl(nullptr);

        return result;
    }

    static void Reset(wxWeakRef<wxWindow>* bookStorage)
    {
        ms_constructed = 0;
        ms_destroyed = 0;
        ms_bookStorage = bookStorage;
    }

    static void ClearStorage() { ms_bookStorage = nullptr; }
    static int GetConstructedCount() { return ms_constructed; }
    static int GetDestroyedCount() { return ms_destroyed; }

private:
    bool m_replaced { false };
    static int ms_constructed;
    static int ms_destroyed;
    static wxWeakRef<wxWindow>* ms_bookStorage;

    wxDECLARE_DYNAMIC_CLASS(ReplacingXrcPropertySheetBookPage);
};

int ReplacingXrcPropertySheetBookPage::ms_constructed = 0;
int ReplacingXrcPropertySheetBookPage::ms_destroyed = 0;
wxWeakRef<wxWindow>* ReplacingXrcPropertySheetBookPage::ms_bookStorage =
    nullptr;

wxIMPLEMENT_DYNAMIC_CLASS(ReplacingXrcPropertySheetBookPage, wxPanel);

class ReparentingXrcPropertySheetBookPage final : public wxPanel
{
public:
    ReparentingXrcPropertySheetBookPage() { ++ms_constructed; }
    ~ReparentingXrcPropertySheetBookPage() override { ++ms_destroyed; }

    bool SetBackgroundColour(const wxColour& colour) override
    {
        const bool result = wxPanel::SetBackgroundColour(colour);
        if ( m_reparented )
            return result;

        m_reparented = true;
        wxBookCtrlBase* const book =
            wxDynamicCast(GetParent(), wxBookCtrlBase);
        if ( book && ms_bookStorage )
            *ms_bookStorage = book;
        if ( book && ms_foreignParent )
        {
            ++ms_reparentCalls;
            book->Reparent(ms_foreignParent);
        }

        return result;
    }

    static void Reset(wxWindow* foreignParent,
                      wxWeakRef<wxWindow>* bookStorage)
    {
        ms_constructed = 0;
        ms_destroyed = 0;
        ms_reparentCalls = 0;
        ms_foreignParent = foreignParent;
        ms_bookStorage = bookStorage;
    }

    static void ClearStorage()
    {
        ms_foreignParent = nullptr;
        ms_bookStorage = nullptr;
    }

    static int GetConstructedCount() { return ms_constructed; }
    static int GetDestroyedCount() { return ms_destroyed; }
    static int GetReparentCalls() { return ms_reparentCalls; }

private:
    bool m_reparented { false };
    static int ms_constructed;
    static int ms_destroyed;
    static int ms_reparentCalls;
    static wxWindow* ms_foreignParent;
    static wxWeakRef<wxWindow>* ms_bookStorage;

    wxDECLARE_DYNAMIC_CLASS(ReparentingXrcPropertySheetBookPage);
};

int ReparentingXrcPropertySheetBookPage::ms_constructed = 0;
int ReparentingXrcPropertySheetBookPage::ms_destroyed = 0;
int ReparentingXrcPropertySheetBookPage::ms_reparentCalls = 0;
wxWindow* ReparentingXrcPropertySheetBookPage::ms_foreignParent = nullptr;
wxWeakRef<wxWindow>*
    ReparentingXrcPropertySheetBookPage::ms_bookStorage = nullptr;

wxIMPLEMENT_DYNAMIC_CLASS(ReparentingXrcPropertySheetBookPage, wxPanel);

class TopologyMutatingXrcPropertySheet final
    : public wxPropertySheetDialog
{
public:
    enum class Action
    {
        SetupReparentOwner,
        SetupReplaceTopSizer,
        LayoutReparentOwner,
        LayoutReplaceTopSizer
    };

    TopologyMutatingXrcPropertySheet(Action action, wxWindow* foreignOwner)
        : m_action(action), m_foreignOwner(foreignOwner)
    {
    }

    ~TopologyMutatingXrcPropertySheet() override { RestoreTopSizer(); }

    bool SetBackgroundColour(const wxColour& colour) override
    {
        const bool result =
            wxPropertySheetDialog::SetBackgroundColour(colour);
        if ( m_action == Action::SetupReparentOwner ||
             m_action == Action::SetupReplaceTopSizer )
        {
            MutateTopology();
        }
        return result;
    }

    void LayoutDialog(int centreFlags) override
    {
        wxPropertySheetDialog::LayoutDialog(centreFlags);
        if ( m_action == Action::LayoutReparentOwner ||
             m_action == Action::LayoutReplaceTopSizer )
        {
            MutateTopology();
        }
    }

    int GetMutationCount() const { return m_mutationCount; }
    wxSizer* GetOriginalTopSizer() const { return m_originalTopSizer; }

private:
    void MutateTopology()
    {
        if ( m_mutated )
            return;

        m_mutated = true;
        ++m_mutationCount;
        if ( m_action == Action::SetupReparentOwner ||
             m_action == Action::LayoutReparentOwner )
        {
            Reparent(m_foreignOwner);
            return;
        }

        m_originalTopSizer = GetSizer();
        SetSizer(new wxBoxSizer(wxVERTICAL), false);
    }

    void RestoreTopSizer()
    {
        if ( !m_originalTopSizer || GetSizer() == m_originalTopSizer )
            return;

        wxSizer* const replacement = GetSizer();
        SetSizer(m_originalTopSizer, false);
        delete replacement;
        m_originalTopSizer = nullptr;
    }

    const Action m_action;
    wxWindow* const m_foreignOwner;
    bool m_mutated { false };
    int m_mutationCount { 0 };
    wxSizer* m_originalTopSizer { nullptr };
};

#if defined(__WXWINUI__) && wxUSE_WINUI3
class DestroyingXrcPropertySheet final : public wxPropertySheetDialog
{
public:
    enum class Action
    {
        CreateButtons,
        Centre
    };

    DestroyingXrcPropertySheet(Action action,
                               int* buttonCalls,
                               int* layoutCalls,
                               int* centreCalls,
                               std::unique_ptr<
                                   wxWinUITLWHostWindowEventGuard>* guard)
        : m_action(action),
          m_buttonCalls(buttonCalls),
          m_layoutCalls(layoutCalls),
          m_centreCalls(centreCalls),
          m_guard(guard)
    {
    }

    void CreateButtons(int flags) override
    {
        ++*m_buttonCalls;
        wxPropertySheetDialog::CreateButtons(flags);
        if ( m_action == Action::CreateButtons )
        {
            m_guard->reset(new wxWinUITLWHostWindowEventGuard(this));
            Destroy();
        }
    }

    void LayoutDialog(int centreFlags) override
    {
        ++*m_layoutCalls;
        wxPropertySheetDialog::LayoutDialog(centreFlags);
    }

protected:
    void DoCentre(int direction) override
    {
        ++*m_centreCalls;
        wxPropertySheetDialog::DoCentre(direction);
        if ( m_action == Action::Centre )
        {
            m_guard->reset(new wxWinUITLWHostWindowEventGuard(this));
            Destroy();
        }
    }

private:
    const Action m_action;
    int* const m_buttonCalls;
    int* const m_layoutCalls;
    int* const m_centreCalls;
    std::unique_ptr<wxWinUITLWHostWindowEventGuard>* const m_guard;
};
#endif
#endif // wxUSE_XRC

class NullBookPropertySheet final : public wxPropertySheetDialog
{
public:
    wxBookCtrlBase* CreateBookCtrl() override { return nullptr; }
};

class ForeignBookPropertySheet final : public wxPropertySheetDialog
{
public:
    explicit ForeignBookPropertySheet(wxWindow* foreignParent)
        : m_foreignParent(foreignParent)
    {
    }

    wxBookCtrlBase* CreateBookCtrl() override
    {
        return new wxBookCtrl(m_foreignParent, wxID_ANY);
    }

private:
    wxWindow* const m_foreignParent;
};

class AutoAdoptingBookPropertySheet final : public wxPropertySheetDialog
{
public:
    ~AutoAdoptingBookPropertySheet() override
    {
        wxWindow* const liveBook = m_bookLifetime.get();
        if ( liveBook && m_adoptionSizer &&
             liveBook->GetContainingSizer() == m_adoptionSizer )
        {
            m_adoptionSizer->Detach(liveBook);
        }
        delete m_adoptionSizer;
    }

    wxBookCtrlBase* CreateBookCtrl() override
    {
        wxBookCtrl* const book = new wxBookCtrl(this, wxID_ANY);
        m_bookLifetime = book;
        m_adoptionSizer = new wxBoxSizer(wxVERTICAL);
        m_adoptionSizer->Add(book);
        return book;
    }

    wxBookCtrlBase* GetAdoptedBook() const
    {
        return wxDynamicCast(m_bookLifetime.get(), wxBookCtrlBase);
    }

    wxSizer* GetAdoptionSizer() const { return m_adoptionSizer; }

private:
    wxWeakRef<wxWindow> m_bookLifetime;
    wxSizer* m_adoptionSizer { nullptr };
};

class MutatingTabOrderButton final : public wxButton
{
public:
    enum class Action
    {
        ReplaceNegativeSlot,
        NestedRealize
    };

    MutatingTabOrderButton(wxWindow* parent,
                           wxWindowID id,
                           wxStdDialogButtonSizer* sizer,
                           Action action,
                           wxButton* replacement = nullptr)
        : wxButton(parent, id),
          m_sizer(sizer),
          m_action(action),
          m_replacement(replacement)
    {
    }

    int GetCallbackCount() const { return m_callbackCount; }

protected:
    void DoMoveInTabOrder(wxWindow* window, WindowOrder move) override
    {
        if ( m_callbackCount++ == 0 )
        {
            if ( m_action == Action::ReplaceNegativeSlot )
                m_sizer->SetNegativeButton(m_replacement);
            else
                m_sizer->Realize();
        }

        wxButton::DoMoveInTabOrder(window, move);
    }

private:
    wxStdDialogButtonSizer* const m_sizer;
    const Action m_action;
    wxButton* const m_replacement;
    int m_callbackCount { 0 };
};

#if wxUSE_EXCEPTIONS
class ThrowingFocusButton final : public wxButton
{
public:
    ThrowingFocusButton(wxWindow* parent, const wxString& label)
        : wxButton(parent, wxID_ANY, label)
    {
    }

    void ArmThrowOnce() { m_armed = true; }
    int GetThrowCount() const { return m_throwCount; }

    void SetFocus() override
    {
        // Restore the observable focus first, then model an application
        // override failing on its way back to the transaction guard.
        wxButton::SetFocus();
        if ( m_armed && m_throwCount == 0 )
        {
            ++m_throwCount;
            throw 1;
        }
    }

private:
    bool m_armed { false };
    int m_throwCount { 0 };
};

class FocusTopologyInvalidatingPropertySheet final
    : public wxPropertySheetDialog
{
public:
    ~FocusTopologyInvalidatingPropertySheet() override
    {
        RestoreTopSizer();
    }

    void InvalidateTopSizer()
    {
        if ( m_originalTopSizer )
            return;

        m_originalTopSizer = GetSizer();
        SetSizer(new wxBoxSizer(wxVERTICAL), false);
    }

    wxSizer* GetOriginalTopSizer() const { return m_originalTopSizer; }

private:
    void RestoreTopSizer()
    {
        if ( !m_originalTopSizer || GetSizer() == m_originalTopSizer )
            return;

        wxSizer* const replacement = GetSizer();
        SetSizer(m_originalTopSizer, false);
        delete replacement;
        m_originalTopSizer = nullptr;
    }

    wxSizer* m_originalTopSizer { nullptr };
};

class TransactionalInnerSizer final : public wxBoxSizer
{
public:
    enum class Mode
    {
        RejectButtonSizerAndInvalidateTop,
        ConsumeButtonSizerAndReturnNull,
        ConsumeButtonSizerAndThrow,
        ThrowOnSpacerAndDetach
    };

    TransactionalInnerSizer(Mode mode,
                            const std::function<void()>& invalidateTop)
        : wxBoxSizer(wxVERTICAL),
          m_mode(mode),
          m_invalidateTop(invalidateTop)
    {
    }

    void Arm() { m_armed = true; }
    wxSizer* GetTrackedButtonSizer() const { return m_buttonSizer; }
    int GetRejectedInsertCount() const { return m_rejectedInsertCount; }
    int GetConsumedInsertCount() const { return m_consumedInsertCount; }
    int GetSpacerThrowCount() const { return m_spacerThrowCount; }
    int GetDetachThrowCount() const { return m_detachThrowCount; }

    bool Detach(wxSizer* sizer) override
    {
        if ( m_armed &&
             m_mode == Mode::ThrowOnSpacerAndDetach &&
             sizer == m_buttonSizer && m_detachThrowCount == 0 )
        {
            ++m_detachThrowCount;
            throw 3;
        }

        return wxBoxSizer::Detach(sizer);
    }

protected:
    wxSizerItem* DoInsert(size_t index, wxSizerItem* item) override
    {
        if ( !m_armed )
            return wxBoxSizer::DoInsert(index, item);

        if ( wxSizer* const childSizer = item->GetSizer() )
        {
            if ( m_mode == Mode::ConsumeButtonSizerAndReturnNull ||
                 m_mode == Mode::ConsumeButtonSizerAndThrow )
            {
                ++m_consumedInsertCount;

                // Deliberately don't call DetachSizer(): this models a valid
                // virtual insertion boundary consuming both its item and the
                // child before reporting failure. No test retains or reads
                // the child address after this point.
                delete item;
                if ( m_mode == Mode::ConsumeButtonSizerAndThrow )
                    throw 4;
                return nullptr;
            }

            if ( m_buttonSizer )
                return wxBoxSizer::DoInsert(index, item);

            m_buttonSizer = childSizer;
            if ( m_mode == Mode::RejectButtonSizerAndInvalidateTop )
            {
                ++m_rejectedInsertCount;
                m_invalidateTop();
                item->DetachSizer();
                delete item;
                return nullptr;
            }

            return wxBoxSizer::DoInsert(index, item);
        }

        if ( m_mode == Mode::ThrowOnSpacerAndDetach &&
             m_buttonSizer && item->IsSpacer() )
        {
            ++m_spacerThrowCount;
            delete item;
            throw 2;
        }

        return wxBoxSizer::DoInsert(index, item);
    }

private:
    const Mode m_mode;
    const std::function<void()> m_invalidateTop;
    bool m_armed { false };
    wxSizer* m_buttonSizer { nullptr };
    int m_rejectedInsertCount { 0 };
    int m_consumedInsertCount { 0 };
    int m_spacerThrowCount { 0 };
    int m_detachThrowCount { 0 };
};

class TransactionalInnerPropertySheet final
    : public wxPropertySheetDialog
{
public:
    ~TransactionalInnerPropertySheet() override { RestoreTopSizer(); }

    bool InstallInnerSizer(TransactionalInnerSizer::Mode mode)
    {
        wxSizer* const topSizer = GetSizer();
        wxSizer* const oldInnerSizer = GetInnerSizer();
        wxBookCtrlBase* const book = GetBookCtrl();
        if ( !topSizer || !oldInnerSizer || !book ||
             !topSizer->Detach(oldInnerSizer) ||
             !oldInnerSizer->Detach(book) )
        {
            return false;
        }

        m_inner = new TransactionalInnerSizer(
            mode, [this]() { InvalidateTopSizer(); });
        if ( !m_inner->Add(book) ||
             !topSizer->Add(m_inner,
                            1,
                            wxGROW | wxALL,
                            GetSheetOuterBorder()) )
        {
            return false;
        }

        SetInnerSizer(m_inner);
        SetBookCtrl(book);
        delete oldInnerSizer;
        return true;
    }

    TransactionalInnerSizer* GetTransactionalInnerSizer() const
    {
        return m_inner;
    }

    wxSizer* GetOriginalTopSizer() const { return m_originalTopSizer; }

private:
    void InvalidateTopSizer()
    {
        if ( m_originalTopSizer )
            return;

        m_originalTopSizer = GetSizer();
        SetSizer(new wxBoxSizer(wxVERTICAL), false);
    }

    void RestoreTopSizer()
    {
        if ( !m_originalTopSizer || GetSizer() == m_originalTopSizer )
            return;

        wxSizer* const replacement = GetSizer();
        SetSizer(m_originalTopSizer, false);
        delete replacement;
        m_originalTopSizer = nullptr;
    }

    TransactionalInnerSizer* m_inner { nullptr };
    wxSizer* m_originalTopSizer { nullptr };
};
#endif // wxUSE_EXCEPTIONS

class DpiMutatingPropertySheet final : public wxPropertySheetDialog
{
public:
    enum class Action
    {
        ReparentOwner,
        ReplaceTopSizer
    };

    DpiMutatingPropertySheet(Action action, wxWindow* foreignOwner)
        : m_action(action), m_foreignOwner(foreignOwner)
    {
    }

    ~DpiMutatingPropertySheet() override { RestoreTopSizer(); }

    void Arm() { m_armed = true; }
    int GetMutationCount() const { return m_mutationCount; }
    wxSizer* GetOriginalTopSizer() const { return m_originalTopSizer; }

    wxSize GetDPI() const override
    {
        const wxSize dpi = wxPropertySheetDialog::GetDPI();
        if ( m_armed && !m_mutated )
        {
            m_mutated = true;
            ++m_mutationCount;
            const_cast<DpiMutatingPropertySheet*>(this)->MutateTopology();
        }
        return dpi;
    }

private:
    void MutateTopology()
    {
        if ( m_action == Action::ReparentOwner )
        {
            Reparent(m_foreignOwner);
            return;
        }

        m_originalTopSizer = GetSizer();
        SetSizer(new wxBoxSizer(wxVERTICAL), false);
    }

    void RestoreTopSizer()
    {
        if ( !m_originalTopSizer || GetSizer() == m_originalTopSizer )
            return;

        wxSizer* const replacement = GetSizer();
        SetSizer(m_originalTopSizer, false);
        delete replacement;
        m_originalTopSizer = nullptr;
    }

    const Action m_action;
    wxWindow* const m_foreignOwner;
    mutable bool m_armed { false };
    mutable bool m_mutated { false };
    mutable int m_mutationCount { 0 };
    wxSizer* m_originalTopSizer { nullptr };
};

class ReparentingAddBookPropertySheet final : public wxPropertySheetDialog
{
public:
    ReparentingAddBookPropertySheet(wxWindow* foreignParent,
                                    int* callbackCalls,
                                    wxWeakRef<wxWindow>* bookStorage)
        : m_foreignParent(foreignParent),
          m_callbackCalls(callbackCalls),
          m_bookStorage(bookStorage)
    {
    }

    void AddBookCtrl(wxSizer* sizer) override
    {
        wxPropertySheetDialog::AddBookCtrl(sizer);
        wxBookCtrlBase* const book = wxPropertySheetDialog::GetBookCtrl();
        *m_bookStorage = book;
        ++*m_callbackCalls;
        book->Reparent(m_foreignParent);

        // Model application code re-entering the public accessor before the
        // virtual AddBookCtrl() boundary returns.
        m_reentrantBook = wxPropertySheetDialog::GetBookCtrl();
    }

    wxBookCtrlBase* GetReentrantBook() const { return m_reentrantBook; }

private:
    wxWindow* const m_foreignParent;
    int* const m_callbackCalls;
    wxWeakRef<wxWindow>* const m_bookStorage;
    wxBookCtrlBase* m_reentrantBook { nullptr };
};

class BoundaryPropertySheetBook final : public wxBookCtrl
{
public:
    enum class Action
    {
        Delete,
        Reparent
    };

    explicit BoundaryPropertySheetBook(int* callbackCalls)
        : m_callbackCalls(callbackCalls)
    {
    }

    void Arm(Action action, wxWindow* foreignParent)
    {
        m_action = action;
        m_foreignParent = foreignParent;
        m_armed = true;
    }

    int GetSelection() const override
    {
        const int selection = wxBookCtrl::GetSelection();
        if ( !m_armed )
            return selection;

        m_armed = false;
        ++*m_callbackCalls;
        BoundaryPropertySheetBook* const self =
            const_cast<BoundaryPropertySheetBook*>(this);
        if ( m_action == Action::Reparent )
        {
            self->Reparent(m_foreignParent);
        }
        else
        {
            delete self;
        }

        return selection;
    }

private:
    int* const m_callbackCalls;
    Action m_action { Action::Delete };
    wxWindow* m_foreignParent { nullptr };
    mutable bool m_armed { false };
};

class BoundaryBookPropertySheet final : public wxPropertySheetDialog
{
public:
    BoundaryBookPropertySheet(BoundaryPropertySheetBook::Action action,
                              wxWindow* foreignParent,
                              bool armDuringCreate,
                              int* callbackCalls,
                              wxWeakRef<wxWindow>* bookStorage)
        : m_action(action),
          m_foreignParent(foreignParent),
          m_armDuringCreate(armDuringCreate),
          m_callbackCalls(callbackCalls),
          m_bookStorage(bookStorage)
    {
    }

    void ArmBook()
    {
        wxASSERT(m_book);
        m_book->Arm(m_action, m_foreignParent);
    }

    wxBookCtrlBase* CreateBookCtrl() override
    {
        BoundaryPropertySheetBook* const book =
            new BoundaryPropertySheetBook(m_callbackCalls);
        if ( !book->Create(this,
                           wxID_ANY,
                           wxDefaultPosition,
                           wxDefaultSize,
                           wxCLIP_CHILDREN | wxBK_DEFAULT) )
        {
            delete book;
            return nullptr;
        }

        m_book = book;
        *m_bookStorage = book;
        return book;
    }

    void AddBookCtrl(wxSizer* sizer) override
    {
        wxPropertySheetDialog::AddBookCtrl(sizer);
        if ( m_armDuringCreate )
            ArmBook();
    }

private:
    const BoundaryPropertySheetBook::Action m_action;
    wxWindow* const m_foreignParent;
    const bool m_armDuringCreate;
    int* const m_callbackCalls;
    wxWeakRef<wxWindow>* const m_bookStorage;
    BoundaryPropertySheetBook* m_book { nullptr };
};

class ValidatingPropertySheet final : public wxPropertySheetDialog
{
public:
    bool Validate() override
    {
        ++validateCalls;
        return allowValidation;
    }

    bool TransferDataFromWindow() override
    {
        ++transferCalls;
        return allowTransfer;
    }

    bool allowValidation = false;
    bool allowTransfer = true;
    int validateCalls = 0;
    int transferCalls = 0;
};

class EndingValidationPropertySheet final : public wxPropertySheetDialog
{
public:
    bool Validate() override
    {
        ++validateCalls;
        EndModal(wxID_CANCEL);
        return true;
    }

    bool TransferDataFromWindow() override
    {
        ++transferCalls;
        return true;
    }

    int validateCalls { 0 };
    int transferCalls { 0 };
};

class DestructiveValidationPropertySheet final
    : public wxPropertySheetDialog
{
public:
    enum class Action
    {
        DeleteInValidate,
        DeleteInTransfer,
        DestroyInValidate,
        DestroyInTransfer
    };

    DestructiveValidationPropertySheet(
        Action action,
        std::unique_ptr<DestructiveValidationPropertySheet>* owner,
        bool* callbackRan,
        int* validateCalls,
        int* transferCalls)
        : m_action(action),
          m_owner(owner),
          m_callbackRan(callbackRan),
          m_validateCalls(validateCalls),
          m_transferCalls(transferCalls)
    {
    }

    void AcceptForTest() { AcceptAndClose(); }

    bool Validate() override
    {
        ++*m_validateCalls;
        if ( m_action == Action::DeleteInValidate )
        {
            *m_callbackRan = true;
            std::unique_ptr<DestructiveValidationPropertySheet>* const owner =
                m_owner;
            owner->reset();
            return true;
        }
        if ( m_action == Action::DestroyInValidate )
        {
            *m_callbackRan = true;
            Destroy();
        }
        return true;
    }

    bool TransferDataFromWindow() override
    {
        ++*m_transferCalls;
        if ( m_action == Action::DeleteInTransfer )
        {
            *m_callbackRan = true;
            std::unique_ptr<DestructiveValidationPropertySheet>* const owner =
                m_owner;
            owner->reset();
            return true;
        }
        if ( m_action == Action::DestroyInTransfer )
        {
            *m_callbackRan = true;
            Destroy();
        }
        return true;
    }

private:
    const Action m_action;
    std::unique_ptr<DestructiveValidationPropertySheet>* const m_owner;
    bool* const m_callbackRan;
    int* const m_validateCalls;
    int* const m_transferCalls;
};

class PartialPropertySheetLayoutAdapter final
    : public wxStandardDialogLayoutAdapter
{
public:
    void ReparentControls(wxWindow* source,
                          wxWindow* destination,
                          wxSizer* excludedSizer = nullptr) override
    {
        ++reparentCalls;
        for ( wxWindowList::compatibility_iterator node =
                  source->GetChildren().GetFirst();
              node;
              node = node->GetNext() )
        {
            wxWindow* const child = node->GetData();
            if ( child != destination &&
                 (!excludedSizer || !excludedSizer->GetItem(child)) )
            {
                child->Reparent(destination);
                return;
            }
        }
    }

    int reparentCalls { 0 };
};

class RejectingPropertySheetFitAdapter final
    : public wxStandardDialogLayoutAdapter
{
public:
    bool FitWithScrolling(wxDialog*, wxWindowList&) override
    {
        ++fitCalls;
        return false;
    }

    bool FitWithScrolling(wxDialog*, wxScrolledWindow*) override
    {
        ++fitCalls;
        return false;
    }

    int fitCalls { 0 };
};

class MoveThenRejectPropertySheetPanel final : public wxPanel
{
public:
    MoveThenRejectPropertySheetPanel(wxWindow* parent, int* reparentCalls)
        : wxPanel(parent), m_reparentCalls(reparentCalls)
    {
    }

    bool Reparent(wxWindowBase* newParent) override
    {
        ++*m_reparentCalls;
        const bool moved = wxPanel::Reparent(newParent);
        if ( !m_rejectedOnce )
        {
            m_rejectedOnce = true;
            return false;
        }

        return moved;
    }

private:
    int* const m_reparentCalls;
    bool m_rejectedOnce { false };
};

#if defined(__WXWINUI__) && wxUSE_WINUI3
class DestroyingPropertySheetBook final : public wxBookCtrl
{
public:
    enum class Action
    {
        PageCount,
        Page
    };

    DestroyingPropertySheetBook(int* pageCountCalls, int* pageCalls)
        : m_pageCountCalls(pageCountCalls), m_pageCalls(pageCalls)
    {
    }

    void Arm(wxWindow* dialog, Action action)
    {
        m_dialog = dialog;
        m_action = action;
        m_armed = true;
    }

    size_t GetPageCount() const override
    {
        ++*m_pageCountCalls;
        const size_t count = wxBookCtrl::GetPageCount();
        RequestDestroy(Action::PageCount);
        return count;
    }

    wxWindow* GetPage(size_t index) const override
    {
        ++*m_pageCalls;
        wxWindow* const page = wxBookCtrl::GetPage(index);
        RequestDestroy(Action::Page);
        return page;
    }

private:
    void RequestDestroy(Action action) const
    {
        if ( !m_armed || m_fired || m_action != action )
            return;

        m_fired = true;
        if ( wxWindow* const dialog = m_dialog.get() )
            dialog->Destroy();
    }

    int* const m_pageCountCalls;
    int* const m_pageCalls;
    wxWeakRef<wxWindow> m_dialog;
    Action m_action { Action::PageCount };
    mutable bool m_armed { false };
    mutable bool m_fired { false };
};

class DestroyingBookPropertySheet final : public wxPropertySheetDialog
{
public:
    DestroyingBookPropertySheet(DestroyingPropertySheetBook::Action action,
                                int* pageCountCalls,
                                int* pageCalls)
        : m_action(action),
          m_pageCountCalls(pageCountCalls),
          m_pageCalls(pageCalls)
    {
    }

    void ArmBook()
    {
        wxASSERT(m_book);
        m_book->Arm(this, m_action);
    }

    wxBookCtrlBase* CreateBookCtrl() override
    {
        DestroyingPropertySheetBook* const book =
            new DestroyingPropertySheetBook(m_pageCountCalls, m_pageCalls);
        if ( !book->Create(this,
                           wxID_ANY,
                           wxDefaultPosition,
                           wxDefaultSize,
                           wxCLIP_CHILDREN | wxBK_DEFAULT) )
        {
            delete book;
            return nullptr;
        }

        m_book = book;
        return book;
    }

private:
    const DestroyingPropertySheetBook::Action m_action;
    int* const m_pageCountCalls;
    int* const m_pageCalls;
    DestroyingPropertySheetBook* m_book { nullptr };
};

class DestroyPropertySheetOnReparentPanel final : public wxPanel
{
public:
    DestroyPropertySheetOnReparentPanel(wxWindow* parent,
                                        wxPropertySheetDialog* dialog,
                                        int* reparentCalls)
        : wxPanel(parent),
          m_dialog(dialog),
          m_reparentCalls(reparentCalls)
    {
    }

    bool Reparent(wxWindowBase* newParent) override
    {
        ++*m_reparentCalls;
        const bool moved = wxPanel::Reparent(newParent);
        if ( !m_destroyRequested )
        {
            m_destroyRequested = true;
            if ( wxPropertySheetDialog* const dialog = m_dialog.get() )
                dialog->Destroy();
        }
        return moved;
    }

private:
    wxWeakRef<wxPropertySheetDialog> m_dialog;
    int* const m_reparentCalls;
    bool m_destroyRequested { false };
};
#endif

std::vector<long> GetAvailablePropertySheetStyles()
{
    std::vector<long> styles;
    styles.push_back(wxPROPSHEET_DEFAULT);
#if wxUSE_NOTEBOOK
    styles.push_back(wxPROPSHEET_NOTEBOOK);
#endif
#if wxUSE_CHOICEBOOK
    styles.push_back(wxPROPSHEET_CHOICEBOOK);
#endif
#if wxUSE_TOOLBOOK
    styles.push_back(wxPROPSHEET_TOOLBOOK);
    styles.push_back(wxPROPSHEET_BUTTONTOOLBOOK);
#endif
#if wxUSE_LISTBOOK
    styles.push_back(wxPROPSHEET_LISTBOOK);
#endif
#if wxUSE_TREEBOOK
    styles.push_back(wxPROPSHEET_TREEBOOK);
#endif
    return styles;
}

bool IsExpectedPropertySheetBook(wxBookCtrlBase* book, long sheetStyle)
{
    if ( sheetStyle == wxPROPSHEET_DEFAULT )
        return wxDynamicCast(book, wxBookCtrl) != nullptr;
#if wxUSE_NOTEBOOK
    if ( sheetStyle == wxPROPSHEET_NOTEBOOK )
        return wxDynamicCast(book, wxNotebook) != nullptr;
#endif
#if wxUSE_CHOICEBOOK
    if ( sheetStyle == wxPROPSHEET_CHOICEBOOK )
        return wxDynamicCast(book, wxChoicebook) != nullptr;
#endif
#if wxUSE_TOOLBOOK
    if ( sheetStyle == wxPROPSHEET_TOOLBOOK ||
         sheetStyle == wxPROPSHEET_BUTTONTOOLBOOK )
    {
        return wxDynamicCast(book, wxToolbook) != nullptr;
    }
#endif
#if wxUSE_LISTBOOK
    if ( sheetStyle == wxPROPSHEET_LISTBOOK )
        return wxDynamicCast(book, wxListbook) != nullptr;
#endif
#if wxUSE_TREEBOOK
    if ( sheetStyle == wxPROPSHEET_TREEBOOK )
        return wxDynamicCast(book, wxTreebook) != nullptr;
#endif
    return false;
}

std::vector<long> GetUnavailablePropertySheetStyles()
{
    std::vector<long> styles;
#if !wxUSE_NOTEBOOK
    styles.push_back(wxPROPSHEET_NOTEBOOK);
#endif
#if !wxUSE_CHOICEBOOK
    styles.push_back(wxPROPSHEET_CHOICEBOOK);
#endif
#if !wxUSE_TOOLBOOK
    styles.push_back(wxPROPSHEET_TOOLBOOK);
    styles.push_back(wxPROPSHEET_BUTTONTOOLBOOK);
#endif
#if !wxUSE_LISTBOOK
    styles.push_back(wxPROPSHEET_LISTBOOK);
#endif
#if !wxUSE_TREEBOOK
    styles.push_back(wxPROPSHEET_TREEBOOK);
#endif
    return styles;
}

#endif // wxUSE_BOOKCTRL

#if wxUSE_PREFERENCES_EDITOR

#ifndef wxHAS_PREF_EDITOR_MODELESS

size_t CountDynamicEventEntries(wxEvtHandler* handler)
{
    size_t count = 0;
    size_t cookie;
    for ( wxDynamicEventTableEntry* entry =
              handler->GetFirstDynamicEntry(cookie);
          entry;
          entry = handler->GetNextDynamicEntry(cookie) )
    {
        ++count;
    }
    return count;
}

#endif // !wxHAS_PREF_EDITOR_MODELESS

class TrackingPreferencesPage final : public wxPreferencesPage
{
public:
    typedef std::function<void(wxWindow*)> CreateCallback;
    typedef std::function<void()> NameCallback;

    TrackingPreferencesPage(const wxString& name,
                            std::vector<wxString>* calls = nullptr,
                            CreateCallback onCreate = CreateCallback(),
                            NameCallback onName = NameCallback())
        : m_name(name),
          m_calls(calls),
          m_onCreate(onCreate),
          m_onName(onName)
    {
    }

    wxString GetName() const override
    {
        if ( m_calls )
            m_calls->push_back("name:" + m_name);
        if ( m_onName )
            m_onName();
        return m_name;
    }

    wxWindow* CreateWindow(wxWindow* parent) override
    {
        if ( m_calls )
            m_calls->push_back("create:" + m_name);
        wxPanel* const page = CreateSizedPage(parent, m_name);
        if ( m_onCreate )
            m_onCreate(page);
        return page;
    }

private:
    const wxString m_name;
    std::vector<wxString>* const m_calls;
    const CreateCallback m_onCreate;
    const NameCallback m_onName;
};

#endif // wxUSE_PREFERENCES_EDITOR

} // anonymous namespace

#if wxUSE_WIZARDDLG

TEST_CASE("WinUIWizardContracts", "[wizard][winui-beta-dialogs]")
{
    const wxWeakRef<wxWindow> emptyWindowLifetime;
    CHECK(wxWindowIsUnavailableForCallbacks(nullptr));
    CHECK_FALSE(wxWeakWindowIsAvailableForCallbacks(
        emptyWindowLifetime, static_cast<wxWindow*>(nullptr)));

    wxWindow* const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    SECTION("Default_Create_direct_constructor_bitmap_and_layout")
    {
        wxWizard created;
        created.SetExtraStyle(wxWIZARD_EX_HELPBUTTON);
        REQUIRE(created.Create(parent, wxID_ANY, "Created wizard"));
        created.SetPageSize(wxSize(360, 220));
        created.SetBorder(9);
        created.SetBitmapPlacement(wxWIZARD_VALIGN_CENTRE |
                                   wxWIZARD_HALIGN_CENTRE);
        created.SetMinimumBitmapWidth(48);

        wxBitmap bitmap(24, 32);
        REQUIRE(bitmap.IsOk());
        created.SetBitmap(wxBitmapBundle::FromBitmap(bitmap));
        CHECK(created.GetBitmap().IsOk());
        CHECK(created.GetMinimumBitmapWidth() == 48);
        CHECK(created.GetBitmapPlacement() ==
              (wxWIZARD_VALIGN_CENTRE | wxWIZARD_HALIGN_CENTRE));

        TrackingWizardPage* const first =
            new TrackingWizardPage(&created, "Sized first page");
        TrackingWizardPage* const second =
            new TrackingWizardPage(&created, "Sized second page");
        first->SetNext(second);
        second->SetPrev(first);
        AddWizardPage(&created, first);
        AddWizardPage(&created, second);
        created.FitToPage(first);
        CHECK(created.GetPageSize().x >= 360);
        CHECK(created.GetPageSize().y >= 220);
        CHECK(created.FindWindow(wxID_FORWARD));
        CHECK(created.FindWindow(wxID_BACKWARD));
        CHECK(created.FindWindow(wxID_CANCEL));
        CHECK(created.FindWindow(wxID_HELP));

        wxWizard direct(parent, wxID_ANY, "Direct wizard");
        CHECK(direct.GetPageAreaSizer() != nullptr);
        CHECK(direct.FindWindow(wxID_FORWARD) != nullptr);
    }

    SECTION("Forward_back_validation_dynamic_chain_help_and_finish")
    {
        wxWizard wizard;
        wizard.SetExtraStyle(wxWIZARD_EX_HELPBUTTON);
        REQUIRE(wizard.Create(parent, wxID_ANY, "Navigation wizard"));

        TrackingWizardPage* const first =
            new TrackingWizardPage(&wizard, "First");
        TrackingWizardPage* const second =
            new TrackingWizardPage(&wizard, "Second");
        TrackingWizardPage* const inserted =
            new TrackingWizardPage(&wizard, "Inserted");
        first->SetNext(second);
        second->SetPrev(first);
        AddWizardPage(&wizard, first);
        AddWizardPage(&wizard, second);
        AddWizardPage(&wizard, inserted);

        int changingEvents = 0;
        int changedEvents = 0;
        int helpEvents = 0;
        int finishedEvents = 0;
        first->Bind(wxEVT_WIZARD_PAGE_CHANGING,
                    [&changingEvents](wxWizardEvent& event)
                    {
                        ++changingEvents;
                        event.Skip();
                    });
        second->Bind(wxEVT_WIZARD_PAGE_CHANGED,
                     [&changedEvents](wxWizardEvent& event)
                     {
                         ++changedEvents;
                         event.Skip();
                     });
        inserted->Bind(wxEVT_WIZARD_HELP,
                       [&helpEvents](wxWizardEvent& event)
                       {
                           ++helpEvents;
                           event.Skip();
                       });
        wizard.Bind(wxEVT_WIZARD_FINISHED,
                    [&finishedEvents](wxWizardEvent& event)
                    {
                        ++finishedEvents;
                        event.Skip();
                    });

        bool commandsFound = true;
        bool validationVetoStayed = false;
        bool reachedSecond = false;
        bool returnedToFirst = false;
        bool reachedInserted = false;
        first->allowValidation = false;
        wizard.CallAfter([&]()
        {
            commandsFound = CommandDialogButton(&wizard, wxID_FORWARD);
            validationVetoStayed = wizard.GetCurrentPage() == first;

            first->allowValidation = true;
            commandsFound = CommandDialogButton(&wizard, wxID_FORWARD) &&
                            commandsFound;
            reachedSecond = wizard.GetCurrentPage() == second;

            commandsFound = CommandDialogButton(&wizard, wxID_BACKWARD) &&
                            commandsFound;
            returnedToFirst = wizard.GetCurrentPage() == first;

            // The next page is allowed to change between visits.
            first->SetNext(inserted);
            inserted->SetPrev(first);
            commandsFound = CommandDialogButton(&wizard, wxID_FORWARD) &&
                            commandsFound;
            reachedInserted = wizard.GetCurrentPage() == inserted;
            commandsFound = CommandDialogButton(&wizard, wxID_HELP) &&
                            commandsFound;
            commandsFound = CommandDialogButton(&wizard, wxID_FORWARD) &&
                            commandsFound;

            if ( wizard.IsModal() &&
                 wizard.GetReturnCode() != wxID_OK )
                wizard.EndModal(wxID_CANCEL);
        });

        CHECK(wizard.RunWizard(first));
        CHECK(commandsFound);
        CHECK(validationVetoStayed);
        CHECK(reachedSecond);
        CHECK(returnedToFirst);
        CHECK(reachedInserted);
        CHECK(first->validateCalls == 3);
        CHECK(first->transferFromCalls == 2);
        CHECK(second->validateCalls == 1);
        CHECK(second->transferFromCalls == 1);
        CHECK(changingEvents == 2);
        CHECK(changedEvents == 1);
        CHECK(helpEvents == 1);
        CHECK(finishedEvents == 1);
        CHECK(wizard.GetCurrentPage() == nullptr);
    }

    SECTION("Cancel_veto_then_cancel")
    {
        wxWizard wizard(parent, wxID_ANY, "Cancelable wizard");
        TrackingWizardPage* const page =
            new TrackingWizardPage(&wizard, "Cancelable page");
        AddWizardPage(&wizard, page);

        int cancelEvents = 0;
        bool vetoFirst = true;
        page->Bind(wxEVT_WIZARD_CANCEL,
                   [&cancelEvents, &vetoFirst](wxWizardEvent& event)
                   {
                       ++cancelEvents;
                       if ( vetoFirst )
                       {
                           vetoFirst = false;
                           event.Veto();
                       }
                       else
                       {
                           event.Skip();
                       }
                   });

        bool commandsFound = true;
        bool stayedAfterVeto = false;
        wizard.CallAfter([&]()
        {
            commandsFound = CommandDialogButton(&wizard, wxID_CANCEL);
            stayedAfterVeto = wizard.GetCurrentPage() == page &&
                              wizard.IsModal();
            commandsFound = CommandDialogButton(&wizard, wxID_CANCEL) &&
                            commandsFound;
            if ( wizard.IsModal() )
                wizard.EndModal(wxID_CANCEL);
        });

        CHECK_FALSE(wizard.RunWizard(page));
        CHECK(commandsFound);
        CHECK(stayedAfterVeto);
        CHECK(cancelEvents == 2);
    }

    SECTION("Destroyed_target_rolls_back_and_weak_links_expire")
    {
        wxWizard wizard(parent, wxID_ANY, "Destructive wizard");
        TrackingWizardPage* const first =
            new TrackingWizardPage(&wizard, "Surviving page");
        TrackingWizardPage* const target =
            new TrackingWizardPage(&wizard, "Destroyed target");
        first->SetNext(target);
        target->SetPrev(first);
        AddWizardPage(&wizard, first);
        AddWizardPage(&wizard, target);
        REQUIRE(wizard.ShowPage(first));

        const wxWeakRef<wxWindow> weakTarget(target);
        first->Bind(wxEVT_WIZARD_PAGE_CHANGING,
                    [target](wxWizardEvent& event)
                    {
                        target->Destroy();
                        event.Skip();
                    });

        CHECK_FALSE(wizard.ShowPage(target));
        CHECK(wizard.GetCurrentPage() == first);
        CHECK(first->IsShown());
        CHECK(first->GetNext() == nullptr);
        REQUIRE(WaitFor("destroyed wizard target cleanup",
                        [&weakTarget]()
                        {
                            return weakTarget.get() == nullptr;
                        }));
    }

    SECTION("Scheduled_wizard_rejects_navigation_before_application_callbacks")
    {
        wxWizard* const wizard =
            new wxWizard(parent, wxID_ANY, "Scheduled navigation wizard");
        const wxWeakRef<wxWindow> weakWizard(wizard);
        TrackingWizardPage* const first =
            new TrackingWizardPage(wizard, "Current page");
        TrackingWizardPage* const second =
            new TrackingWizardPage(wizard, "Rejected page");
        first->SetNext(second);
        second->SetPrev(first);
        AddWizardPage(wizard, first);
        AddWizardPage(wizard, second);
        REQUIRE(wizard->ShowPage(first));
        first->validateCalls = 0;
        first->transferFromCalls = 0;
        second->transferToCalls = 0;
        int changingEvents = 0;
        first->Bind(wxEVT_WIZARD_PAGE_CHANGING,
                    [&changingEvents](wxWizardEvent& event)
                    {
                        ++changingEvents;
                        event.Skip();
                    });

#if defined(__WXWINUI__) && wxUSE_WINUI3
        {
            wxWinUITLWHostWindowEventGuard retainedCallback(wizard);
            wizard->Destroy();
            CHECK(wxWinUITLWHostIsDestroyScheduled(wizard));
            CHECK_FALSE(wizard->ShowPage(second));
            SendButtonCommand(wizard, wxID_FORWARD);
            CHECK(weakWizard.get() == wizard);
            CHECK(first->validateCalls == 0);
            CHECK(first->transferFromCalls == 0);
            CHECK(second->transferToCalls == 0);
            CHECK(changingEvents == 0);
        }
#else
        wizard->Destroy();
        CHECK_FALSE(wizard->ShowPage(second));
        SendButtonCommand(wizard, wxID_FORWARD);
        CHECK(first->validateCalls == 0);
        CHECK(first->transferFromCalls == 0);
        CHECK(second->transferToCalls == 0);
        CHECK(changingEvents == 0);
#endif

        REQUIRE(WaitFor("scheduled wizard navigation cleanup",
                        [&weakWizard]()
                        {
                            return weakWizard.get() == nullptr;
                        }));
    }

    SECTION("Nested_navigation_wins_over_outer_transition")
    {
        wxWizard wizard(parent, wxID_ANY, "Nested navigation wizard");
        TrackingWizardPage* const first =
            new TrackingWizardPage(&wizard, "First");
        TrackingWizardPage* const outerTarget =
            new TrackingWizardPage(&wizard, "Outer target");
        TrackingWizardPage* const nestedTarget =
            new TrackingWizardPage(&wizard, "Nested target");
        AddWizardPage(&wizard, first);
        AddWizardPage(&wizard, outerTarget);
        AddWizardPage(&wizard, nestedTarget);
        REQUIRE(wizard.ShowPage(first));

        bool insideNestedTransition = false;
        bool nestedTransitionResult = false;
        first->Bind(wxEVT_WIZARD_PAGE_CHANGING,
                    [&](wxWizardEvent& event)
                    {
                        if ( !insideNestedTransition )
                        {
                            insideNestedTransition = true;
                            nestedTransitionResult =
                                wizard.ShowPage(nestedTarget);
                        }
                        event.Skip();
                    });

        CHECK_FALSE(wizard.ShowPage(outerTarget));
        CHECK(nestedTransitionResult);
        CHECK(wizard.GetCurrentPage() == nestedTarget);
        CHECK(nestedTarget->IsShown());
        CHECK_FALSE(outerTarget->IsShown());
    }

    SECTION("Failed_nested_navigation_restores_the_outer_transaction_token")
    {
        wxWizard wizard(parent, wxID_ANY, "Failed nested navigation wizard");
        TrackingWizardPage* const first =
            new TrackingWizardPage(&wizard, "First");
        TrackingWizardPage* const second =
            new TrackingWizardPage(&wizard, "Second");
        TrackingWizardPage* const nestedTarget =
            new TrackingWizardPage(&wizard, "Nested veto target");
        AddWizardPage(&wizard, first);
        AddWizardPage(&wizard, second);
        AddWizardPage(&wizard, nestedTarget);
        REQUIRE(wizard.ShowPage(first));

        int nestedVetoes = 0;
        bool nestedResult = true;
        second->Bind(wxEVT_WIZARD_PAGE_CHANGING,
                     [&nestedVetoes](wxWizardEvent& event)
                     {
                         ++nestedVetoes;
                         event.Veto();
                     });
        second->onTransferTo = [&]()
        {
            nestedResult = wizard.ShowPage(nestedTarget);
        };

        CHECK(wizard.ShowPage(second));
        CHECK_FALSE(nestedResult);
        CHECK(nestedVetoes == 1);
        CHECK(wizard.GetCurrentPage() == second);
        CHECK(second->IsShown());
        CHECK_FALSE(first->IsShown());
    }

    SECTION("Accepted_cancel_stops_an_inflight_modal_page_transition")
    {
        wxWizard wizard(parent, wxID_ANY, "Cancel during transition wizard");
        TrackingWizardPage* const first =
            new TrackingWizardPage(&wizard, "First");
        TrackingWizardPage* const second =
            new TrackingWizardPage(&wizard, "Second");
        first->SetNext(second);
        second->SetPrev(first);
        AddWizardPage(&wizard, first);
        AddWizardPage(&wizard, second);

        int changedEvents = 0;
        int shownEvents = 0;
        bool cancelFound = false;
        second->Bind(wxEVT_WIZARD_PAGE_CHANGED,
                     [&](wxWizardEvent& event)
                     {
                         ++changedEvents;
                         cancelFound =
                             CommandDialogButton(&wizard, wxID_CANCEL);
                         event.Skip();
                     });
        second->Bind(wxEVT_WIZARD_PAGE_SHOWN,
                     [&shownEvents](wxWizardEvent& event)
                     {
                         ++shownEvents;
                         event.Skip();
                     });

        bool forwardFound = false;
        wizard.CallAfter([&]()
        {
            forwardFound = CommandDialogButton(&wizard, wxID_FORWARD);
            if ( wizard.IsModal() )
                wizard.EndModal(wxID_CANCEL);
        });

        CHECK_FALSE(wizard.RunWizard(first));
        CHECK(forwardFound);
        CHECK(cancelFound);
        CHECK(changedEvents == 1);
        CHECK(shownEvents == 0);
        CHECK(wizard.GetCurrentPage() == nullptr);
    }

    SECTION("Nested_modal_dialog_during_transition")
    {
        wxWizard wizard(parent, wxID_ANY, "Nested dialog wizard");
        TrackingWizardPage* const first =
            new TrackingWizardPage(&wizard, "First");
        TrackingWizardPage* const second =
            new TrackingWizardPage(&wizard, "Second");
        AddWizardPage(&wizard, first);
        AddWizardPage(&wizard, second);
        REQUIRE(wizard.ShowPage(first));

        int nestedResult = wxID_NONE;
        first->Bind(wxEVT_WIZARD_PAGE_CHANGING,
                    [&](wxWizardEvent& event)
                    {
                        wxDialog nested(&wizard,
                                        wxID_ANY,
                                        "Nested wizard dialog");
                        nested.CallAfter([&nested]()
                                         {
                                             nested.EndModal(wxID_OK);
                                         });
                        nestedResult = nested.ShowModal();
                        event.Skip();
                    });

        CHECK(wizard.ShowPage(second));
        CHECK(nestedResult == wxID_OK);
        CHECK(wizard.GetCurrentPage() == second);
    }

    SECTION("One_hundred_create_navigate_finish_cycles")
    {
        for ( int cycle = 0; cycle < 100; ++cycle )
        {
            CAPTURE(cycle);
            wxWizard wizard(parent, wxID_ANY, "Wizard lifecycle cycle");
            TrackingWizardPage* const page =
                new TrackingWizardPage(&wizard, "Only page");
            AddWizardPage(&wizard, page);
            bool finishFound = false;
            wizard.CallAfter([&]()
            {
                finishFound =
                    CommandDialogButton(&wizard, wxID_FORWARD);
                if ( wizard.IsModal() &&
                     wizard.GetReturnCode() != wxID_OK )
                    wizard.EndModal(wxID_CANCEL);
            });
            REQUIRE(wizard.RunWizard(page));
            CHECK(finishFound);
            CHECK(wizard.GetCurrentPage() == nullptr);
            CHECK(wizard.GetReturnCode() == wxID_OK);
        }
    }

    SECTION("Premodal_nested_finish_and_cancel_do_not_destroy_the_wizard")
    {
        {
            wxWizard wizard(parent, wxID_ANY, "Premodal finish wizard");
            TrackingWizardPage* const page =
                new TrackingWizardPage(&wizard, "Finish immediately");
            AddWizardPage(&wizard, page);
            int finishedEvents = 0;
            bool nestedFinish = false;
            page->Bind(wxEVT_WIZARD_PAGE_SHOWN,
                       [&](wxWizardEvent& event)
                       {
                           nestedFinish = wizard.ShowPage(nullptr);
                           event.Skip();
                       });
            wizard.Bind(wxEVT_WIZARD_FINISHED,
                        [&finishedEvents](wxWizardEvent& event)
                        {
                            ++finishedEvents;
                            event.Skip();
                        });

            CHECK(wizard.RunWizard(page));
            CHECK(nestedFinish);
            CHECK(finishedEvents == 1);
            CHECK(wizard.GetCurrentPage() == nullptr);
            CHECK_FALSE(wizard.IsBeingDeleted());
        }

        {
            wxWizard wizard(parent, wxID_ANY, "Premodal cancel wizard");
            TrackingWizardPage* const page =
                new TrackingWizardPage(&wizard, "Cancel immediately");
            TrackingWizardPage* const nestedTarget =
                new TrackingWizardPage(&wizard, "Must not reopen");
            AddWizardPage(&wizard, page);
            AddWizardPage(&wizard, nestedTarget);
            int cancelEvents = 0;
            bool cancelFound = false;
            bool terminalCleanupStarted = false;
            bool nestedShowAttempted = false;
            bool nestedShowResult = true;
            page->Bind(wxEVT_WIZARD_PAGE_SHOWN,
                       [&](wxWizardEvent& event)
                       {
                           terminalCleanupStarted = true;
                           cancelFound =
                               CommandDialogButton(&wizard, wxID_CANCEL);
                           event.Skip();
                       });
            page->Bind(wxEVT_SHOW,
                       [&](wxShowEvent& event)
                       {
                           if ( terminalCleanupStarted && !event.IsShown() )
                           {
                               nestedShowAttempted = true;
                               nestedShowResult =
                                   wizard.ShowPage(nestedTarget);
                           }
                           event.Skip();
                       });
            page->Bind(wxEVT_WIZARD_CANCEL,
                       [&cancelEvents](wxWizardEvent& event)
                       {
                           ++cancelEvents;
                           event.Skip();
                       });

            CHECK_FALSE(wizard.RunWizard(page));
            CHECK(cancelFound);
            CHECK(cancelEvents == 1);
            CHECK(nestedShowAttempted);
            CHECK_FALSE(nestedShowResult);
            CHECK(wizard.GetCurrentPage() == nullptr);
            CHECK_FALSE(wizard.IsBeingDeleted());
        }
    }

    SECTION("Modeless_finish_and_cancel_are_single_terminal_transactions")
    {
        {
            wxWizard* const wizard =
                new wxWizard(parent, wxID_ANY, "Modeless finish wizard");
            const wxWeakRef<wxWindow> weakWizard(wizard);
            TrackingWizardPage* const page =
                new TrackingWizardPage(wizard, "Finish modeless");
            TrackingWizardPage* const nestedTarget =
                new TrackingWizardPage(wizard, "Must not reopen");
            AddWizardPage(wizard, page);
            AddWizardPage(wizard, nestedTarget);
            REQUIRE(wizard->ShowPage(page));

            int finishedEvents = 0;
            bool nestedShowResult = true;
            page->Bind(wxEVT_SHOW,
                       [wizard, nestedTarget,
                        &nestedShowResult](wxShowEvent& event)
                       {
                           if ( !event.IsShown() )
                               nestedShowResult = wizard->ShowPage(nestedTarget);
                           event.Skip();
                       });
            wizard->Bind(wxEVT_WIZARD_FINISHED,
                         [&finishedEvents](wxWizardEvent& event)
                         {
                             ++finishedEvents;
                             event.Skip();
                         });

            CHECK(wizard->ShowPage(nullptr));
            CHECK_FALSE(nestedShowResult);
            CHECK(finishedEvents == 1);
            REQUIRE(WaitFor("modeless wizard finish cleanup",
                            [&weakWizard]()
                            {
                                return weakWizard.get() == nullptr;
                            }));
        }

        {
            wxWizard* const wizard =
                new wxWizard(parent, wxID_ANY, "Modeless cancel wizard");
            const wxWeakRef<wxWindow> weakWizard(wizard);
            TrackingWizardPage* const page =
                new TrackingWizardPage(wizard, "Cancel modeless");
            AddWizardPage(wizard, page);
            REQUIRE(wizard->ShowPage(page));

            int cancelEvents = 0;
            page->Bind(wxEVT_WIZARD_CANCEL,
                       [&cancelEvents](wxWizardEvent& event)
                       {
                           ++cancelEvents;
                           event.Skip();
                       });

            REQUIRE(CommandDialogButton(wizard, wxID_CANCEL));
            CHECK(cancelEvents == 1);
            REQUIRE(WaitFor("modeless wizard cancel cleanup",
                            [&weakWizard]()
                            {
                                return weakWizard.get() == nullptr;
                            }));
        }
    }

    SECTION("Destroying_modeless_owner_does_not_orphan_wizard")
    {
        wxFrame* const owner = new wxFrame(parent, wxID_ANY, "Wizard owner");
        const wxWeakRef<wxWindow> weakOwner(owner);
        wxWizard* const wizard =
            new wxWizard(owner, wxID_ANY, "Owned modeless wizard");
        const wxWeakRef<wxWindow> weakWizard(wizard);
        TrackingWizardPage* const page =
            new TrackingWizardPage(wizard, "Owned page");
        AddWizardPage(wizard, page);
        REQUIRE(wizard->ShowPage(page));

        owner->Destroy();
        REQUIRE(WaitFor("modeless wizard owner cleanup",
                        [&weakOwner, &weakWizard]()
                        {
                            return weakOwner.get() == nullptr &&
                                   weakWizard.get() == nullptr;
                        }));
    }

    SECTION("Current_page_tracking_does_not_bind_application_pages")
    {
        wxWizard wizard(parent, wxID_ANY, "Non-binding page tracking");
        int dynamicBindCalls = 0;
        DestructiveDynamicBindWizardPage* const page =
            new DestructiveDynamicBindWizardPage(&wizard, &dynamicBindCalls);
        const wxWeakRef<wxWindow> weakPage(page);
        AddWizardPage(&wizard, page);

        CHECK(wizard.ShowPage(page));
        CHECK(dynamicBindCalls == 0);
        CHECK(weakPage.get() == page);
        CHECK(wizard.GetCurrentPage() == page);
    }

    SECTION("Protected_raw_page_members_remain_source_compatible")
    {
        ProtectedStateWizard wizard(parent, "Protected state wizard");
        TrackingWizardPage* const first =
            new TrackingWizardPage(&wizard, "Direct current");
        TrackingWizardPage* const second =
            new TrackingWizardPage(&wizard, "Direct first");
        first->SetNext(second);
        second->SetPrev(first);
        AddWizardPage(&wizard, first);
        AddWizardPage(&wizard, second);

        wizard.PublishCurrentPageDirectly(first);
        wizard.PublishFirstPageDirectly(second);
        CHECK(wizard.GetCurrentPage() == first);
        CHECK(wizard.IsRunning());
        REQUIRE(wizard.ShowPage(second));
        wxButton* const back = wxDynamicCast(
            wizard.FindWindow(wxID_BACKWARD), wxButton);
        REQUIRE(back);
        CHECK_FALSE(back->IsEnabled());

        wizard.PublishCurrentPageDirectly(nullptr);
        CHECK(wizard.GetCurrentPage() == nullptr);
        CHECK_FALSE(wizard.IsRunning());
    }

    SECTION("Forced_layout_adaptation_preserves_page_topology_and_navigation")
    {
        wxWizard wizard(parent, wxID_ANY, "Adapted wizard");
        wizard.SetLayoutAdaptationMode(wxDIALOG_ADAPTATION_MODE_ENABLED);
        wizard.SetLayoutAdaptationLevel(
            wxDIALOG_ADAPTATION_STANDARD_SIZER);

        TrackingWizardPage* const first =
            new TrackingWizardPage(&wizard, "Adapted first");
        TrackingWizardPage* const second =
            new TrackingWizardPage(&wizard, "Adapted second");
        first->SetNext(second);
        second->SetPrev(first);
        AddWizardPage(&wizard, first);
        AddWizardPage(&wizard, second);

        wxSizer* const firstOriginalSizer = first->GetSizer();
        wxSizer* const secondOriginalSizer = second->GetSizer();
        wxStaticText* const firstControl =
            FindDescendantOfType<wxStaticText>(first);
        wxStaticText* const secondControl =
            FindDescendantOfType<wxStaticText>(second);
        REQUIRE(firstOriginalSizer);
        REQUIRE(secondOriginalSizer);
        REQUIRE(firstControl);
        REQUIRE(secondControl);

        // ENABLED still lets the standard adapter decide that the current
        // display has enough room. Exercise the adaptation transaction itself
        // deterministically instead of depending on workstation geometry.
        REQUIRE(wizard.DoLayoutAdaptation());

        bool reachedSecond = false;
        bool finished = false;
        wizard.CallAfter([&]()
        {
            reachedSecond = CommandDialogButton(&wizard, wxID_FORWARD) &&
                            wizard.GetCurrentPage() == second;
            finished = CommandDialogButton(&wizard, wxID_FORWARD);
            if ( wizard.IsModal() &&
                 wizard.GetReturnCode() != wxID_OK )
                wizard.EndModal(wxID_CANCEL);
        });

        CHECK(wizard.RunWizard(first));
        CHECK(reachedSecond);
        CHECK(finished);
        CHECK(wizard.GetLayoutAdaptationDone());

        wxScrolledWindow* const firstScrolled =
            FindDescendantOfType<wxScrolledWindow>(first);
        wxScrolledWindow* const secondScrolled =
            FindDescendantOfType<wxScrolledWindow>(second);
        REQUIRE(firstScrolled);
        REQUIRE(secondScrolled);
        CHECK(first->GetSizer() != firstOriginalSizer);
        CHECK(second->GetSizer() != secondOriginalSizer);
        CHECK(firstScrolled->GetParent() == first);
        CHECK(secondScrolled->GetParent() == second);
        CHECK(firstScrolled->GetSizer() == firstOriginalSizer);
        CHECK(secondScrolled->GetSizer() == secondOriginalSizer);
        CHECK(firstControl->GetParent() == firstScrolled);
        CHECK(secondControl->GetParent() == secondScrolled);
    }

    SECTION("Rejected_reparent_rolls_layout_adaptation_back")
    {
        wxWizard wizard(parent, wxID_ANY, "Rejected adaptation wizard");
        wizard.SetLayoutAdaptationMode(wxDIALOG_ADAPTATION_MODE_ENABLED);

        wxWizardPageSimple* const page = new wxWizardPageSimple(&wizard);
        wxBoxSizer* const originalSizer = new wxBoxSizer(wxVERTICAL);
        int reparentCalls = 0;
        RejectWizardReparentPanel* const rejectingControl =
            new RejectWizardReparentPanel(page, &reparentCalls);
        originalSizer->Add(rejectingControl, wxSizerFlags(1).Expand());
        page->SetSizer(originalSizer);
        AddWizardPage(&wizard, page);

        CHECK_FALSE(wizard.DoLayoutAdaptation());
        CHECK(reparentCalls == 1);
        CHECK_FALSE(wizard.GetLayoutAdaptationDone());
        CHECK(page->GetSizer() == originalSizer);
        CHECK(rejectingControl->GetParent() == page);
    }

    SECTION("Replacing_the_original_sizer_during_reparent_is_safe")
    {
        wxWizard wizard(parent, wxID_ANY, "Replaced sizer wizard");
        wizard.SetLayoutAdaptationMode(wxDIALOG_ADAPTATION_MODE_ENABLED);

        wxWizardPageSimple* const page = new wxWizardPageSimple(&wizard);
        wxBoxSizer* const originalSizer = new wxBoxSizer(wxVERTICAL);
        wxSizer* replacementSizer = nullptr;
        int reparentCalls = 0;
        ReplaceWizardSizerOnReparentPanel* const control =
            new ReplaceWizardSizerOnReparentPanel(page,
                                                   &replacementSizer,
                                                   &reparentCalls);
        const wxWeakRef<wxWindow> weakControl(control);
        originalSizer->Add(control, wxSizerFlags(1).Expand());
        page->SetSizer(originalSizer);
        AddWizardPage(&wizard, page);

        CHECK_FALSE(wizard.DoLayoutAdaptation());
        REQUIRE(replacementSizer);
        CHECK(page->GetSizer() == replacementSizer);
        CHECK(reparentCalls == 2);
        CHECK(weakControl.get() == control);
        CHECK_FALSE(control->IsBeingDeleted());
        CHECK(control->GetParent() == page);
    }

    SECTION("Navigation_from_GetNext_rolls_committed_adaptation_back")
    {
        wxWizard wizard(parent, wxID_ANY, "GetNext rollback wizard");
        wizard.SetLayoutAdaptationMode(wxDIALOG_ADAPTATION_MODE_ENABLED);

        NavigateOnGetNextWizardPage* const first =
            new NavigateOnGetNextWizardPage(&wizard);
        wxWizardPageSimple* const second = new wxWizardPageSimple(&wizard);
        first->SetNavigationTarget(second);
        second->SetPrev(first);
        wxBoxSizer* const originalSizer = new wxBoxSizer(wxVERTICAL);
        wxPanel* const control = new wxPanel(first);
        originalSizer->Add(control, wxSizerFlags(1).Expand());
        first->SetSizer(originalSizer);
        second->SetSizer(new wxBoxSizer(wxVERTICAL));
        AddWizardPage(&wizard, first);
        AddWizardPage(&wizard, second);

        CHECK_FALSE(wizard.DoLayoutAdaptation());
        CHECK_FALSE(wizard.GetLayoutAdaptationDone());
        CHECK(first->GetSizer() == originalSizer);
        CHECK(control->GetParent() == first);
        CHECK(FindDescendantOfType<wxScrolledWindow>(first) == nullptr);
    }

    SECTION("Navigation_during_rollback_does_not_interrupt_cleanup")
    {
        wxWizard wizard(parent, wxID_ANY, "Rollback navigation wizard");
        wizard.SetLayoutAdaptationMode(wxDIALOG_ADAPTATION_MODE_ENABLED);

        wxWizardPageSimple* const first = new wxWizardPageSimple(&wizard);
        wxWizardPageSimple* const second = new wxWizardPageSimple(&wizard);
        first->SetNext(second);
        second->SetPrev(first);

        wxBoxSizer* const firstOriginalSizer = new wxBoxSizer(wxVERTICAL);
        int firstReparentCalls = 0;
        NavigateDuringRollbackPanel* const firstControl =
            new NavigateDuringRollbackPanel(first, &wizard, second,
                                             &firstReparentCalls);
        firstOriginalSizer->Add(firstControl, wxSizerFlags(1).Expand());
        first->SetSizer(firstOriginalSizer);

        wxBoxSizer* const secondOriginalSizer = new wxBoxSizer(wxVERTICAL);
        int rejectedReparentCalls = 0;
        RejectWizardReparentPanel* const rejectingControl =
            new RejectWizardReparentPanel(second, &rejectedReparentCalls);
        secondOriginalSizer->Add(rejectingControl, wxSizerFlags(1).Expand());
        second->SetSizer(secondOriginalSizer);
        AddWizardPage(&wizard, first);
        AddWizardPage(&wizard, second);

        CHECK_FALSE(wizard.DoLayoutAdaptation());
        CHECK_FALSE(wizard.GetLayoutAdaptationDone());
        CHECK(firstReparentCalls == 2);
        CHECK(rejectedReparentCalls == 1);
        CHECK(first->GetSizer() == firstOriginalSizer);
        CHECK(firstControl->GetParent() == first);
        CHECK(FindDescendantOfType<wxScrolledWindow>(first) == nullptr);
    }

    SECTION("Destroyed_GetNext_result_is_never_weakly_bound")
    {
        SECTION("FitToPage")
        {
            wxWizard wizard(parent, wxID_ANY, "Fit stale next wizard");
            DeleteTargetOnGetNextWizardPage* const first =
                new DeleteTargetOnGetNextWizardPage(&wizard);
            wxWizardPageSimple* const doomed = new wxWizardPageSimple(&wizard);
            const wxWeakRef<wxWindow> weakDoomed(doomed);
            first->SetSizer(new wxBoxSizer(wxVERTICAL));
            doomed->SetSizer(new wxBoxSizer(wxVERTICAL));
            first->SetDeletionTarget(doomed);
            AddWizardPage(&wizard, first);
            AddWizardPage(&wizard, doomed);

            wizard.FitToPage(first);
            CHECK_FALSE(weakDoomed);
            CHECK(first->GetParent() == &wizard);
        }

        SECTION("LayoutAdaptation")
        {
            wxWizard wizard(parent, wxID_ANY, "Layout stale next wizard");
            wizard.SetLayoutAdaptationMode(wxDIALOG_ADAPTATION_MODE_ENABLED);
            DeleteTargetOnGetNextWizardPage* const first =
                new DeleteTargetOnGetNextWizardPage(&wizard);
            wxWizardPageSimple* const doomed = new wxWizardPageSimple(&wizard);
            const wxWeakRef<wxWindow> weakDoomed(doomed);
            wxBoxSizer* const originalSizer = new wxBoxSizer(wxVERTICAL);
            wxPanel* const control = new wxPanel(first);
            originalSizer->Add(control, wxSizerFlags(1).Expand());
            first->SetSizer(originalSizer);
            doomed->SetSizer(new wxBoxSizer(wxVERTICAL));
            first->SetDeletionTarget(doomed);
            AddWizardPage(&wizard, first);
            AddWizardPage(&wizard, doomed);

            CHECK_FALSE(wizard.DoLayoutAdaptation());
            CHECK_FALSE(weakDoomed);
            CHECK(first->GetSizer() == originalSizer);
            CHECK(control->GetParent() == first);
            CHECK(FindDescendantOfType<wxScrolledWindow>(first) == nullptr);
        }
    }

    SECTION("Refused_inverse_reparent_preserves_coherent_adapted_topology")
    {
        wxWizard wizard(parent, wxID_ANY, "Refused rollback wizard");
        wizard.SetLayoutAdaptationMode(wxDIALOG_ADAPTATION_MODE_ENABLED);

        wxWizardPageSimple* const first = new wxWizardPageSimple(&wizard);
        wxWizardPageSimple* const second = new wxWizardPageSimple(&wizard);
        first->SetNext(second);
        second->SetPrev(first);

        wxBoxSizer* const firstOriginalSizer = new wxBoxSizer(wxVERTICAL);
        int rollbackControlCalls = 0;
        RefuseWizardRollbackPanel* const rollbackControl =
            new RefuseWizardRollbackPanel(first, &rollbackControlCalls);
        firstOriginalSizer->Add(rollbackControl, wxSizerFlags(1).Expand());
        first->SetSizer(firstOriginalSizer);

        wxBoxSizer* const secondOriginalSizer = new wxBoxSizer(wxVERTICAL);
        int rejectedForwardCalls = 0;
        RejectWizardReparentPanel* const rejectingControl =
            new RejectWizardReparentPanel(second, &rejectedForwardCalls);
        secondOriginalSizer->Add(rejectingControl, wxSizerFlags(1).Expand());
        second->SetSizer(secondOriginalSizer);
        AddWizardPage(&wizard, first);
        AddWizardPage(&wizard, second);

        CHECK_FALSE(wizard.DoLayoutAdaptation());
        CHECK_FALSE(wizard.GetLayoutAdaptationDone());
        CHECK(rollbackControlCalls == 2);
        CHECK(rejectedForwardCalls == 1);

        wxScrolledWindow* const scroller =
            FindDescendantOfType<wxScrolledWindow>(first);
        REQUIRE(scroller);
        CHECK(first->GetSizer() != firstOriginalSizer);
        CHECK(scroller->GetSizer() == firstOriginalSizer);
        CHECK(rollbackControl->GetParent() == scroller);
    }

    SECTION("Refused_local_rollback_preserves_coherent_adapted_topology")
    {
        wxWizard wizard(parent, wxID_ANY, "Refused local rollback wizard");
        wizard.SetLayoutAdaptationMode(wxDIALOG_ADAPTATION_MODE_ENABLED);

        wxWizardPageSimple* const first = new wxWizardPageSimple(&wizard);
        wxWizardPageSimple* const second = new wxWizardPageSimple(&wizard);
        first->SetNext(second);
        second->SetPrev(first);

        wxBoxSizer* const originalSizer = new wxBoxSizer(wxVERTICAL);
        int reparentCalls = 0;
        NavigateAndRefuseWizardRollbackPanel* const control =
            new NavigateAndRefuseWizardRollbackPanel(first, &wizard, second,
                                                      &reparentCalls);
        originalSizer->Add(control, wxSizerFlags(1).Expand());
        first->SetSizer(originalSizer);
        second->SetSizer(new wxBoxSizer(wxVERTICAL));
        AddWizardPage(&wizard, first);
        AddWizardPage(&wizard, second);

        CHECK_FALSE(wizard.DoLayoutAdaptation());
        CHECK_FALSE(wizard.GetLayoutAdaptationDone());
        CHECK(reparentCalls == 2);

        wxScrolledWindow* const scroller =
            FindDescendantOfType<wxScrolledWindow>(first);
        REQUIRE(scroller);
        CHECK(first->GetSizer() != originalSizer);
        CHECK(scroller->GetSizer() == originalSizer);
        CHECK(control->GetParent() == scroller);
    }

    SECTION("Destroy_requested_during_layout_adaptation_aborts_transaction")
    {
        ForcedLayoutAdaptationWizard* const wizard =
            new ForcedLayoutAdaptationWizard(
                parent, wxID_ANY, "Destructive adaptation wizard");
        const wxWeakRef<wxWindow> weakWizard(wizard);
        wizard->SetLayoutAdaptationMode(wxDIALOG_ADAPTATION_MODE_ENABLED);

        wxWizardPageSimple* const page = new wxWizardPageSimple(wizard);
        wxBoxSizer* const pageSizer = new wxBoxSizer(wxVERTICAL);
        int reparentCalls = 0;
        pageSizer->Add(new DestroyWizardOnReparentPanel(page,
                                                        wizard,
                                                        &reparentCalls),
                       wxSizerFlags(1).Expand());
        pageSizer->Add(new wxPanel(page), wxSizerFlags(1).Expand());
        page->SetSizer(pageSizer);
        AddWizardPage(wizard, page);

#if defined(__WXWINUI__) && wxUSE_WINUI3
        {
            wxWinUITLWHostWindowEventGuard retainedCallback(wizard);
            CHECK_FALSE(wizard->RunWizard(page));
            // The first call moves the control into the provisional scroller;
            // scheduling destruction invalidates the transaction, whose
            // bounded rollback performs the second call back to the page.
            CHECK(reparentCalls == 2);
            CHECK(wxWinUITLWHostIsDestroyScheduled(wizard));
            CHECK(weakWizard.get() == wizard);
        }
#else
        CHECK_FALSE(wizard->RunWizard(page));
        CHECK(reparentCalls == 2);
#endif

        REQUIRE(WaitFor("wizard destroyed during adaptation cleanup",
                        [&weakWizard]()
                        {
                            return weakWizard.get() == nullptr;
                        }));
    }

#if wxUSE_XRC
    SECTION("XRC_Create_chains_pages_and_runs_to_completion")
    {
        static const char xrcText[] = R"XRC(
<resource xmlns="http://www.wxwidgets.org/wxxrc" version="2.5.3.0">
  <object class="wxWizard" name="phase013Wizard">
    <title>XRC wizard</title>
    <border>7</border>
    <object class="wxWizardPageSimple" name="phase013WizardFirst">
      <object class="wxStaticText" name="phase013WizardFirstLabel">
        <label>First XRC page</label>
      </object>
    </object>
    <object class="wxWizardPageSimple" name="phase013WizardSecond">
      <object class="wxStaticText" name="phase013WizardSecondLabel">
        <label>Second XRC page</label>
      </object>
    </object>
  </object>
</resource>
)XRC";
        static const wxString xrcName = "phase013-wizard-contract.xrc";
        wxXmlResource* const xrc = wxXmlResource::Get();
        xrc->InitAllHandlers();
        wxStringInputStream stream(wxString::FromUTF8(xrcText));
        std::unique_ptr<wxXmlDocument> document(new wxXmlDocument(stream));
        REQUIRE(document->IsOk());
        REQUIRE(xrc->LoadDocument(document.release(), xrcName));
        XrcUnloadGuard unload(xrc, xrcName);

        wxWizard* const wizard = wxDynamicCast(
            xrc->LoadObject(parent,
                            "phase013Wizard",
                            "wxWizard"),
            wxWizard);
        REQUIRE(wizard);
        std::unique_ptr<wxWizard> ownedWizard(wizard);
        wxWizardPageSimple* const first = wxDynamicCast(
            wizard->FindWindow(XRCID("phase013WizardFirst")),
            wxWizardPageSimple);
        wxWizardPageSimple* const second = wxDynamicCast(
            wizard->FindWindow(XRCID("phase013WizardSecond")),
            wxWizardPageSimple);
        REQUIRE(first);
        REQUIRE(second);
        CHECK(first->GetNext() == second);
        CHECK(second->GetPrev() == first);
        CHECK(wizard->FindWindow(XRCID("phase013WizardFirstLabel")));
        CHECK(wizard->FindWindow(XRCID("phase013WizardSecondLabel")));

        bool forwardFound = false;
        bool finishFound = false;
        wizard->CallAfter([&]()
        {
            forwardFound = CommandDialogButton(wizard, wxID_FORWARD);
            finishFound = CommandDialogButton(wizard, wxID_FORWARD);
            if ( wizard->IsModal() &&
                 wizard->GetReturnCode() != wxID_OK )
                wizard->EndModal(wxID_CANCEL);
        });
        CHECK(wizard->RunWizard(first));
        CHECK(forwardFound);
        CHECK(finishFound);
        CHECK(wizard->GetCurrentPage() == nullptr);
    }

    SECTION("Nested_XRC_wizard_restores_outer_handler_context")
    {
        static const char xrcText[] = R"XRC(
<resource xmlns="http://www.wxwidgets.org/wxxrc" version="2.5.3.0">
  <object class="wxWizard" name="phase013OuterWizard">
    <object class="wxWizardPageSimple"
            subclass="NestedLoadingWizardPage"
            name="phase013OuterFirst">
      <bg>#102030</bg>
      <fg>#405060</fg>
    </object>
    <object class="wxWizardPageSimple" name="phase013OuterSecond"/>
  </object>
  <object class="wxWizard" name="phase013NestedWizard">
    <object class="wxWizardPageSimple" name="phase013NestedFirst"/>
    <object class="wxWizardPageSimple" name="phase013NestedSecond"/>
  </object>
</resource>
)XRC";
        static const wxString xrcName = "phase013-nested-wizard.xrc";
        wxXmlResource* const xrc = wxXmlResource::Get();
        xrc->InitAllHandlers();
        wxStringInputStream stream(wxString::FromUTF8(xrcText));
        std::unique_ptr<wxXmlDocument> document(new wxXmlDocument(stream));
        REQUIRE(document->IsOk());
        REQUIRE(xrc->LoadDocument(document.release(), xrcName));
        XrcUnloadGuard unload(xrc, xrcName);

        NestedLoadingWizardPage::Reset(parent);
        std::unique_ptr<wxWizard> outer(wxDynamicCast(
            xrc->LoadObject(parent, "phase013OuterWizard", "wxWizard"),
            wxWizard));
        REQUIRE(outer);
        std::unique_ptr<wxWizard> nested(
            NestedLoadingWizardPage::TakeNestedWizard());
        REQUIRE(nested);

        wxWizardPageSimple* const outerFirst = wxDynamicCast(
            outer->FindWindow(XRCID("phase013OuterFirst")),
            wxWizardPageSimple);
        wxWizardPageSimple* const outerSecond = wxDynamicCast(
            outer->FindWindow(XRCID("phase013OuterSecond")),
            wxWizardPageSimple);
        wxWizardPageSimple* const nestedFirst = wxDynamicCast(
            nested->FindWindow(XRCID("phase013NestedFirst")),
            wxWizardPageSimple);
        wxWizardPageSimple* const nestedSecond = wxDynamicCast(
            nested->FindWindow(XRCID("phase013NestedSecond")),
            wxWizardPageSimple);
        REQUIRE(outerFirst);
        REQUIRE(outerSecond);
        REQUIRE(nestedFirst);
        REQUIRE(nestedSecond);
        CHECK(outerFirst->GetNext() == outerSecond);
        CHECK(outerSecond->GetPrev() == outerFirst);
        CHECK(nestedFirst->GetNext() == nestedSecond);
        CHECK(nestedSecond->GetPrev() == nestedFirst);
        CHECK(NestedLoadingWizardPage::GetForegroundCalls() == 1);
    }

    SECTION("XRC_subclass_factory_ownership_is_transactional")
    {
        static const char xrcText[] = R"XRC(
<resource xmlns="http://www.wxwidgets.org/wxxrc" version="2.5.3.0">
  <object class="wxWizard"
          subclass="XrcOwnedWizard"
          name="phase013OwnedWizardFailure">
    <title>Owned wizard failure</title>
    <object class="wxPanel" name="phase013InvalidDirectWizardChild"/>
  </object>
</resource>
)XRC";
        static const wxString xrcName =
            "phase013-owned-wizard-failure.xrc";
        wxXmlResource* const xrc = wxXmlResource::Get();
        xrc->InitAllHandlers();
        wxStringInputStream stream(wxString::FromUTF8(xrcText));
        std::unique_ptr<wxXmlDocument> document(new wxXmlDocument(stream));
        REQUIRE(document->IsOk());
        REQUIRE(xrc->LoadDocument(document.release(), xrcName));
        XrcUnloadGuard unload(xrc, xrcName);

        XrcOwnedWizard::ResetCounters();
        {
            XrcOwnedWizard callerOwned;
            const wxWeakRef<wxWindow> weakCaller(&callerOwned);
            wxLogNull suppressExpectedCreateError;
            CHECK_FALSE(xrc->LoadObject(&callerOwned,
                                        parent,
                                        "phase013OwnedWizardFailure",
                                        "wxWizard"));
            CHECK(weakCaller.get() == &callerOwned);
            CHECK(XrcOwnedWizard::GetConstructedCount() == 1);
            CHECK(XrcOwnedWizard::GetDestroyedCount() == 0);
        }
        CHECK(XrcOwnedWizard::GetDestroyedCount() == 1);

        XrcOwnedWizard::ResetCounters();
        {
            wxLogNull suppressExpectedCreateError;
            CHECK_FALSE(xrc->LoadObject(parent,
                                        "phase013OwnedWizardFailure",
                                        "wxWizard"));
        }
        CHECK(XrcOwnedWizard::GetConstructedCount() == 1);
        CHECK(XrcOwnedWizard::GetDestroyedCount() == 1);
    }

    SECTION("XRC_factory_owned_wrong_types_are_deleted")
    {
        static const char xrcText[] = R"XRC(
<resource xmlns="http://www.wxwidgets.org/wxxrc" version="2.5.3.0">
  <object class="wxWizard"
          subclass="WrongXrcWizardObject"
          name="phase013WrongWizardSubclass"/>
  <object class="wxWizard" name="phase013WrongPageRoot">
    <object class="wxWizardPage"
            subclass="WrongXrcWizardObject"
            name="phase013WrongPageSubclass"/>
  </object>
</resource>
)XRC";
        static const wxString xrcName =
            "phase013-wrong-wizard-subclasses.xrc";
        wxXmlResource* const xrc = wxXmlResource::Get();
        xrc->InitAllHandlers();
        wxStringInputStream stream(wxString::FromUTF8(xrcText));
        std::unique_ptr<wxXmlDocument> document(new wxXmlDocument(stream));
        REQUIRE(document->IsOk());
        REQUIRE(xrc->LoadDocument(document.release(), xrcName));
        XrcUnloadGuard unload(xrc, xrcName);

        WrongXrcWizardObject::ResetCounters();
        {
            wxLogNull suppressExpectedCreateError;
            CHECK_FALSE(xrc->LoadObject(parent,
                                        "phase013WrongWizardSubclass",
                                        "wxWizard"));
        }
        CHECK(WrongXrcWizardObject::GetConstructedCount() == 1);
        CHECK(WrongXrcWizardObject::GetDestroyedCount() == 1);

        WrongXrcWizardObject::ResetCounters();
        wxWizard callerOwned;
        {
            wxLogNull suppressExpectedCreateError;
            CHECK_FALSE(xrc->LoadObject(&callerOwned,
                                        parent,
                                        "phase013WrongPageRoot",
                                        "wxWizard"));
        }
        CHECK(WrongXrcWizardObject::GetConstructedCount() == 1);
        CHECK(WrongXrcWizardObject::GetDestroyedCount() == 1);
        CHECK_FALSE(callerOwned.IsBeingDeleted());
    }

    SECTION("XRC_factory_owned_abstract_page_is_deleted_after_reparent")
    {
        static const char xrcText[] = R"XRC(
<resource xmlns="http://www.wxwidgets.org/wxxrc" version="2.5.3.0">
  <object class="wxWizard" name="phase013ReparentedPageRoot">
    <object class="wxWizardPage"
            subclass="XrcOwnedWizardPage"
            name="phase013ReparentedOwnedPage">
      <bg>#102030</bg>
    </object>
    <object class="wxWizardPageSimple"
            name="phase013UnexpectedPageAfterReparent"/>
  </object>
</resource>
)XRC";
        static const wxString xrcName =
            "phase013-reparented-owned-wizard-page.xrc";
        wxXmlResource* const xrc = wxXmlResource::Get();
        xrc->InitAllHandlers();
        wxStringInputStream stream(wxString::FromUTF8(xrcText));
        std::unique_ptr<wxXmlDocument> document(new wxXmlDocument(stream));
        REQUIRE(document->IsOk());
        REQUIRE(xrc->LoadDocument(document.release(), xrcName));
        XrcUnloadGuard unload(xrc, xrcName);

        wxPanel foreignParent(parent);
        wxWizard callerOwned;
        const wxWeakRef<wxWindow> weakWizard(&callerOwned);
        XrcOwnedWizardPage::Reset(&foreignParent);
        bool result;
        {
            wxLogNull suppressExpectedCreateError;
            result = xrc->LoadObject(&callerOwned,
                                     parent,
                                     "phase013ReparentedPageRoot",
                                     "wxWizard");
        }
        XrcOwnedWizardPage::ClearForeignParent();

        CHECK_FALSE(result);
        CHECK(weakWizard.get() == &callerOwned);
        CHECK(XrcOwnedWizardPage::GetConstructedCount() == 1);
        CHECK(XrcOwnedWizardPage::GetDestroyedCount() == 1);
        CHECK(foreignParent.GetChildren().empty());
        CHECK(callerOwned.FindWindow(
                  XRCID("phase013UnexpectedPageAfterReparent")) == nullptr);
    }

    SECTION("XRC_caller_owned_wizard_may_delete_itself_in_Create")
    {
        static const char xrcText[] = R"XRC(
<resource xmlns="http://www.wxwidgets.org/wxxrc" version="2.5.3.0">
  <object class="wxWizard" name="phase013SelfDeletingWizard">
    <title>Self-deleting XRC wizard</title>
  </object>
</resource>
)XRC";
        static const wxString xrcName =
            "phase013-self-deleting-wizard.xrc";
        wxXmlResource* const xrc = wxXmlResource::Get();
        xrc->InitAllHandlers();
        wxStringInputStream stream(wxString::FromUTF8(xrcText));
        std::unique_ptr<wxXmlDocument> document(new wxXmlDocument(stream));
        REQUIRE(document->IsOk());
        REQUIRE(xrc->LoadDocument(document.release(), xrcName));
        XrcUnloadGuard unload(xrc, xrcName);

        bool destroyed = false;
        SelfDeletingWizard* const wizard =
            new SelfDeletingWizard(&destroyed);
        const wxWeakRef<wxWindow> weakWizard(wizard);
        {
            wxLogNull suppressExpectedCreateError;
            CHECK_FALSE(xrc->LoadObject(wizard,
                                        parent,
                                        "phase013SelfDeletingWizard",
                                        "wxWizard"));
        }
        CHECK(destroyed);
        CHECK(weakWizard.get() == nullptr);
    }

    SECTION("XRC_page_destroyed_in_SetupWindow_aborts_page_chain")
    {
        static const char xrcText[] = R"XRC(
<resource xmlns="http://www.wxwidgets.org/wxxrc" version="2.5.3.0">
  <object class="wxWizard" name="phase013DestructivePageWizard">
    <title>Destructive XRC page wizard</title>
    <object class="wxWizardPageSimple"
            subclass="SelfDeletingWizardPage"
            name="phase013SelfDeletingPage">
      <bg>#010203</bg>
      <fg>#040506</fg>
      <enabled>0</enabled>
    </object>
    <object class="wxWizardPageSimple" name="phase013UnexpectedSecondPage"/>
  </object>
</resource>
)XRC";
        static const wxString xrcName =
            "phase013-self-deleting-wizard-page.xrc";
        wxXmlResource* const xrc = wxXmlResource::Get();
        xrc->InitAllHandlers();
        wxStringInputStream stream(wxString::FromUTF8(xrcText));
        std::unique_ptr<wxXmlDocument> document(new wxXmlDocument(stream));
        REQUIRE(document->IsOk());
        REQUIRE(xrc->LoadDocument(document.release(), xrcName));
        XrcUnloadGuard unload(xrc, xrcName);

        SelfDeletingWizardPage::ResetCounters();
        wxWizard callerOwned;
        const wxWeakRef<wxWindow> weakWizard(&callerOwned);
        {
            wxLogNull suppressExpectedCreateError;
            CHECK_FALSE(xrc->LoadObject(&callerOwned,
                                        parent,
                                        "phase013DestructivePageWizard",
                                        "wxWizard"));
        }
        CHECK(weakWizard.get() == &callerOwned);
        CHECK_FALSE(callerOwned.IsBeingDeleted());
        CHECK(SelfDeletingWizardPage::GetConstructedCount() == 1);
        CHECK(SelfDeletingWizardPage::GetDestroyedCount() == 1);
        CHECK(callerOwned.FindWindow(
                  XRCID("phase013UnexpectedSecondPage")) == nullptr);
    }

    SECTION("XRC_stale_created_child_address_is_not_cast")
    {
        static const char xrcText[] = R"XRC(
<resource xmlns="http://www.wxwidgets.org/wxxrc" version="2.5.3.0">
  <object class="wxWizard" name="phase013StaleChildWizard">
    <title>Stale child XRC wizard</title>
    <object class="phase013DeletedWizardChild"/>
  </object>
</resource>
)XRC";
        static const wxString xrcName =
            "phase013-stale-created-wizard-child.xrc";
        wxXmlResource* const xrc = wxXmlResource::Get();
        xrc->InitAllHandlers();
        static bool handlerAdded = false;
        if ( !handlerAdded )
        {
            xrc->AddHandler(new DeletedWizardChildXmlHandler);
            handlerAdded = true;
        }

        wxStringInputStream stream(wxString::FromUTF8(xrcText));
        std::unique_ptr<wxXmlDocument> document(new wxXmlDocument(stream));
        REQUIRE(document->IsOk());
        REQUIRE(xrc->LoadDocument(document.release(), xrcName));
        XrcUnloadGuard unload(xrc, xrcName);

        wxWizard callerOwned;
        const wxWeakRef<wxWindow> weakWizard(&callerOwned);
        {
            wxLogNull suppressExpectedCreateError;
            CHECK_FALSE(xrc->LoadObject(&callerOwned,
                                        parent,
                                        "phase013StaleChildWizard",
                                        "wxWizard"));
        }
        CHECK(weakWizard.get() == &callerOwned);
        CHECK(FindDescendantOfType<wxWizardPage>(&callerOwned) == nullptr);
    }

    SECTION("XRC_owner_scheduled_in_SetupWindow_stops_remaining_properties")
    {
        static const char xrcText[] = R"XRC(
<resource xmlns="http://www.wxwidgets.org/wxxrc" version="2.5.3.0">
  <object class="wxWizard" name="phase013ScheduledOwnerWizard">
    <title>Scheduled owner XRC wizard</title>
    <object class="wxWizardPageSimple"
            subclass="DestroyingOwnerWizardPage"
            name="phase013DestroyingOwnerPage">
      <bg>#111213</bg>
      <fg>#212223</fg>
      <enabled>0</enabled>
    </object>
    <object class="wxWizardPageSimple"
            name="phase013UnexpectedPageAfterOwnerDestroy"/>
  </object>
</resource>
)XRC";
        static const wxString xrcName =
            "phase013-scheduled-owner-wizard-page.xrc";
        wxXmlResource* const xrc = wxXmlResource::Get();
        xrc->InitAllHandlers();
        wxStringInputStream stream(wxString::FromUTF8(xrcText));
        std::unique_ptr<wxXmlDocument> document(new wxXmlDocument(stream));
        REQUIRE(document->IsOk());
        REQUIRE(xrc->LoadDocument(document.release(), xrcName));
        XrcUnloadGuard unload(xrc, xrcName);

        DestroyingOwnerWizardPage::ResetCounters();
        wxWizard* const wizard = new wxWizard;
        const wxWeakRef<wxWindow> weakWizard(wizard);
#if defined(__WXWINUI__) && wxUSE_WINUI3
        std::unique_ptr<wxWinUITLWHostWindowEventGuard> retainedCallback;
        DestroyingOwnerWizardPage::SetRetainedGuardStorage(
            &retainedCallback);
        {
            wxLogNull suppressExpectedCreateError;
            CHECK_FALSE(xrc->LoadObject(wizard,
                                        parent,
                                        "phase013ScheduledOwnerWizard",
                                        "wxWizard"));
            CHECK(wxWinUITLWHostIsDestroyScheduled(wizard));
            CHECK(weakWizard.get() == wizard);
            CHECK(DestroyingOwnerWizardPage::GetForegroundCalls() == 0);
            CHECK(wizard->FindWindow(
                      XRCID("phase013UnexpectedPageAfterOwnerDestroy")) ==
                  nullptr);
        }
        DestroyingOwnerWizardPage::SetRetainedGuardStorage(nullptr);
        retainedCallback.reset();
#else
        {
            wxLogNull suppressExpectedCreateError;
            CHECK_FALSE(xrc->LoadObject(wizard,
                                        parent,
                                        "phase013ScheduledOwnerWizard",
                                        "wxWizard"));
        }
        CHECK(DestroyingOwnerWizardPage::GetForegroundCalls() == 0);
#endif

        REQUIRE(WaitFor("scheduled XRC wizard cleanup",
                        [&weakWizard]()
                        {
                            return weakWizard.get() == nullptr;
                        }));
        CHECK(DestroyingOwnerWizardPage::GetConstructedCount() == 1);
        CHECK(DestroyingOwnerWizardPage::GetDestroyedCount() == 1);
    }
#endif // wxUSE_XRC
}

#endif // wxUSE_WIZARDDLG

#if wxUSE_BOOKCTRL

TEST_CASE("WinUIPropertySheetContracts",
          "[propertysheet][winui-beta-dialogs]")
{
    wxWindow* const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    SECTION("Default_constructor_Create_and_all_advertised_books")
    {
        for ( const long sheetStyle : GetAvailablePropertySheetStyles() )
        {
            wxPropertySheetDialog dialog;
            dialog.SetSheetStyle(sheetStyle);
            INFO("sheet style: " << sheetStyle);
            REQUIRE(dialog.Create(parent,
                                  wxID_ANY,
                                  "Property sheet contract",
                                  wxDefaultPosition,
                                  wxDefaultSize,
                                  wxDEFAULT_DIALOG_STYLE));
            REQUIRE(dialog.GetBookCtrl());
            CHECK(dialog.GetBookCtrl()->GetParent() == &dialog);
            CHECK(dialog.GetContentWindow() == dialog.GetBookCtrl());
            CHECK(IsExpectedPropertySheetBook(dialog.GetBookCtrl(),
                                              sheetStyle));

            wxPanel* const page =
                CreateSizedPage(dialog.GetBookCtrl(), "Style page");
            CHECK(dialog.GetBookCtrl()->AddPage(page, "Page", true));
            dialog.CreateButtons(wxOK | wxCANCEL);
            dialog.LayoutDialog(0);

            CHECK(dialog.FindWindow(wxID_OK));
            CHECK(dialog.FindWindow(wxID_CANCEL));
            CHECK(dialog.GetSize().x >= dialog.GetMinSize().x);
            CHECK(dialog.GetSize().y >= dialog.GetMinSize().y);
        }
    }

    SECTION("Unavailable_requested_book_styles_fail_without_fallback")
    {
        for ( const long sheetStyle :
                  GetUnavailablePropertySheetStyles() )
        {
            wxPropertySheetDialog dialog;
            dialog.SetSheetStyle(sheetStyle);
            INFO("unavailable sheet style: " << sheetStyle);
            CHECK_FALSE(dialog.Create(parent,
                                      wxID_ANY,
                                      "Unsupported property sheet style"));
            CHECK(dialog.GetBookCtrl() == nullptr);
        }
    }

    SECTION("Pages_before_and_after_buttons_events_and_refit")
    {
        wxPropertySheetDialog dialog;
        dialog.SetSheetStyle(wxPROPSHEET_DEFAULT |
                             wxPROPSHEET_SHRINKTOFIT);
        REQUIRE(dialog.Create(parent, wxID_ANY, "Property sheet lifecycle"));
        wxBookCtrlBase* const book = dialog.GetBookCtrl();
        REQUIRE(book);

        wxPanel* const first = CreateSizedPage(book, "First page");
        REQUIRE(book->AddPage(first, "First", true));
        dialog.CreateButtons(wxOK | wxCANCEL | wxAPPLY | wxHELP);
        wxPanel* const second =
            CreateSizedPage(book, wxString(160, 'W'), wxSize(520, 260));
        REQUIRE(book->AddPage(second, "Second"));
        dialog.LayoutDialog(0);

        CHECK(book->GetPageCount() == 2);
        CHECK(dialog.FindWindow(wxID_OK));
        CHECK(dialog.FindWindow(wxID_CANCEL));
        CHECK(dialog.FindWindow(wxID_APPLY));
        CHECK(dialog.FindWindow(wxID_HELP));

        int changingEvents = 0;
        int changedEvents = 0;
        bool vetoNextChange = true;
        book->Bind(wxEVT_BOOKCTRL_PAGE_CHANGING,
                   [&changingEvents, &vetoNextChange](wxBookCtrlEvent& event)
                   {
                       ++changingEvents;
                       if ( vetoNextChange )
                       {
                           vetoNextChange = false;
                           event.Veto();
                       }
                       else
                       {
                           event.Skip();
                       }
                   });
        book->Bind(wxEVT_BOOKCTRL_PAGE_CHANGED,
                   [&changedEvents](wxBookCtrlEvent& event)
                   {
                       ++changedEvents;
                       event.Skip();
                   });
        const wxSize sizeBefore = dialog.GetSize();

        // A vetoed transition must neither change the page nor schedule the
        // shrink-to-fit work normally associated with PAGE_CHANGED.
        REQUIRE(book->SetSelection(1) != wxNOT_FOUND);
        CHECK(changingEvents == 1);
        CHECK(changedEvents == 0);
        CHECK(book->GetSelection() == 0);

        REQUIRE(book->SetSelection(1) != wxNOT_FOUND);
        wxIdleEvent idle;
        dialog.OnIdle(idle);

        CHECK(changingEvents == 2);
        CHECK(changedEvents == 1);
        CHECK(book->GetSelection() == 1);
        CHECK(dialog.GetSize().x >= sizeBefore.x);
        CHECK(dialog.GetSize().y >= sizeBefore.y);

        int applyEvents = 0;
        int helpEvents = 0;
        dialog.Bind(wxEVT_BUTTON,
                    [&applyEvents, &helpEvents](wxCommandEvent& event)
                    {
                        if ( event.GetId() == wxID_APPLY )
                            ++applyEvents;
                        else if ( event.GetId() == wxID_HELP )
                            ++helpEvents;
                        else
                            event.Skip();
                    });
        SendButtonCommand(&dialog, wxID_APPLY);
        SendButtonCommand(&dialog, wxID_HELP);
        CHECK(applyEvents == 1);
        CHECK(helpEvents == 1);
    }

    SECTION("Pages_can_move_between_property_sheets")
    {
        wxPropertySheetDialog source;
        wxPropertySheetDialog destination;
        REQUIRE(source.Create(parent, wxID_ANY, "Source sheet"));
        REQUIRE(destination.Create(parent, wxID_ANY, "Destination sheet"));

        wxPanel* const page =
            CreateSizedPage(source.GetBookCtrl(), "Movable page");
        REQUIRE(source.GetBookCtrl()->AddPage(page, "Movable", true));
        REQUIRE(source.GetBookCtrl()->RemovePage(0));
        REQUIRE(page->Reparent(destination.GetBookCtrl()));
        REQUIRE(destination.GetBookCtrl()->AddPage(page, "Moved", true));

        CHECK(source.GetBookCtrl()->GetPageCount() == 0);
        CHECK(destination.GetBookCtrl()->GetPageCount() == 1);
        CHECK(page->GetParent() == destination.GetBookCtrl());
    }

    SECTION("Layout_adaptation_rolls_back_partial_virtual_reparent")
    {
        wxPropertySheetDialog dialog;
        REQUIRE(dialog.Create(parent,
                              wxID_ANY,
                              "Transactional property sheet layout"));
        wxBookCtrlBase* const book = dialog.GetBookCtrl();
        REQUIRE(book);

        wxPanel* const page = new wxPanel(book);
        wxBoxSizer* const pageSizer = new wxBoxSizer(wxVERTICAL);
        wxPanel* const first = new wxPanel(page);
        wxPanel* const second = new wxPanel(page);
        pageSizer->Add(first, wxSizerFlags().Expand());
        pageSizer->Add(second, wxSizerFlags().Expand());
        page->SetSizer(pageSizer);
        REQUIRE(book->AddPage(page, "Partial", true));

        wxSizer* const dialogSizer = dialog.GetSizer();
        const wxSize originalSize = dialog.GetSize();
        const wxSize originalMinSize = dialog.GetMinSize();
        const wxSize originalMaxSize = dialog.GetMaxSize();
        PartialPropertySheetLayoutAdapter adapter;

        CHECK_FALSE(adapter.DoLayoutAdaptation(&dialog));
        CHECK(adapter.reparentCalls == 1);
        CHECK(dialog.GetSizer() == dialogSizer);
        CHECK(page->GetSizer() == pageSizer);
        CHECK(first->GetParent() == page);
        CHECK(second->GetParent() == page);
        CHECK(FindDescendantOfType<wxScrolledWindow>(page) == nullptr);
        CHECK(dialog.GetSize() == originalSize);
        CHECK(dialog.GetMinSize() == originalMinSize);
        CHECK(dialog.GetMaxSize() == originalMaxSize);
        CHECK_FALSE(dialog.GetLayoutAdaptationDone());
    }

    SECTION("Layout_adaptation_rolls_back_window_refusal_and_fit_failure")
    {
        {
            wxPropertySheetDialog dialog;
            REQUIRE(dialog.Create(parent,
                                  wxID_ANY,
                                  "Rejected property sheet reparent"));
            wxBookCtrlBase* const book = dialog.GetBookCtrl();
            REQUIRE(book);

            wxPanel* const page = new wxPanel(book);
            wxBoxSizer* const pageSizer = new wxBoxSizer(wxVERTICAL);
            int reparentCalls = 0;
            MoveThenRejectPropertySheetPanel* const rejecting =
                new MoveThenRejectPropertySheetPanel(page, &reparentCalls);
            pageSizer->Add(rejecting, wxSizerFlags().Expand());
            page->SetSizer(pageSizer);
            REQUIRE(book->AddPage(page, "Rejected", true));

            wxStandardDialogLayoutAdapter adapter;
            CHECK_FALSE(adapter.DoLayoutAdaptation(&dialog));
            CHECK(reparentCalls == 2);
            CHECK(page->GetSizer() == pageSizer);
            CHECK(rejecting->GetParent() == page);
            CHECK(FindDescendantOfType<wxScrolledWindow>(page) == nullptr);
            CHECK_FALSE(dialog.GetLayoutAdaptationDone());
        }

        {
            wxPropertySheetDialog dialog;
            REQUIRE(dialog.Create(parent,
                                  wxID_ANY,
                                  "Rejected property sheet fit"));
            wxBookCtrlBase* const book = dialog.GetBookCtrl();
            REQUIRE(book);

            wxPanel* const firstPage = new wxPanel(book);
            wxBoxSizer* const firstSizer = new wxBoxSizer(wxVERTICAL);
            wxPanel* const firstChild = new wxPanel(firstPage);
            firstSizer->Add(firstChild, wxSizerFlags().Expand());
            firstPage->SetSizer(firstSizer);
            REQUIRE(book->AddPage(firstPage, "First", true));

            wxPanel* const secondPage = new wxPanel(book);
            wxBoxSizer* const secondSizer = new wxBoxSizer(wxVERTICAL);
            wxPanel* const secondChild = new wxPanel(secondPage);
            secondSizer->Add(secondChild, wxSizerFlags().Expand());
            secondPage->SetSizer(secondSizer);
            REQUIRE(book->AddPage(secondPage, "Second"));

            wxSizer* const dialogSizer = dialog.GetSizer();
            const wxSize originalSize = dialog.GetSize();
            const wxSize originalMinSize = dialog.GetMinSize();
            const wxSize originalMaxSize = dialog.GetMaxSize();
            RejectingPropertySheetFitAdapter adapter;

            CHECK_FALSE(adapter.DoLayoutAdaptation(&dialog));
            CHECK(adapter.fitCalls == 1);
            CHECK(dialog.GetSizer() == dialogSizer);
            CHECK(firstPage->GetSizer() == firstSizer);
            CHECK(secondPage->GetSizer() == secondSizer);
            CHECK(firstChild->GetParent() == firstPage);
            CHECK(secondChild->GetParent() == secondPage);
            CHECK(FindDescendantOfType<wxScrolledWindow>(firstPage) ==
                  nullptr);
            CHECK(FindDescendantOfType<wxScrolledWindow>(secondPage) ==
                  nullptr);
            CHECK(dialog.GetSize() == originalSize);
            CHECK(dialog.GetMinSize() == originalMinSize);
            CHECK(dialog.GetMaxSize() == originalMaxSize);
            CHECK_FALSE(dialog.GetLayoutAdaptationDone());
        }
    }

    SECTION("Arbitrary_layout_rollback_preserves_gridbag_item_and_user_data")
    {
        int userDataDestroyed = 0;
        {
            wxDialog dialog;
            REQUIRE(dialog.Create(parent,
                                  wxID_ANY,
                                  "Transactional grid bag layout"));

            wxGridBagSizer* const grid = new wxGridBagSizer;
            wxPanel* const body = new wxPanel(&dialog);
            wxButton* const ok = new wxButton(&dialog, wxID_OK);
            wxPanel* const tail = new wxPanel(&dialog);
            REQUIRE(grid->Add(body, wxGBPosition(0, 0)));

            TrackingSizerUserData* const userData =
                new TrackingSizerUserData(&userDataDestroyed);
            wxGBSizerItem* const originalItem = new wxGBSizerItem(
                ok,
                wxGBPosition(1, 2),
                wxGBSpan(2, 3),
                wxALIGN_CENTER | wxALL,
                7,
                userData);
            REQUIRE(grid->Add(originalItem) == originalItem);
            REQUIRE(grid->Add(tail, wxGBPosition(4, 0)));
            dialog.SetSizer(grid);

            REQUIRE(grid->GetItem(1) == originalItem);
            RejectingPropertySheetFitAdapter adapter;
            CHECK_FALSE(adapter.DoLayoutAdaptation(&dialog));

            CHECK(adapter.fitCalls == 1);
            CHECK(dialog.GetSizer() == grid);
            REQUIRE(grid->GetItem(ok));
            CHECK(grid->GetItem(ok) == originalItem);
            CHECK(grid->GetItem(1) == originalItem);
            CHECK(originalItem->GetPos() == wxGBPosition(1, 2));
            CHECK(originalItem->GetSpan() == wxGBSpan(2, 3));
            CHECK(originalItem->GetFlag() == (wxALIGN_CENTER | wxALL));
            CHECK(originalItem->GetBorder() == 7);
            CHECK(originalItem->GetUserData() == userData);
            CHECK(ok->GetContainingSizer() == grid);
            CHECK(body->GetParent() == &dialog);
            CHECK(tail->GetParent() == &dialog);
            CHECK(FindDescendantOfType<wxScrolledWindow>(&dialog) ==
                  nullptr);
            CHECK(userDataDestroyed == 0);
        }

        CHECK(userDataDestroyed == 1);
    }

#if defined(__WXWINUI__) && wxUSE_WINUI3
    SECTION("Layout_adaptation_rolls_back_before_retained_destroy_drains")
    {
        wxPropertySheetDialog* const dialog =
            new wxPropertySheetDialog;
        REQUIRE(dialog->Create(parent,
                               wxID_ANY,
                               "Destroyed property sheet adaptation"));
        wxBookCtrlBase* const book = dialog->GetBookCtrl();
        REQUIRE(book);

        wxPanel* const page = new wxPanel(book);
        wxBoxSizer* const pageSizer = new wxBoxSizer(wxVERTICAL);
        int reparentCalls = 0;
        DestroyPropertySheetOnReparentPanel* const destructive =
            new DestroyPropertySheetOnReparentPanel(page,
                                                    dialog,
                                                    &reparentCalls);
        pageSizer->Add(destructive, wxSizerFlags().Expand());
        page->SetSizer(pageSizer);
        REQUIRE(book->AddPage(page, "Destroyed", true));

        const wxWeakRef<wxWindow> weakDialog(dialog);
        {
            wxWinUITLWHostWindowEventGuard retainedCallback(dialog);
            wxStandardDialogLayoutAdapter adapter;
            CHECK_FALSE(adapter.DoLayoutAdaptation(dialog));
            CHECK(reparentCalls == 2);
            CHECK(wxWinUITLWHostIsDestroyScheduled(dialog));
            CHECK(page->GetSizer() == pageSizer);
            CHECK(destructive->GetParent() == page);
            CHECK(FindDescendantOfType<wxScrolledWindow>(page) == nullptr);
            CHECK_FALSE(dialog->GetLayoutAdaptationDone());
        }

        REQUIRE(WaitFor("destroyed property sheet adaptation cleanup",
                        [&weakDialog]()
                        {
                            return weakDialog.get() == nullptr;
                        }));
    }

    SECTION("Layout_adaptation_stops_after_destructive_book_virtuals")
    {
        using Action = DestroyingPropertySheetBook::Action;
        for ( const Action action : { Action::PageCount, Action::Page } )
        {
            int pageCountCalls = 0;
            int pageCalls = 0;
            DestroyingBookPropertySheet* const dialog =
                new DestroyingBookPropertySheet(action,
                                                &pageCountCalls,
                                                &pageCalls);
            REQUIRE(dialog->Create(parent,
                                   wxID_ANY,
                                   "Destructive virtual book"));
            wxBookCtrlBase* const book = dialog->GetBookCtrl();
            REQUIRE(book);
            REQUIRE(book->AddPage(CreateSizedPage(book, "Page"),
                                  "Page",
                                  true));

            const wxWeakRef<wxWindow> weakDialog(dialog);
            const int pageCountCallsBeforeArm = pageCountCalls;
            const int pageCallsBeforeArm = pageCalls;
            dialog->ArmBook();
            {
                wxWinUITLWHostWindowEventGuard retainedCallback(dialog);
                wxStandardDialogLayoutAdapter adapter;
                CHECK_FALSE(adapter.DoLayoutAdaptation(dialog));
                CHECK(wxWinUITLWHostIsDestroyScheduled(dialog));
                CHECK(pageCountCalls - pageCountCallsBeforeArm == 1);
                CHECK(pageCalls - pageCallsBeforeArm ==
                      (action == Action::Page ? 1 : 0));
            }

            REQUIRE(WaitFor("destructive virtual book cleanup",
                            [&weakDialog]()
                            {
                                return weakDialog.get() == nullptr;
                            }));
        }
    }
#endif

    SECTION("Book_creation_failure_is_reported_without_adoption")
    {
        NullBookPropertySheet nullBook;
        CHECK_FALSE(nullBook.Create(parent, wxID_ANY, "Null book"));
        CHECK(nullBook.GetBookCtrl() == nullptr);

        wxPanel foreignParent(parent);
        ForeignBookPropertySheet foreignBook(&foreignParent);
        CHECK_FALSE(foreignBook.Create(parent, wxID_ANY, "Foreign book"));
        CHECK(foreignBook.GetBookCtrl() == nullptr);
        REQUIRE(foreignParent.GetChildren().GetCount() == 1);
        wxBookCtrlBase* const transferredBook =
            FindDescendantOfType<wxBookCtrlBase>(&foreignParent);
        REQUIRE(transferredBook);
        CHECK(transferredBook->GetParent() == &foreignParent);
        CHECK(transferredBook->GetContainingSizer() == nullptr);
    }

    SECTION("Book_created_already_adopted_is_preserved_without_double_item")
    {
        AutoAdoptingBookPropertySheet dialog;
        CHECK_FALSE(dialog.Create(parent,
                                  wxID_ANY,
                                  "Auto-adopted property sheet book"));
        CHECK(dialog.GetBookCtrl() == nullptr);

        wxBookCtrlBase* const book = dialog.GetAdoptedBook();
        REQUIRE(book);
        wxSizer* const adoptionSizer = dialog.GetAdoptionSizer();
        REQUIRE(adoptionSizer);
        CHECK(book->GetParent() == &dialog);
        CHECK(book->GetContainingSizer() == adoptionSizer);
        CHECK(CountSizerWindowItemsForTest(adoptionSizer, book) == 1);
        REQUIRE(dialog.GetInnerSizer());
        CHECK(CountSizerWindowItemsForTest(dialog.GetInnerSizer(), book) ==
              0);
    }

    SECTION("Create_event_owner_transfer_aborts_before_layout_publication")
    {
        wxFrame foreignOwner(parent, wxID_ANY, "Create event owner");
        wxPropertySheetDialog dialog;
        int createCallbacks = 0;
        dialog.Bind(wxEVT_CREATE,
                    [&](wxWindowCreateEvent& event)
                    {
                        if ( event.GetEventObject() == &dialog )
                        {
                            ++createCallbacks;
                            dialog.Reparent(&foreignOwner);
                        }
                        event.Skip();
                    });

        CHECK_FALSE(dialog.Create(parent,
                                  wxID_ANY,
                                  "Create event transferred sheet"));
        CHECK(createCallbacks == 1);
        CHECK(dialog.GetParent() == &foreignOwner);
        CHECK(dialog.GetSizer() == nullptr);
        CHECK(dialog.GetBookCtrl() == nullptr);
    }

    SECTION("Book_callback_boundaries_clear_destroyed_or_reparented_books")
    {
        using Action = BoundaryPropertySheetBook::Action;
        for ( const Action action : { Action::Delete, Action::Reparent } )
        {
            CAPTURE(action);
            wxPanel foreignParent(parent);
            wxWeakRef<wxWindow> weakBook;
            int callbackCalls = 0;
            BoundaryBookPropertySheet dialog(action,
                                              &foreignParent,
                                              true,
                                              &callbackCalls,
                                              &weakBook);

            CHECK_FALSE(dialog.Create(parent,
                                      wxID_ANY,
                                      "Property sheet book boundary"));
            CHECK(callbackCalls == 1);
            CHECK(dialog.GetBookCtrl() == nullptr);

            if ( action == Action::Delete )
            {
                CHECK(weakBook.get() == nullptr);
            }
            else
            {
                wxBookCtrlBase* const book =
                    wxDynamicCast(weakBook.get(), wxBookCtrlBase);
                REQUIRE(book);
                CHECK(book->GetParent() == &foreignParent);
                CHECK(book->GetContainingSizer() == nullptr);
            }
        }

        for ( const Action action : { Action::Delete, Action::Reparent } )
        {
            CAPTURE(action);
            wxPanel foreignParent(parent);
            wxWeakRef<wxWindow> weakBook;
            int callbackCalls = 0;
            BoundaryBookPropertySheet dialog(action,
                                              &foreignParent,
                                              false,
                                              &callbackCalls,
                                              &weakBook);
            dialog.SetSheetStyle(wxPROPSHEET_DEFAULT |
                                 wxPROPSHEET_SHRINKTOFIT);
            REQUIRE(dialog.Create(parent,
                                  wxID_ANY,
                                  "Property sheet idle book boundary"));
            wxBookCtrlBase* const book = dialog.GetBookCtrl();
            REQUIRE(book);
            REQUIRE(book->AddPage(CreateSizedPage(book, "Page"),
                                  "Page",
                                  true));
            dialog.ArmBook();

            wxIdleEvent idle;
            dialog.OnIdle(idle);

            CHECK(callbackCalls == 1);
            CHECK(dialog.GetBookCtrl() == nullptr);
            if ( action == Action::Delete )
            {
                CHECK(weakBook.get() == nullptr);
            }
            else
            {
                wxBookCtrlBase* const movedBook =
                    wxDynamicCast(weakBook.get(), wxBookCtrlBase);
                REQUIRE(movedBook);
                CHECK(movedBook->GetParent() == &foreignParent);
                CHECK(movedBook->GetContainingSizer() == nullptr);
            }
        }
    }

    SECTION("AddBookCtrl_reparent_is_rejected_during_reentrant_access")
    {
        wxPanel foreignParent(parent);
        wxWeakRef<wxWindow> weakBook;
        int callbackCalls = 0;
        ReparentingAddBookPropertySheet dialog(&foreignParent,
                                               &callbackCalls,
                                               &weakBook);

        CHECK_FALSE(dialog.Create(parent,
                                  wxID_ANY,
                                  "Reparenting AddBookCtrl"));
        CHECK(callbackCalls == 1);
        CHECK(dialog.GetReentrantBook() == nullptr);
        CHECK(dialog.GetBookCtrl() == nullptr);
        wxBookCtrlBase* const book =
            wxDynamicCast(weakBook.get(), wxBookCtrlBase);
        REQUIRE(book);
        CHECK(book->GetParent() == &foreignParent);
        CHECK(book->GetContainingSizer() == nullptr);
    }

    SECTION("Standard_button_sizer_rejects_expired_slot_before_Realize")
    {
        wxDialog owner(parent, wxID_ANY, "Expired button slot owner");
        wxStdDialogButtonSizer sizer;
        wxButton* const deadButton =
            new wxButton(&owner, wxID_OK, "Dead before Realize");
        const wxWeakRef<wxWindow> weakDeadButton(deadButton);
        sizer.AddButton(deadButton);
        delete deadButton;
        REQUIRE(weakDeadButton.get() == nullptr);

        sizer.Realize();
        CHECK(sizer.GetItemCount() == 0);
    }

    SECTION("Standard_button_sizer_aborts_slot_mutation_and_bounds_reentry")
    {
        using Action = MutatingTabOrderButton::Action;
        for ( const Action action : { Action::ReplaceNegativeSlot,
                                      Action::NestedRealize } )
        {
            CAPTURE(action);
            wxDialog owner(parent, wxID_ANY, "Button sizer callback owner");
            wxStdDialogButtonSizer sizer;
            wxButton* const affirmative =
                new wxButton(&owner, wxID_YES, "Affirmative");
            wxButton* const replacement =
                new wxButton(&owner, wxID_ANY, "Replacement negative");
            MutatingTabOrderButton* const negative =
                new MutatingTabOrderButton(&owner,
                                           wxID_NO,
                                           &sizer,
                                           action,
                                           replacement);
            sizer.AddButton(affirmative);
            sizer.AddButton(negative);
            const std::vector<wxWindow*> originalChildOrder =
                GetDirectChildOrder(&owner);

            sizer.Realize();
            CHECK(negative->GetCallbackCount() == 1);
            CHECK(GetDirectChildOrder(&owner) == originalChildOrder);
            if ( action == Action::ReplaceNegativeSlot )
            {
                CHECK(sizer.GetNegativeButton() == replacement);
                CHECK(sizer.GetItemCount() == 0);
                CHECK(affirmative->GetContainingSizer() == nullptr);
                CHECK(negative->GetContainingSizer() == nullptr);
                CHECK(replacement->GetContainingSizer() == nullptr);
            }
            else
            {
                CHECK(sizer.GetNegativeButton() == negative);
                CHECK(sizer.GetItemCount() == 0);
                CHECK(affirmative->GetContainingSizer() == nullptr);
                CHECK(negative->GetContainingSizer() == nullptr);
                CHECK(replacement->GetContainingSizer() == nullptr);
            }
        }
    }

    SECTION("Button_factory_preserves_a_button_adopted_during_focus")
    {
        wxPropertySheetDialog dialog;
        REQUIRE(dialog.Create(parent,
                              wxID_ANY,
                              "Adopted property sheet button"));
        wxBoxSizer foreignSizer(wxHORIZONTAL);
        wxButton* const originalDefault =
            new wxButton(&dialog, wxID_ANY, "Original permanent default");
        wxButton* const originalTmpDefault =
            new wxButton(&dialog, wxID_ANY, "Original temporary default");
        dialog.Show();
        originalDefault->SetDefault();
        dialog.SetTmpDefaultItem(originalTmpDefault);
        originalTmpDefault->SetFocus();
        REQUIRE(WaitFor("original focus before button adoption",
                        [originalTmpDefault]()
                        {
                            return wxWindow::FindFocus() == originalTmpDefault;
                        }));
        const int originalAffirmativeId = dialog.GetAffirmativeId();

        int adoptionCallbacks = 0;
        wxWeakRef<wxWindow> weakAdoptedButton;
        dialog.Bind(wxEVT_CHILD_FOCUS,
                    [&](wxChildFocusEvent& event)
                    {
                        wxWindow* const focused = event.GetWindow();
                        if ( focused && focused->GetId() == wxID_YES &&
                             adoptionCallbacks == 0 )
                        {
                            ++adoptionCallbacks;
                            weakAdoptedButton = focused;
                            foreignSizer.Add(focused);
                        }
                        event.Skip();
                    });

        dialog.CreateButtons(wxYES | wxNO | wxCANCEL);

        CHECK(adoptionCallbacks == 1);
        wxButton* const adoptedButton =
            wxDynamicCast(weakAdoptedButton.get(), wxButton);
        REQUIRE(adoptedButton);
        CHECK(adoptedButton->GetParent() == &dialog);
        CHECK(adoptedButton->GetContainingSizer() == &foreignSizer);
        CHECK(CountSizerWindowItemsForTest(&foreignSizer, adoptedButton) ==
              1);
        CHECK(CountDescendantsWithId(&dialog, wxID_YES) == 1);
        CHECK(CountDescendantsWithId(&dialog, wxID_NO) == 0);
        CHECK(CountDescendantsWithId(&dialog, wxID_CANCEL) == 0);
        CHECK(dialog.GetInnerSizer()->GetItemCount() == 1);
        CHECK(dialog.GetAffirmativeId() == originalAffirmativeId);
        CHECK(dialog.GetTmpDefaultItem() == originalTmpDefault);
        CHECK(dialog.GetDefaultItem() == originalTmpDefault);
        CHECK(wxWindow::FindFocus() == originalTmpDefault);
        dialog.SetTmpDefaultItem(nullptr);
        CHECK(dialog.GetDefaultItem() == originalDefault);

        REQUIRE(foreignSizer.Detach(adoptedButton));
        delete adoptedButton;
        CHECK(weakAdoptedButton.get() == nullptr);
        CHECK(CountDescendantsWithId(&dialog, wxID_YES) == 0);
    }

    SECTION("Nested_CreateButtons_keeps_only_the_latest_factory_result")
    {
        for ( const int nestedFlags : { wxYES | wxCANCEL,
                                        wxOK | wxCANCEL } )
        {
            CAPTURE(nestedFlags);
            wxPropertySheetDialog dialog;
            REQUIRE(dialog.Create(parent,
                                  wxID_ANY,
                                  "Nested property sheet buttons"));
            dialog.Show();

            bool nestedStarted = false;
            int nestedCallbacks = 0;
            dialog.Bind(wxEVT_CHILD_FOCUS,
                        [&](wxChildFocusEvent& event)
                        {
                            wxWindow* const focused = event.GetWindow();
                            if ( !nestedStarted && focused &&
                                 focused->GetId() == wxID_YES )
                            {
                                nestedStarted = true;
                                ++nestedCallbacks;
                                dialog.CreateButtons(nestedFlags);
                            }
                            event.Skip();
                        });

            dialog.CreateButtons(wxYES | wxNO | wxCANCEL);

            CHECK(nestedCallbacks == 1);
            const int expectedAffirmative =
                (nestedFlags & wxOK) ? wxID_OK : wxID_YES;
            wxButton* const defaultButton = wxDynamicCast(
                dialog.FindWindow(expectedAffirmative), wxButton);
            REQUIRE(defaultButton);
            CHECK(CountDescendantsWithId(&dialog, expectedAffirmative) == 1);
            CHECK(CountDescendantsWithId(&dialog, wxID_CANCEL) == 1);
            CHECK(CountDescendantsWithId(&dialog, wxID_NO) == 0);
            CHECK(CountDescendantsWithId(
                      &dialog,
                      expectedAffirmative == wxID_YES ? wxID_OK : wxID_YES)
                  == 0);
            CHECK(dialog.GetAffirmativeId() == expectedAffirmative);
            CHECK(dialog.GetDefaultItem() == defaultButton);
            CHECK(dialog.GetTmpDefaultItem() == defaultButton);
            CHECK(wxWindow::FindFocus() == defaultButton);
            CHECK(dialog.GetInnerSizer()->GetItemCount() == 3);
        }
    }

    SECTION("Failed_nested_button_factory_returns_writer_ownership_to_outer")
    {
        wxPropertySheetDialog dialog;
        REQUIRE(dialog.Create(parent,
                              wxID_ANY,
                              "Nested failed property sheet buttons"));
        wxBoxSizer foreignSizer(wxHORIZONTAL);
        wxButton* const originalDefault =
            new wxButton(&dialog, wxID_ANY, "Initial permanent default");
        wxButton* const originalTmpDefault =
            new wxButton(&dialog, wxID_ANY, "Initial temporary default");
        dialog.Show();
        originalDefault->SetDefault();
        dialog.SetTmpDefaultItem(originalTmpDefault);
        originalTmpDefault->SetFocus();
        REQUIRE(WaitFor("initial focus before failed nested factory",
                        [originalTmpDefault]()
                        {
                            return wxWindow::FindFocus() == originalTmpDefault;
                        }));
        dialog.SetAffirmativeId(wxID_HELP);

        bool nestedStarted = false;
        int outerFocusCallbacks = 0;
        int nestedFocusCallbacks = 0;
        wxWeakRef<wxWindow> weakAdoptedButton;
        dialog.Bind(wxEVT_CHILD_FOCUS,
                    [&](wxChildFocusEvent& event)
                    {
                        wxWindow* const focused = event.GetWindow();
                        if ( !focused )
                        {
                            event.Skip();
                            return;
                        }

                        if ( focused->GetId() == wxID_YES &&
                             !nestedStarted )
                        {
                            nestedStarted = true;
                            ++outerFocusCallbacks;
                            dialog.CreateButtons(wxOK | wxCANCEL);
                        }
                        else if ( focused->GetId() == wxID_OK &&
                                  !weakAdoptedButton.get() )
                        {
                            ++nestedFocusCallbacks;
                            weakAdoptedButton = focused;
                            foreignSizer.Add(focused);
                        }
                        event.Skip();
                    });

        dialog.CreateButtons(wxYES | wxCANCEL);

        CHECK(outerFocusCallbacks == 1);
        CHECK(nestedFocusCallbacks == 1);
        wxButton* const adoptedButton =
            wxDynamicCast(weakAdoptedButton.get(), wxButton);
        REQUIRE(adoptedButton);
        CHECK(adoptedButton->GetParent() == &dialog);
        CHECK(adoptedButton->GetContainingSizer() == &foreignSizer);
        CHECK(CountSizerWindowItemsForTest(&foreignSizer, adoptedButton) ==
              1);
        CHECK(CountDescendantsWithId(&dialog, wxID_OK) == 1);
        CHECK(CountDescendantsWithId(&dialog, wxID_YES) == 0);
        CHECK(CountDescendantsWithId(&dialog, wxID_CANCEL) == 0);
        CHECK(dialog.GetInnerSizer()->GetItemCount() == 1);
        CHECK(dialog.GetAffirmativeId() == wxID_HELP);
        CHECK(dialog.GetTmpDefaultItem() == originalTmpDefault);
        CHECK(dialog.GetDefaultItem() == originalTmpDefault);
        CHECK(wxWindow::FindFocus() == originalTmpDefault);
        dialog.SetTmpDefaultItem(nullptr);
        CHECK(dialog.GetDefaultItem() == originalDefault);

        REQUIRE(foreignSizer.Detach(adoptedButton));
        delete adoptedButton;
        CHECK(weakAdoptedButton.get() == nullptr);
        CHECK(CountDescendantsWithId(&dialog, wxID_OK) == 0);
    }

#if wxUSE_EXCEPTIONS
    SECTION("Nested_factory_cleanup_survives_focus_restore_exception")
    {
        wxPropertySheetDialog dialog;
        REQUIRE(dialog.Create(parent,
                              wxID_ANY,
                              "Throwing nested button rollback"));
        wxBoxSizer foreignSizer(wxHORIZONTAL);
        wxButton* const originalDefault =
            new wxButton(&dialog, wxID_ANY, "Initial permanent default");
        ThrowingFocusButton* const originalFocus =
            new ThrowingFocusButton(&dialog, "Throwing initial focus");
        dialog.Show();
        originalDefault->SetDefault();
        dialog.SetTmpDefaultItem(originalFocus);
        originalFocus->SetFocus();
        REQUIRE(WaitFor("initial throwing focus before nested factory",
                        [originalFocus]()
                        {
                            return wxWindow::FindFocus() == originalFocus;
                        }));
        dialog.SetAffirmativeId(wxID_HELP);
        originalFocus->ArmThrowOnce();

        bool nestedStarted = false;
        wxWeakRef<wxWindow> weakAdoptedButton;
        dialog.Bind(wxEVT_CHILD_FOCUS,
                    [&](wxChildFocusEvent& event)
                    {
                        wxWindow* const focused = event.GetWindow();
                        if ( focused && focused->GetId() == wxID_YES &&
                             !nestedStarted )
                        {
                            nestedStarted = true;
                            dialog.CreateButtons(wxOK | wxCANCEL);
                        }
                        else if ( focused && focused->GetId() == wxID_OK &&
                                  !weakAdoptedButton.get() )
                        {
                            weakAdoptedButton = focused;
                            foreignSizer.Add(focused);
                        }
                        event.Skip();
                    });

        dialog.CreateButtons(wxYES | wxCANCEL);

        CHECK(nestedStarted);
        CHECK(originalFocus->GetThrowCount() == 1);
        wxButton* const adoptedButton =
            wxDynamicCast(weakAdoptedButton.get(), wxButton);
        REQUIRE(adoptedButton);
        CHECK(adoptedButton->GetContainingSizer() == &foreignSizer);
        CHECK(CountSizerWindowItemsForTest(&foreignSizer, adoptedButton) ==
              1);
        CHECK(CountDescendantsWithId(&dialog, wxID_OK) == 1);
        CHECK(CountDescendantsWithId(&dialog, wxID_YES) == 0);
        CHECK(CountDescendantsWithId(&dialog, wxID_CANCEL) == 0);
        CHECK(dialog.GetInnerSizer()->GetItemCount() == 1);
        CHECK(dialog.GetAffirmativeId() == wxID_HELP);
        CHECK(dialog.GetTmpDefaultItem() == originalFocus);
        CHECK(dialog.GetDefaultItem() == originalFocus);
        CHECK(wxWindow::FindFocus() == originalFocus);
        dialog.SetTmpDefaultItem(nullptr);
        CHECK(dialog.GetDefaultItem() == originalDefault);

        REQUIRE(foreignSizer.Detach(adoptedButton));
        delete adoptedButton;
        CHECK(weakAdoptedButton.get() == nullptr);
        CHECK(CountDescendantsWithId(&dialog, wxID_OK) == 0);
    }

    SECTION("Property_cleanup_survives_focus_restore_exception")
    {
        FocusTopologyInvalidatingPropertySheet dialog;
        REQUIRE(dialog.Create(parent,
                              wxID_ANY,
                              "Throwing outer button rollback"));
        wxSizer* const originalTopSizer = dialog.GetSizer();
        wxButton* const originalDefault =
            new wxButton(&dialog, wxID_ANY, "Outer permanent default");
        ThrowingFocusButton* const originalFocus =
            new ThrowingFocusButton(&dialog, "Outer throwing focus");
        dialog.Show();
        originalDefault->SetDefault();
        dialog.SetTmpDefaultItem(originalFocus);
        originalFocus->SetFocus();
        REQUIRE(WaitFor("initial throwing focus before outer rollback",
                        [originalFocus]()
                        {
                            return wxWindow::FindFocus() == originalFocus;
                        }));
        dialog.SetAffirmativeId(wxID_HELP);
        originalFocus->ArmThrowOnce();

        int topologyCallbacks = 0;
        int affirmativeDuringCallback = wxID_NONE;
        dialog.Bind(wxEVT_CHILD_FOCUS,
                    [&](wxChildFocusEvent& event)
                    {
                        wxWindow* const focused = event.GetWindow();
                        if ( focused && focused->GetId() == wxID_YES &&
                             topologyCallbacks == 0 )
                        {
                            ++topologyCallbacks;
                            affirmativeDuringCallback =
                                dialog.GetAffirmativeId();
                            dialog.InvalidateTopSizer();
                        }
                        event.Skip();
                    });

        dialog.CreateButtons(wxYES | wxCANCEL);

        CHECK(topologyCallbacks == 1);
        CHECK(affirmativeDuringCallback == wxID_YES);
        CHECK(originalFocus->GetThrowCount() == 1);
        CHECK(dialog.GetOriginalTopSizer() == originalTopSizer);
        CHECK(dialog.GetSizer() != originalTopSizer);
        CHECK(dialog.GetSizer()->GetItemCount() == 0);
        CHECK(dialog.GetInnerSizer()->GetItemCount() == 1);
        CHECK(CountDescendantsWithId(&dialog, wxID_YES) == 0);
        CHECK(CountDescendantsWithId(&dialog, wxID_CANCEL) == 0);
        CHECK(dialog.GetAffirmativeId() == wxID_HELP);
        CHECK(dialog.GetTmpDefaultItem() == originalFocus);
        CHECK(dialog.GetDefaultItem() == originalFocus);
        CHECK(wxWindow::FindFocus() == originalFocus);
        dialog.SetTmpDefaultItem(nullptr);
        CHECK(dialog.GetDefaultItem() == originalDefault);
    }

    SECTION("Nested_Property_writer_snapshots_restore_outer_then_initial")
    {
        using Mode = TransactionalInnerSizer::Mode;
        TransactionalInnerPropertySheet dialog;
        REQUIRE(dialog.Create(parent,
                              wxID_ANY,
                              "Composed Property button writers"));
        REQUIRE(dialog.InstallInnerSizer(
            Mode::RejectButtonSizerAndInvalidateTop));
        TransactionalInnerSizer* const inner =
            dialog.GetTransactionalInnerSizer();
        REQUIRE(inner);

        wxButton* const originalDefault =
            new wxButton(&dialog, wxID_ANY, "Composed permanent default");
        wxButton* const originalFocus =
            new wxButton(&dialog, wxID_ANY, "Composed temporary default");
        dialog.Show();
        originalDefault->SetDefault();
        dialog.SetTmpDefaultItem(originalFocus);
        originalFocus->SetFocus();
        REQUIRE(WaitFor("initial focus before composed Property factories",
                        [originalFocus]()
                        {
                            return wxWindow::FindFocus() == originalFocus;
                        }));
        dialog.SetAffirmativeId(wxID_HELP);
        inner->Arm();

        bool nestedStarted = false;
        dialog.Bind(wxEVT_CHILD_FOCUS,
                    [&](wxChildFocusEvent& event)
                    {
                        wxWindow* const focused = event.GetWindow();
                        if ( focused && focused->GetId() == wxID_YES &&
                             !nestedStarted )
                        {
                            nestedStarted = true;
                            dialog.CreateButtons(wxOK | wxCANCEL);
                        }
                        event.Skip();
                    });

        dialog.CreateButtons(wxYES | wxCANCEL);

        CHECK(nestedStarted);
        CHECK(inner->GetRejectedInsertCount() == 1);
        wxSizer* const rejectedSizer = inner->GetTrackedButtonSizer();
        REQUIRE(rejectedSizer);
        CHECK(CountSizerChildItemsForTest(inner, rejectedSizer) == 0);
        CHECK(dialog.GetOriginalTopSizer() != nullptr);
        CHECK(dialog.GetSizer() != dialog.GetOriginalTopSizer());
        CHECK(inner->GetItemCount() == 1);
        CHECK(CountDescendantsWithId(&dialog, wxID_OK) == 0);
        CHECK(CountDescendantsWithId(&dialog, wxID_YES) == 0);
        CHECK(CountDescendantsWithId(&dialog, wxID_CANCEL) == 0);
        CHECK(dialog.GetAffirmativeId() == wxID_HELP);
        CHECK(dialog.GetTmpDefaultItem() == originalFocus);
        CHECK(dialog.GetDefaultItem() == originalFocus);
        CHECK(wxWindow::FindFocus() == originalFocus);
        dialog.SetTmpDefaultItem(nullptr);
        CHECK(dialog.GetDefaultItem() == originalDefault);
    }

    SECTION("Property_cleanup_bypasses_throwing_virtual_Detach")
    {
        using Mode = TransactionalInnerSizer::Mode;
        TransactionalInnerPropertySheet dialog;
        REQUIRE(dialog.Create(parent,
                              wxID_ANY,
                              "Throwing Property inner sizer"));
        REQUIRE(dialog.InstallInnerSizer(Mode::ThrowOnSpacerAndDetach));
        TransactionalInnerSizer* const inner =
            dialog.GetTransactionalInnerSizer();
        REQUIRE(inner);

        wxButton* const originalDefault =
            new wxButton(&dialog, wxID_ANY, "Detach permanent default");
        wxButton* const originalFocus =
            new wxButton(&dialog, wxID_ANY, "Detach temporary default");
        dialog.Show();
        originalDefault->SetDefault();
        dialog.SetTmpDefaultItem(originalFocus);
        originalFocus->SetFocus();
        REQUIRE(WaitFor("initial focus before throwing inner sizer",
                        [originalFocus]()
                        {
                            return wxWindow::FindFocus() == originalFocus;
                        }));
        dialog.SetAffirmativeId(wxID_HELP);
        inner->Arm();

        CHECK_THROWS(dialog.CreateButtons(wxYES | wxCANCEL));

        CHECK(inner->GetSpacerThrowCount() == 1);
        CHECK(inner->GetDetachThrowCount() == 1);
        wxSizer* const detachedSizer = inner->GetTrackedButtonSizer();
        REQUIRE(detachedSizer);
        CHECK(CountSizerChildItemsForTest(inner, detachedSizer) == 0);
        CHECK(inner->GetItemCount() == 1);
        CHECK(CountDescendantsWithId(&dialog, wxID_YES) == 0);
        CHECK(CountDescendantsWithId(&dialog, wxID_CANCEL) == 0);
        CHECK(dialog.GetAffirmativeId() == wxID_HELP);
        CHECK(dialog.GetTmpDefaultItem() == originalFocus);
        CHECK(dialog.GetDefaultItem() == originalFocus);
        CHECK(wxWindow::FindFocus() == originalFocus);
        dialog.SetTmpDefaultItem(nullptr);
        CHECK(dialog.GetDefaultItem() == originalDefault);
    }

    SECTION("Property_Add_consume_and_return_null_never_reuses_child")
    {
        using Mode = TransactionalInnerSizer::Mode;
        TransactionalInnerPropertySheet dialog;
        REQUIRE(dialog.Create(parent,
                              wxID_ANY,
                              "Consuming Property inner sizer"));
        REQUIRE(dialog.InstallInnerSizer(
            Mode::ConsumeButtonSizerAndReturnNull));
        TransactionalInnerSizer* const inner =
            dialog.GetTransactionalInnerSizer();
        REQUIRE(inner);

        wxButton* const originalDefault =
            new wxButton(&dialog, wxID_ANY, "Consume permanent default");
        wxButton* const originalFocus =
            new wxButton(&dialog, wxID_ANY, "Consume temporary default");
        dialog.Show();
        originalDefault->SetDefault();
        dialog.SetTmpDefaultItem(originalFocus);
        originalFocus->SetFocus();
        REQUIRE(WaitFor("initial focus before consuming insertion",
                        [originalFocus]()
                        {
                            return wxWindow::FindFocus() == originalFocus;
                        }));
        dialog.SetAffirmativeId(wxID_HELP);
        inner->Arm();

        CHECK_NOTHROW(dialog.CreateButtons(wxYES | wxCANCEL));

        CHECK(inner->GetConsumedInsertCount() == 1);
        CHECK(inner->GetItemCount() == 1);
        CHECK(CountDescendantsWithId(&dialog, wxID_YES) == 0);
        CHECK(CountDescendantsWithId(&dialog, wxID_CANCEL) == 0);
        CHECK(dialog.GetAffirmativeId() == wxID_HELP);
        CHECK(dialog.GetTmpDefaultItem() == originalFocus);
        CHECK(dialog.GetDefaultItem() == originalFocus);
        CHECK(wxWindow::FindFocus() == originalFocus);
        dialog.SetTmpDefaultItem(nullptr);
        CHECK(dialog.GetDefaultItem() == originalDefault);
    }

    SECTION("Property_Add_consume_and_throw_never_reuses_child")
    {
        using Mode = TransactionalInnerSizer::Mode;
        TransactionalInnerPropertySheet dialog;
        REQUIRE(dialog.Create(parent,
                              wxID_ANY,
                              "Throwing consuming Property inner sizer"));
        REQUIRE(dialog.InstallInnerSizer(Mode::ConsumeButtonSizerAndThrow));
        TransactionalInnerSizer* const inner =
            dialog.GetTransactionalInnerSizer();
        REQUIRE(inner);

        wxButton* const originalDefault = new wxButton(
            &dialog, wxID_ANY, "Throw-consume permanent default");
        wxButton* const originalFocus = new wxButton(
            &dialog, wxID_ANY, "Throw-consume temporary default");
        dialog.Show();
        originalDefault->SetDefault();
        dialog.SetTmpDefaultItem(originalFocus);
        originalFocus->SetFocus();
        REQUIRE(WaitFor("initial focus before throwing consuming insertion",
                        [originalFocus]()
                        {
                            return wxWindow::FindFocus() == originalFocus;
                        }));
        dialog.SetAffirmativeId(wxID_HELP);
        inner->Arm();

        CHECK_THROWS(dialog.CreateButtons(wxYES | wxCANCEL));

        CHECK(inner->GetConsumedInsertCount() == 1);
        CHECK(inner->GetItemCount() == 1);
        CHECK(CountDescendantsWithId(&dialog, wxID_YES) == 0);
        CHECK(CountDescendantsWithId(&dialog, wxID_CANCEL) == 0);
        CHECK(dialog.GetAffirmativeId() == wxID_HELP);
        CHECK(dialog.GetTmpDefaultItem() == originalFocus);
        CHECK(dialog.GetDefaultItem() == originalFocus);
        CHECK(wxWindow::FindFocus() == originalFocus);
        dialog.SetTmpDefaultItem(nullptr);
        CHECK(dialog.GetDefaultItem() == originalDefault);
    }
#endif // wxUSE_EXCEPTIONS

    SECTION("DPI_callback_topology_change_precedes_button_factory")
    {
        using Action = DpiMutatingPropertySheet::Action;
        wxFrame foreignOwner(parent, wxID_ANY, "DPI callback owner");
        for ( const Action action : { Action::ReparentOwner,
                                      Action::ReplaceTopSizer } )
        {
            CAPTURE(action);
            DpiMutatingPropertySheet dialog(action, &foreignOwner);
            REQUIRE(dialog.Create(parent,
                                  wxID_ANY,
                                  "DPI-mutating property sheet"));
            wxSizer* const originalTopSizer = dialog.GetSizer();
            wxButton* const originalDefault =
                new wxButton(&dialog, wxID_ANY, "Original DPI default");
            wxButton* const originalTmpDefault =
                new wxButton(&dialog, wxID_ANY, "Original DPI temporary");
            dialog.Show();
            originalDefault->SetDefault();
            dialog.SetTmpDefaultItem(originalTmpDefault);
            originalTmpDefault->SetFocus();
            REQUIRE(WaitFor("original focus before DPI callback",
                            [originalTmpDefault]()
                            {
                                return wxWindow::FindFocus() ==
                                       originalTmpDefault;
                            }));
            const int originalAffirmativeId = dialog.GetAffirmativeId();
            dialog.Arm();

            dialog.CreateButtons(wxYES | wxNO | wxCANCEL);

            CHECK(dialog.GetMutationCount() == 1);
            CHECK(CountDescendantsWithId(&dialog, wxID_YES) == 0);
            CHECK(CountDescendantsWithId(&dialog, wxID_NO) == 0);
            CHECK(CountDescendantsWithId(&dialog, wxID_CANCEL) == 0);
            CHECK(dialog.GetAffirmativeId() == originalAffirmativeId);
            CHECK(dialog.GetTmpDefaultItem() == originalTmpDefault);
            CHECK(dialog.GetDefaultItem() == originalTmpDefault);
            CHECK(wxWindow::FindFocus() == originalTmpDefault);
            if ( action == Action::ReparentOwner )
            {
                CHECK(dialog.GetParent() == &foreignOwner);
                CHECK(dialog.GetSizer() == originalTopSizer);
            }
            else
            {
                CHECK(dialog.GetParent() == parent);
                CHECK(dialog.GetSizer() != originalTopSizer);
                CHECK(dialog.GetOriginalTopSizer() == originalTopSizer);
            }
        }
    }

    SECTION("Button_sizer_rolls_back_focus_default_and_affirmative_state")
    {
        wxPanel foreignParent(parent);
        wxPropertySheetDialog dialog;
        REQUIRE(dialog.Create(parent,
                              wxID_ANY,
                              "Transactional property sheet buttons"));

        wxButton* const originalDefault =
            new wxButton(&dialog, wxID_ANY, "Original permanent default");
        wxButton* const originalTmpDefault =
            new wxButton(&dialog, wxID_ANY, "Original temporary default");
        dialog.Show();
        originalDefault->SetDefault();
        dialog.SetTmpDefaultItem(originalTmpDefault);
        originalTmpDefault->SetFocus();
        REQUIRE(WaitFor("original property sheet button focus",
                        [originalTmpDefault]()
                        {
                            return wxWindow::FindFocus() == originalTmpDefault;
                        }));
        const int originalAffirmativeId = dialog.GetAffirmativeId();

        int focusCallbacks = 0;
        wxWeakRef<wxWindow> weakMovedButton;
        dialog.Bind(wxEVT_CHILD_FOCUS,
                    [&](wxChildFocusEvent& event)
                    {
                        wxWindow* const focused = event.GetWindow();
                        if ( focused && focused->GetId() == wxID_YES &&
                             focusCallbacks == 0 )
                        {
                            ++focusCallbacks;
                            weakMovedButton = focused;
                            focused->Reparent(&foreignParent);
                        }
                        event.Skip();
                    });

        CHECK_NOTHROW(dialog.CreateButtons(wxYES | wxNO | wxCANCEL));

        CHECK(focusCallbacks == 1);
        wxButton* const movedButton =
            wxDynamicCast(weakMovedButton.get(), wxButton);
        REQUIRE(movedButton);
        CHECK(movedButton->GetParent() == &foreignParent);
        CHECK(movedButton->GetContainingSizer() == nullptr);
        CHECK(dialog.FindWindow(wxID_NO) == nullptr);
        CHECK(dialog.FindWindow(wxID_CANCEL) == nullptr);
        CHECK(dialog.GetAffirmativeId() == originalAffirmativeId);
        CHECK(dialog.GetTmpDefaultItem() == originalTmpDefault);
        CHECK(dialog.GetDefaultItem() == originalTmpDefault);
        CHECK(wxWindow::FindFocus() == originalTmpDefault);

        // Reveal and verify the permanent identity restored behind the
        // temporary override.
        dialog.SetTmpDefaultItem(nullptr);
        CHECK(dialog.GetDefaultItem() == originalDefault);
    }

#if defined(__WXWINUI__) && wxUSE_WINUI3
    SECTION("Button_sizer_stops_when_focus_schedules_dialog_destruction")
    {
        wxPropertySheetDialog* const dialog =
            new wxPropertySheetDialog;
        REQUIRE(dialog->Create(parent,
                               wxID_ANY,
                               "Destroyed property sheet buttons"));
        dialog->Show();

        int focusCallbacks = 0;
        wxWeakRef<wxWindow> weakFocusedButton;
        std::unique_ptr<wxWinUITLWHostWindowEventGuard> retainedCallback;
        dialog->Bind(wxEVT_CHILD_FOCUS,
                     [dialog, &focusCallbacks, &weakFocusedButton,
                      &retainedCallback](wxChildFocusEvent& event)
                     {
                         wxWindow* const focused = event.GetWindow();
                         if ( focused && focused->GetId() == wxID_YES &&
                              focusCallbacks == 0 )
                         {
                             ++focusCallbacks;
                             weakFocusedButton = focused;
                             retainedCallback.reset(
                                 new wxWinUITLWHostWindowEventGuard(dialog));
                             dialog->Destroy();
                         }
                         event.Skip();
                     });

        const wxWeakRef<wxWindow> weakDialog(dialog);
        dialog->CreateButtons(wxYES | wxNO | wxCANCEL);

        CHECK(focusCallbacks == 1);
        CHECK(wxWinUITLWHostIsDestroyScheduled(dialog));
        CHECK(weakFocusedButton.get() == nullptr);

        retainedCallback.reset();
        REQUIRE(WaitFor("destroyed property sheet button cleanup",
                        [&weakDialog]()
                        {
                            return weakDialog.get() == nullptr;
                        }));
    }
#endif

    SECTION("Apply_and_OK_honour_validation")
    {
        ValidatingPropertySheet dialog;
        REQUIRE(dialog.Create(parent, wxID_ANY, "Validated property sheet"));
        REQUIRE(dialog.GetBookCtrl()->AddPage(
            CreateSizedPage(dialog.GetBookCtrl(), "Validated page"),
            "Page",
            true));
        dialog.CreateButtons(wxOK | wxCANCEL | wxAPPLY);
        dialog.LayoutDialog(0);

        SendButtonCommand(&dialog, wxID_APPLY);
        CHECK(dialog.validateCalls == 1);
        CHECK(dialog.transferCalls == 0);

        dialog.allowValidation = true;
        SendButtonCommand(&dialog, wxID_APPLY);
        CHECK(dialog.validateCalls == 2);
        CHECK(dialog.transferCalls == 1);

        dialog.allowTransfer = false;
        SendButtonCommand(&dialog, wxID_APPLY);
        CHECK(dialog.validateCalls == 3);
        CHECK(dialog.transferCalls == 2);
        dialog.allowTransfer = true;

        dialog.allowValidation = false;
        bool firstOkCommand = false;
        bool secondOkCommand = false;
        dialog.CallAfter([&dialog, &firstOkCommand]()
                         {
                             firstOkCommand =
                                 CommandDialogButton(&dialog, wxID_OK);
                             if ( !firstOkCommand )
                                 dialog.EndModal(wxID_CANCEL);
                         });
        dialog.CallAfter([&dialog, &secondOkCommand]()
                         {
                             dialog.allowValidation = true;
                             secondOkCommand =
                                 CommandDialogButton(&dialog, wxID_OK);
                             if ( !secondOkCommand )
                                 dialog.EndModal(wxID_CANCEL);
                         });
        CHECK(dialog.ShowModal() == wxID_OK);
        CHECK(firstOkCommand);
        CHECK(secondOkCommand);
        CHECK(dialog.validateCalls == 5);
        CHECK(dialog.transferCalls == 3);
    }

    SECTION("Validation_cannot_overwrite_an_ended_modal_transaction")
    {
        for ( const bool useApply : { false, true } )
        {
            CAPTURE(useApply);
            EndingValidationPropertySheet dialog;
            REQUIRE(dialog.Create(parent,
                                  wxID_ANY,
                                  "Ending property sheet validation"));
            REQUIRE(dialog.GetBookCtrl()->AddPage(
                CreateSizedPage(dialog.GetBookCtrl(), "Ending page"),
                "Page",
                true));
            dialog.CreateButtons(useApply ? wxAPPLY : wxOK);
            dialog.LayoutDialog(0);

            bool commandSent = false;
            dialog.CallAfter([&dialog, &commandSent, useApply]()
            {
                commandSent = CommandDialogButton(
                    &dialog, useApply ? wxID_APPLY : wxID_OK);
                if ( !commandSent && dialog.IsModal() )
                    dialog.EndModal(wxID_CANCEL);
            });

            CHECK(dialog.ShowModal() == wxID_CANCEL);
            CHECK(commandSent);
            CHECK(dialog.validateCalls == 1);
            CHECK(dialog.transferCalls == 0);
        }
    }

    SECTION("Validation_callbacks_may_delete_or_destroy_the_dialog")
    {
        using Dialog = DestructiveValidationPropertySheet;

        for ( const Dialog::Action action :
              { Dialog::Action::DeleteInValidate,
                Dialog::Action::DeleteInTransfer } )
        {
            bool callbackRan = false;
            int validateCalls = 0;
            int transferCalls = 0;
            std::unique_ptr<Dialog> owner;
            owner.reset(new Dialog(action,
                                   &owner,
                                   &callbackRan,
                                   &validateCalls,
                                   &transferCalls));
            Dialog* const dialog = owner.get();
            REQUIRE(dialog->Create(parent,
                                   wxID_ANY,
                                   "Self-deleting property sheet"));
            const wxWeakRef<wxWindow> weakDialog(dialog);

            // Exercise wxDialogBase::AcceptAndClose() directly so that a
            // synchronous deletion cannot leave a queued button event with a
            // stale event object.
            dialog->AcceptForTest();

            CHECK(callbackRan);
            CHECK(owner == nullptr);
            CHECK(weakDialog.get() == nullptr);
            CHECK(validateCalls == 1);
            CHECK(transferCalls ==
                  (action == Dialog::Action::DeleteInTransfer ? 1 : 0));
        }

        for ( const Dialog::Action action :
              { Dialog::Action::DestroyInValidate,
                Dialog::Action::DestroyInTransfer } )
        {
            for ( const bool useApply : { false, true } )
            {
                CAPTURE(action, useApply);
                bool callbackRan = false;
                int validateCalls = 0;
                int transferCalls = 0;
                Dialog* const dialog =
                    new Dialog(action,
                               nullptr,
                               &callbackRan,
                               &validateCalls,
                               &transferCalls);
                REQUIRE(dialog->Create(parent,
                                       wxID_ANY,
                                       "Self-destroying property sheet"));
                if ( useApply )
                {
                    dialog->CreateButtons(wxAPPLY);
                    dialog->LayoutDialog(0);
                }
                const wxWeakRef<wxWindow> weakDialog(dialog);

#if defined(__WXWINUI__) && wxUSE_WINUI3
                {
                    // Keep Destroy() in the private retained-XAML queue long
                    // enough to prove that a live weak pointer is not a valid
                    // licence to continue the dialog transaction.
                    wxWinUITLWHostWindowEventGuard retainedCallback(dialog);
                    if ( useApply )
                        SendButtonCommand(dialog, wxID_APPLY);
                    else
                        dialog->AcceptForTest();

                    CHECK(wxWinUITLWHostIsDestroyScheduled(dialog));
                    CHECK(weakDialog.get() == dialog);
                    CHECK(callbackRan);
                    CHECK(validateCalls == 1);
                    CHECK(transferCalls ==
                          (action == Dialog::Action::DestroyInTransfer
                               ? 1
                               : 0));
                }
#else
                if ( useApply )
                    SendButtonCommand(dialog, wxID_APPLY);
                else
                    dialog->AcceptForTest();

                CHECK(callbackRan);
                CHECK(validateCalls == 1);
                CHECK(transferCalls ==
                      (action == Dialog::Action::DestroyInTransfer ? 1 : 0));
#endif

                REQUIRE(WaitFor("self-destroying property sheet cleanup",
                                [&weakDialog]()
                                {
                                    return weakDialog.get() == nullptr;
                                }));
            }
        }
    }

    SECTION("Direct_constructor_and_owned_children_destroy_cleanly")
    {
        wxWeakRef<wxWindow> weakBook;
        wxWeakRef<wxWindow> weakPage;
        {
            std::unique_ptr<wxPropertySheetDialog> dialog(
                new wxPropertySheetDialog(parent,
                                          wxID_ANY,
                                          "Direct property sheet"));
            REQUIRE(dialog->GetBookCtrl());
            wxPanel* const page =
                CreateSizedPage(dialog->GetBookCtrl(), "Owned page");
            REQUIRE(dialog->GetBookCtrl()->AddPage(page, "Page", true));
            weakBook = dialog->GetBookCtrl();
            weakPage = page;
        }

        CHECK(weakPage.get() == nullptr);
        CHECK(weakBook.get() == nullptr);
    }

    SECTION("OK_and_Cancel_keep_standard_modal_results")
    {
        wxPropertySheetDialog dialog;
        REQUIRE(dialog.Create(parent, wxID_ANY, "Modal property sheet"));
        REQUIRE(dialog.GetBookCtrl()->AddPage(
            CreateSizedPage(dialog.GetBookCtrl(), "Modal page"),
            "Page",
            true));
        dialog.CreateButtons(wxOK | wxCANCEL);
        dialog.LayoutDialog(0);

        bool okCommand = false;
        dialog.CallAfter([&dialog, &okCommand]()
                         {
                             okCommand = CommandDialogButton(&dialog, wxID_OK);
                             if ( !okCommand )
                                 dialog.EndModal(wxID_CANCEL);
                         });
        CHECK(dialog.ShowModal() == wxID_OK);
        CHECK(okCommand);

        bool cancelCommand = false;
        dialog.CallAfter([&dialog, &cancelCommand]()
                         {
                             cancelCommand =
                                 CommandDialogButton(&dialog, wxID_CANCEL);
                             if ( !cancelCommand )
                                 dialog.EndModal(wxID_CANCEL);
                         });
        CHECK(dialog.ShowModal() == wxID_CANCEL);
        CHECK(cancelCommand);
    }

#if wxUSE_XRC
    SECTION("XRC_default_Create_pages_and_all_standard_buttons")
    {
        static const char xrcText[] = R"XRC(
<resource xmlns="http://www.wxwidgets.org/wxxrc" version="2.5.3.0">
  <object class="wxPropertySheetDialog" name="phase013PropertySheet">
    <title>XRC property sheet</title>
    <buttons>wxOK|wxCANCEL|wxAPPLY|wxHELP</buttons>
    <object class="propertysheetpage">
      <label>First page</label>
      <selected>1</selected>
      <object class="wxPanel" name="phase013PropertySheetPage"/>
    </object>
  </object>
</resource>
)XRC";
        static const wxString xrcName =
            "phase013-property-sheet-contract.xrc";
        wxXmlResource* const xrc = wxXmlResource::Get();
        xrc->InitAllHandlers();
        wxStringInputStream stream(wxString::FromUTF8(xrcText));
        std::unique_ptr<wxXmlDocument> document(new wxXmlDocument(stream));
        REQUIRE(document->IsOk());
        REQUIRE(xrc->LoadDocument(document.release(), xrcName));
        XrcUnloadGuard unload(xrc, xrcName);

        NullBookPropertySheet callerOwnedDialog;
        const wxWeakRef<wxWindow> weakCallerOwned(&callerOwnedDialog);
        {
            wxLogNull suppressExpectedCreateError;
            CHECK_FALSE(xrc->LoadObject(&callerOwnedDialog,
                                        parent,
                                        "phase013PropertySheet",
                                        "wxPropertySheetDialog"));
        }
        CHECK(weakCallerOwned.get() == &callerOwnedDialog);
        CHECK_FALSE(callerOwnedDialog.IsBeingDeleted());
        CHECK(callerOwnedDialog.GetBookCtrl() == nullptr);

        wxPropertySheetDialog* const dialog = wxDynamicCast(
            xrc->LoadObject(parent,
                            "phase013PropertySheet",
                            "wxPropertySheetDialog"),
            wxPropertySheetDialog);
        REQUIRE(dialog);
        std::unique_ptr<wxPropertySheetDialog> ownedDialog(dialog);
        REQUIRE(dialog->GetBookCtrl());
        CHECK(dialog->GetBookCtrl()->GetPageCount() == 1);
        CHECK(dialog->GetBookCtrl()->GetSelection() == 0);
        CHECK(dialog->FindWindow(wxID_OK));
        CHECK(dialog->FindWindow(wxID_CANCEL));
        CHECK(dialog->FindWindow(wxID_APPLY));
        CHECK(dialog->FindWindow(wxID_HELP));
    }

    SECTION("XRC_maps_wxCLOSE_to_the_standard_close_button")
    {
        static const char xrcText[] = R"XRC(
<resource xmlns="http://www.wxwidgets.org/wxxrc" version="2.5.3.0">
  <object class="wxPropertySheetDialog" name="phase013CloseSheet">
    <title>XRC close property sheet</title>
    <buttons>wxCLOSE</buttons>
    <object class="propertysheetpage">
      <label>Close page</label>
      <object class="wxPanel" name="phase013CloseSheetPage"/>
    </object>
  </object>
</resource>
)XRC";
        static const wxString xrcName =
            "phase013-close-property-sheet.xrc";
        wxXmlResource* const xrc = wxXmlResource::Get();
        xrc->InitAllHandlers();
        wxStringInputStream stream(wxString::FromUTF8(xrcText));
        std::unique_ptr<wxXmlDocument> document(new wxXmlDocument(stream));
        REQUIRE(document->IsOk());
        REQUIRE(xrc->LoadDocument(document.release(), xrcName));
        XrcUnloadGuard unload(xrc, xrcName);

        wxPropertySheetDialog* const dialog = wxDynamicCast(
            xrc->LoadObject(parent,
                            "phase013CloseSheet",
                            "wxPropertySheetDialog"),
            wxPropertySheetDialog);
        REQUIRE(dialog);
        std::unique_ptr<wxPropertySheetDialog> ownedDialog(dialog);
        wxButton* const closeButton =
            wxDynamicCast(dialog->FindWindow(wxID_CLOSE), wxButton);
        REQUIRE(closeButton);
        CHECK(CountDescendantsWithId(dialog, wxID_CLOSE) == 1);
        CHECK(CountDescendantsWithId(dialog, wxID_OK) == 0);
        CHECK(dialog->GetAffirmativeId() == wxID_CLOSE);
        CHECK(closeButton->GetContainingSizer() != nullptr);
    }

    SECTION("XRC_freezes_owner_and_sizers_through_Setup_and_Layout")
    {
        static const char xrcText[] = R"XRC(
<resource xmlns="http://www.wxwidgets.org/wxxrc" version="2.5.3.0">
  <object class="wxPropertySheetDialog" name="phase013TopologySheet">
    <title>Topology-mutating XRC property sheet</title>
    <bg>#112233</bg>
    <buttons>wxYES|wxCANCEL</buttons>
    <object class="propertysheetpage">
      <label>Topology page</label>
      <object class="wxPanel" name="phase013TopologyPage"/>
    </object>
  </object>
</resource>
)XRC";
        static const wxString xrcName =
            "phase013-topology-property-sheet.xrc";
        wxXmlResource* const xrc = wxXmlResource::Get();
        xrc->InitAllHandlers();
        wxStringInputStream stream(wxString::FromUTF8(xrcText));
        std::unique_ptr<wxXmlDocument> document(new wxXmlDocument(stream));
        REQUIRE(document->IsOk());
        REQUIRE(xrc->LoadDocument(document.release(), xrcName));
        XrcUnloadGuard unload(xrc, xrcName);

        using Action = TopologyMutatingXrcPropertySheet::Action;
        wxFrame foreignOwner(parent, wxID_ANY, "XRC topology owner");
        for ( const Action action :
              { Action::SetupReparentOwner,
                Action::SetupReplaceTopSizer,
                Action::LayoutReparentOwner,
                Action::LayoutReplaceTopSizer } )
        {
            CAPTURE(action);
            TopologyMutatingXrcPropertySheet dialog(action, &foreignOwner);
            bool loaded = false;
            {
                wxLogNull suppressExpectedCreateError;
                loaded = xrc->LoadObject(&dialog,
                                         parent,
                                         "phase013TopologySheet",
                                         "wxPropertySheetDialog");
            }

            CHECK_FALSE(loaded);
            CHECK_FALSE(dialog.IsBeingDeleted());
            CHECK(dialog.GetMutationCount() == 1);
            const bool reparented =
                action == Action::SetupReparentOwner ||
                action == Action::LayoutReparentOwner;
            if ( reparented )
            {
                CHECK(dialog.GetParent() == &foreignOwner);
                CHECK(dialog.GetOriginalTopSizer() == nullptr);
            }
            else
            {
                CHECK(dialog.GetParent() == parent);
                REQUIRE(dialog.GetOriginalTopSizer());
                CHECK(dialog.GetSizer() != dialog.GetOriginalTopSizer());
            }

            const bool reachedLayout =
                action == Action::LayoutReparentOwner ||
                action == Action::LayoutReplaceTopSizer;
            CHECK(CountDescendantsWithId(&dialog, wxID_YES) ==
                  (reachedLayout ? 1 : 0));
            CHECK(CountDescendantsWithId(&dialog, wxID_CANCEL) ==
                  (reachedLayout ? 1 : 0));
        }
    }

    SECTION("XRC_book_replacement_invalidates_page_transaction")
    {
        static const char xrcText[] = R"XRC(
<resource xmlns="http://www.wxwidgets.org/wxxrc" version="2.5.3.0">
  <object class="wxPropertySheetDialog" name="phase013ReplacingBookSheet">
    <title>Replacing book XRC property sheet</title>
    <object class="propertysheetpage">
      <label>Replacing page</label>
      <object class="wxPanel"
              subclass="ReplacingXrcPropertySheetBookPage"
              name="phase013ReplacingBookPage">
        <bg>#112233</bg>
      </object>
    </object>
  </object>
</resource>
)XRC";
        static const wxString xrcName =
            "phase013-replacing-book-property-sheet.xrc";
        wxXmlResource* const xrc = wxXmlResource::Get();
        xrc->InitAllHandlers();
        wxStringInputStream stream(wxString::FromUTF8(xrcText));
        std::unique_ptr<wxXmlDocument> document(new wxXmlDocument(stream));
        REQUIRE(document->IsOk());
        REQUIRE(xrc->LoadDocument(document.release(), xrcName));
        XrcUnloadGuard unload(xrc, xrcName);

        wxPropertySheetDialog dialog;
        wxWeakRef<wxWindow> weakOriginalBook;
        ReplacingXrcPropertySheetBookPage::Reset(&weakOriginalBook);
        bool loaded = false;
        {
            wxLogNull suppressExpectedCreateError;
            loaded = xrc->LoadObject(&dialog,
                                     parent,
                                     "phase013ReplacingBookSheet",
                                     "wxPropertySheetDialog");
        }
        ReplacingXrcPropertySheetBookPage::ClearStorage();

        CHECK_FALSE(loaded);
        CHECK(dialog.GetBookCtrl() == nullptr);
        REQUIRE(weakOriginalBook);
        wxBookCtrlBase* const originalBook = wxDynamicCast(
            weakOriginalBook.get(), wxBookCtrlBase);
        REQUIRE(originalBook);
        CHECK(originalBook->GetParent() == &dialog);
        CHECK(originalBook->GetPageCount() == 0);
        CHECK(dialog.FindWindow(XRCID("phase013ReplacingBookPage")) ==
              nullptr);
        CHECK(ReplacingXrcPropertySheetBookPage::GetConstructedCount() == 1);
        CHECK(ReplacingXrcPropertySheetBookPage::GetDestroyedCount() == 1);
    }

    SECTION("XRC_book_reparent_invalidates_and_rolls_back_page_transaction")
    {
        static const char xrcText[] = R"XRC(
<resource xmlns="http://www.wxwidgets.org/wxxrc" version="2.5.3.0">
  <object class="wxPropertySheetDialog" name="phase013ReparentingBookSheet">
    <title>Reparenting book XRC property sheet</title>
    <object class="propertysheetpage">
      <label>Reparenting page</label>
      <object class="wxPanel"
              subclass="ReparentingXrcPropertySheetBookPage"
              name="phase013ReparentingBookPage">
        <bg>#112233</bg>
      </object>
    </object>
  </object>
</resource>
)XRC";
        static const wxString xrcName =
            "phase013-reparenting-book-property-sheet.xrc";
        wxXmlResource* const xrc = wxXmlResource::Get();
        xrc->InitAllHandlers();
        wxStringInputStream stream(wxString::FromUTF8(xrcText));
        std::unique_ptr<wxXmlDocument> document(new wxXmlDocument(stream));
        REQUIRE(document->IsOk());
        REQUIRE(xrc->LoadDocument(document.release(), xrcName));
        XrcUnloadGuard unload(xrc, xrcName);

        wxPanel foreignParent(parent);
        wxPropertySheetDialog dialog;
        wxWeakRef<wxWindow> weakOriginalBook;
        ReparentingXrcPropertySheetBookPage::Reset(
            &foreignParent, &weakOriginalBook);
        bool loaded = false;
        {
            wxLogNull suppressExpectedCreateError;
            loaded = xrc->LoadObject(&dialog,
                                     parent,
                                     "phase013ReparentingBookSheet",
                                     "wxPropertySheetDialog");
        }
        ReparentingXrcPropertySheetBookPage::ClearStorage();

        CHECK_FALSE(loaded);
        CHECK(dialog.GetBookCtrl() == nullptr);
        wxBookCtrlBase* const originalBook = wxDynamicCast(
            weakOriginalBook.get(), wxBookCtrlBase);
        REQUIRE(originalBook);
        CHECK(originalBook->GetParent() == &foreignParent);
        CHECK(originalBook->GetContainingSizer() == nullptr);
        CHECK(originalBook->GetPageCount() == 0);
        CHECK(dialog.FindWindow(XRCID("phase013ReparentingBookPage")) ==
              nullptr);
        CHECK(ReparentingXrcPropertySheetBookPage::GetReparentCalls() == 1);
        CHECK(ReparentingXrcPropertySheetBookPage::GetConstructedCount() == 1);
        CHECK(ReparentingXrcPropertySheetBookPage::GetDestroyedCount() == 1);
    }

#if defined(__WXWINUI__) && wxUSE_WINUI3
    SECTION("XRC_button_and_centre_callbacks_may_schedule_destruction")
    {
        static const char xrcText[] = R"XRC(
<resource xmlns="http://www.wxwidgets.org/wxxrc" version="2.5.3.0">
  <object class="wxPropertySheetDialog" name="phase013CallbackSheet">
    <title>Callback XRC property sheet</title>
    <buttons>wxOK|wxCANCEL</buttons>
    <centered>1</centered>
    <object class="propertysheetpage">
      <label>Page</label>
      <object class="wxPanel" name="phase013CallbackSheetPage"/>
    </object>
  </object>
</resource>
)XRC";
        static const wxString xrcName =
            "phase013-callback-property-sheet.xrc";
        wxXmlResource* const xrc = wxXmlResource::Get();
        xrc->InitAllHandlers();
        wxStringInputStream stream(wxString::FromUTF8(xrcText));
        std::unique_ptr<wxXmlDocument> document(new wxXmlDocument(stream));
        REQUIRE(document->IsOk());
        REQUIRE(xrc->LoadDocument(document.release(), xrcName));
        XrcUnloadGuard unload(xrc, xrcName);

        using Action = DestroyingXrcPropertySheet::Action;
        for ( const Action action : { Action::CreateButtons,
                                      Action::Centre } )
        {
            int buttonCalls = 0;
            int layoutCalls = 0;
            int centreCalls = 0;
            std::unique_ptr<wxWinUITLWHostWindowEventGuard>
                retainedCallback;
            DestroyingXrcPropertySheet* const dialog =
                new DestroyingXrcPropertySheet(action,
                                               &buttonCalls,
                                               &layoutCalls,
                                               &centreCalls,
                                               &retainedCallback);
            const wxWeakRef<wxWindow> weakDialog(dialog);
            {
                wxLogNull suppressExpectedCreateError;
                CHECK_FALSE(xrc->LoadObject(dialog,
                                            parent,
                                            "phase013CallbackSheet",
                                            "wxPropertySheetDialog"));
                CHECK(wxWinUITLWHostIsDestroyScheduled(dialog));
                CHECK(buttonCalls == 1);
                CHECK(layoutCalls ==
                      (action == Action::Centre ? 1 : 0));
                CHECK(centreCalls ==
                      (action == Action::Centre ? 1 : 0));
            }

            retainedCallback.reset();
            REQUIRE(WaitFor("callback XRC property sheet cleanup",
                            [&weakDialog]()
                            {
                                return weakDialog.get() == nullptr;
                            }));
        }
    }
#endif

    SECTION("XRC_page_destroying_property_sheet_aborts_remaining_pages")
    {
        static const char xrcText[] = R"XRC(
<resource xmlns="http://www.wxwidgets.org/wxxrc" version="2.5.3.0">
  <object class="wxPropertySheetDialog" name="phase013DestructiveSheet">
    <title>Destructive XRC property sheet</title>
    <object class="propertysheetpage">
      <label>Destructive page</label>
      <object class="wxPanel"
              subclass="DestroyingXrcBookPage"
              name="phase013DestroyingBookPage">
        <bg>#112233</bg>
        <fg>#445566</fg>
      </object>
    </object>
    <object class="propertysheetpage">
      <label>Must not be created</label>
      <object class="wxPanel"
              subclass="DestroyingXrcBookPage"
              name="phase013UnexpectedBookPage"/>
    </object>
  </object>
</resource>
)XRC";
        static const wxString xrcName =
            "phase013-destructive-property-sheet.xrc";
        wxXmlResource* const xrc = wxXmlResource::Get();
        xrc->InitAllHandlers();
        wxStringInputStream stream(wxString::FromUTF8(xrcText));
        std::unique_ptr<wxXmlDocument> document(new wxXmlDocument(stream));
        REQUIRE(document->IsOk());
        REQUIRE(xrc->LoadDocument(document.release(), xrcName));
        XrcUnloadGuard unload(xrc, xrcName);

        DestroyingXrcBookPage::Reset();
        wxPropertySheetDialog* const dialog = new wxPropertySheetDialog;
        const wxWeakRef<wxWindow> weakDialog(dialog);
#if defined(__WXWINUI__) && wxUSE_WINUI3
        std::unique_ptr<wxWinUITLWHostWindowEventGuard> retainedCallback;
        DestroyingXrcBookPage::SetRetainedGuardStorage(&retainedCallback);
#endif
        {
            wxLogNull suppressExpectedCreateError;
            CHECK_FALSE(xrc->LoadObject(dialog,
                                        parent,
                                        "phase013DestructiveSheet",
                                        "wxPropertySheetDialog"));
        }
        CHECK(DestroyingXrcBookPage::GetConstructedCount() == 1);
        CHECK(DestroyingXrcBookPage::GetForegroundCalls() == 0);
#if defined(__WXWINUI__) && wxUSE_WINUI3
        CHECK(wxWinUITLWHostIsDestroyScheduled(dialog));
        DestroyingXrcBookPage::SetRetainedGuardStorage(nullptr);
        retainedCallback.reset();
#endif
        REQUIRE(WaitFor("destructive XRC property sheet cleanup",
                        [&weakDialog]()
                        {
                            return weakDialog.get() == nullptr;
                        }));
        CHECK(DestroyingXrcBookPage::GetDestroyedCount() == 1);
    }
#endif
}

#endif // wxUSE_BOOKCTRL

#if wxUSE_PREFERENCES_EDITOR

TEST_CASE("WinUIPreferencesEditorContracts",
          "[preferences][winui-beta-dialogs]")
{
    wxWindow* const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

#ifdef wxHAS_PREF_EDITOR_MODELESS
    SECTION("Modeless_hidden_dialog_replacement_is_one_transaction")
    {
        const wxString title =
            "Phase 013 modeless preferences replacement";
        wxPreferencesEditor editor(title);
        editor.AddPage(new TrackingPreferencesPage("General"));

        wxFrame firstOwner(parent, wxID_ANY, "First preferences owner");
        wxFrame secondOwner(parent, wxID_ANY, "Second preferences owner");
        firstOwner.Show();
        secondOwner.Show();

        editor.Show(&firstOwner);
        wxDialog* const firstDialog = FindDialogWithTitle(title);
        REQUIRE(firstDialog);
        REQUIRE(firstDialog->GetParent() == &firstOwner);

        editor.Dismiss();
        REQUIRE_FALSE(firstDialog->IsShown());

        int nestedShowCalls = 0;
        firstDialog->Bind(
            wxEVT_DESTROY,
            [&](wxWindowDestroyEvent& event)
            {
                event.Skip();
                if ( event.GetWindow() != firstDialog )
                    return;

                ++nestedShowCalls;
                editor.Show(&secondOwner);
            });

        editor.Show(&secondOwner);
        REQUIRE(WaitFor("old modeless preferences dialog cleanup",
                        [firstDialog, title]()
                        {
                            return FindDialogWithTitle(title) != firstDialog;
                        }));

        wxDialog* const replacement = FindDialogWithTitle(title);
        REQUIRE(replacement);
        CHECK(replacement->GetParent() == &secondOwner);
        CHECK(CountDialogsWithTitle(title) == 1);
        CHECK(nestedShowCalls == 1);
        editor.Dismiss();
    }

    SECTION("Modeless_replacement_weakly_snapshots_the_requested_owner")
    {
        const wxString title =
            "Phase 013 modeless preferences owner callback";
        wxPreferencesEditor editor(title);
        editor.AddPage(new TrackingPreferencesPage("General"));

        wxFrame firstOwner(parent, wxID_ANY,
                           "Modeless previous preferences owner");
        firstOwner.Show();
        editor.Show(&firstOwner);
        wxDialog* const previous = FindDialogWithTitle(title);
        REQUIRE(previous);
        editor.Dismiss();

        wxFrame* requestedOwner = new wxFrame(
            parent, wxID_ANY, "Modeless requested preferences owner");
        requestedOwner->Show();
        wxWindow* const requestedIdentity = requestedOwner;
        const wxWeakRef<wxWindow> weakRequestedOwner(requestedOwner);
        previous->Bind(
            wxEVT_DESTROY,
            [requestedOwner](wxWindowDestroyEvent& event)
            {
                event.Skip();
                delete requestedOwner;
            });

        // Destroying the previous hidden dialog dispatches the callback above.
        // Show() must validate the weak snapshot captured before Destroy()
        // instead of dereferencing this now-stale raw argument afterwards.
        editor.Show(requestedIdentity);
        CHECK_FALSE(weakRequestedOwner);
        CHECK(FindDialogWithTitle(title) == nullptr);
    }
#else
    SECTION("CreateWindow_stale_or_transferred_returns_abort_safely")
    {
        const wxString staleTitle =
            "Phase 013 stale preferences page return";
        wxPreferencesEditor staleEditor(staleTitle);
        bool staleCallbackRan = false;
        bool stalePageWasDestroyed = false;
        staleEditor.AddPage(new TrackingPreferencesPage(
            "Stale return",
            nullptr,
            [&](wxWindow* page)
            {
                staleCallbackRan = true;
                const wxWeakRef<wxWindow> pageLifetime(page);
                delete page;
                stalePageWasDestroyed = !pageLifetime;
            }));

        wxAssertHandler_t oldAssertHandler = wxSetAssertHandler(nullptr);
        {
            wxLogNull suppressExpectedContractError;
            staleEditor.Show(parent);
        }
        wxSetAssertHandler(oldAssertHandler);
        CHECK(staleCallbackRan);
        CHECK(stalePageWasDestroyed);
        CHECK(FindDialogWithTitle(staleTitle) == nullptr);

        const wxString transferredTitle =
            "Phase 013 transferred preferences page return";
        wxPreferencesEditor transferredEditor(transferredTitle);
        wxPanel foreignParent(parent);
        wxWeakRef<wxWindow> transferredPage;
        bool reparented = false;
        transferredEditor.AddPage(new TrackingPreferencesPage(
            "Transferred return",
            nullptr,
            [&](wxWindow* page)
            {
                transferredPage = page;
                reparented = page->Reparent(&foreignParent);
            }));

        oldAssertHandler = wxSetAssertHandler(nullptr);
        {
            wxLogNull suppressExpectedContractError;
            transferredEditor.Show(parent);
        }
        wxSetAssertHandler(oldAssertHandler);
        REQUIRE(reparented);
        REQUIRE(transferredPage);
        CHECK(transferredPage->GetParent() == &foreignParent);
        CHECK(FindDialogWithTitle(transferredTitle) == nullptr);
    }

    SECTION("Notebook_parent_identity_is_revalidated_after_callbacks")
    {
        const wxString title =
            "Phase 013 reparented preferences notebook";
        wxPreferencesEditor editor(title);
        wxPanel foreignParent(parent);
        wxWeakRef<wxNotebook> transferredNotebook;
        wxWeakRef<wxWindow> transferredPage;
        bool reparented = false;
        bool rescueRan = false;
        bool modalWasEntered = false;

        editor.AddPage(new TrackingPreferencesPage(
            "Transferred notebook",
            nullptr,
            [&](wxWindow* page)
            {
                transferredPage = page;
            },
            [&]()
            {
                wxDialog* const dialog = FindDialogWithTitle(title);
                wxNotebook* const notebook =
                    dialog ? FindDescendantOfType<wxNotebook>(dialog)
                           : nullptr;
                REQUIRE(notebook);
                transferredNotebook = notebook;
                reparented = notebook->Reparent(&foreignParent);
            }));

        // This is only a watchdog for the broken implementation, which used
        // to enter ShowModal() after losing ownership of its notebook. The
        // fixed transaction returns before this callback is dispatched.
        wxTheApp->CallAfter([&]()
        {
            rescueRan = true;
            wxDialog* const dialog = FindDialogWithTitle(title);
            modalWasEntered = dialog && dialog->IsModal();
            if ( dialog && dialog->IsModal() )
                dialog->EndModal(wxID_CANCEL);
        });

        editor.Show(parent);
        REQUIRE(WaitFor("reparented notebook watchdog",
                        [&rescueRan]() { return rescueRan; }));
        REQUIRE(reparented);
        REQUIRE(transferredNotebook);
        REQUIRE(transferredPage);
        CHECK(transferredNotebook->GetParent() == &foreignParent);
        CHECK(transferredPage->GetParent() == transferredNotebook.get());
        CHECK_FALSE(modalWasEntered);
        CHECK(FindDialogWithTitle(title) == nullptr);
    }

    SECTION("Nested_Show_does_not_publish_a_half_built_dialog")
    {
        const wxString title =
            "Phase 013 preferences nested construction";
        wxPreferencesEditor editor(title);
        bool createCallbackRan = false;
        bool dialogWasVisibleBeforeNestedShow = true;
        bool dialogWasVisibleAfterNestedShow = true;
        size_t dialogCountAfterNestedShow = 0;
        editor.AddPage(new TrackingPreferencesPage(
            "Nested construction",
            nullptr,
            [&](wxWindow*)
            {
                createCallbackRan = true;
                wxDialog* const dialog = FindDialogWithTitle(title);
                REQUIRE(dialog);
                dialogWasVisibleBeforeNestedShow = dialog->IsShown();
                editor.Show(parent);
                dialogWasVisibleAfterNestedShow = dialog->IsShown();
                dialogCountAfterNestedShow = CountDialogsWithTitle(title);
            }));

        bool closed = false;
        wxTheApp->CallAfter([&]()
        {
            wxDialog* const dialog = FindDialogWithTitle(title);
            closed = CommandDialogButton(dialog, wxID_CANCEL);
            if ( dialog && !closed )
                dialog->EndModal(wxID_CANCEL);
            else if ( !dialog )
                AbortAnyModalDialog();
        });

        editor.Show(parent);
        CHECK(createCallbackRan);
        CHECK_FALSE(dialogWasVisibleBeforeNestedShow);
        CHECK_FALSE(dialogWasVisibleAfterNestedShow);
        CHECK(dialogCountAfterNestedShow == 1);
        CHECK(closed);
        CHECK(FindDialogWithTitle(title) == nullptr);
    }

    SECTION("Init_nested_Show_and_preshow_Dismiss_use_one_modal_session")
    {
        const wxString title =
            "Phase 013 preferences init reentrancy";
        wxPreferencesEditor editor(title);
        int initCalls = 0;
        size_t dialogsDuringInit = 0;
        bool nestedShowReturned = false;
        wxWeakRef<wxWindow> weakDialog;

        editor.AddPage(new TrackingPreferencesPage(
            "Init reentrancy",
            nullptr,
            [&](wxWindow* page)
            {
                wxDialog* const dialog = wxDynamicCast(
                    page->GetParent() ? page->GetParent()->GetParent()
                                      : nullptr,
                    wxDialog);
                REQUIRE(dialog);
                weakDialog = dialog;
                dialog->Bind(
                    wxEVT_INIT_DIALOG,
                    [&](wxInitDialogEvent& event)
                    {
                        event.Skip();
                        ++initCalls;
                        dialogsDuringInit = CountDialogsWithTitle(title);

                        // The nested call occurs before wxDialog::Show() has
                        // made the TLW visible. It must neither call Show()
                        // (and InitDialog) again nor enter another modal loop.
                        editor.Show(parent);
                        nestedShowReturned = true;

                        // EndModal() is ineffective this early because the
                        // outer Show() resumes afterwards. Dismiss() must queue
                        // one weak close for the imminent modal loop.
                        editor.Dismiss();
                    });
            }));

        editor.Show(parent);
        CHECK(nestedShowReturned);
        CHECK(initCalls == 1);
        CHECK(dialogsDuringInit == 1);
        CHECK_FALSE(weakDialog);
        CHECK(FindDialogWithTitle(title) == nullptr);
    }

#if defined(__WXWINUI__) && wxUSE_WINUI3
    SECTION("Owner_destroyed_from_Init_is_deferred_until_modal_unwind")
    {
        const wxString title =
            "Phase 013 preferences init owner destruction";
        wxFrame* owner = new wxFrame(
            parent, wxID_ANY, "Init-destroyed preferences owner");
        owner->Show();
        wxWindow* const ownerIdentity = owner;
        const wxWeakRef<wxWindow> weakOwner(owner);

        wxPreferencesEditor editor(title);
        wxWeakRef<wxWindow> weakDialog;
        bool initRan = false;
        bool dialogAliveAfterOwnerDelete = false;
        editor.AddPage(new TrackingPreferencesPage(
            "Init owner destruction",
            nullptr,
            [&](wxWindow* page)
            {
                wxDialog* const dialog = wxDynamicCast(
                    page->GetParent() ? page->GetParent()->GetParent()
                                      : nullptr,
                    wxDialog);
                REQUIRE(dialog);
                weakDialog = dialog;
                dialog->Bind(
                    wxEVT_INIT_DIALOG,
                    [&](wxInitDialogEvent& event)
                    {
                        event.Skip();
                        initRan = true;
                        delete owner;
                        owner = nullptr;
                        dialogAliveAfterOwnerDelete =
                            weakDialog.get() != nullptr;
                    });
            }));

        editor.Show(ownerIdentity);
        CHECK(initRan);
        CHECK_FALSE(weakOwner);
        CHECK(dialogAliveAfterOwnerDelete);
        REQUIRE(WaitFor("init owner preferences dialog cleanup",
                        [&weakDialog]()
                        {
                            return weakDialog.get() == nullptr;
                        }));
    }


    SECTION("Init_reparent_then_old_owner_destroy_preserves_new_ownership")
    {
        const wxString title =
            "Phase 013 preferences init owner transfer";
        wxFrame* oldOwner = new wxFrame(
            parent, wxID_ANY, "Init-transferred preferences owner");
        oldOwner->Show();
        wxWindow* const oldOwnerIdentity = oldOwner;
        const wxWeakRef<wxWindow> weakOldOwner(oldOwner);
        wxFrame newOwner(parent, wxID_ANY,
                         "Init preferences new owner");
        newOwner.Show();

        wxPreferencesEditor editor(title);
        wxWeakRef<wxWindow> transferredDialog;
        bool initRan = false;
        bool reparented = false;
        editor.AddPage(new TrackingPreferencesPage(
            "Init owner transfer",
            nullptr,
            [&](wxWindow* page)
            {
                wxDialog* const dialog = wxDynamicCast(
                    page->GetParent() ? page->GetParent()->GetParent()
                                      : nullptr,
                    wxDialog);
                REQUIRE(dialog);
                transferredDialog = dialog;
                dialog->Bind(
                    wxEVT_INIT_DIALOG,
                    [&, dialog](wxInitDialogEvent& event)
                    {
                        event.Skip();
                        initRan = true;
                        reparented = dialog->Reparent(&newOwner);
                        delete oldOwner;
                        oldOwner = nullptr;
                    });
            }));

        editor.Show(oldOwnerIdentity);
        CHECK(initRan);
        REQUIRE(reparented);
        CHECK_FALSE(weakOldOwner);
        REQUIRE(transferredDialog);
        CHECK(transferredDialog->GetParent() == &newOwner);

        delete transferredDialog.get();
        CHECK_FALSE(transferredDialog);
    }
#endif

    SECTION("Dismiss_hide_callback_cannot_open_a_second_session")
    {
        const wxString title =
            "Phase 013 preferences dismiss reentrancy";
        wxPreferencesEditor editor(title);
        editor.AddPage(new TrackingPreferencesPage("Dismiss reentrancy"));

        int hideCalls = 0;
        size_t dialogsAfterNestedShow = 0;
        wxWeakRef<wxWindow> weakDialog;
        wxTheApp->CallAfter([&]()
        {
            wxDialog* const dialog = FindDialogWithTitle(title);
            REQUIRE(dialog);
            weakDialog = dialog;
            dialog->Bind(
                wxEVT_SHOW,
                [&](wxShowEvent& event)
                {
                    event.Skip();
                    if ( event.IsShown() )
                        return;

                    ++hideCalls;
                    editor.Show(parent);
                    dialogsAfterNestedShow = CountDialogsWithTitle(title);
                });

            editor.Dismiss();
        });

        editor.Show(parent);
        CHECK(hideCalls == 1);
        CHECK(dialogsAfterNestedShow == 1);
        CHECK_FALSE(weakDialog);
        CHECK(FindDialogWithTitle(title) == nullptr);
    }

    SECTION("Reparented_dialog_is_relinquished_to_its_new_owner")
    {
        const wxString title =
            "Phase 013 transferred preferences dialog";
        wxFrame originalOwner(parent, wxID_ANY,
                              "Original preferences owner");
        wxFrame newOwner(parent, wxID_ANY,
                         "Transferred preferences owner");
        originalOwner.Show();
        newOwner.Show();
        const size_t originalOwnerEntries =
            CountDynamicEventEntries(&originalOwner);

        wxPreferencesEditor editor(title);
        wxWeakRef<wxWindow> transferredDialog;
        bool reparented = false;
        editor.AddPage(new TrackingPreferencesPage(
            "Transferred dialog",
            nullptr,
            TrackingPreferencesPage::CreateCallback(),
            [&]()
            {
                wxDialog* const dialog = FindDialogWithTitle(title);
                REQUIRE(dialog);
                transferredDialog = dialog;
                reparented = dialog->Reparent(&newOwner);
            }));

        editor.Show(&originalOwner);
        REQUIRE(reparented);
        REQUIRE(transferredDialog);
        CHECK(transferredDialog->GetParent() == &newOwner);
        wxDialog* const transferred =
            wxDynamicCast(transferredDialog.get(), wxDialog);
        REQUIRE(transferred);
        CHECK_FALSE(transferred->IsModal());
        CHECK(CountDynamicEventEntries(&originalOwner) ==
              originalOwnerEntries);

        // The editor deliberately relinquished this object; clean up through
        // its new ownership only after proving it survived the aborted Show().
        delete transferred;
        CHECK_FALSE(transferredDialog);
    }

    SECTION("Transferred_dialog_survives_immediate_old_owner_destruction")
    {
        const wxString title =
            "Phase 013 transferred preferences owner destruction";
        wxFrame* oldOwner = new wxFrame(
            parent, wxID_ANY, "Immediately destroyed preferences owner");
        oldOwner->Show();
        wxWindow* const oldOwnerIdentity = oldOwner;
        const wxWeakRef<wxWindow> weakOldOwner(oldOwner);
        wxFrame newOwner(parent, wxID_ANY,
                         "Immediate transfer preferences owner");
        newOwner.Show();

        wxPreferencesEditor editor(title);
        wxWeakRef<wxWindow> transferredDialog;
        bool reparented = false;
        editor.AddPage(new TrackingPreferencesPage(
            "Immediate owner transfer",
            nullptr,
            TrackingPreferencesPage::CreateCallback(),
            [&]()
            {
                wxDialog* const dialog = FindDialogWithTitle(title);
                REQUIRE(dialog);
                transferredDialog = dialog;
                reparented = dialog->Reparent(&newOwner);
                delete oldOwner;
                oldOwner = nullptr;
            }));

        editor.Show(oldOwnerIdentity);
        REQUIRE(reparented);
        CHECK_FALSE(weakOldOwner);
        REQUIRE(transferredDialog);
        CHECK(transferredDialog->GetParent() == &newOwner);

        delete transferredDialog.get();
        CHECK_FALSE(transferredDialog);
    }

    SECTION("Page_order_nested_Show_OK_Cancel_and_selection_restore")
    {
        const wxString title = "Phase 013 preferences order";
        std::vector<wxString> calls;
        wxPreferencesEditor editor(title);
        editor.AddPage(new TrackingPreferencesPage("First", &calls));
        editor.AddPage(new TrackingPreferencesPage("Second", &calls));

        bool firstInspected = false;
        bool firstButton = false;
        size_t nestedDialogCount = 0;
        wxWeakRef<wxWindow> firstDialog;
        wxTheApp->CallAfter([&]()
        {
            wxDialog* const dialog = FindDialogWithTitle(title);
            if ( !dialog )
            {
                AbortAnyModalDialog();
                return;
            }

            firstDialog = dialog;
            wxNotebook* const notebook =
                FindDescendantOfType<wxNotebook>(dialog);
            if ( notebook && notebook->GetPageCount() == 2 )
            {
                firstInspected = notebook->GetPageText(0) == "First" &&
                                 notebook->GetPageText(1) == "Second";
                notebook->SetSelection(1);
            }

            editor.Show(parent);
            nestedDialogCount = CountDialogsWithTitle(title);
            firstButton = CommandDialogButton(dialog, wxID_OK);
            if ( !firstButton )
                dialog->EndModal(wxID_CANCEL);
        });
        editor.Show(parent);

        CHECK(firstInspected);
        CHECK(firstButton);
        CHECK(nestedDialogCount == 1);
        CHECK_FALSE(firstDialog);
        const std::vector<wxString> expectedCalls = {
            "create:First", "name:First",
            "create:Second", "name:Second"
        };
        CHECK(calls == expectedCalls);

        bool cancelRestoredPage = false;
        bool cancelButton = false;
        wxTheApp->CallAfter([&]()
        {
            wxDialog* const dialog = FindDialogWithTitle(title);
            wxNotebook* const notebook =
                dialog ? FindDescendantOfType<wxNotebook>(dialog) : nullptr;
            cancelRestoredPage = notebook && notebook->GetSelection() == 1;
            if ( notebook )
                notebook->SetSelection(0);
            cancelButton = CommandDialogButton(dialog, wxID_CANCEL);
            if ( dialog && !cancelButton )
                dialog->EndModal(wxID_CANCEL);
            else if ( !dialog )
                AbortAnyModalDialog();
        });
        editor.Show(parent);
        CHECK(cancelRestoredPage);
        CHECK(cancelButton);

        bool selectionAfterCancel = false;
        wxTheApp->CallAfter([&]()
        {
            wxDialog* const dialog = FindDialogWithTitle(title);
            wxNotebook* const notebook =
                dialog ? FindDescendantOfType<wxNotebook>(dialog) : nullptr;
            selectionAfterCancel = notebook && notebook->GetSelection() == 1;
            if ( dialog )
                dialog->EndModal(wxID_CANCEL);
            else
                AbortAnyModalDialog();
        });
        editor.Show(parent);
        CHECK(selectionAfterCancel);
    }

    SECTION("Selection_restore_stays_gated_and_revalidates_the_notebook")
    {
        const wxString title =
            "Phase 013 preferences selection transaction";
        wxPreferencesEditor editor(title);
        wxPanel foreignParent(parent);
        bool armRestoreCallback = false;
        bool restoreCallbackRan = false;
        bool visibleBeforeNestedShow = true;
        bool visibleAfterNestedShow = true;
        bool notebookReparented = false;
        bool firstSessionClosed = false;
        bool rescueRan = false;
        bool modalWasEnteredAfterTransfer = false;
        size_t nestedDialogCount = 0;
        wxWeakRef<wxNotebook> transferredNotebook;

        editor.AddPage(new TrackingPreferencesPage("First"));
        editor.AddPage(new TrackingPreferencesPage(
            "Second",
            nullptr,
            [&](wxWindow* page)
            {
                page->Bind(
                    wxEVT_SHOW,
                    [&, page](wxShowEvent& event)
                    {
                        event.Skip();
                        if ( !armRestoreCallback || !event.IsShown() ||
                             restoreCallbackRan )
                        {
                            return;
                        }

                        restoreCallbackRan = true;
                        wxDialog* const dialog = FindDialogWithTitle(title);
                        REQUIRE(dialog);
                        visibleBeforeNestedShow = dialog->IsShown();
                        editor.Show(parent);
                        visibleAfterNestedShow = dialog->IsShown();
                        nestedDialogCount = CountDialogsWithTitle(title);

                        wxNotebook* const notebook = wxDynamicCast(
                            page->GetParent(), wxNotebook);
                        REQUIRE(notebook);
                        transferredNotebook = notebook;
                        notebookReparented =
                            notebook->Reparent(&foreignParent);
                    });
            }));

        wxTheApp->CallAfter([&]()
        {
            wxDialog* const dialog = FindDialogWithTitle(title);
            wxNotebook* const notebook =
                dialog ? FindDescendantOfType<wxNotebook>(dialog) : nullptr;
            if ( notebook )
                notebook->SetSelection(1);
            firstSessionClosed = CommandDialogButton(dialog, wxID_OK);
            if ( dialog && !firstSessionClosed )
                dialog->EndModal(wxID_CANCEL);
            else if ( !dialog )
                AbortAnyModalDialog();
        });
        editor.Show(parent);
        REQUIRE(firstSessionClosed);

        armRestoreCallback = true;
        wxTheApp->CallAfter([&]()
        {
            rescueRan = true;
            wxDialog* const dialog = FindDialogWithTitle(title);
            modalWasEnteredAfterTransfer = dialog && dialog->IsModal();
            if ( dialog && dialog->IsModal() )
                dialog->EndModal(wxID_CANCEL);
        });
        editor.Show(parent);

        REQUIRE(WaitFor("selection transaction watchdog",
                        [&rescueRan]() { return rescueRan; }));
        REQUIRE(restoreCallbackRan);
        CHECK_FALSE(visibleBeforeNestedShow);
        CHECK_FALSE(visibleAfterNestedShow);
        CHECK(nestedDialogCount == 1);
        REQUIRE(notebookReparented);
        REQUIRE(transferredNotebook);
        CHECK(transferredNotebook->GetParent() == &foreignParent);
        CHECK_FALSE(modalWasEnteredAfterTransfer);
        CHECK(FindDialogWithTitle(title) == nullptr);
    }

    SECTION("Dismiss_reopen_two_owners_and_owner_destruction")
    {
        const wxString title = "Phase 013 preferences lifetime";
        wxPreferencesEditor editor(title);
        editor.AddPage(new TrackingPreferencesPage("General"));

        std::unique_ptr<wxFrame> firstOwner(
            new wxFrame(parent, wxID_ANY, "Preferences owner one"));
        std::unique_ptr<wxFrame> secondOwner(
            new wxFrame(parent, wxID_ANY, "Preferences owner two"));
        firstOwner->Show();
        secondOwner->Show();

        bool firstOwnerMatched = false;
        wxWeakRef<wxWindow> dismissedDialog;
        wxTheApp->CallAfter([&]()
        {
            wxDialog* const dialog = FindDialogWithTitle(title);
            if ( !dialog )
            {
                AbortAnyModalDialog();
                return;
            }
            dismissedDialog = dialog;
            firstOwnerMatched = dialog->GetParent() == firstOwner.get();
            editor.Dismiss();
        });
        editor.Show(firstOwner.get());
        CHECK(firstOwnerMatched);
        CHECK_FALSE(dismissedDialog);

        bool secondOwnerMatched = false;
        wxTheApp->CallAfter([&]()
        {
            wxDialog* const dialog = FindDialogWithTitle(title);
            secondOwnerMatched = dialog &&
                                 dialog->GetParent() == secondOwner.get();
            if ( dialog )
                dialog->EndModal(wxID_CANCEL);
            else
                AbortAnyModalDialog();
        });
        editor.Show(secondOwner.get());
        CHECK(secondOwnerMatched);

        wxFrame* const doomedOwner =
            new wxFrame(parent, wxID_ANY, "Doomed preferences owner");
        doomedOwner->Show();
        const wxWeakRef<wxWindow> weakOwner(doomedOwner);
        wxWeakRef<wxWindow> ownerDialog;
#if defined(__WXWINUI__) && wxUSE_WINUI3
        bool dialogAliveImmediatelyAfterOwnerDelete = false;
        bool destroyRequestedFromHide = false;
#endif
        wxTheApp->CallAfter([&]()
        {
            wxDialog* const dialog = FindDialogWithTitle(title);
            ownerDialog = dialog;
#if defined(__WXWINUI__) && wxUSE_WINUI3
            REQUIRE(dialog);
            dialog->Bind(
                wxEVT_SHOW,
                [dialog, &destroyRequestedFromHide](wxShowEvent& event)
                {
                    event.Skip();
                    if ( event.IsShown() )
                        return;

                    destroyRequestedFromHide = dialog->Destroy();
                });
#endif
            delete doomedOwner;
#if defined(__WXWINUI__) && wxUSE_WINUI3
            // The owner's observer ended modality and the hide callback asked
            // for direct destruction, but the real ShowModal() lifetime guard
            // must keep the dialog object alive until its stack has unwound.
            dialogAliveImmediatelyAfterOwnerDelete = ownerDialog.get() != nullptr;
#endif
        });
        editor.Show(doomedOwner);
        CHECK_FALSE(weakOwner);
#if defined(__WXWINUI__) && wxUSE_WINUI3
        CHECK(destroyRequestedFromHide);
        CHECK(dialogAliveImmediatelyAfterOwnerDelete);
        REQUIRE(WaitFor("owner-destroyed preferences dialog cleanup",
                        [&ownerDialog]()
                        {
                            return ownerDialog.get() == nullptr;
                        }));
#else
        CHECK_FALSE(ownerDialog);
#endif
    }

    SECTION("Owner_or_editor_may_die_inside_page_callbacks")
    {
        const wxString createTitle =
            "Phase 013 preferences editor destroyed in CreateWindow";
        std::unique_ptr<wxPreferencesEditor> createEditor(
            new wxPreferencesEditor(createTitle));
        bool createCallbackRan = false;
        createEditor->AddPage(new TrackingPreferencesPage(
            "Destroy in create",
            nullptr,
            [&createEditor, &createCallbackRan](wxWindow*)
            {
                createCallbackRan = true;
                createEditor.reset();
            }));
        wxPreferencesEditor* const createRaw = createEditor.get();
        createRaw->Show(parent);
        CHECK(createCallbackRan);
        CHECK_FALSE(createEditor);
        CHECK(FindDialogWithTitle(createTitle) == nullptr);

        const wxString nameTitle =
            "Phase 013 preferences editor destroyed in GetName";
        std::unique_ptr<wxPreferencesEditor> nameEditor(
            new wxPreferencesEditor(nameTitle));
        bool nameCallbackRan = false;
        nameEditor->AddPage(new TrackingPreferencesPage(
            "Destroy in name",
            nullptr,
            TrackingPreferencesPage::CreateCallback(),
            [&nameEditor, &nameCallbackRan]()
            {
                nameCallbackRan = true;
                nameEditor.reset();
            }));
        wxPreferencesEditor* const nameRaw = nameEditor.get();
        nameRaw->Show(parent);
        CHECK(nameCallbackRan);
        CHECK_FALSE(nameEditor);
        CHECK(FindDialogWithTitle(nameTitle) == nullptr);

        const wxString ownerTitle =
            "Phase 013 preferences owner destroyed in CreateWindow";
        wxPreferencesEditor ownerEditor(ownerTitle);
        wxFrame* owner =
            new wxFrame(parent, wxID_ANY, "Callback-destroyed owner");
        owner->Show();
        const wxWeakRef<wxWindow> weakOwner(owner);
        bool ownerCallbackRan = false;
        ownerEditor.AddPage(new TrackingPreferencesPage(
            "Destroy owner",
            nullptr,
            [&owner, &ownerCallbackRan](wxWindow*)
            {
                ownerCallbackRan = true;
                delete owner;
                owner = nullptr;
            }));
        ownerEditor.Show(owner);
        CHECK(ownerCallbackRan);
        CHECK_FALSE(weakOwner);
        CHECK(FindDialogWithTitle(ownerTitle) == nullptr);

        const wxString pendingTitle =
            "Phase 013 preferences dialog pending destruction";
        wxPreferencesEditor pendingEditor(pendingTitle);
        bool pendingCallbackRan = false;
        bool unexpectedSecondPage = false;
        wxWeakRef<wxWindow> weakPendingDialog;
#if defined(__WXWINUI__) && wxUSE_WINUI3
        std::unique_ptr<wxWinUITLWHostWindowEventGuard> retainedCallback;
#endif
        pendingEditor.AddPage(new TrackingPreferencesPage(
            "Destroy dialog",
            nullptr,
            [&](wxWindow* pageWindow)
            {
                pendingCallbackRan = true;
                wxWindow* const notebook = pageWindow->GetParent();
                wxDialog* const dialog = wxDynamicCast(
                    notebook ? notebook->GetParent() : nullptr,
                    wxDialog);
                REQUIRE(dialog);
                weakPendingDialog = dialog;
#if defined(__WXWINUI__) && wxUSE_WINUI3
                retainedCallback.reset(
                    new wxWinUITLWHostWindowEventGuard(dialog));
#endif
                dialog->Destroy();
            }));
        pendingEditor.AddPage(new TrackingPreferencesPage(
            "Must not be created",
            nullptr,
            [&unexpectedSecondPage](wxWindow*)
            {
                unexpectedSecondPage = true;
            }));

        pendingEditor.Show(parent);
        CHECK(pendingCallbackRan);
        CHECK_FALSE(unexpectedSecondPage);
#if defined(__WXWINUI__) && wxUSE_WINUI3
        REQUIRE(weakPendingDialog);
        CHECK(wxWinUITLWHostIsDestroyScheduled(weakPendingDialog.get()));
        retainedCallback.reset();
#endif
        REQUIRE(WaitFor("pending preferences dialog cleanup",
                        [&weakPendingDialog]()
                        {
                            return weakPendingDialog.get() == nullptr;
                        }));
        CHECK(FindDialogWithTitle(pendingTitle) == nullptr);

#if defined(__WXWINUI__) && wxUSE_WINUI3
        // Regression: the dialog used to bind the owner's destroy event to a
        // raw member-function receiver. If the owner was retained in the
        // WinUI host queue while deleting the editor closed/deleted the modal
        // dialog, draining the owner later dispatched into freed memory.
        const wxString retainedOwnerTitle =
            "Phase 013 retained preferences owner";
        std::unique_ptr<wxPreferencesEditor> retainedOwnerEditor(
            new wxPreferencesEditor(retainedOwnerTitle));
        wxFrame* retainedOwner =
            new wxFrame(parent, wxID_ANY, "Retained preferences owner");
        retainedOwner->Show();
        const wxWeakRef<wxWindow> weakRetainedOwner(retainedOwner);
        wxWeakRef<wxWindow> weakRetainedDialog;
        std::unique_ptr<wxWinUITLWHostWindowEventGuard> retainedOwnerGuard;
        bool retainedOwnerCallbackRan = false;

        retainedOwnerEditor->AddPage(new TrackingPreferencesPage(
            "Destroy retained owner and editor",
            nullptr,
            [&](wxWindow* pageWindow)
            {
                retainedOwnerCallbackRan = true;
                wxWindow* const notebook = pageWindow->GetParent();
                wxDialog* const dialog = wxDynamicCast(
                    notebook ? notebook->GetParent() : nullptr,
                    wxDialog);
                REQUIRE(dialog);
                weakRetainedDialog = dialog;

                retainedOwnerGuard.reset(
                    new wxWinUITLWHostWindowEventGuard(retainedOwner));
                retainedOwner->Destroy();
                REQUIRE(wxWinUITLWHostIsDestroyScheduled(retainedOwner));

                retainedOwnerEditor.reset();
            }));

        wxPreferencesEditor* const retainedEditorRaw =
            retainedOwnerEditor.get();
        retainedEditorRaw->Show(retainedOwner);
        CHECK(retainedOwnerCallbackRan);
        CHECK_FALSE(retainedOwnerEditor);
        CHECK_FALSE(weakRetainedDialog);
        REQUIRE(weakRetainedOwner);

        retainedOwnerGuard.reset();
        REQUIRE(WaitFor("retained preferences owner cleanup",
                        [&weakRetainedOwner]()
                        {
                            return weakRetainedOwner.get() == nullptr;
                        }));
        retainedOwner = nullptr;
#endif
    }

    SECTION("One_hundred_create_show_cancel_destroy_cycles")
    {
        for ( int i = 0; i < 100; ++i )
        {
            const wxString title =
                wxString::Format("Phase 013 preferences cycle %d", i);
            wxPreferencesEditor editor(title);
            editor.AddPage(new TrackingPreferencesPage("Cycle page"));
            bool closed = false;
            wxTheApp->CallAfter([&]()
            {
                wxDialog* const dialog = FindDialogWithTitle(title);
                closed = CommandDialogButton(dialog, wxID_CANCEL);
                if ( dialog && !closed )
                    dialog->EndModal(wxID_CANCEL);
                else if ( !dialog )
                    AbortAnyModalDialog();
            });
            editor.Show(parent);
            INFO("preferences cycle: " << i);
            REQUIRE(closed);
            REQUIRE(FindDialogWithTitle(title) == nullptr);
        }
    }

    SECTION("Owner_destroy_observer_is_unbound_after_each_session")
    {
        const wxString title =
            "Phase 013 preferences owner observer teardown";
        wxFrame owner(parent, wxID_ANY, "Preferences observer owner");
        owner.Show();
        const size_t baselineEntries = CountDynamicEventEntries(&owner);

        wxPreferencesEditor editor(title);
        editor.AddPage(new TrackingPreferencesPage("Observer page"));
        for ( int i = 0; i < 5; ++i )
        {
            bool closed = false;
            wxTheApp->CallAfter([&]()
            {
                wxDialog* const dialog = FindDialogWithTitle(title);
                closed = CommandDialogButton(dialog, wxID_CANCEL);
                if ( dialog && !closed )
                    dialog->EndModal(wxID_CANCEL);
                else if ( !dialog )
                    AbortAnyModalDialog();
            });
            editor.Show(&owner);
            INFO("preferences observer session: " << i);
            REQUIRE(closed);
            CHECK(CountDynamicEventEntries(&owner) == baselineEntries);
        }
    }
#endif // !wxHAS_PREF_EDITOR_MODELESS
}

#endif // wxUSE_PREFERENCES_EDITOR

#endif // wxUSE_BUTTON && (bookctrl || wizarddlg || preferences editor)
