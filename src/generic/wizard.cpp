///////////////////////////////////////////////////////////////////////////////
// Name:        generic/wizard.cpp
// Purpose:     generic implementation of wxWizard class
// Author:      Vadim Zeitlin
// Modified by: Robert Cavanaugh
//              1) Added capability for wxWizardPage to accept resources
//              2) Added "Help" button handler stub
//              3) Fixed ShowPage() bug on displaying bitmaps
//              Robert Vazan (sizers)
// Created:     15.08.99
// RCS-ID:      $Id$
// Copyright:   (c) 1999 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "wizardg.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_WIZARDDLG

#ifndef WX_PRECOMP
    #include "wx/dynarray.h"
    #include "wx/intl.h"
    #include "wx/statbmp.h"
    #include "wx/button.h"
#endif //WX_PRECOMP

#include "wx/statline.h"
#include "wx/sizer.h"

#include "wx/wizard.h"

// ----------------------------------------------------------------------------
// wxWizardSizer
// ----------------------------------------------------------------------------

class wxWizardSizer : public wxSizer
{
public:
    wxWizardSizer(wxWizard *owner);

    void RecalcSizes();
    wxSize CalcMin();

    wxSize GetMaxChildSize();
    int Border() const;

private:
    wxSize SiblingSize(wxSizerItem *child);

    wxWizard *m_owner;
    bool m_childSizeValid;
    wxSize m_childSize;
};

// ----------------------------------------------------------------------------
// event tables and such
// ----------------------------------------------------------------------------

DEFINE_EVENT_TYPE(wxEVT_WIZARD_PAGE_CHANGED)
DEFINE_EVENT_TYPE(wxEVT_WIZARD_PAGE_CHANGING)
DEFINE_EVENT_TYPE(wxEVT_WIZARD_CANCEL)
DEFINE_EVENT_TYPE(wxEVT_WIZARD_FINISHED)
DEFINE_EVENT_TYPE(wxEVT_WIZARD_HELP)

BEGIN_EVENT_TABLE(wxWizard, wxDialog)
    EVT_BUTTON(wxID_CANCEL, wxWizard::OnCancel)
    EVT_BUTTON(wxID_BACKWARD, wxWizard::OnBackOrNext)
    EVT_BUTTON(wxID_FORWARD, wxWizard::OnBackOrNext)
    EVT_BUTTON(wxID_HELP, wxWizard::OnHelp)

    EVT_WIZARD_PAGE_CHANGED(wxID_ANY, wxWizard::OnWizEvent)
    EVT_WIZARD_PAGE_CHANGING(wxID_ANY, wxWizard::OnWizEvent)
    EVT_WIZARD_CANCEL(wxID_ANY, wxWizard::OnWizEvent)
    EVT_WIZARD_FINISHED(wxID_ANY, wxWizard::OnWizEvent)
    EVT_WIZARD_HELP(wxID_ANY, wxWizard::OnWizEvent)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(wxWizard, wxDialog)

/*
    TODO PROPERTIES :
    wxWizard
        extstyle
        title
*/

IMPLEMENT_ABSTRACT_CLASS(wxWizardPage, wxPanel)
IMPLEMENT_DYNAMIC_CLASS(wxWizardPageSimple, wxWizardPage)
IMPLEMENT_DYNAMIC_CLASS(wxWizardEvent, wxNotifyEvent)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxWizardPage
// ----------------------------------------------------------------------------

void wxWizardPage::Init()
{
    m_bitmap = wxNullBitmap;
}

wxWizardPage::wxWizardPage(wxWizard *parent,
                           const wxBitmap& bitmap,
                           const wxChar *resource)
{
    Create(parent, bitmap, resource);
}

bool wxWizardPage::Create(wxWizard *parent,
                          const wxBitmap& bitmap,
                          const wxChar *resource)
{
    if ( !wxPanel::Create(parent, wxID_ANY) )
        return false;

    if ( resource != NULL )
    {
#if wxUSE_WX_RESOURCES
#if 0
       if ( !LoadFromResource(this, resource) )
        {
            wxFAIL_MSG(wxT("wxWizardPage LoadFromResource failed!!!!"));
        }
#endif
#endif // wxUSE_RESOURCES
    }

    m_bitmap = bitmap;

    // initially the page is hidden, it's shown only when it becomes current
    Hide();

    return true;
}

// ----------------------------------------------------------------------------
// wxWizardPageSimple
// ----------------------------------------------------------------------------

wxWizardPage *wxWizardPageSimple::GetPrev() const
{
    return m_prev;
}

wxWizardPage *wxWizardPageSimple::GetNext() const
{
    return m_next;
}

// ----------------------------------------------------------------------------
// wxWizardSizer
// ----------------------------------------------------------------------------

wxWizardSizer::wxWizardSizer(wxWizard *owner)
    : m_owner(owner)
{
    m_childSizeValid = false;
}

void wxWizardSizer::RecalcSizes()
{
    // Effect of this function depends on m_owner->m_page and
    // it should be called whenever it changes (wxWizard::ShowPage)
    if ( m_owner->m_page )
    {
        m_owner->m_page->SetSize(m_position.x,m_position.y, m_size.x,m_size.y);
    }
}

wxSize wxWizardSizer::CalcMin()
{
    return m_owner->GetPageSize();
}

wxSize wxWizardSizer::GetMaxChildSize()
{
#if !defined(__WXDEBUG__)
    if ( m_childSizeValid )
        return m_childSize;
#endif

    wxSize maxOfMin;
    wxSizerItemList::compatibility_iterator childNode;

    for(childNode = m_children.GetFirst(); childNode;
        childNode = childNode->GetNext())
    {
        wxSizerItem *child = childNode->GetData();
        maxOfMin.IncTo(child->CalcMin());
        maxOfMin.IncTo(SiblingSize(child));
    }

#ifdef __WXDEBUG__
    if ( m_childSizeValid && m_childSize != maxOfMin )
    {
        wxFAIL_MSG( _T("Size changed in wxWizard::GetPageAreaSizer()")
                    _T("after RunWizard().\n")
                    _T("Did you forget to call GetSizer()->Fit(this) ")
                    _T("for some page?")) ;

        return m_childSize;
    }
#endif // __WXDEBUG__

    if ( m_owner->m_started )
    {
        m_childSizeValid = true;
        m_childSize = maxOfMin;
    }

    return maxOfMin;
}

int wxWizardSizer::Border() const
{
    if ( m_owner->m_calledSetBorder )
        return m_owner->m_border;

    return m_children.IsEmpty() ? 5 : 0;
}

wxSize wxWizardSizer::SiblingSize(wxSizerItem *child)
{
    wxSize maxSibling;

    if ( child->IsWindow() )
    {
        wxWizardPage *page = wxDynamicCast(child->GetWindow(), wxWizardPage);
        if ( page )
        {
            for ( wxWizardPage *sibling = page->GetNext();
                  sibling;
                  sibling = sibling->GetNext() )
            {
                if ( sibling->GetSizer() )
                {
                    maxSibling.IncTo(sibling->GetSizer()->CalcMin());
                }
            }
        }
    }

    return maxSibling;
}

// ----------------------------------------------------------------------------
// generic wxWizard implementation
// ----------------------------------------------------------------------------

void wxWizard::Init()
{
    m_posWizard = wxDefaultPosition;
    m_page = (wxWizardPage *)NULL;
    m_btnPrev = m_btnNext = NULL;
    m_statbmp = NULL;
    m_sizerBmpAndPage = NULL;
    m_sizerPage = NULL;
    m_calledSetBorder = false;
    m_border = 0;
    m_started = false;
}

bool wxWizard::Create(wxWindow *parent,
                      int id,
                      const wxString& title,
                      const wxBitmap& bitmap,
                      const wxPoint& pos,
                      long style)
{
    bool result = wxDialog::Create(parent,id,title,pos,wxDefaultSize,style);

    m_posWizard = pos;
    m_bitmap = bitmap ;

    DoCreateControls();

    return result;
}

void wxWizard::AddBitmapRow(wxBoxSizer *mainColumn)
{
    m_sizerBmpAndPage = new wxBoxSizer(wxHORIZONTAL);
    mainColumn->Add(
        m_sizerBmpAndPage,
        1, // Vertically stretchable
        wxEXPAND // Horizonal stretching, no border
    );
    mainColumn->Add(0,5,
        0, // No vertical stretching
        wxEXPAND // No border, (mostly useless) horizontal stretching
    );

#if wxUSE_STATBMP
    if ( m_bitmap.Ok() )
    {
        m_statbmp = new wxStaticBitmap(this, wxID_ANY, m_bitmap);
        m_sizerBmpAndPage->Add(
            m_statbmp,
            0, // No horizontal stretching
            wxALL, // Border all around, top alignment
            5 // Border width
        );
        m_sizerBmpAndPage->Add(
            5,0,
            0, // No horizontal stretching
            wxEXPAND // No border, (mostly useless) vertical stretching
        );
    }
#endif

    // Added to m_sizerBmpAndPage in FinishLayout
    m_sizerPage = new wxWizardSizer(this);
}

void wxWizard::AddStaticLine(wxBoxSizer *mainColumn)
{
#if wxUSE_STATLINE
    mainColumn->Add(
        new wxStaticLine(this, wxID_ANY),
        0, // Vertically unstretchable
        wxEXPAND | wxALL, // Border all around, horizontally stretchable
        5 // Border width
    );
    mainColumn->Add(0,5,
        0, // No vertical stretching
        wxEXPAND // No border, (mostly useless) horizontal stretching
    );
#else
    (void)mainColumn;
#endif // wxUSE_STATLINE
}

void wxWizard::AddBackNextPair(wxBoxSizer *buttonRow)
{
    wxASSERT_MSG( m_btnNext && m_btnPrev,
                  _T("You must create the buttons before calling ")
                  _T("wxWizard::AddBackNextPair") );

    // margin between Back and Next buttons
#ifdef __WXMAC__
    static const int BACKNEXT_MARGIN = 10;
#else
    static const int BACKNEXT_MARGIN = 0;
#endif

    wxBoxSizer *backNextPair = new wxBoxSizer(wxHORIZONTAL);
    buttonRow->Add(
        backNextPair,
        0, // No horizontal stretching
        wxALL, // Border all around
        5 // Border width
    );

    backNextPair->Add(m_btnPrev);
    backNextPair->Add(BACKNEXT_MARGIN,0,
        0, // No horizontal stretching
        wxEXPAND // No border, (mostly useless) vertical stretching
    );
    backNextPair->Add(m_btnNext);
}

void wxWizard::AddButtonRow(wxBoxSizer *mainColumn)
{
    // the order in which the buttons are created determines the TAB order - at least under MSWindows...
    // although the 'back' button appears before the 'next' button, a more userfriendly tab order is
    // to activate the 'next' button first (create the next button before the back button).
    // The reason is: The user will repeatedly enter information in the wizard pages and then wants to
    // press 'next'. If a user uses mostly the keyboard, he would have to skip the 'back' button
    // everytime. This is annoying. There is a second reason: RETURN acts as TAB. If the 'next'
    // button comes first in the TAB order, the user can enter information very fast using the RETURN
    // key to TAB to the next entry field and page. This would not be possible, if the 'back' button
    // was created before the 'next' button.

    wxBoxSizer *buttonRow = new wxBoxSizer(wxHORIZONTAL);
#ifdef __WXMAC__
    if (GetExtraStyle() & wxWIZARD_EX_HELPBUTTON)
        mainColumn->Add(
            buttonRow,
            0, // Vertically unstretchable
            wxGROW|wxALIGN_CENTRE
            );
    else
#endif
    mainColumn->Add(
        buttonRow,
        0, // Vertically unstretchable
        wxALIGN_RIGHT // Right aligned, no border
    );

    // Desired TAB order is 'next', 'cancel', 'help', 'back'. This makes the 'back' button the last control on the page.
    // Create the buttons in the right order...
    wxButton *btnHelp=0;
#ifdef __WXMAC__
    if (GetExtraStyle() & wxWIZARD_EX_HELPBUTTON)
        btnHelp=new wxButton(this, wxID_HELP, _("&Help"));
#endif

    m_btnNext = new wxButton(this, wxID_FORWARD, _("&Next >"));
    wxButton *btnCancel=new wxButton(this, wxID_CANCEL, _("&Cancel"));
#ifndef __WXMAC__
    if (GetExtraStyle() & wxWIZARD_EX_HELPBUTTON)
        btnHelp=new wxButton(this, wxID_HELP, _("&Help"));
#endif
    m_btnPrev = new wxButton(this, wxID_BACKWARD, _("< &Back"));

    if (btnHelp)
    {
        buttonRow->Add(
            btnHelp,
            0, // Horizontally unstretchable
            wxALL, // Border all around, top aligned
            5 // Border width
            );
#ifdef __WXMAC__
        // Put stretchable space between help button and others
        buttonRow->Add(0, 0, 1, wxALIGN_CENTRE, 0);
#endif
    }

    AddBackNextPair(buttonRow);

    buttonRow->Add(
        btnCancel,
        0, // Horizontally unstretchable
        wxALL, // Border all around, top aligned
        5 // Border width
    );
}

void wxWizard::DoCreateControls()
{
    // do nothing if the controls were already created
    if ( WasCreated() )
        return;

    // wxWindow::SetSizer will be called at end
    wxBoxSizer *windowSizer = new wxBoxSizer(wxVERTICAL);

    wxBoxSizer *mainColumn = new wxBoxSizer(wxVERTICAL);
    windowSizer->Add(
        mainColumn,
        1, // Vertical stretching
        wxALL | wxEXPAND, // Border all around, horizontal stretching
        5 // Border width
    );

    AddBitmapRow(mainColumn);
    AddStaticLine(mainColumn);
    AddButtonRow(mainColumn);

    // wxWindow::SetSizer should be followed by wxWindow::Fit, but
    // this is done in FinishLayout anyway so why duplicate it
    SetSizer(windowSizer);
}

void wxWizard::SetPageSize(const wxSize& size)
{
    wxCHECK_RET(!m_started,wxT("wxWizard::SetPageSize after RunWizard"));
    m_sizePage = size;
}

void wxWizard::FinishLayout()
{
    // Set to enable wxWizardSizer::GetMaxChildSize
    m_started = true;

    m_sizerBmpAndPage->Add(
        m_sizerPage,
        1, // Horizontal stretching
        wxEXPAND | wxALL, // Vertically stretchable
        m_sizerPage->Border()
    );

    GetSizer()->SetSizeHints(this);
    if ( m_posWizard == wxDefaultPosition )
        CentreOnScreen();
}

void wxWizard::FitToPage(const wxWizardPage *page)
{
    wxCHECK_RET(!m_started,wxT("wxWizard::FitToPage after RunWizard"));

    while ( page )
    {
        wxSize size = page->GetBestSize();

        m_sizePage.IncTo(size);

        page = page->GetNext();
    }
}

bool wxWizard::ShowPage(wxWizardPage *page, bool goingForward)
{
    wxASSERT_MSG( page != m_page, wxT("this is useless") );

    // we'll use this to decide whether we have to change the label of this
    // button or not (initially the label is "Next")
    bool btnLabelWasNext = true;

    // Modified 10-20-2001 Robert Cavanaugh.
    // Fixed bug for displaying a new bitmap
    // in each *consecutive* page

    // flag to indicate if this page uses a new bitmap
    bool bmpIsDefault = true;

    // use these labels to determine if we need to change the bitmap
    // for this page
    wxBitmap bmpPrev, bmpCur;

    // check for previous page
    if ( m_page )
    {
        // send the event to the old page
        wxWizardEvent event(wxEVT_WIZARD_PAGE_CHANGING, GetId(), goingForward, m_page);
        if ( m_page->GetEventHandler()->ProcessEvent(event) &&
             !event.IsAllowed() )
        {
            // vetoed by the page
            return false;
        }

        m_page->Hide();

        btnLabelWasNext = HasNextPage(m_page);

        // Get the bitmap of the previous page (if it exists)
        if ( m_page->GetBitmap().Ok() )
        {
            bmpPrev = m_page->GetBitmap();
        }
    }

    // set the new page
    m_page = page;

    // is this the end?
    if ( !m_page )
    {
        // terminate successfully
        if(IsModal())
        {
            EndModal(wxID_OK);
        }
        else
        {
            SetReturnCode(wxID_OK);
            Hide();
        }

        // and notify the user code (this is especially useful for modeless
        // wizards)
        wxWizardEvent event(wxEVT_WIZARD_FINISHED, GetId(), false, 0);
        (void)GetEventHandler()->ProcessEvent(event);

        return true;
    }

    // position and show the new page
    (void)m_page->TransferDataToWindow();

    // wxWizardSizer::RecalcSizes wants to be called when m_page changes
    m_sizerPage->RecalcSizes();

    // check if bitmap needs to be updated
    // update default flag as well
    if ( m_page->GetBitmap().Ok() )
    {
        bmpCur = m_page->GetBitmap();
        bmpIsDefault = false;
    }

#if wxUSE_STATBMP
    // change the bitmap if:
    // 1) a default bitmap was selected in constructor
    // 2) this page was constructed with a bitmap
    // 3) this bitmap is not the previous bitmap
    if ( m_statbmp && (bmpCur != bmpPrev) )
    {
        wxBitmap bmp;
        if ( bmpIsDefault )
            bmp = m_bitmap;
        else
            bmp = m_page->GetBitmap();
        m_statbmp->SetBitmap(bmp);
    }
#endif

    // and update the buttons state
    m_btnPrev->Enable(HasPrevPage(m_page));

    bool hasNext = HasNextPage(m_page);
    if ( btnLabelWasNext != hasNext )
    {
        // need to update
        if (btnLabelWasNext)
            m_btnNext->SetLabel(_("&Finish"));
        else
            m_btnNext->SetLabel(_("&Next >"));
    }
    m_btnNext->SetDefault();
    // nothing to do: the label was already correct

    // send the change event to the new page now
    wxWizardEvent event(wxEVT_WIZARD_PAGE_CHANGED, GetId(), goingForward, m_page);
    (void)m_page->GetEventHandler()->ProcessEvent(event);

    // and finally show it
    m_page->Show();
    m_page->SetFocus();

    return true;
}

bool wxWizard::RunWizard(wxWizardPage *firstPage)
{
    wxCHECK_MSG( firstPage, false, wxT("can't run empty wizard") );

    // This cannot be done sooner, because user can change layout options
    // up to this moment
    FinishLayout();

    // can't return false here because there is no old page
    (void)ShowPage(firstPage, true /* forward */);

    return ShowModal() == wxID_OK;
}

wxWizardPage *wxWizard::GetCurrentPage() const
{
    return m_page;
}

wxSize wxWizard::GetPageSize() const
{
    wxSize pageSize(GetManualPageSize());
    pageSize.IncTo(m_sizerPage->GetMaxChildSize());
    return pageSize;
}

wxSizer *wxWizard::GetPageAreaSizer() const
{
    return m_sizerPage;
}

void wxWizard::SetBorder(int border)
{
    wxCHECK_RET(!m_started,wxT("wxWizard::SetBorder after RunWizard"));

    m_calledSetBorder = true;
    m_border = border;
}

wxSize wxWizard::GetManualPageSize() const
{
    // default width and height of the page
    static const int DEFAULT_PAGE_WIDTH = 270;
    //static const int DEFAULT_PAGE_HEIGHT = 290;
    // For compatibility with 2.4: there's too much
    // space under the bitmap, probably due to differences in
    // the sizer implementation. This makes it reasonable again.
    static const int DEFAULT_PAGE_HEIGHT = 270;

    wxSize totalPageSize(DEFAULT_PAGE_WIDTH,DEFAULT_PAGE_HEIGHT);

    totalPageSize.IncTo(m_sizePage);

    if ( m_statbmp )
    {
        totalPageSize.IncTo(wxSize(0, m_bitmap.GetHeight()));
    }

    return totalPageSize;
}

void wxWizard::OnCancel(wxCommandEvent& WXUNUSED(eventUnused))
{
    // this function probably can never be called when we don't have an active
    // page, but a small extra check won't hurt
    wxWindow *win = m_page ? (wxWindow *)m_page : (wxWindow *)this;

    wxWizardEvent event(wxEVT_WIZARD_CANCEL, GetId(), false, m_page);
    if ( !win->GetEventHandler()->ProcessEvent(event) || event.IsAllowed() )
    {
        // no objections - close the dialog
        if(IsModal())
        {
            EndModal(wxID_CANCEL);
        }
        else
        {
            SetReturnCode(wxID_CANCEL);
            Hide();
        }
    }
    //else: request to Cancel ignored
}

void wxWizard::OnBackOrNext(wxCommandEvent& event)
{
    wxASSERT_MSG( (event.GetEventObject() == m_btnNext) ||
                  (event.GetEventObject() == m_btnPrev),
                  wxT("unknown button") );

    // ask the current page first: notice that we do it before calling
    // GetNext/Prev() because the data transfered from the controls of the page
    // may change the value returned by these methods
    if ( m_page && (!m_page->Validate() || !m_page->TransferDataFromWindow()) )
    {
        // the page data is incorrect, don't do anything
        return;
    }

    bool forward = event.GetEventObject() == m_btnNext;

    wxWizardPage *page;
    if ( forward )
    {
        page = m_page->GetNext();
    }
    else // back
    {
        page = m_page->GetPrev();

        wxASSERT_MSG( page, wxT("\"<Back\" button should have been disabled") );
    }

    // just pass to the new page (or may be not - but we don't care here)
    (void)ShowPage(page, forward);
}

void wxWizard::OnHelp(wxCommandEvent& WXUNUSED(event))
{
    // this function probably can never be called when we don't have an active
    // page, but a small extra check won't hurt
    if(m_page != NULL)
    {
        // Create and send the help event to the specific page handler
        // event data contains the active page so that context-sensitive
        // help is possible
        wxWizardEvent eventHelp(wxEVT_WIZARD_HELP, GetId(), true, m_page);
        (void)m_page->GetEventHandler()->ProcessEvent(eventHelp);
    }
}

void wxWizard::OnWizEvent(wxWizardEvent& event)
{
    // the dialogs have wxWS_EX_BLOCK_EVENTS style on by default but we want to
    // propagate wxEVT_WIZARD_XXX to the parent (if any), so do it manually
    if ( !(GetExtraStyle() & wxWS_EX_BLOCK_EVENTS) )
    {
        // the event will be propagated anyhow
        event.Skip();
    }
    else
    {
        wxWindow *parent = GetParent();

        if ( !parent || !parent->GetEventHandler()->ProcessEvent(event) )
        {
            event.Skip();
        }
    }

    if ( !IsModal() &&
         event.IsAllowed() &&
         ( event.GetEventType() == wxEVT_WIZARD_FINISHED ||
           event.GetEventType() == wxEVT_WIZARD_CANCEL
         )
       )
    {
        Destroy();
    }
}

// ----------------------------------------------------------------------------
// our public interface
// ----------------------------------------------------------------------------

#if WXWIN_COMPATIBILITY_2_2

/* static */
wxWizard *wxWizardBase::Create(wxWindow *parent,
                               int id,
                               const wxString& title,
                               const wxBitmap& bitmap,
                               const wxPoint& pos,
                               const wxSize& WXUNUSED(size))
{
    return new wxWizard(parent, id, title, bitmap, pos);
}

#endif // WXWIN_COMPATIBILITY_2_2

// ----------------------------------------------------------------------------
// wxWizardEvent
// ----------------------------------------------------------------------------

wxWizardEvent::wxWizardEvent(wxEventType type, int id, bool direction, wxWizardPage* page)
             : wxNotifyEvent(type, id)
{
    // Modified 10-20-2001 Robert Cavanaugh
    // add the active page to the event data
    m_direction = direction;
    m_page = page;
}

#endif // wxUSE_WIZARDDLG
