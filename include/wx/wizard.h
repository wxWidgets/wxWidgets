///////////////////////////////////////////////////////////////////////////////
// Name:        wizard.h
// Purpose:     wxWizard class: a GUI control presenting the user with a
//              sequence of dialogs which allows to simply perform some task
// Author:      Vadim Zeitlin (partly based on work by Ron Kuris and Kevin B.
//              Smith)
// Modified by:
// Created:     15.08.99
// RCS-ID:      $Id$
// Copyright:   (c) 1999 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WIZARD_H_
#define _WX_WIZARD_H_

// ----------------------------------------------------------------------------
// headers and other simple declarations
// ----------------------------------------------------------------------------

#ifndef WX_PRECOMP
    #include "wx/dialog.h"      // the base class

    #include "wx/event.h"       // wxEVT_XXX constants
#endif // WX_PRECOMP

// forward declarations
class WXDLLEXPORT wxWizard;

// ----------------------------------------------------------------------------
// wxWizardPage is one of the wizards screen: it must know what are the
// following and preceding pages (which may be NULL for the first/last page).
//
// Other than GetNext/Prev() functions, wxWizardPage is just a panel and may be
// used as such (i.e. controls may be placed directly on it &c).
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxWizardPage : public wxPanel
{
public:
    // ctor accepts an optional bitmap which will be used for this page instead
    // of the default one for this wizard (should be of the same size). Notice
    // that no other parameters are needed because the wizard will resize and
    // reposition the page anyhow
    wxWizardPage(wxWizard *parent, const wxBitmap& bitmap = wxNullBitmap);

    // these functions are used by the wizard to show another page when the
    // user chooses "Back" or "Next" button
    virtual wxWizardPage *GetPrev() const = 0;
    virtual wxWizardPage *GetNext() const = 0;

    // default GetBitmap() will just return m_bitmap which is ok in 99% of
    // cases - override this method if you want to create the bitmap to be used
    // dynamically or to do something even more fancy. It's ok to return
    // wxNullBitmap from here - the default one will be used then.
    virtual wxBitmap GetBitmap() const { return m_bitmap; }

protected:
    wxBitmap m_bitmap;

private:
    DECLARE_ABSTRACT_CLASS(wxWizardPage)
};

// ----------------------------------------------------------------------------
// wxWizardPageSimple just returns the pointers given to the ctor and is useful
// to create a simple wizard where the order of pages never changes.
//
// OTOH, it is also possible to dynamicly decide which page to return (i.e.
// depending on the user's choices) as the wizard sample shows - in order to do
// this, you must derive from wxWizardPage directly.
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxWizardPageSimple : public wxWizardPage
{
public:
    // ctor takes the previous and next pages
    wxWizardPageSimple(wxWizard *parent = NULL, // let it be default ctor too
                       wxWizardPage *prev = (wxWizardPage *)NULL,
                       wxWizardPage *next = (wxWizardPage *)NULL)
        : wxWizardPage(parent)
    {
        m_prev = prev;
        m_next = next;
    }

    // the pointers may be also set later - but before starting the wizard
    void SetPrev(wxWizardPage *prev) { m_prev = prev; }
    void SetNext(wxWizardPage *next) { m_next = next; }

    // a convenience function to make the pages follow each other
    static void Chain(wxWizardPageSimple *first, wxWizardPageSimple *second)
    {
        wxCHECK_RET( first && second,
                     wxT("NULL passed to wxWizardPageSimple::Chain") );

        first->SetNext(second);
        second->SetPrev(first);
    }

    // base class pure virtuals
    virtual wxWizardPage *GetPrev() const;
    virtual wxWizardPage *GetNext() const;

private:
    // pointers are private, the derived classes shouldn't mess with them -
    // just derive from wxWizardPage directly to implement different behaviour
    wxWizardPage *m_prev,
                 *m_next;

    DECLARE_DYNAMIC_CLASS(wxWizardPageSimple)
};

// ----------------------------------------------------------------------------
// wxWizard
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxWizardBase : public wxDialog
{
public:
    // create the wizard control
    static wxWizard *Create(wxWindow *parent,
                            int id = -1,
                            const wxString& title = wxEmptyString,
                            const wxBitmap& bitmap = wxNullBitmap,
                            const wxPoint& pos = wxDefaultPosition,
                            const wxSize& size = wxDefaultSize);

    // executes the wizard starting from the given page, returns TRUE if it was
    // successfully finished, FALSE if user cancelled it
    virtual bool RunWizard(wxWizardPage *firstPage) = 0;

    // get the current page (NULL if RunWizard() isn't running)
    virtual wxWizardPage *GetCurrentPage() const = 0;

    // get the size available for the page: the wizards are not resizeable, so
    // this size doesn't change
    virtual wxSize GetPageSize() const = 0;
};

// include the real class declaration
#include "wx/generic/wizard.h"

// ----------------------------------------------------------------------------
// wxWizardEvent class represents an event generated by the wizard: this event
// is first sent to the page itself and, if not processed there, goes up the
// window hierarchy as usual
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxWizardEvent : public wxNotifyEvent
{
public:
    wxWizardEvent(wxEventType type = wxEVT_NULL,
                  int id = -1,
                  bool direction = TRUE);

    // for EVT_WIZARD_PAGE_CHANGING, return TRUE if we're going forward or
    // FALSE otherwise and for EVT_WIZARD_PAGE_CHANGED return TRUE if we came
    // from the previous page and FALSE if we returned from the next one
    // (this function doesn't make sense for CANCEL events)
    bool GetDirection() const { return m_direction; }

private:
    bool m_direction;

    DECLARE_DYNAMIC_CLASS(wxWizardEvent)
};

// ----------------------------------------------------------------------------
// macros for handling wxWizardEvents
// ----------------------------------------------------------------------------

typedef void (wxEvtHandler::*wxWizardEventFunction)(wxWizardEvent&);

// notifies that the page has just been changed (can't be vetoed)
#define EVT_WIZARD_PAGE_CHANGED(id, fn) { wxEVT_WIZARD_PAGE_CHANGED, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxWizardEventFunction) & fn, (wxObject *)NULL },

// the user pressed "<Back" or "Next>" button and the page is going to be
// changed - unless the event handler vetoes the event
#define EVT_WIZARD_PAGE_CHANGING(id, fn) { wxEVT_WIZARD_PAGE_CHANGING, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxWizardEventFunction) & fn, (wxObject *)NULL },

// the user pressed "Cancel" button and the wizard is going to be dismissed -
// unless the event handler vetoes the event
#define EVT_WIZARD_CANCEL(id, fn) { wxEVT_WIZARD_CANCEL, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxWizardEventFunction) & fn, (wxObject *)NULL },

#endif // _WX_WIZARD_H_
