/////////////////////////////////////////////////////////////////////////////
// Name:        dirdlgg.h
// Purpose:     wxDirDialog
// Author:      Harm van der Heijden and Robert Roebling
// Modified by:
// Created:     12/12/98
// Copyright:   (c) Harm van der Heijden and Robert Roebling
// Licence:   	wxWindows licence
//
// Notes:       wxDirDialog class written by Harm van der Heijden, 
//              uses wxDirCtrl class written by Robert Roebling for the
//              wxFile application, modified by Harm van der Heijden
//
// Description: This generic dirdialog implementation defines three classes:
//              1) wxDirItemData(public wxTreeItemData) stores pathname and
//              displayed name for each item in the directory tree
//              2) wxDirCtrl(public wxTreeCtrl) is a tree widget that 
//              displays a directory tree. It is possible to define sections
//              for fast access to parts of the file system (such as the 
//              user's homedir, /usr/local, /tmp ...etc), similar to
//              Win95 Explorer's shortcuts to 'My Computer', 'Desktop', etc.
//              3) wxDirDialog is the dialog box itself. The user can choose
//              a directory by navigating the tree, or by typing a dir
//              in an inputbox. The inputbox displays paths selected in the
//              tree. It is possible to create new directories. The user
//              will automatically be prompted for dir creation if he 
//              enters a non-existing dir.
//
// TODO/BUGS:   - standard sections only have reasonable defaults for Unix
//                (but others are easily added in wxDirCtrl::SetupSections)
//              - No direct support for "show hidden" toggle. Partly due
//                to laziness on my part and partly because 
//                wxFindFirst/NextFile never seems to find hidden dirs
//                anyway.
//              - No automatic update of the tree (branch) after directory
//                creation.
//              - I call wxBeginBusyCursor while expanding items (creating
//                a new branch might take a few seconds, especially if a
//                CDROM drive or something is involved) but that doesn't
//                seem to do anything. Need to look into that.
//              - Am still looking for an efficient way to delete wxTreeCtrl
//                branches. DeleteChildren has disappeared and 
//                CollapseAndReset( parent ) deletes the parent as well.
//              - The dialog window layout is done using wxConstraints. It
//                works, but it's not as simple as I'd like it to be (see 
//                comments in wxDirDialog::doSize)
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DIRDLGG_H_
#define _WX_DIRDLGG_H_

#ifdef __GNUG__
#pragma interface "dirdlgg.h"
#endif

#include "wx/defs.h"

#if wxUSE_DIRDLG

#include "wx/dialog.h"
#include "wx/checkbox.h"
#include "wx/treectrl.h"

//-----------------------------------------------------------------------------
// data
//-----------------------------------------------------------------------------

WXDLLEXPORT_DATA(extern const wxChar*) wxFileSelectorPromptStr;

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxDirItemData;
class wxDirCtrl;
class wxDirDialog;

//-----------------------------------------------------------------------------
// wxDirItemData
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxDirItemData : public wxTreeItemData
{
public:
  wxDirItemData(wxString& path, wxString& name);
  ~wxDirItemData();
  bool HasSubDirs();
  void SetNewDirName( wxString path );
  wxString m_path, m_name;
  bool m_isHidden;
  bool m_hasSubDirs;
};

//-----------------------------------------------------------------------------
// wxDirCtrl
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxDirCtrl: public wxTreeCtrl
{
public:
    bool           m_showHidden;
    wxTreeItemId   m_rootId;
  
    wxDirCtrl();
    wxDirCtrl(wxWindow *parent, const wxWindowID id = -1, 
	      const wxString &dir = "/",
	      const wxPoint& pos = wxDefaultPosition,
	      const wxSize& size = wxDefaultSize,
	      const long style = wxTR_HAS_BUTTONS,
	      const wxString& name = "wxTreeCtrl" );
    void ShowHidden( const bool yesno );
    void OnExpandItem(wxTreeEvent &event );
    void OnCollapseItem(wxTreeEvent &event );
    void OnBeginEditItem(wxTreeEvent &event );
    void OnEndEditItem(wxTreeEvent &event );
    
protected:
    void CreateItems(const wxTreeItemId &parent);
    void SetupSections();
    wxArrayString m_paths, m_names;
    
private:
    DECLARE_EVENT_TABLE()
    DECLARE_DYNAMIC_CLASS(wxDirCtrl)
};

//-----------------------------------------------------------------------------
// wxDirDialog
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxDirDialog: public wxDialog
{
public:
    wxDirDialog(wxWindow *parent, 
		const wxString& message = wxFileSelectorPromptStr,
		const wxString& defaultPath = wxEmptyString,
		long style = 0, const wxPoint& pos = wxDefaultPosition);
    inline void SetMessage(const wxString& message) { m_message = message; }
    inline void SetPath(const wxString& path) { m_path = path; }
    inline void SetStyle(long style) { m_dialogStyle = style; }

    inline wxString GetMessage() const { return m_message; }
    inline wxString GetPath() const { return m_path; }
    inline long GetStyle() const { return m_dialogStyle; }

    int ShowModal();

    void OnTreeSelected( wxTreeEvent &event );
    void OnTreeKeyDown( wxTreeEvent &event );
    void OnOK(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event); 
    void OnNew(wxCommandEvent& event);
    // void OnCheck(wxCommandEvent& event);

protected:
    // implementation
    wxString       m_message;
    long           m_dialogStyle;
    wxString       m_path;
    wxDirCtrl     *m_dir;
    wxTextCtrl    *m_input;
    wxCheckBox    *m_check;  // not yet used
    wxButton      *m_ok, *m_cancel, *m_new;
    
private:
    DECLARE_EVENT_TABLE()
    DECLARE_DYNAMIC_CLASS(wxDirDialog)
};

#endif

#endif
    // _WX_DIRDLGG_H_

