///////////////////////////////////////////////////////////////////////////////
// Name:        generic/wizard.cpp
// Purpose:     generic implementation of wxWizard class
// Author:      Vadim Zeitlin
// Modified by:
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
    #pragma implementation ".h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/dynarray.h"
    #include "wx/intl.h"
    #include "wx/statbmp.h"
#endif //WX_PRECOMP

#include "wx/statline.h"

#include "wx/wizard.h"

// ----------------------------------------------------------------------------
// simple types
// ----------------------------------------------------------------------------

WX_DEFINE_ARRAY(wxPanel *, wxArrayPages);

// ----------------------------------------------------------------------------
// event tables and such
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxWizard, wxDialog)
    EVT_BUTTON(wxID_CANCEL, wxWizard::OnCancel)
    EVT_BUTTON(-1, wxWizard::OnBackOrNext)
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

wxWizardPage::wxWizardPage(wxWizard *parent, const wxBitmap& bitmap)
            : wxPanel(parent), m_bitmap(bitmap)
{
    // initially the page is hidden, it's shown only when it becomes current
    Hide();
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

wxWizard::wxWizard(wxWindow *parent,
                   int id,
                   const wxString& title,
                   const wxBitmap& bitmap,
                   const wxPoint& pos)
        : m_posWizard(pos), m_bitmap(bitmap)
{
    // just create the dialog itself here, the controls will be created in
    // DoCreateControls() called later when we know our final size
    m_page = (wxWizardPage *)NULL;
    m_btnPrev = m_btnNext = NULL;
    m_statbmp = NULL;

    (void)wxDialog::Create(parent, id, title, pos);
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

    x = m_x + m_width - 3*sizeBtn.x - BUTTON_MARGIN;
    y += SEPARATOR_LINE_MARGIN;
    m_btnPrev = new wxButton(this, -1, _("< &Back"), wxPoint(x, y), sizeBtn);

    x += sizeBtn.x;
    m_btnNext = new wxButton(this, -1, _("&Next >"), wxPoint(x, y), sizeBtn);

    x += sizeBtn.x + BUTTON_MARGIN;
    (void)new wxButton(this, wxID_CANCEL, _("Cancel"), wxPoint(x, y), sizeBtn);

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

bool wxWizard::ShowPage(wxWizardPage *page, bool goingForward)
{
    wxASSERT_MSG( page != m_page, wxT("this is useless") );

    // we'll use this to decide whether we have to change the label of this
    // button or not (initially the label is "Next")
    bool btnLabelWasNext = TRUE;

    // and this tells us whether we already had the default bitmap before
    bool bmpWasDefault = TRUE;

    if ( m_page )
    {
        // ask the current page first
        if ( !m_page->TransferDataFromWindow() )
        {
            // the page data is incorrect
            return FALSE;
        }

        // send the event to the old page
        wxWizardEvent event(wxEVT_WIZARD_PAGE_CHANGING, GetId(), goingForward);
        if ( m_page->GetEventHandler()->ProcessEvent(event) &&
             !event.IsAllowed() )
        {
            // vetoed by the page
            return FALSE;
        }

        m_page->Hide();

        btnLabelWasNext = m_page->GetNext() != (wxWizardPage *)NULL;
        bmpWasDefault = !m_page->GetBitmap().Ok();
    }

    // set the new one
    m_page = page;

    // is this the end?
    if ( !m_page )
    {
        // terminate successfully
        EndModal(wxID_OK);

        return TRUE;
    }

    // send the event to the new page now
    wxWizardEvent event(wxEVT_WIZARD_PAGE_CHANGED, GetId(), goingForward);
    (void)m_page->GetEventHandler()->ProcessEvent(event);

    // position and show the new page
    (void)m_page->TransferDataToWindow();
    m_page->SetSize(m_x, m_y, m_width, m_height);
    m_page->Show();

    // change the bitmap if necessary (and if we have it at all)
    bool bmpIsDefault = !m_page->GetBitmap().Ok();
    if ( m_statbmp && (bmpIsDefault != bmpWasDefault) )
    {
        wxBitmap bmp;
        if ( bmpIsDefault )
            bmp = m_bitmap;
        else
            bmp = m_page->GetBitmap();
        m_statbmp->SetBitmap(bmp);
    }

    // and update the buttons state
    m_btnPrev->Enable(m_page->GetPrev() != (wxWizardPage *)NULL);

    bool hasNext = m_page->GetNext() != (wxWizardPage *)NULL;
    if ( btnLabelWasNext != hasNext )
    {
        // need to update
        if (btnLabelWasNext)
            m_btnNext->SetLabel(_("&Finish"));
        else
            m_btnNext->SetLabel(_("&Next >"));
    }
    // nothing to do: the label was already correct

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
    return wxSize(m_width, m_height);
}

void wxWizard::OnCancel(wxCommandEvent& WXUNUSED(event))
{
    // this function probably can never be called when we don't have an active
    // page, but a small extra check won't hurt
    wxWindow *win = m_page ? (wxWindow *)m_page : (wxWindow *)this;

    wxWizardEvent event(wxEVT_WIZARD_CANCEL, GetId());
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

// ----------------------------------------------------------------------------
// our public interface
// ----------------------------------------------------------------------------

/* static */
wxWizard *wxWizardBase::Create(wxWindow *parent,
                               int id,
                               const wxString& title,
                               const wxBitmap& bitmap,
                               const wxPoint& pos,
                               const wxSize& size)
{
    return new wxWizard(parent, id, title, bitmap, pos, size);
}

// ----------------------------------------------------------------------------
// wxWizardEvent
// ----------------------------------------------------------------------------

wxWizardEvent::wxWizardEvent(wxEventType type, int id, bool direction)
             : wxNotifyEvent(type, id)
{
    m_direction = direction;
}

