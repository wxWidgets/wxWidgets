/*
 * Author: Robert Roebling
 *
 * Copyright: (C) 1997,1998 Robert Roebling
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the wxWindows Licence, which
 * you have received with this library (see Licence.htm).
 *
 */

#ifndef FileList_h
#define FileList_h

#ifdef __GNUG__
#pragma interface
#endif

#include "wx/wx.h"
#include "wx/listctrl.h"

//-----------------------------------------------------------------------------
// derived classes
//-----------------------------------------------------------------------------

class wxFileData;
class wxFileCtrl;

//-----------------------------------------------------------------------------
//  wxFileData
//-----------------------------------------------------------------------------

class wxFileData : public wxObject
{
  DECLARE_DYNAMIC_CLASS(wxFileData);

  private:
    wxString m_name;
    wxString m_fileName;
    long     m_size;
    int      m_hour;
    int      m_minute;
    int      m_year;
    int      m_month;
    int      m_day;
    wxString m_permissions;
    bool     m_isDir;
    bool     m_isLink;
    bool     m_isExe;

  public:
    wxFileData( void ) {};
    wxFileData( const wxString &name, const wxString &fname );
    wxString GetName(void) const;
    wxString GetFullName(void) const;
    wxString GetHint(void) const;
    wxString GetEntry( const int num );
    bool IsDir( void );
    bool IsLink( void );
    bool IsExe( void );
    long GetSize( void );
    bool NewNameIsLegal( const wxString &s );
    bool Rename( const wxString &s );
    void MakeItem( wxListItem &item );
};

//-----------------------------------------------------------------------------
//  wxFileCtrl
//-----------------------------------------------------------------------------

class wxFileCtrl : public wxListCtrl
{
  DECLARE_DYNAMIC_CLASS(wxFileCtrl);

  public:
  
    static wxFileCtrl* m_lastFocus;
      
  private:
    wxString      m_dirName;
    bool          m_showHidden;

  public:
    wxFileCtrl( void );
    wxFileCtrl( wxWindow *win, const wxWindowID id, const wxString &dirName,
      const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize, 
      const long style = wxLC_LIST, const wxString &name = "filelist" );
    void ChangeToListMode();
    void ChangeToReportMode();
    void ChangeToIconMode();
    void ShowHidden( bool show = TRUE );
    void Update( void );
    virtual void StatusbarText( char *WXUNUSED(text) ) {};
    int FillList( wxStringList &list );
    void DeleteFiles(void);
    void CopyFiles( char *dest );
    void MoveFiles( char *dest );
    void RenameFile(void);
    void MakeDir(void);
    void GoToParentDir(void);
    void GoToHomeDir(void);
    void GoToDir( const wxString &dir );
    void GetDir( wxString &dir );
    void OnListDeleteItem( wxListEvent &event );
    void OnListKeyDown( wxListEvent &event );
    void OnListEndLabelEdit( wxListEvent &event );
    void OnSetFocus( wxFocusEvent &event );
    
  DECLARE_EVENT_TABLE()
};

#endif // FileList_h
