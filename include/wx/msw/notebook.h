/////////////////////////////////////////////////////////////////////////////
// Name:        msw/notebook.h
// Purpose:     MSW/GTK compatible notebook (a.k.a. property sheet)
// Author:      Robert Roebling
// Modified by: Vadim Zeitlin for Windows version
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
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

// ----------------------------------------------------------------------------
// wxNotebook
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxNotebook : public wxNotebookBase
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
             const wxString& name = wxNOTEBOOK_NAME);
    // Create() function
  bool Create(wxWindow *parent,
              wxWindowID id,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize,
              long style = 0,
              const wxString& name = wxNOTEBOOK_NAME);

  // accessors
  // ---------
    // get number of pages in the dialog
  int GetPageCount() const;

    // set the currently selected page, return the index of the previously
    // selected one (or -1 on error)
    // NB: this function will _not_ generate wxEVT_NOTEBOOK_PAGE_xxx events
  int SetSelection(int nPage);
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
    // remove all pages
  bool DeleteAllPages();
    // inserts a new page to the notebook (it will be deleted ny the notebook,
    // don't delete it yourself). If bSelect, this page becomes active.
  bool InsertPage(int nPage,
                  wxNotebookPage *pPage,
                  const wxString& strText,
                  bool bSelect = FALSE,
                  int imageId = -1);

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
  virtual bool MSWOnScroll(int orientation, WXWORD nSBCode,
                           WXWORD pos, WXHWND control);

#if wxUSE_CONSTRAINTS
  virtual void SetConstraintSizes(bool recurse = TRUE);
  virtual bool DoPhase(int nPhase);
#endif // wxUSE_CONSTRAINTS

protected:
  // common part of all ctors
  void Init();

  // translate wxWin styles to the Windows ones
  virtual WXDWORD MSWGetStyle(long flags, WXDWORD *exstyle = NULL) const;

  // remove one page from the notebook, without deleting
  virtual wxNotebookPage *DoRemovePage(int nPage);

  // the current selection (-1 if none)
  int m_nSelection;

  DECLARE_DYNAMIC_CLASS(wxNotebook)
  DECLARE_EVENT_TABLE()
};

#endif // wxUSE_NOTEBOOK

#endif // _NOTEBOOK_H
