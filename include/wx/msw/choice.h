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

#ifdef __GNUG__
    #pragma interface "choice.h"
#endif

// ----------------------------------------------------------------------------
// Choice item
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxChoice : public wxChoiceBase
{
    DECLARE_DYNAMIC_CLASS(wxChoice)

public:
    // ctors
    wxChoice() { }

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

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                int n = 0, const wxString choices[] = NULL,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxChoiceNameStr);

    // implement base class pure virtuals
    virtual void DoAppend(const wxString& item);
    virtual void Delete(int n);
    virtual void Clear();

    virtual int GetCount() const;
    virtual int GetSelection() const;
    virtual void SetSelection(int n);

    virtual int FindString(const wxString& s) const;
    virtual wxString GetString(int n) const;

    // MSW only
    virtual bool MSWCommand(WXUINT param, WXWORD id);
    long MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam);

protected:
    virtual void DoSetClientData( int n, void* clientData );
    virtual void* DoGetClientData( int n ) const;
    virtual void DoSetClientObject( int n, wxClientData* clientData );
    virtual wxClientData* DoGetClientObject( int n ) const;

    // MSW implementation
    virtual wxSize DoGetBestSize();
    virtual void DoSetSize(int x, int y,
                           int width, int height,
                           int sizeFlags = wxSIZE_AUTO);
};

#endif // _WX_CHOICE_H_
