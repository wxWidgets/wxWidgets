/////////////////////////////////////////////////////////////////////////////
// Name:        dirctrlg.h
// Purpose:     wxGenericDirCtrl class
//              Builds on wxDirCtrl class written by Robert Roebling for the
//              wxFile application, modified by Harm van der Heijden.
//              Further modified for Windows.
// Author:      Julian Smart et al
// Modified by:
// Created:     21/3/2000
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DIRCTRL_H_
#define _WX_DIRCTRL_H_

#ifdef __GNUG__
#pragma interface "dirctrlg.h"
#endif

#include "wx/treectrl.h"
#include "wx/dirdlg.h"
#include "wx/choice.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxDirItemData;
class wxDirCtrl;

//-----------------------------------------------------------------------------
// Extra styles for wxGenericDirCtrl
//-----------------------------------------------------------------------------

// Only allow directory viewing/selection, no files
#define wxDIRCTRL_DIR_ONLY       0x0010
// When setting the default path, select the first file in the directory
#define wxDIRCTRL_SELECT_FIRST   0x0020
// Show the filter list
#define wxDIRCTRL_SHOW_FILTERS   0x0040
// Use 3D borders on internal controls
#define wxDIRCTRL_3D_INTERNAL    0x0080

//-----------------------------------------------------------------------------
// wxDirItemData
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxDirItemDataEx : public wxTreeItemData
{
public:
  wxDirItemDataEx(const wxString& path, const wxString& name, bool isDir);
  ~wxDirItemDataEx();
  bool HasSubDirs();
  void SetNewDirName( wxString path );
  wxString m_path, m_name;
  bool m_isHidden;
  bool m_hasSubDirs;
  bool m_isExpanded;
  bool m_isDir;
};

//-----------------------------------------------------------------------------
// wxDirCtrl
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxDirFilterListCtrl;

class WXDLLEXPORT wxGenericDirCtrl: public wxControl
{
public:
    wxGenericDirCtrl();
    wxGenericDirCtrl(wxWindow *parent, const wxWindowID id = -1,
              const wxString &dir = wxDirDialogDefaultFolderStr,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize,
              long style = wxDIRCTRL_3D_INTERNAL|wxSUNKEN_BORDER,
              const wxString& filter = wxEmptyString,
              int defaultFilter = 0,
              const wxString& name = wxTreeCtrlNameStr )
    {
        Init();
        Create(parent, id, dir, pos, size, style, filter, defaultFilter, name);
    }
    
    bool Create(wxWindow *parent, const wxWindowID id = -1,
              const wxString &dir = wxDirDialogDefaultFolderStr,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize,
              long style = wxDIRCTRL_3D_INTERNAL|wxSUNKEN_BORDER,
              const wxString& filter = wxEmptyString,
              int defaultFilter = 0,
              const wxString& name = wxTreeCtrlNameStr );

    void Init();

    ~wxGenericDirCtrl();

    void OnExpandItem(wxTreeEvent &event );
    void OnCollapseItem(wxTreeEvent &event );
    void OnBeginEditItem(wxTreeEvent &event );
    void OnEndEditItem(wxTreeEvent &event );
    void OnSize(wxSizeEvent &event );

    // Try to expand as much of the given path as possible.
    bool ExpandPath(const wxString& path);

    // Accessors

    inline wxString GetDefaultPath() const { return m_defaultPath; }
    void SetDefaultPath(const wxString& path) { m_defaultPath = path; }

    //inline long GetStyleEx() const { return m_styleEx; }
    //void SetStyleEx(long styleEx) { m_styleEx = styleEx; }

    // Get dir or filename
    wxString GetPath() const ;
    // Get selected filename path only (else empty string).
    // I.e. don't count a directory as a selection
    wxString GetFilePath() const ;
    void SetPath(const wxString& path) ;

    wxString GetFilter() const { return m_filter; }
    void SetFilter(const wxString& filter);

    int GetFilterIndex() const { return m_currentFilter; }
    void SetFilterIndex(int n) ;

    wxTreeItemId GetRootId() { return m_rootId; }

    wxTreeCtrl* GetTreeCtrl() const { return m_treeCtrl; }
    wxDirFilterListCtrl* GetFilterListCtrl() const { return m_filterListCtrl; }

//// Helpers
    void SetupSections();
    // Parse the filter into an array of filters and an array of descriptions
    int ParseFilter(const wxString& filterStr, wxArrayString& filters, wxArrayString& descriptions);
    // Find the child that matches the first part of 'path'.
    // E.g. if a child path is "/usr" and 'path' is "/usr/include"
    // then the child for /usr is returned.
    // If the path string has been used (we're at the leaf), done is set to TRUE
    wxTreeItemId FindChild(wxTreeItemId parentId, const wxString& path, bool& done);
    
    // Resize the components of the control
    void DoResize();
protected:
    void ExpandDir(wxTreeItemId parentId);
    void AddSection(const wxString& path, const wxString& name, int imageId = 0);
    //void FindChildFiles(wxTreeItemId id, int dirFlags, wxArrayString& filenames);

    // Extract description and actual filter from overall filter string
    bool ExtractWildcard(const wxString& filterStr, int n, wxString& filter, wxString& description);

private:
    bool            m_showHidden;
    wxTreeItemId    m_rootId;
    wxImageList*    m_imageList;
    wxString        m_defaultPath; // Starting path
    long            m_styleEx; // Extended style
    wxString        m_filter;  // Wildcards in same format as per wxFileDialog
    int             m_currentFilter; // The current filter index
    wxString        m_currentFilterStr; // Current filter string
    wxTreeCtrl*     m_treeCtrl;
    wxDirFilterListCtrl* m_filterListCtrl;

private:
    DECLARE_EVENT_TABLE()
    DECLARE_DYNAMIC_CLASS(wxGenericDirCtrl)
};

//-----------------------------------------------------------------------------
// wxDirFilterListCtrl
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxDirFilterListCtrl: public wxChoice
{
public:
    wxDirFilterListCtrl() { Init(); }
    wxDirFilterListCtrl(wxGenericDirCtrl* parent, const wxWindowID id = -1,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize,
              long style = 0)
    {
        Init();
        Create(parent, id, pos, size, style);
    }
    
    bool Create(wxGenericDirCtrl* parent, const wxWindowID id = -1,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize,
              long style = 0);

    void Init();

    ~wxDirFilterListCtrl() {};

//// Operations
    void FillFilterList(const wxString& filter, int defaultFilter);

//// Events
    void OnSelFilter(wxCommandEvent& event);

protected:
    wxGenericDirCtrl*    m_dirCtrl;

    DECLARE_EVENT_TABLE()
    DECLARE_CLASS(wxDirFilterListCtrl)
};

#define wxID_TREECTRL          7000
#define wxID_FILTERLISTCTRL    7001

//-----------------------------------------------------------------------------
// wxGenericDirDialog
//
//-----------------------------------------------------------------------------

class wxGenericDirDialog: public wxDialog
{
DECLARE_EVENT_TABLE()
public:
    wxGenericDirDialog(): wxDialog() {}
    wxGenericDirDialog(wxWindow* parent, const wxString& title,
        const wxString& defaultPath = wxEmptyString, long style = wxDEFAULT_DIALOG_STYLE, const wxPoint& pos = wxDefaultPosition, const wxSize& sz = wxSize(450, 550), const wxString& name = "dialog");

    void OnCloseWindow(wxCloseEvent& event);
    void OnOK(wxCommandEvent& event);

    inline void SetMessage(const wxString& message) { m_message = message; }
    void SetPath(const wxString& path) ;
    inline void SetStyle(long style) { m_dialogStyle = style; }

    inline wxString GetMessage(void) const { return m_message; }
    wxString GetPath(void) const ;
    inline long GetStyle(void) const { return m_dialogStyle; }

protected:
    wxString    m_message;
    long        m_dialogStyle;
    wxString    m_path;
    wxGenericDirCtrl* m_dirCtrl;

};

#endif
    // _WX_DIRCTRLG_H_
