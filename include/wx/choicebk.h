///////////////////////////////////////////////////////////////////////////////
// Name:        wx/choicebk.h
// Purpose:     wxChoicebook: wxChoice and wxNotebook combination
// Author:      Vadim Zeitlin
// Modified by: Wlodzimierz ABX Skiba from wx/listbook.h
// Created:     15.09.04
// RCS-ID:      $Id$
// Copyright:   (c) Vadim Zeitlin, Wlodzimierz Skiba
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CHOICEBOOK_H_
#define _WX_CHOICEBOOK_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "choicebook.h"
#endif

#include "wx/defs.h"

#if wxUSE_CHOICEBOOK

#include "wx/bookctrl.h"

class WXDLLEXPORT wxChoice;

// ----------------------------------------------------------------------------
// wxChoicebook
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxChoicebook : public wxBookCtrlBase
{
public:
    wxChoicebook()
    {
        Init();
    }

    wxChoicebook(wxWindow *parent,
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

    // returns true if we have wxCHB_TOP or wxCHB_BOTTOM style
    bool IsVertical() const { return HasFlag(wxCHB_BOTTOM | wxCHB_TOP); }

    virtual bool DeleteAllPages();

protected:
    virtual wxWindow *DoRemovePage(size_t page);

    // get the size which the choice control should have
    wxSize GetChoiceSize() const;

    // get the page area
    wxRect GetPageRect() const;

    // event handlers
    void OnSize(wxSizeEvent& event);
    void OnChoiceSelected(wxCommandEvent& event);

    // the choice control we use for showing the pages index
    wxChoice *m_choice;

    // the currently selected page or wxNOT_FOUND if none
    int m_selection;

private:
    // common part of all constructors
    void Init();

    DECLARE_EVENT_TABLE()
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxChoicebook)
};

// ----------------------------------------------------------------------------
// choicebook event class and related stuff
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxChoicebookEvent : public wxBookCtrlBaseEvent
{
public:
    wxChoicebookEvent(wxEventType commandType = wxEVT_NULL, int id = 0,
                      int nSel = -1, int nOldSel = -1)
        : wxBookCtrlBaseEvent(commandType, id, nSel, nOldSel)
    {
    }

private:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxChoicebookEvent)
};

extern WXDLLIMPEXP_CORE const wxEventType wxEVT_COMMAND_CHOICEBOOK_PAGE_CHANGED;
extern WXDLLIMPEXP_CORE const wxEventType wxEVT_COMMAND_CHOICEBOOK_PAGE_CHANGING;

typedef void (wxEvtHandler::*wxChoicebookEventFunction)(wxChoicebookEvent&);

#define EVT_CHOICEBOOK_PAGE_CHANGED(id, fn)                                 \
  DECLARE_EVENT_TABLE_ENTRY(                                                \
    wxEVT_COMMAND_CHOICEBOOK_PAGE_CHANGED,                                  \
    id,                                                                     \
    wxID_ANY,                                                               \
    (wxObjectEventFunction)(wxEventFunction) wxStaticCastEvent( wxChoicebookEventFunction, &fn ),  \
    NULL                                                                    \
  ),

#define EVT_CHOICEBOOK_PAGE_CHANGING(id, fn)                                \
  DECLARE_EVENT_TABLE_ENTRY(                                                \
    wxEVT_COMMAND_CHOICEBOOK_PAGE_CHANGING,                                 \
    id,                                                                     \
    wxID_ANY,                                                               \
    (wxObjectEventFunction)(wxEventFunction) wxStaticCastEvent( wxChoicebookEventFunction, &fn ),  \
    NULL                                                                    \
  ),

#endif // wxUSE_CHOICEBOOK

#endif // _WX_CHOICEBOOK_H_
