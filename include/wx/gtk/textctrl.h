/////////////////////////////////////////////////////////////////////////////
// Name:        textctrl.h
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:     wxWindows licence
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

#if wxUSE_IOSTREAMH
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
  DECLARE_EVENT_TABLE()
  DECLARE_DYNAMIC_CLASS(wxTextCtrl);

  public:
    wxTextCtrl();
    wxTextCtrl( wxWindow *parent, wxWindowID id, const wxString &value = "",
      const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize,
      int style = 0, const wxValidator& validator = wxDefaultValidator,
      const wxString &name = wxTextCtrlNameStr );
    bool Create( wxWindow *parent, wxWindowID id, const wxString &value = "",
      const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize,
      int style = 0, const wxValidator& validator = wxDefaultValidator,
      const wxString &name = wxTextCtrlNameStr );
    wxString GetValue() const;
    void SetValue( const wxString &value );
    void WriteText( const wxString &text );

    bool LoadFile( const wxString &file );
    bool SaveFile( const wxString &file );
    bool IsModified() const { return m_modified; }
    void SetModified() { m_modified = TRUE; }
    void DiscardEdits() { m_modified = FALSE; }
    wxString GetLineText( long lineNo ) const;
    void OnDropFiles( wxDropFilesEvent &event );
    long PositionToXY( long pos, long *x, long *y ) const;
    long XYToPosition( long x, long y ) const;
    int GetLineLength(long lineNo) const;
    int GetNumberOfLines() const;
    virtual void SetInsertionPoint( long pos );
    virtual void SetInsertionPointEnd();
    virtual void SetEditable( bool editable );
    virtual void SetSelection( long from, long to );
    void ShowPosition( long pos );
    virtual long GetInsertionPoint() const;
    virtual long GetLastPosition() const;
    virtual void Remove( long from, long to );
    virtual void Replace( long from, long to, const wxString &value );
    void Cut();
    void Copy();
    void Paste();
    void Clear();

    void OnChar( wxKeyEvent &event );

    int overflow(int i);
    int sync();
    int underflow();

    wxTextCtrl& operator<<(const wxString& s);
    wxTextCtrl& operator<<(int i);
    wxTextCtrl& operator<<(long i);
    wxTextCtrl& operator<<(float f);
    wxTextCtrl& operator<<(double d);
    wxTextCtrl& operator<<(const char c);

    void SetFont( const wxFont &font );
    void SetForegroundColour(const wxColour &colour);
    void SetBackgroundColour(const wxColour &colour);

  // implementation    
    
    GtkWidget* GetConnectWidget();
    bool IsOwnGtkWindow( GdkWindow *window );
    void ApplyWidgetStyle();
    void CalculateScrollbar();
    
  private:
  
    bool        m_modified;
    GtkWidget  *m_text;
    GtkWidget  *m_vScrollbar;
    bool        m_vScrollbarVisible;
};

#endif // __GTKTEXTCTRLH__


