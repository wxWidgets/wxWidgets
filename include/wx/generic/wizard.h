///////////////////////////////////////////////////////////////////////////////
// Name:        generic/wizard.h
// Purpose:     declaration of generic wxWizard class
// Author:      Vadim Zeitlin
// Modified by:
// Created:     28.09.99
// RCS-ID:      $Id$
// Copyright:   (c) 1999 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// wxWizard
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxButton;
class WXDLLEXPORT wxStaticBitmap;

class WXDLLEXPORT wxWizard : public wxWizardBase
{
public:
    // ctor
    wxWizard(wxWindow *parent = NULL,
             int id = -1,
             const wxString& title = wxEmptyString,
             const wxBitmap& bitmap = wxNullBitmap,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize);

    // implement base class pure virtuals
    virtual bool RunWizard(wxWizardPage *firstPage);
    virtual wxWizardPage *GetCurrentPage() const;
    virtual wxSize GetPageSize() const;

    // implementation only from now on
    // -------------------------------

    // is the wizard running?
    bool IsRunning() const { return m_page != NULL; }

    // show the prev/next page, but call TransferDataFromWindow on the current
    // page first and return FALSE without changing the page if it returns
    // FALSE
    bool ShowPage(wxWizardPage *page, bool goingForward = TRUE);

private:
    // event handlers
    void OnCancel(wxCommandEvent& event);
    void OnBackOrNext(wxCommandEvent& event);

    // wizard dimensions
    int          m_x, m_y;      // the origin for the pages
    int          m_width,       // the size of the page itself
                 m_height;      // (total width is m_width + m_x)

    // wizard state
    wxWizardPage *m_page;       // the current page or NULL
    wxBitmap      m_bitmap;     // the default bitmap to show

    // wizard controls
    wxButton    *m_btnPrev,     // the "<Back" button
                *m_btnNext;     // the "Next>" or "Finish" button
    wxStaticBitmap *m_statbmp;  // the control for the bitmap

    DECLARE_DYNAMIC_CLASS(wxWizard)
    DECLARE_EVENT_TABLE()
};

