/////////////////////////////////////////////////////////////////////////////
// Name:        notebook.h
// Purpose:     wxNotebook class (a.k.a. property sheet, tabbed dialog)
// Author:      Julian Smart
// Modified by:
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_NOTEBOOK_H_
#define _WX_NOTEBOOK_H_

#ifdef __GNUG__
#pragma interface "notebook.h"
#endif

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------
#include "wx/dynarray.h"
#include "wx/event.h"
#include "wx/control.h"
#include "wx/generic/tabg.h"

// ----------------------------------------------------------------------------
// types
// ----------------------------------------------------------------------------

// fwd declarations
class WXDLLEXPORT wxImageList;
class WXDLLEXPORT wxWindow;

// array of notebook pages
typedef wxWindow wxNotebookPage;  // so far, any window can be a page
WX_DEFINE_ARRAY(wxNotebookPage *, wxArrayPages);

// ----------------------------------------------------------------------------
// notebook events
// ----------------------------------------------------------------------------
class WXDLLEXPORT wxNotebookEvent : public wxCommandEvent
{
public:
  wxNotebookEvent(wxEventType commandType = wxEVT_NULL, int id = 0, 
                  int nSel = -1, int nOldSel = -1)
    : wxCommandEvent(commandType, id) { m_nSel = nSel; m_nOldSel = nOldSel; }

  // accessors
  int GetSelection() const { return m_nSel; }
  int GetOldSelection() const { return m_nOldSel; }

  void SetSelection(int sel) { m_nSel = sel; }
  void SetOldSelection(int oldSel) { m_nOldSel = oldSel; }

private:
  int m_nSel,     // currently selected page
      m_nOldSel;  // previously selected page

  DECLARE_DYNAMIC_CLASS(wxNotebookEvent)
};

// ----------------------------------------------------------------------------
// wxNotebook
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxNotebook;

// This reuses wxTabView to draw the tabs.
class WXDLLEXPORT wxNotebookTabView: public wxTabView
{
DECLARE_DYNAMIC_CLASS(wxNotebookTabView)
public:
  wxNotebookTabView(wxNotebook* notebook, long style = wxTAB_STYLE_DRAW_BOX | wxTAB_STYLE_COLOUR_INTERIOR);
  ~wxNotebookTabView(void);

  // Called when a tab is activated
  virtual void OnTabActivate(int activateId, int deactivateId);

protected:
   wxNotebook*      m_notebook;
};

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

  // Find the position of the wxNotebookPage, -1 if not found.
  int FindPagePosition(wxNotebookPage* page) const;

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
    // remove one page from the notebook, and delete the page.
  bool DeletePage(int nPage);
  bool DeletePage(wxNotebookPage* page);
    // remove one page from the notebook, without deleting the page.
  bool RemovePage(int nPage);
  bool RemovePage(wxNotebookPage* page);
    // remove all pages
  bool DeleteAllPages();
    // adds a new page to the notebook (it will be deleted ny the notebook,
    // don't delete it yourself). If bSelect, this page becomes active.
  bool AddPage(wxNotebookPage *pPage,
               const wxString& strText,
               bool bSelect = FALSE,
               int imageId = -1);
    // the same as AddPage(), but adds it at the specified position
  bool InsertPage(int nPage,
                  wxNotebookPage *pPage,
                  const wxString& strText,
                  bool bSelect = FALSE,
                  int imageId = -1);
    // get the panel which represents the given page
  wxNotebookPage *GetPage(int nPage) { return m_aPages[nPage]; }

  // callbacks
  // ---------
  void OnSize(wxSizeEvent& event);
  void OnIdle(wxIdleEvent& event);
  void OnSelChange(wxNotebookEvent& event);
  void OnSetFocus(wxFocusEvent& event);
  void OnNavigationKey(wxNavigationKeyEvent& event);
  
  // base class virtuals
  // -------------------
  virtual void Command(wxCommandEvent& event);
  virtual void SetConstraintSizes(bool recurse = TRUE);
  virtual bool DoPhase(int nPhase);

// Implementation

  // wxNotebook on Motif uses a generic wxTabView to implement itself.
  inline wxTabView *GetTabView() const { return m_tabView; }
  inline void SetTabView(wxTabView *v) { m_tabView = v; }
  
  void OnMouseEvent(wxMouseEvent& event);
  void OnPaint(wxPaintEvent& event);

  virtual wxRect GetAvailableClientSize();

  // Implementation: calculate the layout of the view rect
  // and resize the children if required
  bool RefreshLayout(bool force = TRUE);

protected:
  // common part of all ctors
  void Init();

  // helper functions
  void ChangePage(int nOldSel, int nSel); // change pages

  wxImageList  *m_pImageList; // we can have an associated image list
  wxArrayPages  m_aPages;     // array of pages

  int m_nSelection;           // the current selection (-1 if none)

  wxTabView*   m_tabView;

  DECLARE_DYNAMIC_CLASS(wxNotebook)
  DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// event macros
// ----------------------------------------------------------------------------
typedef void (wxEvtHandler::*wxNotebookEventFunction)(wxNotebookEvent&);

// Because of name truncation!
#if defined(__BORLANDC__) && defined(__WIN16__)

#define EVT_NOTEBOOK_PAGE_CHANGED(id, fn)                                   \
  {                                                                         \
    wxEVT_COMMAND_NB_PAGE_CHANGED,                                    \
    id,                                                                     \
    -1,                                                                     \
    (wxObjectEventFunction)(wxEventFunction)(wxNotebookEventFunction) &fn,  \
    NULL                                                                    \
  },

#define EVT_NOTEBOOK_PAGE_CHANGING(id, fn)                                  \
  {                                                                         \
    wxEVT_COMMAND_NB_PAGE_CHANGING, \                                 \
    id,                                                                     \
    -1,                                                                     \
    (wxObjectEventFunction)(wxEventFunction)(wxNotebookEventFunction) &fn,  \
    NULL                                                                    \
  },

#else

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

#endif // _WX_NOTEBOOK_H_
