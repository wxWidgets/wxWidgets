///////////////////////////////////////////////////////////////////////////////
// Name:        wx/univ/notebook.h
// Purpose:     universal version of wxNotebook
// Author:      Vadim Zeitlin
// Modified by:
// Created:     01.02.01
// RCS-ID:      $Id$
// Copyright:   (c) 2001 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UNIV_NOTEBOOK_H_
#define _WX_UNIV_NOTEBOOK_H_

#ifdef __GNUG__
    #pragma interface "univnotebook.h"
#endif

class WXDLLEXPORT wxSpinButton;

// ----------------------------------------------------------------------------
// the actions supported by this control
// ----------------------------------------------------------------------------

// change the page: to the next/previous/given one
#define wxACTION_NOTEBOOK_NEXT      _T("nexttab")
#define wxACTION_NOTEBOOK_PREV      _T("prevtab")
#define wxACTION_NOTEBOOK_GOTO      _T("gototab")

// ----------------------------------------------------------------------------
// wxNotebook
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxNotebook : public wxNotebookBase
{
public:
    // ctors and such
    // --------------

    wxNotebook() { Init(); }

    wxNotebook(wxWindow *parent,
               wxWindowID id,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = 0,
               const wxString& name = wxNOTEBOOK_NAME)
    {
        Init();

        (void)Create(parent, id, pos, size, style, name);
    }

    // quasi ctor
    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxNOTEBOOK_NAME);

    // dtor
    virtual ~wxNotebook();

    // implement wxNotebookBase pure virtuals
    // --------------------------------------

    virtual int SetSelection(int nPage);
    virtual int GetSelection() const { return m_sel; }

    virtual bool SetPageText(int nPage, const wxString& strText);
    virtual wxString GetPageText(int nPage) const;

    virtual int GetPageImage(int nPage) const;
    virtual bool SetPageImage(int nPage, int nImage);

    virtual void SetPageSize(const wxSize& size);
    virtual void SetPadding(const wxSize& padding);
    virtual void SetTabSize(const wxSize& sz);

    virtual bool DeleteAllPages();

    virtual bool InsertPage(int nPage,
                            wxNotebookPage *pPage,
                            const wxString& strText,
                            bool bSelect = FALSE,
                            int imageId = -1);

    // style tests
    // -----------

    // return TRUE if all tabs have the same width
    bool FixedSizeTabs() const
        { return GetWindowStyle() & wxNB_FIXEDWIDTH != 0; }

    // return wxTOP/wxBOTTOM/wxRIGHT/wxLEFT
    wxDirection GetTabOrientation() const;

    // return TRUE if the notebook has tabs at the sidesand not at the top (or
    // bottom) as usual
    bool IsVertical() const;

    // hit testing
    // -----------

    // return the tab at this position or -1 if none
    int HitTest(const wxPoint& pt) const;

    // input handling
    // --------------

    virtual bool PerformAction(const wxControlAction& action,
                               long numArg = 0l,
                               const wxString& strArg = wxEmptyString);

protected:
    virtual wxNotebookPage *DoRemovePage(int nPage);

    // drawing
    virtual void DoDraw(wxControlRenderer *renderer);
    void DoDrawTab(wxDC& dc, const wxRect& rect, size_t n);

    // resizing
    virtual wxSize DoGetBestClientSize() const;
    virtual void DoMoveWindow(int x, int y, int width, int height);
    virtual void DoSetSize(int x, int y,
                           int width, int height,
                           int sizeFlags = wxSIZE_AUTO);

    // common part of all ctors
    void Init();

    // resize the tab to fit its title (and icon if any)
    void ResizeTab(int page);

    // recalculate the geometry of the notebook completely
    void Relayout();

    // is the spin button currently shown?
    bool HasSpinBtn() const;

    // calculate last (fully) visible tab
    size_t CalcLastVisibleTab() const;

    // show or hide the spin control for tabs scrolling depending on whether it
    // is needed or not
    void UpdateSpinBtn();

    // position the spin button
    void PositionSpinBtn();

    // refresh the given tab only
    void RefreshTab(int page);

    // refresh all tabs
    void RefreshAllTabs();

    // get the tab rect (inefficient, don't use this in a loop)
    wxRect GetTabRect(int page) const;

    // get the rectangle containing all tabs
    wxRect GetAllTabsRect() const;

    // get the part occupied by the tabs - slightly smaller than
    // GetAllTabsRect() because the tabs may be indented from it
    wxRect GetTabsPart() const;

    // calculate the tab size (without padding)
    wxSize CalcTabSize(int page) const;

    // get the (cached) size of a tab
    void GetTabSize(int page, wxCoord *w, wxCoord *h) const;

    // get the (cached) width of the tab
    wxCoord GetTabWidth(int page) const
        { return FixedSizeTabs() ? m_widthMax : m_widths[page]; }

    // return TRUE if the tab has an associated image
    bool HasImage(int page) const
        { return m_imageList && m_images[page] != -1; }

    // get the part of the notebook reserved for the pages (slightly larger
    // than GetPageRect() as we draw a border and leave marginin between)
    wxRect GetPagePart() const;

    // get the page rect in our client coords
    wxRect GetPageRect() const;

    // get our client size from the page size
    wxSize GetSizeForPage(const wxSize& size) const;

    // change thep age and send events about it (can be vetoed by user code)
    void ChangePage(int nPage);

    // scroll the tabs so that the first page shown becomes the given one
    void ScrollTo(int page);

    // scroll the tabs so that the first page shown becomes the given one
    void ScrollLastTo(int page);

    // the pages titles
    wxArrayString m_titles;

    // the current selection
    size_t m_sel;

    // the spin button to change the pages
    wxSpinButton *m_spinbtn;

    // the offset of the first page shown (may be changed with m_spinbtn)
    wxCoord m_offset;

    // the first and last currently visible tabs
    size_t m_firstVisible,
           m_lastVisible;

    // the height of tabs in a normal notebook or the width of tabs in a
    // notebook with tabs on a side
    wxCoord m_heightTab;

    // the biggest height (or width) of a notebook tab (used only if
    // FixedSizeTabs()) or -1 if not calculated yet
    wxCoord m_widthMax;

    // the cached widths (or heights) of tabs
    wxArrayInt m_widths;

    // the icon indices
    wxArrayInt m_images;

    // the accel indexes for labels
    wxArrayInt m_accels;

    // the padding
    wxSize m_sizePad;

    DECLARE_DYNAMIC_CLASS(wxNotebook)
};

// ----------------------------------------------------------------------------
// wxStdNotebookInputHandler: translates SPACE and ENTER keys and the left mouse
// click into button press/release actions
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxStdNotebookInputHandler : public wxStdInputHandler
{
public:
    wxStdNotebookInputHandler(wxInputHandler *inphand);

    virtual bool HandleKey(wxControl *control,
                           const wxKeyEvent& event,
                           bool pressed);
    virtual bool HandleMouse(wxControl *control,
                             const wxMouseEvent& event);
    virtual bool HandleMouseMove(wxControl *control, const wxMouseEvent& event);
    virtual bool HandleFocus(wxControl *control, const wxFocusEvent& event);
    virtual bool HandleActivation(wxControl *control, bool activated);
};

#endif // _WX_UNIV_NOTEBOOK_H_

