/////////////////////////////////////////////////////////////////////////////
// Name:        choice.h
// Purpose:     wxChoice class
// Author:      Julian Smart
// Modified by: Vadim Zeitlin to derive from wxChoiceBase
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CHOICE_H_
#define _WX_CHOICE_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "choice.h"
#endif

// ----------------------------------------------------------------------------
// Choice item
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxChoice : public wxChoiceBase
{
public:
    // ctors
    wxChoice() { }
    virtual ~wxChoice();

    wxChoice(wxWindow *parent,
             wxWindowID id,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
             int n = 0, const wxString choices[] = NULL,
             long style = 0,
             const wxValidator& validator = wxDefaultValidator,
             const wxString& name = wxChoiceNameStr)
    {
        Create(parent, id, pos, size, n, choices, style, validator, name);
    }
    wxChoice(wxWindow *parent,
             wxWindowID id,
             const wxPoint& pos,
             const wxSize& size,
             const wxArrayString& choices,
             long style = 0,
             const wxValidator& validator = wxDefaultValidator,
             const wxString& name = wxChoiceNameStr)
    {
        Create(parent, id, pos, size, choices, style, validator, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                int n = 0, const wxString choices[] = NULL,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxChoiceNameStr);
    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxPoint& pos,
                const wxSize& size,
                const wxArrayString& choices,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxChoiceNameStr);

    // implement base class pure virtuals
    virtual int DoAppend(const wxString& item);
    virtual int DoInsert(const wxString& item, int pos);
    virtual void Delete(int n);
    virtual void Clear();

    virtual int GetCount() const;
    virtual int GetSelection() const;
    virtual void SetSelection(int n);

    virtual int FindString(const wxString& s) const;
    virtual wxString GetString(int n) const;
    virtual void SetString(int n, const wxString& s);

    // MSW only
    virtual bool MSWCommand(WXUINT param, WXWORD id);
    WXLRESULT MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam);
    virtual WXHBRUSH MSWControlColor(WXHDC hDC, WXHWND hWnd);

protected:
    virtual void DoMoveWindow(int x, int y, int width, int height);
    virtual void DoSetItemClientData( int n, void* clientData );
    virtual void* DoGetItemClientData( int n ) const;
    virtual void DoSetItemClientObject( int n, wxClientData* clientData );
    virtual wxClientData* DoGetItemClientObject( int n ) const;

    // MSW implementation
    virtual wxSize DoGetBestSize() const;
    virtual void DoGetSize(int *w, int *h) const;
    virtual void DoSetSize(int x, int y,
                           int width, int height,
                           int sizeFlags = wxSIZE_AUTO);

    virtual bool MSWShouldPreProcessMessage(WXMSG *pMsg);

    virtual WXDWORD MSWGetStyle(long style, WXDWORD *exstyle) const;

    // update the height of the drop down list to fit the number of items we
    // have (without changing the visible height)
    void UpdateVisibleHeight();

    // create and initialize the control
    bool CreateAndInit(wxWindow *parent, wxWindowID id,
                       const wxPoint& pos,
                       const wxSize& size,
                       int n, const wxString choices[],
                       long style,
                       const wxValidator& validator,
                       const wxString& name);

    // free all memory we have (used by Clear() and dtor)
    void Free();

    DECLARE_DYNAMIC_CLASS_NO_COPY(wxChoice)
};

#endif // _WX_CHOICE_H_
