///////////////////////////////////////////////////////////////////////////////
// Name:        wx/listbook.h
// Purpose:     wxListbook: wxListCtrl and wxNotebook combination
// Author:      Vadim Zeitlin
// Modified by:
// Created:     19.08.03
// RCS-ID:      $Id$
// Copyright:   (c) 2003 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_LISTBOOK_H_
#define _WX_LISTBOOK_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "listbook.h"
#endif

#include "wx/defs.h"

#if wxUSE_LISTBOOK

// this can be defined to put a static line as separator between the list
// control and the page area; but I think it finally looks better without it so
// it is not enabled by default
//#define wxUSE_LINE_IN_LISTBOOK 1

#if !wxUSE_STATLINE
    #undef wxUSE_LINE_IN_LISTBOOK
    #define wxUSE_LINE_IN_LISTBOOK 0
#endif

#include "wx/bookctrl.h"

class WXDLLEXPORT wxListView;
class WXDLLEXPORT wxListEvent;

#if wxUSE_LINE_IN_LISTBOOK
class WXDLLEXPORT wxStaticLine;
#endif // wxUSE_LINE_IN_LISTBOOK

// ----------------------------------------------------------------------------
// wxListbook
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxListbook : public wxBookCtrlBase
{
public:
    wxListbook()
    {
        Init();
    }

    wxListbook(wxWindow *parent,
               wxWindowID id,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = 0,
               const wxString& name = wxEmptyString)
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
                const wxString& name = wxEmptyString);


    virtual int GetSelection() const;
    virtual bool SetPageText(size_t n, const wxString& strText);
    virtual wxString GetPageText(size_t n) const;
    virtual int GetPageImage(size_t n) const;
    virtual bool SetPageImage(size_t n, int imageId);
    virtual wxSize CalcSizeFromPage(const wxSize& sizePage) const;
    virtual bool InsertPage(size_t n,
                            wxWindow *page,
                            const wxString& text,
                            bool bSelect = false,
                            int imageId = -1);
    virtual int SetSelection(size_t n);
    virtual void SetImageList(wxImageList *imageList);

    // returns true if we have wxLB_TOP or wxLB_BOTTOM style
    bool IsVertical() const { return HasFlag(wxLB_BOTTOM | wxLB_TOP); }

    virtual bool DeleteAllPages();

    wxListView* GetListView() { return m_list; }

protected:
    virtual wxWindow *DoRemovePage(size_t page);

    // get the size which the list control should have
    wxSize GetListSize() const;

    // get the page area
    wxRect GetPageRect() const;

    // event handlers
    void OnSize(wxSizeEvent& event);
    void OnListSelected(wxListEvent& event);

    // the list control we use for showing the pages index
    wxListView *m_list;

#if wxUSE_LINE_IN_LISTBOOK
    // the line separating it from the page area
    wxStaticLine *m_line;
#endif // wxUSE_LINE_IN_LISTBOOK

    // the currently selected page or wxNOT_FOUND if none
    int m_selection;

private:
    // common part of all constructors
    void Init();

    DECLARE_EVENT_TABLE()
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxListbook)
};

// ----------------------------------------------------------------------------
// listbook event class and related stuff
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxListbookEvent : public wxBookCtrlBaseEvent
{
public:
    wxListbookEvent(wxEventType commandType = wxEVT_NULL, int id = 0,
                    int nSel = wxNOT_FOUND, int nOldSel = wxNOT_FOUND)
        : wxBookCtrlBaseEvent(commandType, id, nSel, nOldSel)
    {
    }

private:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxListbookEvent)
};

extern WXDLLIMPEXP_CORE const wxEventType wxEVT_COMMAND_LISTBOOK_PAGE_CHANGED;
extern WXDLLIMPEXP_CORE const wxEventType wxEVT_COMMAND_LISTBOOK_PAGE_CHANGING;

typedef void (wxEvtHandler::*wxListbookEventFunction)(wxListbookEvent&);

#define wxListbookEventHandler(func) \
    (wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(wxListbookEventFunction, &func)

#define EVT_LISTBOOK_PAGE_CHANGED(winid, fn) \
    wx__DECLARE_EVT1(wxEVT_COMMAND_LISTBOOK_PAGE_CHANGED, winid, wxListbookEventHandler(fn))

#define EVT_LISTBOOK_PAGE_CHANGING(winid, fn) \
    wx__DECLARE_EVT1(wxEVT_COMMAND_LISTBOOK_PAGE_CHANGING, winid, wxListbookEventHandler(fn))

#endif // wxUSE_LISTBOOK

#endif // _WX_LISTBOOK_H_
