#ifndef _WX_NOTEBOOK_H_BASE_
#define _WX_NOTEBOOK_H_BASE_

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/event.h"   // the base class: wxNotifyEvent

// ----------------------------------------------------------------------------
// notebook event class (used by NOTEBOOK_PAGE_CHANGED/ING events)
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxNotebookEvent : public wxNotifyEvent
{
public:
    wxNotebookEvent(wxEventType commandType = wxEVT_NULL, int id = 0,
                    int nSel = -1, int nOldSel = -1)
        : wxNotifyEvent(commandType, id)
        {
            m_nSel = nSel;
            m_nOldSel = nOldSel;
        }

    // accessors
        // the currently selected page (-1 if none)
    int GetSelection() const { return m_nSel; }
    void SetSelection(int nSel) { m_nSel = nSel; }
        // the page that was selected before the change (-1 if none)
    int GetOldSelection() const { return m_nOldSel; }
    void SetOldSelection(int nOldSel) { m_nOldSel = nOldSel; }

private:
    int m_nSel,     // currently selected page
        m_nOldSel;  // previously selected page

    DECLARE_DYNAMIC_CLASS(wxNotebookEvent)
};

// ----------------------------------------------------------------------------
// event macros
// ----------------------------------------------------------------------------

typedef void (wxEvtHandler::*wxNotebookEventFunction)(wxNotebookEvent&);

// Truncation in 16-bit BC++ means we need to define these differently
#if defined(__BORLANDC__) && defined(__WIN16__)
#define EVT_NOTEBOOK_PAGE_CHANGED(id, fn)                                   \
  wxEventTableEntry(                                                        \
    wxEVT_COMMAND_NB_PAGE_CHANGED,                                          \
    id,                                                                     \
    -1,                                                                     \
    (wxObjectEventFunction)(wxEventFunction)(wxNotebookEventFunction) &fn,  \
    NULL                                                                    \
  ),

#define EVT_NOTEBOOK_PAGE_CHANGING(id, fn)                                  \
  wxEventTableEntry(                                                        \
    wxEVT_COMMAND_NB_PAGE_CHANGING,                                         \
    id,                                                                     \
    -1,                                                                     \
    (wxObjectEventFunction)(wxEventFunction)(wxNotebookEventFunction) &fn,  \
    NULL                                                                    \
  ),

#else

#define EVT_NOTEBOOK_PAGE_CHANGED(id, fn)                                   \
  wxEventTableEntry(                                                        \
    wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED,                                    \
    id,                                                                     \
    -1,                                                                     \
    (wxObjectEventFunction)(wxEventFunction)(wxNotebookEventFunction) &fn,  \
    NULL                                                                    \
  ),

#define EVT_NOTEBOOK_PAGE_CHANGING(id, fn)                                  \
  wxEventTableEntry(                                                        \
    wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING,                                   \
    id,                                                                     \
    -1,                                                                     \
    (wxObjectEventFunction)(wxEventFunction)(wxNotebookEventFunction) &fn,  \
    NULL                                                                    \
  ),

#endif

// ----------------------------------------------------------------------------
// wxNotebook class itself
// ----------------------------------------------------------------------------

#if defined(__WXMSW__)
#ifdef __WIN16__
  #include  "wx/generic/notebook.h"
#else
  #include  "wx/msw/notebook.h"
#endif
#elif defined(__WXMOTIF__)
  #include  "wx/generic/notebook.h"
#elif defined(__WXGTK__)
  #include  "wx/gtk/notebook.h"
#elif defined(__WXQT__)
  #include  "wx/qt/notebook.h"
#elif defined(__WXMAC__)
  #include  "wx/mac/notebook.h"
#elif defined(__WXPM__)
  #include  "wx/os2/notebook.h"
#elif defined(__WXSTUBS__)
  #include  "wx/stubs/notebook.h"
#endif

#endif
    // _WX_NOTEBOOK_H_BASE_
