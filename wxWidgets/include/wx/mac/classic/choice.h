/////////////////////////////////////////////////////////////////////////////
// Name:        wx/mac/classic/choice.h
// Purpose:     wxChoice class
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CHOICE_H_
#define _WX_CHOICE_H_

#include "wx/control.h"

#include  "wx/dynarray.h"
#include  "wx/arrstr.h"

WXDLLEXPORT_DATA(extern const wxChar) wxChoiceNameStr[];

WX_DEFINE_ARRAY( char * , wxChoiceDataArray ) ;

// Choice item
class WXDLLEXPORT wxChoice: public wxChoiceBase
{
    DECLARE_DYNAMIC_CLASS(wxChoice)

public:
    wxChoice()
        : m_strings(), m_datas(), m_macPopUpMenuHandle(NULL)
        {}

    virtual ~wxChoice() ;

    wxChoice(wxWindow *parent, wxWindowID id,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
             int n = 0, const wxString choices[] = NULL,
             long style = 0,
             const wxValidator& validator = wxDefaultValidator,
             const wxString& name = wxChoiceNameStr)
    {
        Create(parent, id, pos, size, n, choices, style, validator, name);
    }
    wxChoice(wxWindow *parent, wxWindowID id,
             const wxPoint& pos,
             const wxSize& size,
             const wxArrayString& choices,
             long style = 0,
             const wxValidator& validator = wxDefaultValidator,
             const wxString& name = wxChoiceNameStr)
    {
        Create(parent, id, pos, size, choices, style, validator, name);
    }

    bool Create(wxWindow *parent, wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                int n = 0, const wxString choices[] = NULL,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxChoiceNameStr);
    bool Create(wxWindow *parent, wxWindowID id,
                const wxPoint& pos,
                const wxSize& size,
                const wxArrayString& choices,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxChoiceNameStr);

    // implement base class pure virtuals
    virtual int DoAppend(const wxString& item);
    virtual int DoInsert(const wxString& item, unsigned int pos);
    virtual void Delete(unsigned int n);
    virtual void Clear();

    virtual unsigned int GetCount() const ;
    virtual int GetSelection() const ;
    virtual void SetSelection(int n);

    virtual wxString GetString(unsigned int n) const ;
    virtual void SetString(unsigned int pos, const wxString& s);
    void MacHandleControlClick( WXWidget control , wxInt16 controlpart , bool mouseStillDown ) ;

protected:
    virtual wxSize DoGetBestSize() const ;

public: // for wxComboBox only
    virtual void DoSetItemClientData(unsigned int n, void* clientData );
    virtual void* DoGetItemClientData(unsigned int n) const;
    virtual void DoSetItemClientObject(unsigned int n, wxClientData* clientData);
    virtual wxClientData* DoGetItemClientObject(unsigned int n) const;

protected:
    // free all memory we have (used by Clear() and dtor)
    // prevent collision with some BSD definitions of macro Free()
    void FreeData();

    wxArrayString m_strings;
    wxChoiceDataArray m_datas ;
    WXHMENU    m_macPopUpMenuHandle ;
};

#endif
    // _WX_CHOICE_H_
