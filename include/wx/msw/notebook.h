/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/notebook.h
// Purpose:     MSW/GTK compatible notebook (a.k.a. property sheet)
// Author:      Robert Roebling
// Modified by: Vadim Zeitlin for Windows version
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _NOTEBOOK_H
#define _NOTEBOOK_H

#if wxUSE_NOTEBOOK

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/control.h"

// ----------------------------------------------------------------------------
// wxNotebook
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxNotebook : public wxNotebookBase
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
             const wxString& name = wxASCII_STR(wxNotebookNameStr));
    // Create() function
  bool Create(wxWindow *parent,
              wxWindowID id,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize,
              long style = 0,
              const wxString& name = wxASCII_STR(wxNotebookNameStr));
  virtual ~wxNotebook();

  // accessors
  // ---------
    // get number of pages in the dialog
  virtual size_t GetPageCount() const override;

    // set the currently selected page, return the index of the previously
    // selected one (or wxNOT_FOUND on error)
    // NB: this function will _not_ generate wxEVT_NOTEBOOK_PAGE_xxx events
  int SetSelection(size_t nPage) override;

    // changes selected page without sending events
  int ChangeSelection(size_t nPage) override;

    // set/get the title of a page
  bool SetPageText(size_t nPage, const wxString& strText) override;
  wxString GetPageText(size_t nPage) const override;

    // sets/returns item's image index in the current image list
  int  GetPageImage(size_t nPage) const override;
  bool SetPageImage(size_t nPage, int nImage) override;

    // currently it's always 1 because wxGTK doesn't support multi-row
    // tab controls
  int GetRowCount() const override;

  // control the appearance of the notebook pages
    // set the size (the same for all pages)
  void SetPageSize(const wxSize& size) override;
    // set the padding between tabs (in pixels)
  void SetPadding(const wxSize& padding) override;

  // operations
  // ----------
    // remove all pages
  bool DeleteAllPages() override;

    // inserts a new page to the notebook (it will be deleted ny the notebook,
    // don't delete it yourself). If bSelect, this page becomes active.
  bool InsertPage(size_t nPage,
                  wxNotebookPage *pPage,
                  const wxString& strText,
                  bool bSelect = false,
                  int imageId = NO_IMAGE) override;

    // Windows-only at present. Also, you must use the wxNB_FIXEDWIDTH
    // style.
  void SetTabSize(const wxSize& sz) override;

  // Return the position and size of the tab for the given page
  wxRect GetTabRect(size_t page) const override;

    // hit test
  virtual int HitTest(const wxPoint& pt, long *flags = nullptr) const override;

    // calculate the size of the notebook from the size of its page
  virtual wxSize CalcSizeFromPage(const wxSize& sizePage) const override;

  // callbacks
  // ---------
  void OnSize(wxSizeEvent& event);
  void OnNavigationKey(wxNavigationKeyEvent& event);

  // base class virtuals
  // -------------------

  virtual bool MSWOnNotify(int idCtrl, WXLPARAM lParam, WXLPARAM *result) override;
  virtual bool MSWOnScroll(int orientation, WXWORD nSBCode,
                           WXWORD pos, WXHWND control) override;

#if wxUSE_CONSTRAINTS
  virtual void SetConstraintSizes(bool recurse = true) override;
  virtual bool DoPhase(int nPhase) override;
#endif // wxUSE_CONSTRAINTS

  // Attempts to get colour for UX theme page background
  wxColour GetThemeBackgroundColour() const override;

  // implementation only
  // -------------------

  virtual bool SetBackgroundColour(const wxColour& colour) override;

#if wxUSE_UXTHEME
  // draw child background
  virtual bool MSWPrintChild(WXHDC hDC, wxWindow *win) override;

  virtual bool MSWHasInheritableBackground() const override { return true; }
#endif // wxUSE_UXTHEME

  // translate wxWin styles to the Windows ones
  virtual WXDWORD MSWGetStyle(long flags, WXDWORD *exstyle = nullptr) const override;

protected:
  // common part of all ctors
  void Init();

  // hides the currently shown page and shows the given one (if not -1) and
  // updates m_selection accordingly
  void UpdateSelection(int selNew);

  // remove one page from the notebook, without deleting
  virtual wxNotebookPage *DoRemovePage(size_t nPage) override;

  // update the image list used by the native control
  virtual void OnImagesChanged() override;

  // get the page rectangle for the current notebook size
  //
  // returns empty rectangle if an error occurs, do test for it
  wxRect GetPageSize() const;

  // set the size of the given page to fit in the notebook
  void AdjustPageSize(wxNotebookPage *page);

#if wxUSE_UXTHEME
  virtual void MSWAdjustBrushOrg(int *xOrg, int* yOrg) const override
  {
      *xOrg -= m_bgBrushAdj.x;
      *yOrg -= m_bgBrushAdj.y;
  }

  // return the themed brush for painting our children
  virtual WXHBRUSH MSWGetCustomBgBrush() override { return m_hbrBackground; }

  // gets the bitmap of notebook background and returns a brush from it and
  // sets m_bgBrushAdj
  WXHBRUSH QueryBgBitmap();

  // creates the brush to be used for drawing the tab control background
  void UpdateBgBrush();
#endif // wxUSE_UXTHEME

  // these function are used for reducing flicker on notebook resize
  void OnEraseBackground(wxEraseEvent& event);
  void OnPaint(wxPaintEvent& event);

  // true if we have already subclassed our updown control
  bool m_hasSubclassedUpdown;

#if wxUSE_UXTHEME
  // background brush used to paint the tab control
  WXHBRUSH m_hbrBackground;

  // offset for MSWAdjustBrushOrg()
  wxPoint m_bgBrushAdj;
#endif // wxUSE_UXTHEME


  wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxNotebook);
  wxDECLARE_EVENT_TABLE();
};

#endif // wxUSE_NOTEBOOK

#endif // _NOTEBOOK_H
