/*
 * File:	DirCtrl.h
 * Purpose:	dir tree control
 * Author:	Robert Roebling
 * Created:	1997
 * Updated:	
 * Copyright:
 */

#ifndef __DIRCTRLH__
#define __DIRCTRLH__

#ifdef __GNUG__
#pragma interface
#endif

#include "wx/treectrl.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxDirInfo;
class wxDirCtrl;

//-----------------------------------------------------------------------------
// wxDirInfo
//-----------------------------------------------------------------------------

class wxDirInfo: public wxObject
{
  DECLARE_DYNAMIC_CLASS(wxDirInfo)

 public:

  wxString   m_name;
  wxString   m_path;
  bool       m_showHidden;
  
 
  wxDirInfo() {};
  wxDirInfo( const wxString &path );
  wxString GetName(void) const;
  wxString GetPath(void) const;
};

//-----------------------------------------------------------------------------
// wxDirCtrl
//-----------------------------------------------------------------------------

class wxDirCtrl: public wxTreeCtrl
{
  DECLARE_DYNAMIC_CLASS(wxDirCtrl)
  
  public:

    bool   m_showHidden;
    int    m_dragX,m_dragY;
    long   m_rootId;
  
    wxDirCtrl(void);
    wxDirCtrl(wxWindow *parent, const wxWindowID id = -1, const wxString &dir = "/",
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            const long style = wxTR_HAS_BUTTONS,
            const wxString& name = "wxTreeCtrl" );
    void OnExpandItem( const wxTreeEvent &event );
    void OnCollapseItem( const wxTreeEvent &event );
    void OnDeleteItem( const wxTreeEvent &event );

  DECLARE_EVENT_TABLE()
};

#endif
