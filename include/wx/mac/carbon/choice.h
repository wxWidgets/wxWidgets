/////////////////////////////////////////////////////////////////////////////
// Name:        choice.h
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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "choice.h"
#endif

#include "wx/control.h"

#include  "wx/dynarray.h"
#include  "wx/arrstr.h"

WXDLLEXPORT_DATA(extern const wxChar*) wxChoiceNameStr;

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
  virtual int DoInsert(const wxString& item, int pos);
  virtual void Delete(int n);
  virtual void Clear();

  virtual int GetCount() const ;
  virtual int GetSelection() const ;
  virtual void SetSelection(int n);

  virtual int FindString(const wxString& s) const;
  virtual wxString GetString(int n) const ;
  virtual void SetString( int , const wxString& s ) ;
  virtual wxInt32 MacControlHit( WXEVENTHANDLERREF handler , WXEVENTREF event ) ;

protected:
    virtual wxSize DoGetBestSize() const ;

public: // for wxComboBox only
    virtual void DoSetItemClientData( int n, void* clientData );
    virtual void* DoGetItemClientData( int n ) const;
    virtual void DoSetItemClientObject( int n, wxClientData* clientData );
    virtual wxClientData* DoGetItemClientObject( int n ) const;

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
