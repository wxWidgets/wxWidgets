/*
 * File: FMJobs.h
 * 
 * Author: Robert Roebling
 *
 * Copyright: (C) 1997, GNU (Robert Roebling)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef FMJobs_h
#define FMJobs_h

#ifdef __GNUG__
#pragma interface
#endif

#include "wx/defs.h"
#include "wx/dialog.h"
#include "wx/frame.h"
#include "wx/button.h"
#include "wx/stattext.h"
#include "wx/timer.h"

//-----------------------------------------------------------------------------
// derived classes
//-----------------------------------------------------------------------------

class wxCopyStatusDia;
class wxDeleteStatusDia;
class wxCopyTimer;
class wxDeleteTimer;

//-----------------------------------------------------------------------------
// wxCopyStatusDia
//-----------------------------------------------------------------------------

class wxCopyStatusDia: public wxDialog
{
  DECLARE_DYNAMIC_CLASS( wxCopyStatusDia );

  private:

    wxString        m_dest;
    wxArrayString  *m_files; 
    wxButton       *m_cancelButton;
    wxStaticText   *m_sourceMsg;
    wxStaticText   *m_destMsg;
    wxStaticText   *m_statusMsg;
    bool            m_stop;
    wxTimer        *m_timer;

  public:
   
    wxCopyStatusDia(void) : wxDialog() {};
    wxCopyStatusDia( wxFrame *parent, const wxString &dest, wxArrayString *files );
    ~wxCopyStatusDia();
    void OnCommand( wxCommandEvent &event );
    void DoCopy(void);
    
  private:
    void CopyDir( wxString &srcDir, wxString &destDir );
    void CopyFile( wxString &src, wxString &destDir );
    
  DECLARE_EVENT_TABLE();    
};

//-----------------------------------------------------------------------------
// wxDeleteStatusDia
//-----------------------------------------------------------------------------

/*
class wxDeleteStatusDia: public wxDialog
{
  DECLARE_DYNAMIC_CLASS( wxDeleteStatusDia );

  private:

    wxArrayString  *m_files; 
    wxButton       *m_cancelButton;
    wxStaticText   *m_targetMsg;
    wxStaticText   *m_filesMsg,*m_dirsMsg;
    bool            m_stop;
    wxTimer        *m_timer;
    int             m_countFiles,m_countDirs;

  public:
   
    wxDeleteStatusDia(void) : wxDialog() {};
    wxDeleteStatusDia( wxFrame *parent, wxArrayString *files );
    ~wxDeleteStatusDia();
    void OnCommand( wxCommandEvent &event );
    void DoDelete(void);
    
  private:
    void DeleteDir( wxString &target );
    void DeleteFile( wxString &target );
    
  DECLARE_EVENT_TABLE();
};
*/

//-----------------------------------------------------------------------------
// wxTimer
//-----------------------------------------------------------------------------

class wxCopyTimer: public wxTimer
{
 private:
   wxCopyStatusDia   *m_owner;
   
 public:
   wxCopyTimer( wxCopyStatusDia *owner ) { m_owner = owner; };
   void Notify() { m_owner->DoCopy(); };
};

/*
class wxDeleteTimer: public wxTimer
{
 private:
   wxDeleteStatusDia   *m_owner;
   
 public:
   wxDeleteTimer( wxDeleteStatusDia *owner ) { m_owner = owner; };
   void Notify() { m_owner->DoDelete(); };
};
*/

#endif // FMJobs_h


