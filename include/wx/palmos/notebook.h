/////////////////////////////////////////////////////////////////////////////
// Name:        palmos/notebook.h
// Purpose:     MSW/GTK compatible notebook (a.k.a. property sheet)
// Author:      William Osborne
// Modified by:
// Created:     10/13/04
// RCS-ID:      $Id: 
// Copyright:   (c) William Osborne
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _NOTEBOOK_H
#define _NOTEBOOK_H

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
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

class WXDLLEXPORT wxNotebookPageInfo : public wxObject
{
public :
    wxNotebookPageInfo() { m_page = NULL ; m_imageId = -1 ; m_selected = false ; }
    virtual ~wxNotebookPageInfo() { }

    void Create( wxNotebookPage *page , const wxString &text , bool selected , int imageId ) 
    { m_page = page ; m_text = text ; m_selected = selected ; m_imageId = imageId ; }
    wxNotebookPage* GetPage() const { return m_page ; }
    wxString GetText() const { return m_text ; }
    bool GetSelected() const { return m_selected ; }
    int GetImageId() const { return m_imageId; }
private :
    wxNotebookPage *m_page ;
    wxString m_text ;
    bool m_selected ;
    int m_imageId ;

    DECLARE_DYNAMIC_CLASS(wxNotebookPageInfo) ;
} ;


WX_DECLARE_EXPORTED_LIST(wxNotebookPageInfo, wxNotebookPageInfoList );

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
  virtual size_t GetPageCount() const;

    // set the currently selected page, return the index of the previously
    // selected one (or -1 on error)
    // NB: this function will _not_ generate wxEVT_NOTEBOOK_PAGE_xxx events
  int SetSelection(size_t nPage);
    // get the currently selected page
  int GetSelection() const { return m_nSelection; }

    // set/get the title of a page
  bool SetPageText(size_t nPage, const wxString& strText);
  wxString GetPageText(size_t nPage) const;

  // image list stuff: each page may have an image associated with it. All
  // the images belong to an image list, so you have to
  // 1) create an image list
  // 2) associate it with the notebook
  // 3) set for each page it's image
    // associate image list with a control
  void SetImageList(wxImageList* imageList);

    // sets/returns item's image index in the current image list
  int  GetPageImage(size_t nPage) const;
  bool SetPageImage(size_t nPage, int nImage);

    // currently it's always 1 because wxGTK doesn't support multi-row
    // tab controls
  int GetRowCount() const;

  // control the appearance of the notebook pages
    // set the size (the same for all pages)
  void SetPageSize(const wxSize& size);
    // set the padding between tabs (in pixels)
  void SetPadding(const wxSize& padding);

    // Windows only: attempts to get colour for UX theme page background
  wxColour GetThemeBackgroundColour();

  // operations
  // ----------
    // remove all pages
  bool DeleteAllPages();

    // inserts a new page to the notebook (it will be deleted ny the notebook,
    // don't delete it yourself). If bSelect, this page becomes active.
  bool InsertPage(size_t nPage,
                  wxNotebookPage *pPage,
                  const wxString& strText,
                  bool bSelect = FALSE,
                  int imageId = -1);

  void AddPageInfo( wxNotebookPageInfo* info ) { AddPage( info->GetPage() , info->GetText() , info->GetSelected() , info->GetImageId() ) ; }
  const wxNotebookPageInfoList& GetPageInfos() const ;

    // Windows-only at present. Also, you must use the wxNB_FIXEDWIDTH
    // style.
  void SetTabSize(const wxSize& sz);

    // Windows only: attempts to apply the UX theme page background to this page
  void ApplyThemeBackground(wxWindow* window, const wxColour& colour);

    // hit test
  virtual int HitTest(const wxPoint& pt, long *flags = NULL) const;

    // calculate the size of the notebook from the size of its page
  virtual wxSize CalcSizeFromPage(const wxSize& sizePage) const;

  // callbacks
  // ---------
  void OnSize(wxSizeEvent& event);
  void OnSelChange(wxNotebookEvent& event);
  void OnNavigationKey(wxNavigationKeyEvent& event);

  // base class virtuals
  // -------------------

  virtual bool MSWOnNotify(int idCtrl, WXLPARAM lParam, WXLPARAM *result);
  virtual bool MSWOnScroll(int orientation, WXWORD nSBCode,
                           WXWORD pos, WXHWND control);
  virtual bool MSWTranslateMessage(WXMSG *wxmsg);

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
  virtual wxNotebookPage *DoRemovePage(size_t nPage);

  // set the size of the given page to fit in the notebook
  void AdjustPageSize(wxNotebookPage *page);

    // override WndProc.
#if wxUSE_UXTHEME
    virtual WXLRESULT MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam);
#endif

  // the current selection (-1 if none)
  int m_nSelection;

  wxNotebookPageInfoList m_pageInfos ;


  DECLARE_DYNAMIC_CLASS_NO_COPY(wxNotebook)
  DECLARE_EVENT_TABLE()
};

#endif // wxUSE_NOTEBOOK

#endif // _NOTEBOOK_H
