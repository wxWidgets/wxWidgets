/////////////////////////////////////////////////////////////////////////////
// Name:        choice.h
// Purpose:     wxChoice class
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CHOICE_H_
#define _WX_CHOICE_H_

#ifdef __GNUG__
#pragma interface "choice.h"
#endif

#include "wx/control.h"

  #include  <wx/dynarray.h>

WXDLLEXPORT_DATA(extern const char*) wxChoiceNameStr;

WX_DEFINE_ARRAY( char * , wxChoiceDataArray ) ;

// Choice item
class WXDLLEXPORT wxChoice: public wxChoiceBase
{
  DECLARE_DYNAMIC_CLASS(wxChoice)

 public:
  wxChoice() {}
  ~wxChoice() ;

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

  bool Create(wxWindow *parent, wxWindowID id,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           int n = 0, const wxString choices[] = NULL,
           long style = 0,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxChoiceNameStr);

    virtual int DoAppend(const wxString& item);
  virtual void Delete(int n);
  virtual void Clear();
  virtual int GetCount() const ;
  virtual int GetSelection() const ;
  virtual void SetSelection(int n);
  virtual int FindString(const wxString& s) const;
  virtual wxString GetString(int n) const ;
  virtual void SetString( int , const wxString& s ) ;
	void		MacHandleControlClick( ControlHandle control , SInt16 controlpart ) ;

/*
  virtual void Append(const wxString& item);
  // Added min Append and GetClientData
  virtual void Append(const wxString& item, void *client_data);
  virtual void *GetClientData(int index) const;
  virtual inline void Select( int n ) { SetSelection( n ); }
  virtual void SetSize(int x, int y, int width, int height, int sizeFlags = wxSIZE_AUTO);
  virtual wxString GetStringSelection() const ;
  virtual bool SetStringSelection(const wxString& sel);

  virtual inline int Number() const { return m_strings.GetCount(); }
  virtual inline int GetCount() const { return m_strings.GetCount(); }
  virtual void Command(wxCommandEvent& event);

  virtual inline int GetColumns() const { return 1 ; };
*/
protected:
    virtual void DoSetItemClientData( int n, void* clientData );
    virtual void* DoGetItemClientData( int n ) const;
    virtual void DoSetItemClientObject( int n, wxClientData* clientData );
    virtual wxClientData* DoGetItemClientObject( int n ) const;

    virtual void DoSetSize(int x, int y,
                           int width, int height,
                           int sizeFlags = wxSIZE_AUTO);

    // free all memory we have (used by Clear() and dtor)
    void Free();

  wxArrayString m_strings;
  wxChoiceDataArray m_dataArray ;
  MenuHandle	m_macPopUpMenuHandle ;
};

#endif
	// _WX_CHOICE_H_
