///////////////////////////////////////////////////////////////////////////////
// Name:        wx/notebook.h
// Purpose:     wxNotebook interface
// Author:      Vadim Zeitlin
// Modified by:
// Created:     01.02.01
// RCS-ID:      $Id$
// Copyright:   (c) 1996-2000 wxWindows team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_NOTEBOOK_H_BASE_
#define _WX_NOTEBOOK_H_BASE_

#if defined(__GNUG__) && !defined(__APPLE__)
    #pragma interface "notebookbase.h"
#endif

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/defs.h"

#if wxUSE_NOTEBOOK

#include "wx/control.h"
#include "wx/dynarray.h"
#include "wx/imaglist.h"

// ----------------------------------------------------------------------------
// types
// ----------------------------------------------------------------------------

// array of notebook pages
typedef wxWindow wxNotebookPage;  // so far, any window can be a page

WX_DEFINE_EXPORTED_ARRAY(wxNotebookPage *, wxArrayPages);

#define wxNOTEBOOK_NAME _T("notebook")

// ----------------------------------------------------------------------------
// wxNotebookBase: define wxNotebook interface
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxNotebookBase : public wxControl
{
public:
    // ctor
    wxNotebookBase()
    {
        Init();
    }

    // quasi ctor
    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxNOTEBOOK_NAME);

    // dtor
    virtual ~wxNotebookBase();

    // accessors
    // ---------

    // get number of pages in the dialog
    int GetPageCount() const { return (int) m_pages.GetCount(); }

    // get the panel which represents the given page
    wxNotebookPage *GetPage(int nPage) { return m_pages[nPage]; }

    // get the currently selected page
    virtual int GetSelection() const = 0;

    // set/get the title of a page
    virtual bool SetPageText(int nPage, const wxString& strText) = 0;
    virtual wxString GetPageText(int nPage) const = 0;

    // image list stuff: each page may have an image associated with it (all
    // images belong to the same image list)
    virtual void SetImageList(wxImageList* imageList);

    // as SetImageList() but we will delete the image list ourselves
    void AssignImageList(wxImageList* imageList);

    // get pointer (may be NULL) to the associated image list
    wxImageList* GetImageList() const { return m_imageList; }

    // sets/returns item's image index in the current image list
    virtual int GetPageImage(int nPage) const = 0;
    virtual bool SetPageImage(int nPage, int nImage) = 0;

    // get the number of rows for a control with wxNB_MULTILINE style (not all
    // versions support it - they will always return 1 then)
    virtual int GetRowCount() const { return 1; }

    // set the size (the same for all pages)
    virtual void SetPageSize(const wxSize& size) = 0;

    // set the padding between tabs (in pixels)
    virtual void SetPadding(const wxSize& padding) = 0;

    // set the size of the tabs for wxNB_FIXEDWIDTH controls
    virtual void SetTabSize(const wxSize& sz) = 0;

    // calculate the size of the notebook from the size of its page
    virtual wxSize CalcSizeFromPage(const wxSize& sizePage);

    // operations
    // ----------

    // remove one page from the notebook and delete it
    virtual bool DeletePage(int nPage);

    // remove one page from the notebook, without deleting it
    virtual bool RemovePage(int nPage) { return DoRemovePage(nPage) != NULL; }

    // remove all pages and delete them
    virtual bool DeleteAllPages() { WX_CLEAR_ARRAY(m_pages); return TRUE; }

    // adds a new page to the notebook (it will be deleted by the notebook,
    // don't delete it yourself) and make it the current one if bSelect
    virtual bool AddPage(wxNotebookPage *pPage,
                         const wxString& strText,
                         bool bSelect = FALSE,
                         int imageId = -1)
    {
        return InsertPage(GetPageCount(), pPage, strText, bSelect, imageId);
    }

    // the same as AddPage(), but adds the page at the specified position
    virtual bool InsertPage(int nPage,
                            wxNotebookPage *pPage,
                            const wxString& strText,
                            bool bSelect = FALSE,
                            int imageId = -1) = 0;

    // set the currently selected page, return the index of the previously
    // selected one (or -1 on error)
    //
    // NB: this function will _not_ generate wxEVT_NOTEBOOK_PAGE_xxx events
    virtual int SetSelection(int nPage) = 0;

    // cycle thru the tabs
    void AdvanceSelection(bool forward = TRUE)
    {
        int nPage = GetNextPage(forward);
        if ( nPage != -1 )
            SetSelection(nPage);
    }

protected:
    // remove the page and return a pointer to it
    virtual wxNotebookPage *DoRemovePage(int page);

    // common part of all ctors
    void Init();

    // get the next page wrapping if we reached the end
    int GetNextPage(bool forward) const;

    wxArrayPages  m_pages;      // array of pages
    wxImageList  *m_imageList;  // we can have an associated image list
    bool m_ownsImageList;       // true if we must delete m_imageList

    DECLARE_NO_COPY_CLASS(wxNotebookBase)
};

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
// event types and macros for them
// ----------------------------------------------------------------------------

#if defined(__BORLANDC__) && defined(__WIN16__)
    // For 16-bit BC++, these 2 would be identical otherwise (truncated)
    #define wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED wxEVT_COMMAND_NB_PAGE_CHANGED
    #define wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING wxEVT_COMMAND_NB_PAGE_CHANGING
#endif

BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EVENT_TYPE(wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED, 802)
    DECLARE_EVENT_TYPE(wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING, 803)
END_DECLARE_EVENT_TYPES()

typedef void (wxEvtHandler::*wxNotebookEventFunction)(wxNotebookEvent&);

// Truncation in 16-bit BC++ means we need to define these differently
#if defined(__BORLANDC__) && defined(__WIN16__)
#define EVT_NOTEBOOK_PAGE_CHANGED(id, fn)                                   \
  DECLARE_EVENT_TABLE_ENTRY(                                                \
    wxEVT_COMMAND_NB_PAGE_CHANGED,                                          \
    id,                                                                     \
    -1,                                                                     \
    (wxObjectEventFunction)(wxEventFunction)(wxNotebookEventFunction) &fn,  \
    NULL                                                                    \
  ),

#define EVT_NOTEBOOK_PAGE_CHANGING(id, fn)                                  \
  DECLARE_EVENT_TABLE_ENTRY(                                                \
    wxEVT_COMMAND_NB_PAGE_CHANGING,                                         \
    id,                                                                     \
    -1,                                                                     \
    (wxObjectEventFunction)(wxEventFunction)(wxNotebookEventFunction) &fn,  \
    NULL                                                                    \
  ),

#else

#define EVT_NOTEBOOK_PAGE_CHANGED(id, fn)                                   \
  DECLARE_EVENT_TABLE_ENTRY(                                                \
    wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED,                                    \
    id,                                                                     \
    -1,                                                                     \
    (wxObjectEventFunction)(wxEventFunction)(wxNotebookEventFunction) &fn,  \
    NULL                                                                    \
  ),

#define EVT_NOTEBOOK_PAGE_CHANGING(id, fn)                                  \
  DECLARE_EVENT_TABLE_ENTRY(                                                \
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

#if defined(__WXUNIVERSAL__)
    #include "wx/univ/notebook.h"
#elif defined(__WXMSW__)
    #ifdef __WIN16__
        #include  "wx/generic/notebook.h"
    #else
        #include  "wx/msw/notebook.h"
    #endif
#elif defined(__WXMOTIF__)
    #include  "wx/generic/notebook.h"
#elif defined(__WXGTK__)
    #include  "wx/gtk/notebook.h"
#elif defined(__WXMAC__)
    #include  "wx/mac/notebook.h"
#elif defined(__WXCOCOA__)
    #include  "wx/generic/notebook.h"
#elif defined(__WXPM__)
    #include  "wx/os2/notebook.h"
#endif

#endif // wxUSE_NOTEBOOK

#endif
    // _WX_NOTEBOOK_H_BASE_
