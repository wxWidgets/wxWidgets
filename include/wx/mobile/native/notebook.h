/////////////////////////////////////////////////////////////////////////////
// Name:        wx/mobile/native/notebook.h
// Purpose:     wxMobile/wxiOS (GSoC 2011)
// Author:      Julian Smart, Linas Valiukas
// Modified by:
// Created:     2011-06-14
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Linas Valiukas
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MOBILE_NATIVE_NOTEBOOK_H_
#define _WX_MOBILE_NATIVE_NOTEBOOK_H_

#include "wx/bookctrl.h"
#include "wx/notebook.h"
#include "wx/mobile/native/tabctrl.h"

// Not currently needed
#define wxBK_TEXT   0x00010000
#define wxBK_BITMAP 0x00020000

/**
 @class wxMoNotebook
 
 This is an implementation of the standard wxNotebook class.
 Tabs will show text and/or an image in each tab, according
 to the presence or absence of the text or image. Presently
 you do not need to specify a style to determine text or image
 display.
 
 @category{wxMobile}
 */

//class WXDLLEXPORT wxMoNotebook: public wxBookCtrlBase
class WXDLLEXPORT wxMoNotebook: public wxNotebook
{
public:
    /// Default constructor.
    wxMoNotebook();
    
    /// Constructor.
    wxMoNotebook(wxWindow *parent,
                 wxWindowID id,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = 0,
                 const wxString& name = wxNotebookNameStr);
    
    /// Creation function.
    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxNotebookNameStr);
    
    virtual ~wxMoNotebook();
    
#if 0
    
    /// Gets the number of pages in the notebook.
    virtual size_t GetPageCount() const;
    
    /// Sets the currently selected page, returning the index of the previously
    /// selected one (or -1 on error)
    /// NB: this function will not generate wxEVT_NOTEBOOK_PAGE_xxx events.
    int SetSelection(size_t page);
    
    /// Gets the currently selected page
    int GetSelection() const;
    
    /// Changes the selection without sending events.
    int ChangeSelection(size_t page);
    
    /// Sets the title of a page
    bool SetPageText(size_t page, const wxString& strText);
    
    /// Gets the title of a page
    wxString GetPageText(size_t page) const;
    
    /// Sets the image list.
    void SetImageList(wxImageList* imageList);
    
    /// Sets the item's image index in the current image list.
    bool SetPageImage(size_t page, int nImage);
    
    /// Gets the item's image index in the current image list.
    int GetPageImage(size_t page) const;
    
    /// Returns 1 because multi-row tab controls are not supported.
    int GetRowCount() const;
    
    /// Sets the padding between tabs (in pixels). Not implemented
    void SetPadding(const wxSize& padding);
    
    /// Removes all pages.
    bool DeleteAllPages();
    
    /// Inserts a new page to the notebook (it will be deleted by the notebook,
    /// don't delete it yourself). If bSelect is true, this page becomes active.
    bool InsertPage(size_t page,
                    wxNotebookPage *pPage,
                    const wxString& strText,
                    bool bSelect = false,
                    int imageId = -1);
    
    /// Hit test. Not implemented.
    virtual int HitTest(const wxPoint& pt, long *flags = NULL) const;
    
    /// Calculates the size of the notebook from the size of its page.
    virtual wxSize CalcSizeFromPage(const wxSize& sizePage) const;
    
    void SetTabSize(const wxSize& WXUNUSED(sz)) {}
#endif  // 0
    
    /// Sets a text badge for the given item
    bool SetBadge(int item, const wxString& badge);
    
    /// Gets the text badge for the given item
    wxString GetBadge(int item) const;
    
    // Ignore attempts to set font
    bool SetFont( const wxFont &font ) { }
    
protected:
    // common part of all ctors
    void Init();
    
    // remove one page from the notebook, without deleting
    //virtual wxNotebookPage *DoRemovePage(size_t page);
    
    // get the size which the choice control should have
    //virtual wxSize GetControllerSize() const;
    
    //wxBookCtrlBaseEvent* CreatePageChangingEvent() const;
    //void MakeChangedEvent(wxBookCtrlBaseEvent &event);
    
    //void UpdateSelectedPage(size_t newsel);
    
    void OnEraseBackground(wxEraseEvent& event);
    //void OnTabChanging(wxTabEvent& event);
    //void OnTabChanged(wxTabEvent& event);
    
    // the current selection (-1 if none)
    int               m_selection;
    
    wxMoTabCtrl*      m_tabCtrl;
    
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxMoNotebook)
    DECLARE_EVENT_TABLE()
};

#endif // _WX_MOBILE_NATIVE_NOTEBOOK_H_
