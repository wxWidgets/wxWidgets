/////////////////////////////////////////////////////////////////////////////
// Name:        tabctrl.h
// Purpose:     wxTabCtrl class
// Author:      Robert Roebling
// Modified by:
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __TABCTRLH__
#define __TABCTRLH__

#ifdef __GNUG__
#pragma interface "notebook.h"
#endif

#include "wx/defs.h"
#include "wx/object.h"
#include "wx/string.h"
#include "wx/control.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxImageList;
class wxNotebook;
class wxNotebookPage;

// ----------------------------------------------------------------------------
// notebook events
// ----------------------------------------------------------------------------
class wxNotebookEvent : public wxCommandEvent
{
public:
  wxNotebookEvent(wxEventType commandType = wxEVT_NULL, int id = 0,
                  int nSel = -1, int nOldSel = -1)
    : wxCommandEvent(commandType, id) { m_nSel = nSel; m_nOldSel = nOldSel; }

  // accessors
  int GetSelection() const { return m_nSel; }
  int GetOldSelection() const { return m_nOldSel; }

private:
  int m_nSel,     // currently selected page
      m_nOldSel;  // previously selected page

  DECLARE_DYNAMIC_CLASS(wxNotebookEvent)
};

//-----------------------------------------------------------------------------
// wxNotebook
//-----------------------------------------------------------------------------

class wxNotebook : public wxControl
{
public:
  // ctors
  // -----
    // default for dynamic class
  wxNotebook();
    // the same arguments as for wxControl (@@@ any special styles?)
  wxNotebook(wxWindow *parent,
             const wxWindowID id,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
             const long style = 0,
             const wxString& name = "notebook");
    // Create() function
  bool Create(wxWindow *parent,
              const wxWindowID id,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize,
              const long style = 0,
              const wxString& name = "notebook");
    // dtor
  ~wxNotebook();

  // accessors
  // ---------
    // get number of pages in the dialog
  int GetPageCount() const;

    // set the currently selected page, return the index of the previously
    // selected one (or -1 on error)
    // NB: this function will _not_ generate wxEVT_NOTEBOOK_PAGE_xxx events
  int SetSelection(int nPage);
    // cycle thru the tabs
  void AdvanceSelection(bool bForward = TRUE);
    // get the currently selected page
  int GetSelection() const;

    // set/get the title of a page
  bool SetPageText(int nPage, const wxString& strText);
  wxString GetPageText(int nPage) const;

  // image list stuff: each page may have an image associated with it. All
  // the images belong to an image list, so you have to
  // 1) create an image list
  // 2) associate it with the notebook
  // 3) set for each page it's image
    // associate image list with a control
  void SetImageList(wxImageList* imageList);
    // get pointer (may be NULL) to the associated image list
  wxImageList* GetImageList() const { return m_imageList; }

    // sets/returns item's image index in the current image list
  int  GetPageImage(int nPage) const;
  bool SetPageImage(int nPage, int nImage);

    // currently it's always 1 because wxGTK doesn't support multi-row
    // tab controls
  int GetRowCount() const;

  // control the appearance of the notebook pages
    // set the size (the same for all pages)
  void SetPageSize(const wxSize& size);
    // set the padding between tabs (in pixels)
  void SetPadding(const wxSize& padding);

  // operations
  // ----------
    // remove one page from the notebook
  bool DeletePage(int nPage);
    // remove all pages
  bool DeleteAllPages();
    // adds a new page to the notebook (it will be deleted ny the notebook,
    // don't delete it yourself). If bSelect, this page becomes active.
  bool AddPage(wxWindow *pPage,
               const wxString& strText,
               bool bSelect = FALSE,
               int imageId = -1);
    // @@@@ VZ: I don't know how to implement InsertPage()

    // get the panel which represents the given page
  wxWindow *GetPage(int nPage) const;

  // base class virtuals
  virtual void AddChild(wxWindow *child);

protected:
  // wxWin callbacks
  void OnSize(wxSizeEvent& event);

private:
  // common part of all ctors
  void Init();

  // helper function
  wxNotebookPage* GetNotebookPage(int page) const;

  wxImageList*    m_imageList;
  wxList          m_pages;
  uint            m_idHandler; // the change page handler id

  DECLARE_DYNAMIC_CLASS(wxNotebook)
  DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// event macros
// ----------------------------------------------------------------------------
typedef void (wxEvtHandler::*wxNotebookEventFunction)(wxNotebookEvent&);

#define EVT_NOTEBOOK_PAGE_CHANGED(id, fn)                                   \
  {                                                                         \
    wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED,                                    \
    id,                                                                     \
    -1,                                                                     \
    (wxObjectEventFunction)(wxEventFunction)(wxNotebookEventFunction) &fn,  \
    NULL                                                                    \
  },

#define EVT_NOTEBOOK_PAGE_CHANGING(id, fn)                                  \
  {                                                                         \
    wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING, \                                 \
    id,                                                                     \
    -1,                                                                     \
    (wxObjectEventFunction)(wxEventFunction)(wxNotebookEventFunction) &fn,  \
    NULL                                                                    \
  },

#endif
    // __TABCTRLH__
