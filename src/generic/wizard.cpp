///////////////////////////////////////////////////////////////////////////////
// Name:        generic/wizard.cpp
// Purpose:     generic implementation of wxWizard class
// Author:      Vadim Zeitlin
// Modified by: Robert Cavanaugh
//              1) Added capability for wxWizardPage to accept resources
//              2) Added "Help" button handler stub
//              3) Fixed ShowPage() bug on displaying bitmaps
// Created:     15.08.99
// RCS-ID:      $Id$
// Copyright:   (c) 1999 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
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

#include "wx/wizard.h"

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

    EVT_WIZARD_PAGE_CHANGED(-1, wxWizard::OnWizEvent)
    EVT_WIZARD_PAGE_CHANGING(-1, wxWizard::OnWizEvent)
    EVT_WIZARD_CANCEL(-1, wxWizard::OnWizEvent)
    EVT_WIZARD_FINISHED(-1, wxWizard::OnWizEvent)
    EVT_WIZARD_HELP(-1, wxWizard::OnWizEvent)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(wxWizard, wxDialog)
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
    if ( !wxPanel::Create(parent, -1) )
        return FALSE;

    if ( resource != NULL )
    {
#if wxUSE_WX_RESOURCES
        if ( !LoadFromResource(this, resource) )
        {
            wxFAIL_MSG(wxT("wxWizardPage LoadFromResource failed!!!!"));
        }
#endif // wxUSE_RESOURCES
    }

    m_bitmap = bitmap;

    // initially the page is hidden, it's shown only when it becomes current
    Hide();

    return TRUE;
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
// generic wxWizard implementation
// ----------------------------------------------------------------------------

void wxWizard::Init()
{
    m_posWizard = wxDefaultPosition;
    m_page = (wxWizardPage *)NULL;
    m_btnPrev = m_btnNext = NULL;
    m_statbmp = NULL;
}

bool wxWizard::Create(wxWindow *parent,
                   int id,
                   const wxString& title,
                   const wxBitmap& bitmap,
                   const wxPoint& pos)
{
    m_posWizard = pos;
    m_bitmap = bitmap ;

    // just create the dialog itself here, the controls will be created in
    // DoCreateControls() called later when we know our final size
    m_page = (wxWizardPage *)NULL;
    m_btnPrev = m_btnNext = NULL;
    m_statbmp = NULL;

    return wxDialog::Create(parent, id, title, pos);
}

void wxWizard::DoCreateControls()
{
    // do nothing if the controls were already created
    if ( WasCreated() )
        return;

    // constants defining the dialog layout
    // ------------------------------------

    // these constants define the position of the upper left corner of the
    // bitmap or the page in the wizard
    static const int X_MARGIN = 10;
    static const int Y_MARGIN = 10;

    // margin between the bitmap and the panel
    static const int BITMAP_X_MARGIN = 15;

    // margin between the bitmap and the static line
    static const int BITMAP_Y_MARGIN = 15;

    // margin between the static line and the buttons
    static const int SEPARATOR_LINE_MARGIN = 15;

    // margin between "Next >" and "Cancel" buttons
    static const int BUTTON_MARGIN = 10;

    // margin between Back and Next buttons
#ifdef __WXMAC__
    static const int BACKNEXT_MARGIN = 10;
#else
    static const int BACKNEXT_MARGIN = 0;
#endif

    // default width and height of the page
    static const int DEFAULT_PAGE_WIDTH = 270;
    static const int DEFAULT_PAGE_HEIGHT = 290;

    // create controls
    // ---------------

    wxSize sizeBtn = wxButton::GetDefaultSize();

    // the global dialog layout is: a row of buttons at the bottom (aligned to
    // the right), the static line above them, the bitmap (if any) on the left
    // of the upper part of the dialog and the panel in the remaining space
    m_x = X_MARGIN;
    m_y = Y_MARGIN;

    int defaultHeight;
    if ( m_bitmap.Ok() )
    {
        m_statbmp = new wxStaticBitmap(this, -1, m_bitmap, wxPoint(m_x, m_y));

        m_x += m_bitmap.GetWidth() + BITMAP_X_MARGIN;

        defaultHeight = m_bitmap.GetHeight();
    }
    else
    {
        m_statbmp = (wxStaticBitmap *)NULL;

        defaultHeight = DEFAULT_PAGE_HEIGHT;
    }

    // use default size if none given and also make sure that the dialog is
    // not less than the default size
    m_height = m_sizePage.y == -1 ? defaultHeight : m_sizePage.y;
    m_width = m_sizePage.x == -1 ? DEFAULT_PAGE_WIDTH : m_sizePage.x;
    if ( m_height < defaultHeight )
        m_height = defaultHeight;
    if ( m_width < DEFAULT_PAGE_WIDTH )
        m_width = DEFAULT_PAGE_WIDTH;

    int x = X_MARGIN;
    int y = m_y + m_height + BITMAP_Y_MARGIN;

#if wxUSE_STATLINE
    (void)new wxStaticLine(this, -1, wxPoint(x, y),
                           wxSize(m_x + m_width - x, 2));
#endif // wxUSE_STATLINE

    x = m_x + m_width - 3*sizeBtn.x - BUTTON_MARGIN - BACKNEXT_MARGIN;
    y += SEPARATOR_LINE_MARGIN;

    if (GetExtraStyle() & wxWIZARD_EX_HELPBUTTON)
    {
        x -= sizeBtn.x;
        x -= BUTTON_MARGIN ;

        (void)new wxButton(this, wxID_HELP, _("&Help"), wxPoint(x, y), sizeBtn);
        x += sizeBtn.x;
        x += BUTTON_MARGIN ;
    }

    m_btnPrev = new wxButton(this, wxID_BACKWARD, _("< &Back"), wxPoint(x, y), sizeBtn);

    x += sizeBtn.x;
    x += BACKNEXT_MARGIN;

    m_btnNext = new wxButton(this, wxID_FORWARD, _("&Next >"), wxPoint(x, y), sizeBtn);

    x += sizeBtn.x + BUTTON_MARGIN;
    (void)new wxButton(this, wxID_CANCEL, _("&Cancel"), wxPoint(x, y), sizeBtn);

    // position and size the dialog
    // ----------------------------

    SetClientSize(m_x + m_width + X_MARGIN,
                  m_y + m_height + BITMAP_Y_MARGIN +
                    SEPARATOR_LINE_MARGIN + sizeBtn.y + Y_MARGIN);

    if ( m_posWizard == wxDefaultPosition )
    {
        CentreOnScreen();
    }
}

void wxWizard::SetPageSize(const wxSize& size)
{
    // otherwise it will have no effect now as it's too late...
    wxASSERT_MSG( !WasCreated(), _T("should be called before RunWizard()!") );

    m_sizePage = size;
}

void wxWizard::FitToPage(const wxWizardPage *page)
{
    // otherwise it will have no effect now as it's too late...
    wxASSERT_MSG( !WasCreated(), _T("should be called before RunWizard()!") );

    wxSize sizeMax;
    while ( page )
    {
        wxSize size = page->GetBestSize();

        if ( size.x > sizeMax.x )
            sizeMax.x = size.x;

        if ( size.y > sizeMax.y )
            sizeMax.y = size.y;

        page = page->GetNext();
    }

    if ( sizeMax.x > m_sizePage.x )
        m_sizePage.x = sizeMax.x;

    if ( sizeMax.y > m_sizePage.y )
        m_sizePage.y = sizeMax.y;
}

bool wxWizard::ShowPage(wxWizardPage *page, bool goingForward)
{
    wxASSERT_MSG( page != m_page, wxT("this is useless") );

    // we'll use this to decide whether we have to change the label of this
    // button or not (initially the label is "Next")
    bool btnLabelWasNext = TRUE;

    // Modified 10-20-2001 Robert Cavanaugh.
    // Fixed bug for displaying a new bitmap
    // in each *consecutive* page

    // flag to indicate if this page uses a new bitmap
    bool bmpIsDefault = TRUE;

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
            return FALSE;
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
        EndModal(wxID_OK);
        if ( !IsModal() )
         {
           wxWizardEvent event(wxEVT_WIZARD_FINISHED, GetId(),FALSE, 0);
           (void)GetEventHandler()->ProcessEvent(event);
         }
        return TRUE;
    }

    // position and show the new page
    (void)m_page->TransferDataToWindow();
    m_page->SetSize(m_x, m_y, m_width, m_height);

    // check if bitmap needs to be updated
    // update default flag as well
    if ( m_page->GetBitmap().Ok() )
    {
        bmpCur = m_page->GetBitmap();
        bmpIsDefault = FALSE;
    }

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
    // nothing to do: the label was already correct

    // send the change event to the new page now
    wxWizardEvent event(wxEVT_WIZARD_PAGE_CHANGED, GetId(), goingForward, m_page);
    (void)m_page->GetEventHandler()->ProcessEvent(event);

    // and finally show it
    m_page->Show();
    m_page->SetFocus();

    return TRUE;
}

bool wxWizard::RunWizard(wxWizardPage *firstPage)
{
    wxCHECK_MSG( firstPage, FALSE, wxT("can't run empty wizard") );

    DoCreateControls();

    // can't return FALSE here because there is no old page
    (void)ShowPage(firstPage, TRUE /* forward */);

    return ShowModal() == wxID_OK;
}

wxWizardPage *wxWizard::GetCurrentPage() const
{
    return m_page;
}

wxSize wxWizard::GetPageSize() const
{
    // make sure that the controls are created because otherwise m_width and
    // m_height would be both still -1
    wxConstCast(this, wxWizard)->DoCreateControls();

    return wxSize(m_width, m_height);
}

void wxWizard::OnCancel(wxCommandEvent& WXUNUSED(eventUnused))
{
    // this function probably can never be called when we don't have an active
    // page, but a small extra check won't hurt
    wxWindow *win = m_page ? (wxWindow *)m_page : (wxWindow *)this;

    wxWizardEvent event(wxEVT_WIZARD_CANCEL, GetId(), FALSE, m_page);
    if ( !win->GetEventHandler()->ProcessEvent(event) || event.IsAllowed() )
    {
        // no objections - close the dialog
        EndModal(wxID_CANCEL);
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
    if ( m_page && !m_page->TransferDataFromWindow() )
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
        wxWizardEvent eventHelp(wxEVT_WIZARD_HELP, GetId(), TRUE, m_page);
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
        return;
    }

    wxWindow *parent = GetParent();

    if ( !parent || !parent->GetEventHandler()->ProcessEvent(event) )
    {
        event.Skip();
    }
}

// ----------------------------------------------------------------------------
// our public interface
// ----------------------------------------------------------------------------

#ifdef WXWIN_COMPATIBILITY_2_2

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
