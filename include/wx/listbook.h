///////////////////////////////////////////////////////////////////////////////
// Name:        wx/listbook.h
// Purpose:     wxListbook: wxListCtrl and wxNotebook combination
// Author:      Vadim Zeitlin
// Modified by:
// Created:     19.08.03
// RCS-ID:      $Id$
// Copyright:   (c) 2003 Vadim Zeitlin <vadim@wxwindows.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_LISTBOOK_H_
#define _WX_LISTBOOK_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "listbook.h"
#endif

#include "wx/defs.h"

#if wxUSE_LISTBOOK

#include "wx/bookctrl.h"

class WXDLLEXPORT wxListView;
class WXDLLEXPORT wxListEvent;
class WXDLLEXPORT wxStaticLine;;

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// wxListbook styles
enum
{
    // default alignment: left everywhere except Mac where it is top
    wxLB_DEFAULT = 0,

    // put the list control to the left/right/top/bottom of the page area
    wxLB_TOP    = 0x1,
    wxLB_BOTTOM = 0x2,
    wxLB_LEFT   = 0x4,
    wxLB_RIGHT  = 0x8,

    // the mask which can be used to extract the alignment from the style
    wxLB_ALIGN_MASK = 0xf
};

// ----------------------------------------------------------------------------
// wxListbook
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxListbook : public wxBookCtrl
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

protected:
    virtual wxWindow *DoRemovePage(size_t page);

private:
    // common part of all constructors
    void Init();

    // get the size which the list control should have
    wxSize GetListSize() const;

    // get the page area
    wxRect GetPageRect() const;

    // event handlers
    void OnSize(wxSizeEvent& event);
    void OnListSelected(wxListEvent& event);


    // the list control we use for showing the pages index
    wxListView *m_list;

    // the line separating it from the page area
    wxStaticLine *m_line;

    // the currently selected page or wxNOT_FOUND if none
    int m_selection;


    DECLARE_EVENT_TABLE()
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxListbook)
};

// ----------------------------------------------------------------------------
// listbook event class and related stuff
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxListbookEvent : public wxBookCtrlEvent
{
public:
    wxListbookEvent(wxEventType commandType = wxEVT_NULL, int id = 0,
                    int nSel = -1, int nOldSel = -1)
        : wxBookCtrlEvent(commandType, id, nSel, nOldSel)
    {
    }

private:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxListbookEvent)
};

extern WXDLLIMPEXP_CORE const wxEventType wxEVT_COMMAND_LISTBOOK_PAGE_CHANGED;
extern WXDLLIMPEXP_CORE const wxEventType wxEVT_COMMAND_LISTBOOK_PAGE_CHANGING;

typedef void (wxEvtHandler::*wxListbookEventFunction)(wxListbookEvent&);

#define EVT_LISTBOOK_PAGE_CHANGED(id, fn)                                   \
  DECLARE_EVENT_TABLE_ENTRY(                                                \
    wxEVT_COMMAND_LISTBOOK_PAGE_CHANGED,                                    \
    id,                                                                     \
    -1,                                                                     \
    (wxObjectEventFunction)(wxEventFunction)(wxListbookEventFunction) &fn,  \
    NULL                                                                    \
  ),

#define EVT_LISTBOOK_PAGE_CHANGING(id, fn)                                  \
  DECLARE_EVENT_TABLE_ENTRY(                                                \
    wxEVT_COMMAND_LISTBOOK_PAGE_CHANGING,                                   \
    id,                                                                     \
    -1,                                                                     \
    (wxObjectEventFunction)(wxEventFunction)(wxListbookEventFunction) &fn,  \
    NULL                                                                    \
  ),

#endif // wxUSE_LISTBOOK

#endif // _WX_LISTBOOK_H_
