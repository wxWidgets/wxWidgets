/////////////////////////////////////////////////////////////////////////////
// Name:        choice.h
// Purpose:     wxChoice class
// Author:      David Webster
// Modified by:
// Created:     10/13/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CHOICE_H_
#define _WX_CHOICE_H_

WXDLLEXPORT_DATA(extern const char*) wxChoiceNameStr;

// Choice item
class WXDLLEXPORT wxChoice: public wxChoiceBase
{
  DECLARE_DYNAMIC_CLASS(wxChoice)

public:
    // ctors
    inline wxChoice() { }

    inline wxChoice(wxWindow *parent, wxWindowID id,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
             int n = 0, const wxString choices[] = NULL,
             long style = 0,
#if wxUSE_VALIDATORS
             const wxValidator& validator = wxDefaultValidator,
#endif
             const wxString& name = wxChoiceNameStr)
    {
        Create(parent, id, pos, size, n, choices, style, validator, name);
    }

    bool Create(wxWindow *parent, wxWindowID id,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
             int n = 0, const wxString choices[] = NULL,
             long style = 0,
#if wxUSE_VALIDATORS
             const wxValidator& validator = wxDefaultValidator,
#endif
             const wxString& name = wxChoiceNameStr);

    // Implement base class virtuals
    virtual int  DoAppend(const wxString& item);
    virtual void Delete(int n);
    virtual void Clear();

    virtual int GetCount() const;
    virtual int GetSelection() const ;
    virtual void SetSelection(int n);

    virtual int FindString(const wxString& s) const;
    virtual wxString GetString(int n) const ;
    virtual void SetString(int n, const wxString& s);

    // OS2 only
    virtual bool OS2Command(WXUINT param, WXWORD id);
    MRESULT OS2WindowProc(HWND hwnd, WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam);

protected:
    virtual void DoSetItemClientData( int n, void* clientData );
    virtual void* DoGetItemClientData( int n ) const;
    virtual void DoSetItemClientObject( int n, wxClientData* clientData );
    virtual wxClientData* DoGetItemClientObject( int n ) const;

    // OS2 implementation
    virtual wxSize DoGetBestSize() const;
    virtual void DoSetSize(int x, int y,
                           int width, int height,
                           int sizeFlags = wxSIZE_AUTO);
private:
    // Virtual function hiding supression
/*
    inline virtual void DoSetClientData( void *data )
    { wxWindowBase::DoSetClientData(data); }
    inline virtual void DoSetClientObject( wxClientData *data )
    { wxWindowBase::DoSetClientObject(data); }
    inline virtual wxClientData *DoGetClientObject() const
    { return(wxWindowBase::DoGetClientObject());}
    inline virtual void *DoGetClientData() const
    { return(wxWindowBase::DoGetClientData());}
*/
};

#endif // _WX_CHOICE_H_
