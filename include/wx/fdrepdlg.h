/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/fdrepdlg.h
// Purpose:     wxFindReplaceDialog class
// Author:      Markus Greither
// Modified by: 31.07.01: VZ: integrated into wxWindows
// Created:     23/03/2001
// RCS-ID:
// Copyright:   (c) Markus Greither
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_FINDREPLACEDLG_H_
#define _WX_FINDREPLACEDLG_H_

#ifdef __GNUG__
    #pragma interface "fdrepdlg.h"
#endif

#include "wx/defs.h"

#if wxUSE_FINDREPLDLG

#include "wx/dialog.h"

class WXDLLEXPORT wxFindReplaceDialog;
class WXDLLEXPORT wxFindReplaceData;
class WXDLLEXPORT wxFindReplaceDialogImpl;

// ----------------------------------------------------------------------------
// Flags for wxFindReplaceData.Flags
// ----------------------------------------------------------------------------

// flages used by wxFindDialogEvent::GetFlags()
enum wxFindReplaceFlags
{
    // downward search/replace selected (otherwise - upwards)
    wxFR_DOWN       = 1,

    // whole word search/replace selected
    wxFR_WHOLEWORD  = 2, 

    // case sensitive search/replace selected (otherwise - case insensitive)
    wxFR_MATCHCASE  = 4
};

// these flags can be specified in wxFindReplaceDialog ctor or Create()
enum wxFindReplaceDialogStyles
{
    // replace dialog (otherwise find dialog)
    wxFR_REPLACEDIALOG = 1,

    // don't allow changing the search direction
    wxFR_NOUPDOWN      = 2,

    // don't allow case sensitive searching
    wxFR_NOMATCHCASE   = 4,

    // don't allow whole word searching
    wxFR_NOWHOLEWORD   = 8
};

// ----------------------------------------------------------------------------
// wxFindReplaceData: holds Setup Data/Feedback Data for wxFindReplaceDialog
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxFindReplaceData : public wxObject
{
public:
    wxFindReplaceData() { }
    wxFindReplaceData(wxUint32 flags) { SetFlags(flags); }

    // accessors
    const wxString& GetFindString() { return m_FindWhat; }
    const wxString& GetReplaceString() { return m_ReplaceWith; }

    int GetFlags() const { return m_Flags; }

    // setters: may only be called before showing the dialog, no effect later
    void SetFlags(wxUint32 flags) { m_Flags = flags; }

    void SetFindString(const wxString& str) { m_FindWhat = str; }
    void SetReplaceString(const wxString& str) { m_ReplaceWith = str; }

private:
    wxUint32 m_Flags;
    wxString m_FindWhat,
             m_ReplaceWith;

    friend class wxFindReplaceDialog;
};

// ----------------------------------------------------------------------------
// wxFindReplaceDialog: dialog for searching / replacing text
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxFindReplaceDialog : public wxDialog
{
public:
    // ctors and such
    wxFindReplaceDialog() { Init(); }
    wxFindReplaceDialog(wxWindow *parent,
                        wxFindReplaceData *data,
                        const wxString &title);

    bool Create(wxWindow *parent,
                wxFindReplaceData *data,
                const wxString &title);

    virtual ~wxFindReplaceDialog();

    // find dialog data access
    const wxFindReplaceData *GetData() const { return m_FindReplaceData; }
    void SetData(wxFindReplaceData *data);

    // implementation only from now on

    wxFindReplaceDialogImpl *GetImpl() const { return m_impl; }

    // override some base class virtuals
    virtual bool Show(bool show);
    virtual void SetTitle( const wxString& title);
    virtual wxString GetTitle() const;

protected:
    virtual void DoGetSize(int *width, int *height) const;
    virtual void DoGetClientSize(int *width, int *height) const;
    virtual void DoSetSize(int x, int y,
                           int width, int height,
                           int sizeFlags = wxSIZE_AUTO);

    void Init();

    wxFindReplaceData      *m_FindReplaceData;
    wxString                m_title;

    wxFindReplaceDialogImpl *m_impl;

    DECLARE_DYNAMIC_CLASS(wxFindReplaceDialog)
};

// ----------------------------------------------------------------------------
// wxFindReplaceDialog events
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxFindDialogEvent : public wxCommandEvent
{
public:
    wxFindDialogEvent(wxEventType commandType = wxEVT_NULL, int id = 0)
        : wxCommandEvent(commandType, id) { }

    int GetFlags() const { return GetInt(); }
    wxString GetFindString() const { return GetString(); }
    const wxString& GetReplaceString() const { return m_strReplace; }

    // implementation only
    void SetFlags(int flags) { SetInt(flags); }
    void SetFindString(const wxString& str) { SetString(str); }
    void SetReplaceString(const wxString& str) { m_strReplace = str; }

private:
    wxString m_strReplace;

    DECLARE_DYNAMIC_CLASS(wxFindDialogEvent)
};

BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EVENT_TYPE(wxEVT_COMMAND_FIND_NEXT, 510)
    DECLARE_EVENT_TYPE(wxEVT_COMMAND_REPLACE, 511)
    DECLARE_EVENT_TYPE(wxEVT_COMMAND_REPLACE_ALL, 512)
    DECLARE_EVENT_TYPE(wxEVT_COMMAND_FIND_CLOSE, 513)
END_DECLARE_EVENT_TYPES()

typedef void (wxEvtHandler::*wxFindDialogEventFunction)(wxFindDialogEvent&);

#define EVT_FIND_NEXT(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( \
        wxEVT_COMMAND_FIND_NEXT, id, -1, \
        (wxObjectEventFunction)(wxEventFunction)(wxFindDialogEventFunction) \
        & fn, \
        (wxObject *) NULL \
    ),

#define EVT_REPLACE(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( \
        wxEVT_COMMAND_REPLACE, id, -1, \
        (wxObjectEventFunction)(wxEventFunction)(wxFindDialogEventFunction) \
        & fn, \
        (wxObject *) NULL \
    ),

#define EVT_FIND_REPLACE_ALL(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( \
        wxEVT_COMMAND_REPLACE_ALL, id, -1, \
        (wxObjectEventFunction)(wxEventFunction)(wxFindDialogEventFunction) \
        & fn, \
        (wxObject *) NULL \
    ),

#define EVT_FIND_FIND_CLOSE(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( \
        wxEVT_COMMAND_FIND_CLOSE, id, -1, \
        (wxObjectEventFunction)(wxEventFunction)(wxFindDialogEventFunction) \
        & fn, \
        (wxObject *) NULL \
    ),

#endif // wxUSE_FINDREPLDLG

#endif
    // _WX_FDREPDLG_H
