/////////////////////////////////////////////////////////////////////////////
// Name:        msw/notebook.h
// Purpose:     MSW/GTK compatible notebook (a.k.a. property sheet)
// Author:      Robert Roebling
// Modified by: Vadim Zeitlin for Windows version
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef _NOTEBOOK_H
#define _NOTEBOOK_H

#ifdef __GNUG__
  #pragma interface "notebook.h"
#endif

#if wxUSE_NOTEBOOK

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/control.h"
#include "wx/dynarray.h"

// ----------------------------------------------------------------------------
// types
// ----------------------------------------------------------------------------

// fwd declarations
class WXDLLEXPORT wxImageList;
class WXDLLEXPORT wxWindow;

// array of notebook pages
//typedef wxWindow WXDLLEXPORT wxNotebookPage;  // so far, any window can be a page
typedef wxWindow wxNotebookPage;  // so far, any window can be a page

WX_DEFINE_EXPORTED_ARRAY(wxNotebookPage *, wxArrayPages);

// ----------------------------------------------------------------------------
// wxNotebook
// ----------------------------------------------------------------------------

// FIXME this class should really derive from wxTabCtrl, but the interface is not
//       exactly the same, so I can't do it right now and instead we reimplement
//       part of wxTabCtrl here
class WXDLLEXPORT wxNotebook : public wxControl
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
    // remove one page from the notebook, without deleting
  bool RemovePage(int nPage);
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

    // Windows-only at present. Also, you must use the wxNB_FIXEDWIDTH
    // style.
  void SetTabSize(const wxSize& sz);

  // callbacks
  // ---------
  void OnSize(wxSizeEvent& event);
  void OnSelChange(wxNotebookEvent& event);
  void OnSetFocus(wxFocusEvent& event);
  void OnNavigationKey(wxNavigationKeyEvent& event);

  // base class virtuals
  // -------------------
  virtual bool MSWOnNotify(int idCtrl, WXLPARAM lParam, WXLPARAM *result);
  virtual void SetConstraintSizes(bool recurse = TRUE);
  virtual bool DoPhase(int nPhase);

protected:
  // common part of all ctors
  void Init();

  // helper functions
  void ChangePage(int nOldSel, int nSel); // change pages

  wxImageList  *m_pImageList; // we can have an associated image list
  wxArrayPages  m_aPages;     // array of pages

  int m_nSelection;           // the current selection (-1 if none)

  DECLARE_DYNAMIC_CLASS(wxNotebook)
  DECLARE_EVENT_TABLE()
};

#endif // wxUSE_NOTEBOOK

#endif // _NOTEBOOK_H
