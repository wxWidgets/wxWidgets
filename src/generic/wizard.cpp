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
#endif //WX_PRECOMP

#include "wx/statline.h"

#include "wx/wizard.h"

// ----------------------------------------------------------------------------
// simple types
// ----------------------------------------------------------------------------

WX_DEFINE_ARRAY(wxPanel *, wxArrayPages);

// ----------------------------------------------------------------------------
// wxWizardGeneric - generic implementation of wxWizard
// ----------------------------------------------------------------------------

class wxWizardGeneric : public wxWizard
{
public:
    // ctor
    wxWizardGeneric(wxWindow *parent,
                    int id,
                    const wxString& title,
                    const wxBitmap& bitmap,
                    const wxPoint& pos,
                    const wxSize& size);

    // implement base class pure virtuals
    virtual void AddPage(wxPanel *page);
    virtual void InsertPage(int nPage, wxPanel *page);
    virtual bool RunWizard();
    virtual wxPanel *GetCurrentPage() const;

    // implementation only from now on
    // -------------------------------

    // is the wizard running?
    bool IsRunning() const { return m_page != -1; }

    // show the given page calling TransferDataFromWindow - if it returns
    // FALSE, the old page is not hidden and the function returns FALSE
    bool ShowPage(size_t page);

    // get the current page assuming the wizard is running
    wxPanel *DoGetCurrentPage() const
    {
        wxASSERT_MSG( IsRunning(), _T("no current page!") );

        return m_pages[(size_t)m_page];
    }

    // place the given page correctly and hide it
    void DoAddPage(wxPanel *page);

private:
    // event handlers
    void OnCancel(wxCommandEvent& event);
    void OnBackOrNext(wxCommandEvent& event);

    // wizard dimensions
    int          m_x, m_y;      // the origin for the pages
    int          m_width,       // the size of the page itself
                 m_height;      // (total width is m_width + m_x)

    // wizard state
    int          m_page;        // the current page or -1
    wxArrayPages m_pages;       // the array with all wizards pages

    // wizard controls
    wxButton    *m_btnPrev,     // the "<Back" button
                *m_btnNext;     // the "Next>" or "Finish" button

    DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// event tables and such
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxWizardGeneric, wxDialog)
    EVT_BUTTON(wxID_CANCEL, OnCancel)
    EVT_BUTTON(-1, OnBackOrNext)
END_EVENT_TABLE()

IMPLEMENT_ABSTRACT_CLASS(wxWizard, wxDialog)
IMPLEMENT_DYNAMIC_CLASS(wxWizardEvent, wxNotifyEvent)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// generic wxWizard implementation
// ----------------------------------------------------------------------------

wxWizardGeneric::wxWizardGeneric(wxWindow *parent,
                                 int id,
                                 const wxString& title,
                                 const wxBitmap& bitmap,
                                 const wxPoint& pos,
                                 const wxSize& size)
{
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

    // init members
    // ------------

    m_page = -1;

    // create controls
    // ---------------

    wxSize sizeBtn = wxButton::GetDefaultSize();

    (void)wxDialog::Create(parent, id, title, pos, size);

    // the global dialog layout is: a row of buttons at the bottom (aligned to
    // the right), the static line above them, the bitmap (if any) on the left
    // of the upper part of the dialog and the panel in the remaining space
    m_x = X_MARGIN;
    m_y = Y_MARGIN;
    if ( bitmap.Ok() )
    {
        (void)new wxStaticBitmap(this, -1, bitmap, wxPoint(m_x, m_y));

        m_x += bitmap.GetWidth() + BITMAP_X_MARGIN;
        m_height = bitmap.GetHeight();
    }
    else
    {
        m_height = DEFAULT_PAGE_HEIGHT;
    }

    m_width = DEFAULT_PAGE_WIDTH;

    int x = X_MARGIN;
    int y = m_y + m_height + BITMAP_Y_MARGIN;
    (void)new wxStaticLine(this, -1, wxPoint(x, y),
                           wxSize(m_x + m_width - x, 2));

    x = m_x + m_width - 3*sizeBtn.x - BUTTON_MARGIN;
    y += SEPARATOR_LINE_MARGIN;
    m_btnPrev = new wxButton(this, -1, _("< &Back"), wxPoint(x, y), sizeBtn);

    x += sizeBtn.x;
    m_btnNext = new wxButton(this, -1, _("&Next >"), wxPoint(x, y), sizeBtn);

    x += sizeBtn.x + BUTTON_MARGIN;
    (void)new wxButton(this, wxID_CANCEL, _("Cancel"), wxPoint(x, y), sizeBtn);

    // position and size the dialog
    // ----------------------------

    if ( size == wxDefaultSize )
    {
        SetClientSize(m_x + m_width + X_MARGIN,
                      m_y + m_height + BITMAP_Y_MARGIN +
                        SEPARATOR_LINE_MARGIN + sizeBtn.y + Y_MARGIN);
    }

    if ( pos == wxDefaultPosition )
    {
        Centre();
    }
}

bool wxWizardGeneric::ShowPage(size_t page)
{
    wxCHECK_MSG( page < m_pages.GetCount(), FALSE,
                 _T("invalid wizard page index") );

    wxASSERT_MSG( page != (size_t)m_page, _T("this is useless") );

    size_t last = m_pages.GetCount() - 1;
    bool mustChangeNextBtnLabel = (size_t)m_page == last || page == last;

    if ( m_page != -1 )
    {
        wxPanel *panel = DoGetCurrentPage();
        if ( !panel->TransferDataFromWindow() )
            return FALSE;

        panel->Hide();
    }

    m_page = page;
    DoGetCurrentPage()->Show();

    // update the buttons state
    m_btnPrev->Enable(m_page != 0);
    if ( mustChangeNextBtnLabel )
    {
        m_btnNext->SetLabel((size_t)m_page == last ? _("&Finish")
                                                   : _("&Next >"));
    }

    return TRUE;
}

void wxWizardGeneric::DoAddPage(wxPanel *page)
{
    page->Hide();
    page->SetSize(m_x, m_y, m_width, m_height);
}

void wxWizardGeneric::AddPage(wxPanel *page)
{
    m_pages.Add(page);

    DoAddPage(page);
}

void wxWizardGeneric::InsertPage(int nPage, wxPanel *page)
{
    m_pages.Insert(page, nPage);
    if ( nPage < m_page )
    {
        // the indices of all pages after the inserted one are shifted by 1
        m_page++;
    }

    DoAddPage(page);
}

bool wxWizardGeneric::RunWizard()
{
    wxCHECK_MSG( m_pages.GetCount() != 0, FALSE, _T("can't run empty wizard") );

    // can't return FALSE here because there is no old page
    (void)ShowPage(0u);

    return ShowModal() == wxID_OK;
}

wxPanel *wxWizardGeneric::GetCurrentPage() const
{
    return IsRunning() ? DoGetCurrentPage() : (wxPanel *)NULL;
}

void wxWizardGeneric::OnCancel(wxCommandEvent& WXUNUSED(event))
{
    wxWizardEvent event(wxEVT_WIZARD_CANCEL, GetId());
    if ( !GetEventHandler()->ProcessEvent(event) || event.IsAllowed() )
    {
        // no objections - close the dialog
        EndModal(wxID_CANCEL);
    }
    //else: request to Cancel ignored
}

void wxWizardGeneric::OnBackOrNext(wxCommandEvent& event)
{
    wxASSERT_MSG( (event.GetEventObject() == m_btnNext) ||
                  (event.GetEventObject() == m_btnPrev),
                  _T("unknown button") );

    int delta = event.GetEventObject() == m_btnNext ? 1 : -1;
    int page = m_page + delta;

    wxASSERT_MSG( page >= 0, _T("'Back' button should have been disabled!") );

    if ( (size_t)page == m_pages.GetCount() )
    {
        // check that we have valid data in the last page too
        if ( m_pages.Last()->TransferDataFromWindow() )
        {
            // that's all, folks!
            EndModal(wxID_OK);
        }
    }
    else
    {
        // just pass to the next page (or may be not - but we don't care here)
        (void)ShowPage(page);
    }
}

// ----------------------------------------------------------------------------
// our public interface
// ----------------------------------------------------------------------------

/* static */ wxWizard *wxWizard::Create(wxWindow *parent,
                                        int id,
                                        const wxString& title,
                                        const wxBitmap& bitmap,
                                        const wxPoint& pos,
                                        const wxSize& size)
{
    return new wxWizardGeneric(parent, id, title, bitmap, pos, size);
}

// ----------------------------------------------------------------------------
// wxWizardEvent
// ----------------------------------------------------------------------------

wxWizardEvent::wxWizardEvent(wxEventType type, int id)
             : wxNotifyEvent(type, id)
{
    m_page = m_pageOld = -1;
}
