/////////////////////////////////////////////////////////////////////////////
// Name:        textctrl.h
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef __GTKTEXTCTRLH__
#define __GTKTEXTCTRLH__

#ifdef __GNUG__
#pragma interface
#endif

#include "wx/defs.h"
#include "wx/object.h"
#include "wx/string.h"
#include "wx/control.h"

#if USE_IOSTREAMH
#include <iostream.h>
#else
#include <iostream>
#endif

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxTextCtrl;

//-----------------------------------------------------------------------------
// global data
//-----------------------------------------------------------------------------

extern const char *wxTextCtrlNameStr;

//-----------------------------------------------------------------------------
//  wxTextCtrl
//-----------------------------------------------------------------------------

class wxTextCtrl: public wxControl, public streambuf
{
  DECLARE_DYNAMIC_CLASS(wxTextCtrl);

  public:
  
    wxTextCtrl(void);
    wxTextCtrl( wxWindow *parent, const wxWindowID id, const wxString &value = "", 
      const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize, 
      const int style = 0, const wxString &name = wxTextCtrlNameStr );
    bool Create( wxWindow *parent, const wxWindowID id, const wxString &value = "", 
      const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize, 
      const int style = 0, const wxString &name = wxTextCtrlNameStr );
    wxString GetValue(void) const;
    void SetValue( const wxString &value );
    void WriteText( const wxString &text );
/*
    wxString GetLineText( const long lineNo ) const;
    bool LoadFile( const wxString &file );
    bool SaveFile( const wxString &file );
    void DiscardEdits(void);
    bool IsModified(void);
    void OnDropFiles( wxDropFilesEvent &event );
    long PositionToXY( const long pos, long *x, long *y ) const;
    long XYToPosition( const long x, const long y );
    int GetNumberOfLines(void);
*/
    virtual void SetInsertionPoint( const long pos );
    virtual void SetInsertionPointEnd(void);
    virtual void SetEditable( const bool editable );
    virtual void SetSelection( const long from, const long to );
    void ShowPosition( const long pos );
    virtual long GetInsertionPoint(void) const;
    virtual long GetLastPosition(void) const;
    virtual void Remove( const long from, const long to );
    virtual void Replace( const long from, const long to, const wxString &value );
    void Cut(void);
    void Copy(void);
    void Paste(void);
    void Delete(void);
    
    void OnChar( wxKeyEvent &event );
    
    int overflow(int i);
    int sync(void);
    int underflow(void);

    wxTextCtrl& operator<<(const wxString& s);
    wxTextCtrl& operator<<(const int i);
    wxTextCtrl& operator<<(const long i);
    wxTextCtrl& operator<<(const float f);
    wxTextCtrl& operator<<(const double d);
    wxTextCtrl& operator<<(const char c);

  DECLARE_EVENT_TABLE()
    
};

#endif // __GTKTEXTCTRLH__


