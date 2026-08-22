///////////////////////////////////////////////////////////////////////////////
// Name:        wx/simplebook.h
// Purpose:     wxBookCtrlBase-derived class without any controller.
// Author:      Vadim Zeitlin
// Created:     2012-08-21
// Copyright:   (c) 2012 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_SIMPLEBOOK_H_
#define _WX_SIMPLEBOOK_H_

#include "wx/compositebookctrl.h"

#if wxUSE_BOOKCTRL

#include "wx/private/windowlifetime.h"
#include "wx/vector.h"
#include "wx/weakref.h"

// ----------------------------------------------------------------------------
// wxSimplebook: a book control without any user-actionable controller.
// ----------------------------------------------------------------------------

// NB: This class doesn't use DLL export declaration as it's fully inline.

class wxSimplebook : public wxCompositeBookCtrlBase
{
public:
    wxSimplebook()
    {
        Init();
    }

    wxSimplebook(wxWindow *parent,
                 wxWindowID winid = wxID_ANY,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = 0,
                 const wxString& name = wxEmptyString)
    {
        wxBookCtrlBase::Create(parent, winid, pos, size, style | wxBK_TOP, name);
        Init();
    }

    bool Create(wxWindow *parent,
                wxWindowID winid = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxEmptyString)
    {
        return wxBookCtrlBase::Create(parent, winid, pos, size, style | wxBK_TOP, name);
    }


    // Methods specific to this class.

    // A method allowing to add a new page without any label (which is unused
    // by this control) and show it immediately.
    bool ShowNewPage(wxWindow* page)
    {
        return AddPage(page, wxString(), true /* select it */);
    }


    // Set effect to use for showing/hiding pages.
    void SetEffects(wxShowEffect showEffect, wxShowEffect hideEffect)
    {
        m_showEffect = showEffect;
        m_hideEffect = hideEffect;
    }

    // Or the same effect for both of them.
    void SetEffect(wxShowEffect effect)
    {
        SetEffects(effect, effect);
    }

    // And the same for time outs.
    void SetEffectsTimeouts(unsigned showTimeout, unsigned hideTimeout)
    {
        m_showTimeout = showTimeout;
        m_hideTimeout = hideTimeout;
    }

    void SetEffectTimeout(unsigned timeout)
    {
        SetEffectsTimeouts(timeout, timeout);
    }


    // Implement base class pure virtual methods.

    // Page management
    virtual bool InsertPage(size_t n,
                            wxWindow *page,
                            const wxString& text,
                            bool bSelect = false,
                            int imageId = NO_IMAGE) override
    {
        const wxWeakRef<wxSimplebook> weakThis(this);
        const wxWeakRef<wxWindow> weakPage(page);
        const int selectionBefore = m_selection;
        const InsertPageResult modelResult =
            DoInsertPageIntoModel(n, page, text, bSelect, imageId);
        if ( modelResult == InsertPageResult::Failed )
            return false;
        if ( modelResult == InsertPageResult::OwnershipConsumed )
            return true;

        wxSimplebook* book = weakThis.get();
        if ( !book )
            return true;
        if ( weakPage.get() != page || page->GetParent() != book )
        {
            if ( n < book->wxBookCtrlBase::GetPageCount() &&
                    book->wxBookCtrlBase::GetPage(n) == page )
            {
                book->DoErasePageRange(n, 1);
            }
            return true;
        }

        book->m_pageTexts.insert(book->m_pageTexts.begin() + n, text);
        if ( selectionBefore != wxNOT_FOUND &&
                static_cast<int>(n) <= selectionBefore )
        {
            // Preserve the identity of the selected page after the common
            // model shifts it to the right.
            book->m_selection = selectionBefore + 1;
        }

        const size_t expectedCount = book->wxBookCtrlBase::GetPageCount();
        const auto getCurrent = [&]() -> wxSimplebook*
        {
            wxSimplebook* const current = weakThis.get();
            return current && weakPage.get() == page &&
                   page->GetParent() == current &&
                   current->wxBookCtrlBase::GetPageCount() == expectedCount &&
                   current->m_pageTexts.size() == expectedCount &&
                   n < expectedCount &&
                   current->wxBookCtrlBase::GetPage(n) == page
                        ? current
                        : nullptr;
        };
        const auto finishCommittedInsertion = [&]() -> bool
        {
            if ( wxSimplebook* const current = weakThis.get() )
            {
                if ( (weakPage.get() != page ||
                      page->GetParent() != current) &&
                        n < current->wxBookCtrlBase::GetPageCount() &&
                        current->wxBookCtrlBase::GetPage(n) == page )
                {
                    // A selection/show callback destroyed the just-published
                    // page. Remove its raw identity and the parallel label
                    // before visibility reconciliation can inspect it.
                    current->DoErasePageRange(n, 1);
                    if ( n < current->m_pageTexts.size() )
                        current->m_pageTexts.erase(
                            current->m_pageTexts.begin() + n);
                    current->DoSetSelectionAfterRemoval(n);
                }
                (void)current->DoReconcilePageVisibility();
            }

            // The common model already accepted ownership. A nested latest
            // writer may since have removed or destroyed the candidate, but
            // this must never be reported as a pre-commit failure inviting
            // the caller to delete it again.
            return true;
        };

        book = getCurrent();
        if ( !book )
            return finishCommittedInsertion();

        if ( !book->DoSetSelectionAfterInsertion(n, bSelect) )
        {
            book = getCurrent();
            if ( !book )
                return finishCommittedInsertion();

            page->Hide();
            if ( !getCurrent() )
                return finishCommittedInsertion();
        }

        return finishCommittedInsertion();
    }

    virtual int SetSelection(size_t n) override
    {
        return DoSetSelection(n, SetSelection_SendEvent);
    }

    virtual int ChangeSelection(size_t n) override
    {
        return DoSetSelection(n);
    }

    // Neither labels nor images are supported but we still store the labels
    // just in case the user code attaches some importance to them.
    virtual bool SetPageText(size_t n, const wxString& strText) override
    {
        wxCHECK_MSG( n < GetPageCount(), false, wxS("Invalid page") );

        m_pageTexts[n] = strText;

        return true;
    }

    virtual wxString GetPageText(size_t n) const override
    {
        wxCHECK_MSG( n < GetPageCount(), wxString(), wxS("Invalid page") );

        return m_pageTexts[n];
    }

    virtual bool SetPageImage(size_t WXUNUSED(n), int WXUNUSED(imageId)) override
    {
        return false;
    }

    virtual int GetPageImage(size_t WXUNUSED(n)) const override
    {
        return NO_IMAGE;
    }

    // Override some wxWindow methods too.
    virtual void SetFocus() override
    {
        wxWindow* const page = GetCurrentPage();
        if ( page )
            page->SetFocus();
    }

protected:
    virtual void UpdateSelectedPage(size_t WXUNUSED(newsel)) override
    {
        // Nothing to do here, but must be overridden to avoid the assert in
        // the base class version.
    }

    virtual wxBookCtrlEvent* CreatePageChangingEvent() const override
    {
        return new wxBookCtrlEvent(wxEVT_BOOKCTRL_PAGE_CHANGING,
                                   GetId());
    }

    virtual void MakeChangedEvent(wxBookCtrlEvent& event) override
    {
        event.SetEventType(wxEVT_BOOKCTRL_PAGE_CHANGED);
    }

    virtual wxWindow *DoRemovePage(size_t page) override
    {
        if ( IsDeletingAllPages() &&
                !IsPerformingDeleteAllPageRemoval() )
            return nullptr;

        const wxWeakRef<wxSimplebook> weakThis(this);
        const size_t pageCount = wxBookCtrlBase::GetPageCount();
        wxCHECK_MSG( page < pageCount, nullptr,
                     wxT("invalid simplebook page index") );
        wxWindow* const expectedPage = wxBookCtrlBase::GetPage(page);
        const wxWeakRef<wxWindow> weakExpectedPage(expectedPage);
        wxWindow* const win = wxBookCtrlBase::DoRemovePage(page);
        wxSimplebook* book = weakThis.get();
        if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
                book->wxBookCtrlBase::GetPageCount() != pageCount - 1 ||
                page >= book->m_pageTexts.size() )
        {
            return nullptr;
        }

        // The common erase may have committed before best-size invalidation
        // synchronously destroyed the removed page. Keep the parallel label
        // model coherent even though no ownership-bearing pointer can be
        // returned in that case.
        book->m_pageTexts.erase(book->m_pageTexts.begin() + page);
        book->DoSetSelectionAfterRemoval(page);

        book = weakThis.get();
        if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
                !win || win != expectedPage ||
                weakExpectedPage.get() != win )
            return nullptr;

        // A nested writer may have republished the removed page, in which
        // case ownership remains with the book.
        return book->wxBookCtrlBase::FindPage(win) == wxNOT_FOUND
                    ? win
                    : nullptr;
    }

    virtual void DoSize() override
    {
        const wxWeakRef<wxSimplebook> weakThis(this);
        wxWindow* const page = GetCurrentPage();
        const wxWeakRef<wxWindow> weakPage(page);
        if ( !page )
            return;

        const wxRect pageRect = GetPageRect();
        wxSimplebook* const book = weakThis.get();
        if ( !book || weakPage.get() != page ||
                book->GetCurrentPage() != page )
        {
            return;
        }

        page->SetSize(pageRect);
    }

    virtual void DoShowPage(wxWindow* page, bool show) override
    {
        const wxWeakRef<wxWindow> weakPage(page);
        if ( show )
        {
            page->ShowWithEffect(m_showEffect, m_showTimeout);

            // Unlike simple Show(), ShowWithEffect() doesn't necessarily give
            // focus to the window, but we do expect the new page to have focus
            // if it's currently visible.
            if ( weakPage.get() == page && page->IsShownOnScreen() )
                page->SetFocus();
        }
        else
        {
            page->HideWithEffect(m_hideEffect, m_hideTimeout);
        }
    }

private:
    void Init()
    {
        // We don't need any border as we don't have anything to separate the
        // page contents from.
        SetInternalBorder(0);

        // No effects by default.
        m_showEffect =
        m_hideEffect = wxSHOW_EFFECT_NONE;

        m_showTimeout =
        m_hideTimeout = 0;
    }

    wxVector<wxString> m_pageTexts;

    wxShowEffect m_showEffect,
                 m_hideEffect;

    unsigned m_showTimeout,
             m_hideTimeout;

    wxDECLARE_NO_COPY_CLASS(wxSimplebook);
};

#endif // wxUSE_BOOKCTRL

#endif // _WX_SIMPLEBOOK_H_
