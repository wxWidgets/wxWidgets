/////////////////////////////////////////////////////////////////////////////
// Name:        msw/notebook.h
// Purpose:     MSW/GTK compatible notebook (a.k.a. property sheet)
// Author:      Robert Roebling
// Modified by: Vadim Zeitlin for Windows version
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef _NOTEBOOK_H
#define _NOTEBOOK_H

#ifdef __GNUG__
  #pragma interface "notebook.h"
#endif

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------
#ifndef   _DYNARRAY_H
  #include <wx/dynarray.h>
#endif  //_DYNARRAY_H

// ----------------------------------------------------------------------------
// types
// ----------------------------------------------------------------------------

// fwd declarations
class wxImageList;
class wxWindow;

// array of notebook pages
typedef wxWindow wxNotebookPage;  // so far, any window can be a page
WX_DEFINE_ARRAY(wxNotebookPage *, wxArrayPages);

// ----------------------------------------------------------------------------
// notebook events
// ----------------------------------------------------------------------------
class WXDLLEXPORT wxNotebookEvent : public wxCommandEvent
{
public:
  wxNotebookEvent(WXTYPE commandType = wxEVT_NULL, int id = 0,
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

// ----------------------------------------------------------------------------
// wxNotebook
// ----------------------------------------------------------------------------

// @@@ this class should really derive from wxTabCtrl, but the interface is not
//     exactly the same, so I can't do it right now and instead we reimplement
//     part of wxTabCtrl here
class wxNotebook : public wxControl
{
public:
  // ctors
  // -----
    // default for dynamic class
  wxNotebook();
    // the same arguments as for wxControl (@@@ any special styles?)
  wxNotebook(wxWindow *parent,
             wxWindowID id, 
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
             long style = 0,
             const wxString& name = "notebook");
    // Create() function
  bool Create(wxWindow *parent,
              wxWindowID id, 
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize,
              long style = 0,
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
  int GetSelection() const { return m_nSelection; }

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
  wxImageList* GetImageList() const { return m_pImageList; }

    // sets/returns item's image index in the current image list
  int  GetPageImage(int nPage) const;
  bool SetPageImage(int nPage, int nImage);

  // each page can have some user-defined data associated with it:
    // set/retrieve this pointer
  bool  SetPageData(int nPage, void* data);
  void* GetPageData(int nPage) const;

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
  bool AddPage(wxNotebookPage *pPage,
               const wxString& strText,
               bool bSelect = FALSE,
               int imageId = -1,
               void* data = NULL);
    // the same as AddPage(), but adds it at the specified position
  bool InsertPage(int nPage,
                  wxNotebookPage *pPage,
                  const wxString& strText,
                  bool bSelect = FALSE,
                  int imageId = -1,
                  void* data = NULL);
    // get the panel which represents the given page
  wxNotebookPage *GetPage(int nPage) { return m_aPages[nPage]; }

  // callbacks
  // ---------
  void OnSize(wxSizeEvent& event);
  void OnSelChange(wxNotebookEvent& event);
  
  // base class virtuals
  // -------------------
  virtual void Command(wxCommandEvent& event);
  virtual bool MSWNotify(WXWPARAM wParam, WXLPARAM lParam);

protected:
  // common part of all ctors
  void Init();

  // helper functions
  void ChangePage(int nOldSel, int nSel); // change pages
  void FitPage(wxNotebookPage *pPage);    // fit a page in the tab control

  wxImageList  *m_pImageList; // we can have an associated image list
  wxArrayPages  m_aPages;     // array of pages

  int m_nSelection;           // the current selection (-1 if none)

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

#endif // _NOTEBOOK_H
